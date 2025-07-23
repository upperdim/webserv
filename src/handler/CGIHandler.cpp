#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <errno.h> // TODO: REMOVE THIS
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "CGIHandler.hpp"
#include "HTTP.hpp"

void CGIHandler::handle(const Request& request, Response& response)
{
	LOGC("REQUEST_HANDLER", "-> handle CGI Request", LIGHTMAGENTA, LIGHTCYAN);

	const std::string& scriptPath  = request.resolvedPath;

	int bodyFileFd = -1;
	if (!request.bodyTempFilename.empty()) {
		bodyFileFd = open(request.bodyTempFilename.c_str(), O_RDONLY);
		if (bodyFileFd < 0) {
			LOGT(Log::ERROR, "Failed opening temp body file in CGI handler");
			createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
			return;
		}
	}
	
	int outputPipe[2]; // Child  writes to [1], parent reads from [0]
	if (pipe(outputPipe) < 0) {
		close(bodyFileFd);
		LOGT(Log::ERROR, "Could not pipe for CGI process");
		createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		return;
	}

	pid_t pid = fork();
	if (pid < 0) {
		close(bodyFileFd);
		close(outputPipe[0]);
		close(outputPipe[1]);
		LOGT(Log::ERROR, "Could not fork a CGI process");
		createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		return;
	}

	LOGT(Log::DEBUG, "Using python3 from " << PYTHON3_PATH); // TODO: remove me

	// Child process
	if (pid == 0) {
		// Redirect stdin
		dup2(bodyFileFd, STDIN_FILENO); // We will write here, to STDIN of CGI
		close(bodyFileFd);   // Duplicated copy lives in STDIN of CGI process, we can close this

		// Redirect stdout
		dup2(outputPipe[1], STDOUT_FILENO); // We will read here, from STDOUT of CGi
		close(outputPipe[1]); // Duplicated copy lives in STDOUT of CGI process, we can close this
		close(outputPipe[0]); // We won't use reading end of this pipe

		// Prepare argv
		std::string pythonPath = PYTHON3_PATH;

		char *argv[] = {
			const_cast<char*>(pythonPath.c_str()),
			const_cast<char*>(scriptPath.c_str()),
			NULL
		};

		// Prepare envp
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

		// Execute script
		execve(pythonPath.c_str(), argv, envp.data());

		// TODO: REMOVE THIS
		LOGT(Log::ERROR, "execve errno = " << errno);

		// If we are here, execve() failed
		LOGT(Log::ERROR, "CGI Process execve() failed, PID = " << pid);
		createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
		exit(EXIT_FAILURE);
	}
		
	// Parent process
	close(bodyFileFd);   // Duplicated copy lives in STDIN of CGI process, we can close this
	close(outputPipe[1]); // We won't use writing end of this pipe

	// If POST, write request body to child STDIN
	// if (request.method == HTTP::Method::POST && !requestBody.empty()) {
	// 	ssize_t written = write(inputPipe[1], requestBody.c_str(), requestBody.length());
	// 	if (written < 0) {
	// 		// Error: write failed
	// 	}
	// }

	// Read CGI output
	std::ostringstream cgiOutput;
	char buffer[CGI_OUTPUT_BUFFER_SIZE];
	ssize_t bytesRead;
	do {
		bytesRead = read(outputPipe[0], buffer, CGI_OUTPUT_BUFFER_SIZE - 1);
		if (bytesRead < 0) {
			LOGT(Log::ERROR, "Error reading from CGI output");
			close(outputPipe[0]); // Finished reading
			waitpid(pid, NULL, 0);
			createErrorResponse(request, response, WSSC_INTERNAL_SERVER_ERROR);
			return;
		}
		
		// buffer[bytesRead] = '\0';
		cgiOutput.write(buffer, bytesRead);
	} while (bytesRead != 0);
	
	close(outputPipe[0]);  // Finished reading
	waitpid(pid, NULL, 0); // Wait for child process

	// LOGT(Log::DEBUG, "CGI Output = " << cgiOutput.str());

	// RFC 3875 CGI 1.1
	// Response type: 1 or more header files, blank line, message body (may be null)

	// response.addHeader WIP
	response.setBodyString(cgiOutput.str());
}
