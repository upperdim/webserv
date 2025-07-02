#ifndef WEBSERV_H
# define WEBSERV_H

// a good default RESPONSE_BUFFER_SIZE seams to be 64kb - 128kb
# define RESPONSE_BUFFER_SIZE (64 * 1024)

# include <iostream>
# include <csignal>
# include <string>
# include <unordered_map>
# include <fstream>
# include <sstream>
# include <iostream>
# include "Log.hpp"
# include "colors.hpp"

typedef std::unordered_map<std::string, std::string> HeaderMap;

extern bool	g_running;

void		handleAbort(int signum);
std::string readFile(const std::string& filename);

#endif