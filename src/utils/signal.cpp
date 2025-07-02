#include "webserv.hpp"
#include "ServerEngine.hpp"

void	handleAbort(int signum)
{
	(void)signum;

	ServerEngine::isRunning = false;

	std::cout << "\033[2K\033[1G";
	// std::cout << "\033[K";
	std::cout.flush();
}
