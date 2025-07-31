#ifndef SERVERENGINE_HPP
#define SERVERENGINE_HPP

#include <unordered_map>
#include <csignal>
#include <vector>
#include "poll.h"
#include "ServerSocket.hpp"
#include "Config.hpp"
#include "ClientConnection.hpp"

#define EMPTY_POLLFDS_SLEEP_TIME_MS	200
#define POLL_TIMEOUT_MS				2000
#define POLL_ERROR_EVENTS			(POLLHUP | POLLERR | POLLNVAL | POLLPRI)
#define POLL_LISTEN_EVENTS			(POLLIN | POLL_ERROR_EVENTS)
#define POLL_SEND_EVENTS			(POLLOUT | POLL_ERROR_EVENTS)

class ServerEngine
{
private:
	std::vector<ServerSocket>					serverSockets;
	std::unordered_map<int, ClientConnection>	clients;              // Key: fd, value: ClientConnection

	std::vector<struct pollfd>					pollFds;
	std::vector<int>							pollFdsRegisterQueue; // File descriptors awaiting to be add into pollFds list
	std::vector<int>							pollFdsRemovalQueue;  // File descriptors awaiting to be remove from pollFds list

	void										addToPollFds(int fd);
	void										removeFromPollFds(int fd);
	void										setPollFdEvents(int fd, short events);
	void										printPollFds();

	void										iteratePollFds(int eventCount);
	void										updatePollFds();
	
	void										acceptNewClientConnection(ServerSocket& clientConnectedServerSocket);
	void										disconnectClient(int clientFd);
	void										stopServerSocket(int serverSocketFd, int serverSocketIdx);
	void										readClientIncomingData(int clientFd);
	void										writeToClient(int clientFd);
	
	int											findServerSocketIndexByFd(int fd); // TODO: (Optimization) fd to ServerSocket map for speed?
	ClientConnection&							getClientConnectionByFd(int fd);
	
public:
	ServerEngine(Config config);
	~ServerEngine();

	static volatile std::sig_atomic_t			isRunning;
	void										run();
};

#endif
