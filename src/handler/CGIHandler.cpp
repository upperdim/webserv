#include <chrono>
#include <vector>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <algorithm>
#include <filesystem>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "HTTP.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "CGIHandler.hpp"

void	CGIHandler::failWithErrorResponse(Request& request, Response& response, int errorStatusCode)
{
	createErrorResponse(request, response, errorStatusCode);
	request.cgiSession.state = Request::CgiState::FAILED;
}

void	CGIHandler::initCgi(Request& request, Response& response)
{
	LOGT(Log::DEBUG, "Initializing CGI");

	const std::string& scriptPath  = request.resolvedPath;

	// If there was no body in this request, create an empty one.
	// Empty file will be redirected as STDIN to the CGI script.
	// This will prevent CGI script from hanging if it will wait input from STDIN.
	// No request body = no body file content = no input = empty STDIN.
	if (request.bodyFilename.empty()) {
		if (!request.createBodyFile()) {
			failWithErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
			return;
		}
	}

	uintmax_t bodyFileSize = 0;
	if (request.isChunkedBodyTransfer) {
		// Content-Length wasn't given
		// We have to measure it to pass it into CGI env
		bodyFileSize = std::filesystem::file_size(request.bodyFilename);
	}

	// Open the body file
	int bodyFileFd = -1;
	bodyFileFd = open(request.bodyFilename.c_str(), O_RDONLY);
	if (bodyFileFd < 0) {
		LOGT(Log::ERROR, "Failed opening temp body file in CGI handler");
		failWithErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		return;
	}

	// Create and open CGI output file
	if (!request.createCgiOutFile()) {
		close(bodyFileFd);
		failWithErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		return;
	}
	int cgiOutputFileFd = open(request.cgiOutFilename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (cgiOutputFileFd < 0) {
		close(bodyFileFd);
		failWithErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		return;
	}

	request.cgiSession.startTime = std::chrono::steady_clock::now();
	
	// Fork CGI process
	pid_t pid = fork();
	if (pid < 0) {
		close(bodyFileFd);
		close(cgiOutputFileFd);
		LOGT(Log::ERROR, "Could not fork a CGI process");
		failWithErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		return;
	}

	// Child process to execute the CGI script
	if (pid == 0) {
		// Redirect body file to STDIN
		dup2(bodyFileFd, STDIN_FILENO); // CGI will read from the body file as STDIN
		close(bodyFileFd); // Duplicated copy lives in STDIN of CGI process, we can close this

		// Redirect stdout
		dup2(cgiOutputFileFd, STDOUT_FILENO); // CGI will write to the cgiOut file as STDOUT
		close(cgiOutputFileFd); // Duplicated copy lives in STDOUT of CGI process, we can close this

		char *argv[] = {
			const_cast<char*>(request.resolvedCgiExecPath.c_str()),
			const_cast<char*>(scriptPath.c_str()),
			NULL
		};

		// Prepare script envp
		std::vector<std::string> envStrings;
		envStrings.push_back("GATEWAY_INTERFACE=CGI/1.1");
		envStrings.push_back("SERVER_PROTOCOL=HTTP/1.1");
		envStrings.push_back("REQUEST_METHOD=" + HTTP::methodToString(request.method));
		envStrings.push_back("SCRIPT_NAME=" + scriptPath);
		envStrings.push_back("QUERY_STRING=" + request.queryString);

		if (request.method == HTTP::Method::POST) {
			if (request.contentLength.has_value()) {
				envStrings.push_back("CONTENT_LENGTH="  + std::to_string(request.contentLength.value()));
			} else {
				envStrings.push_back("CONTENT_LENGTH="  + std::to_string(bodyFileSize));
			}

			if (request.contentType.has_value()) {
				envStrings.push_back("CONTENT_TYPE=" + request.contentType->raw);
			} else {
				envStrings.push_back("CONTENT_TYPE=");
			}
		}

		// TODO: add headers into env too. HTTP_<header key in uppercase>=<header value>

		std::vector<char*> envp;
		for (auto& s : envStrings)
			envp.push_back(const_cast<char*>(s.c_str()));
		envp.push_back(NULL);

		// Execute the CGI script
		execve(request.resolvedCgiExecPath.c_str(), argv, envp.data());
		
		// If we are here, execve() failed. We will capture and handle
		// this error with waitpid() once the child process exits.
		exit(EXIT_FAILURE);
	}
		
	// Parent process
	close(bodyFileFd); // We won't read or write to the body file
	close(cgiOutputFileFd);

	request.cgiSession.pid = pid;
	request.cgiSession.state = Request::CgiState::RUNNING;
}

bool	CGIHandler::checkCgiCompletion(Request& request, Response& response)
{
	Request::CgiSession_t& session = request.cgiSession;

	int status;
	pid_t result = waitpid(session.pid, &status, WNOHANG);

	// CGI child process has finished
	if (result == session.pid) {
		// Check if signal exit
		if (WIFSIGNALED(status)) {
			
			int sig = WTERMSIG(status);
			LOGT(Log::ERROR, "CGI script crashed with signal " << sig);

			failWithErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
			return true;
		}
		
		// Check exit status
		if (WIFEXITED(status)) {
			int exitCode = WEXITSTATUS(status);

			if (exitCode != 0) {
				LOGT(Log::ERROR, "CGI script crashed with exit code " << exitCode);
				failWithErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
				return true;
			}

			// Exit success, validate
			if (!validateCgiOutput(request)) {
				failWithErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
				return true;
			}
			
			session.state = Request::CgiState::PROCESS_FINISHED;
			return true;
		}

		// Invalid exit type?
		LOGT(Log::ERROR, "Could not detect CGI script exit type");
		failWithErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		return true;
	} else if (result == 0) {
		// CGI still running
		if (std::chrono::steady_clock::now() - session.startTime > std::chrono::seconds(CGI_TIMEOUT_SECONDS)) {
			// Timeout
			kill(session.pid, SIGKILL);
			waitpid(session.pid, NULL, 0); // clean up process entry from OS
			failWithErrorResponse(request, response, WSSC_GATEWAY_TIMEOUT);
			return true;
		}
		return false;
	} else {
		LOGT(Log::ERROR, "waitpid() failed in checkCgiCompletion()");
		failWithErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		return true;
	}
}

// RFC 3875 (CGI 1.1)
// Response type: 1 or more header files, blank line, message body (may be null)
bool	CGIHandler::validateCgiOutput(const Request& request)
{
	LOGT(Log::DEBUG, "Verifying CGI output");

	// RFC 3875 6.2.2 Verify headers
	std::ifstream file(request.cgiOutFilename, std::ios::binary);
	if (!file) {
		return false;
	}

	std::ostringstream oss;
	oss << file.rdbuf();
	std::string content = oss.str();

	// Find "\r\n\r\n" (end of headers)
	size_t headersEnd = content.find("\r\n\r\n");
	if (headersEnd == std::string::npos) {
		return false; // If can't, invalid
	}

	// Check headers until the found "\r\n"
	std::string headers = content.substr(0, headersEnd);
	std::istringstream headersStream(headers);

	// Check for "Content-Type" (RFC 3875 6.2.1)
	std::string line;
	bool contentTypeFound = false;

	while (std::getline(headersStream, line)) {
		// Remove \r from \r\n
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		if (line.empty())
			break;

		std::string lineLower = line;
		std::transform(lineLower.begin(), lineLower.end(), lineLower.begin(),
			[](unsigned char c){ return std::tolower(c); });

		// If "content-type:" is at index 0 (beginning)
		if (lineLower.find("content-type:") == 0) {
			if (contentTypeFound) {
				return false; // Only 1 Content-Type header should exist
			}

			contentTypeFound = true;
		}
	}

	return contentTypeFound;
}

void	CGIHandler::createCgiResponse(Request& request, Response& response)
{
	LOGT(Log::DEBUG, "Creating CGI response");

	response.setAsCgiResponse();
	response.setBodyFileBufferReader(request.cgiOutFilename);
	request.cgiSession.state = Request::CgiState::COMPLETE;
}
