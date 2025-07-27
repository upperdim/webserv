#include <vector>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "HTTP.hpp"
#include "CGIHandler.hpp"

void	CGIHandler::initCgi(Request& request, Response& response)
{
	LOGT(Log::DEBUG, "Initializing CGI");
	LOGT(Log::DEBUG, "Using python3 from " << PYTHON3_PATH); // TODO: remove me

	const std::string& scriptPath  = request.resolvedPath;

	// If there was no body in this request, create an empty one.
	// Empty file will be redirected as STDIN to the CGI script.
	// This will prevent CGI script from hanging if it will wait input from STDIN.
	// No request body = no body file content = no input = empty STDIN.
	if (request.bodyFilename.empty()) {
		if (!request.createBodyFile()) {
			createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
			request.cgiSession.error = true;
			return;
		}
	}

	// Open the body file
	int bodyFileFd = -1;
	bodyFileFd = open(request.bodyFilename.c_str(), O_RDONLY);
	if (bodyFileFd < 0) {
		LOGT(Log::ERROR, "Failed opening temp body file in CGI handler");
		createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		request.cgiSession.error = true;
		return;
	}

	// Create and open CGI output file
	if (!request.createCgiOutFile()) {
		close(bodyFileFd);
		createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		request.cgiSession.error = true;
		return;
	}
	int cgiOutputFileFd = open(request.cgiOutFilename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (cgiOutputFileFd < 0) {
		close(bodyFileFd);
		createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		request.cgiSession.error = true;
		return;
	}
	
	// Fork CGI process
	pid_t pid = fork();
	if (pid < 0) {
		close(bodyFileFd);
		close(cgiOutputFileFd);
		LOGT(Log::ERROR, "Could not fork a CGI process");
		createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		request.cgiSession.error = true;
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

		// Prepare script argv
		std::string pythonPath = PYTHON3_PATH;

		char *argv[] = {
			const_cast<char*>(pythonPath.c_str()),
			const_cast<char*>(scriptPath.c_str()),
			NULL
		};

		// Prepare script envp
		std::vector<std::string> envStrings;
		envStrings.push_back("GATEWAY_INTERFACE=CGI/1.1");
		envStrings.push_back("SERVER_PROTOCOL=HTTP/1.1");
		envStrings.push_back("REQUEST_METHOD=" + HTTP::methodToString(request.method));
		envStrings.push_back("QUERY_STRING=" + request.queryString);
		envStrings.push_back("SCRIPT_NAME=" + scriptPath);

		if (request.method == HTTP::Method::POST) {
			// To be added
			// envStrings.push_back("CONTENT_LENGTH=" ;
			// envStrings.push_back("CONTENT_TYPE=" ;
		} else {
			envStrings.push_back("CONTENT_LENGTH=0");
			envStrings.push_back("CONTENT_TYPE=");
		}

		std::vector<char*> envp;
		for (auto& s : envStrings)
			envp.push_back(const_cast<char*>(s.c_str()));
		envp.push_back(NULL);

		// Execute the CGI script
		execve(pythonPath.c_str(), argv, envp.data());
		// If we are here, execve() failed

		// TODO: Write internal server error into the cgi out file
		//	 it will be the response back to client
		// createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		
		exit(EXIT_FAILURE);
	}
		
	// Parent process
	close(bodyFileFd); // We won't read or write to the body file
	close(cgiOutputFileFd);

	request.cgiSession.pid = pid;
}

bool	CGIHandler::checkCgiCompletion(Request& request)
{
	LOGT(Log::DEBUG, "Checking CGI completion");

	if (request.cgiSession.error) {
		return true;
	}

	int status;
	pid_t result = waitpid(request.cgiSession.pid, &status, WNOHANG);
	if (result == request.cgiSession.pid) {
		return true;
	} else if (result == 0) {
		// CGI still running
		// TODO: Check for timeout
		return false;
	} else {
		request.cgiSession.error = true;
		return true;
	}
}

void	CGIHandler::createCgiResponse(Request& request, Response& response)
{
	LOGT(Log::DEBUG, "Completing CGI session");
	
	(void) request;
	(void) response;

	// RFC 3875 CGI 1.1
	// Response type: 1 or more header files, blank line, message body (may be null)
	// response.addHeader WIP
	// response.setBodyString(cgiOutput.str()); // TODO: Instead, set BodyFileBufferReader
}
