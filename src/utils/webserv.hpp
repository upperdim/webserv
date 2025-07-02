#ifndef WEBSERV_H
#define WEBSERV_H

#include <string>
#include <unordered_map>

// a good default RESPONSE_BUFFER_SIZE seams to be 64kb - 128kb
#define RESPONSE_BUFFER_SIZE (64 * 1024)

// Request Parser Defines
#define MAX_URI_LENGTH 2048

typedef std::unordered_map<std::string, std::string> HeaderMap;

void		handleAbort(int signum);
std::string readFile(const std::string& filename);

#endif