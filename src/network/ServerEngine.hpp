#ifndef SERVERENGINE_HPP
#define SERVERENGINE_HPP

#include <unordered_map>
#include <vector>
#include "poll.h"
#include "Server.hpp"
#include "Config.hpp"
#include "ClientConnection.hpp"

#define EMPTY_POLLFDS_SLEEP_TIME_MS	200
#define POLL_TIMEOUT_MS				2000

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
	
	void										acceptNewClientConnection(ServerSocket& clientConnectedServer);
	void										disconnectClient(int clientFd);
	void										stopServer(int serverFd, int serverIdx);
	void										readClientIncomingData(int clientFd);
	void										writeToClient(int clientFd);
	
	int											findServerIndexByFd(int fd); // TODO: (Optimization) fd to server map for speed?
	ClientConnection&							getClientConnectionByFd(int fd);
	
public:
	ServerEngine(Config config);
	~ServerEngine();

	static volatile std::sig_atomic_t			isRunning;
	void										run();
};

#endif
