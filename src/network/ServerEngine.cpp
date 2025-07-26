#include <iostream>
#include <unistd.h> // close()
#include "ServerEngine.hpp"
#include "ClientConnection.hpp"
#include "RequestHandler.hpp"
#include "ServerSocket.hpp"
#include "Config.hpp"
#include "Log.hpp"

volatile std::sig_atomic_t ServerEngine::isRunning = false;

ServerEngine::ServerEngine(Config config)
{
	// Create servers

	// Group serverBlocks by host:port
	std::map<std::string, std::vector<ServerBlock*>> hostPortToServerBlocks;

	for (ServerBlock& serverBlock : config.serverBlocks) {
		std::string hostPortStr = serverBlock.listenHostStr + ":" + std::to_string(serverBlock.listenPort);

		hostPortToServerBlocks[hostPortStr].push_back(&serverBlock);
	}
	
	for (const auto& pair : hostPortToServerBlocks) {
		const std::vector<ServerBlock*>& blocks = pair.second;

		// Create ServerSockets for host:port
		ServerSocket serverSocket = ServerSocket(*blocks[0]);

		// Add all serverBlocks for host:port
		for (size_t i = 1; i < blocks.size(); ++i) {
			serverSocket.serverBlocks.push_back(*blocks[i]);
		}

		// Add the created ServerSocket to serverSockets list
		serverSockets.push_back(serverSocket);

		// Register the created ServerSockets fd to pollFds
		addToPollFds(serverSocket.getFd());
	}
}

ServerEngine::~ServerEngine()
{
	LOG("ServerEngine destructor");

	for (size_t i = 0; i < serverSockets.size(); ++i) {
		close(serverSockets[i].getFd());
		LOG("closed server fd = " << serverSockets[i].getFd());
	}

	for (auto it = clients.begin(); it != clients.end(); ++it) {
		close(it->second.getFd());
		LOG("closed ClientConnection fd = " << it->second.getFd());
	}
}

//=============================================================================
// Main (event) loop
//=============================================================================
void ServerEngine::run()
{
	isRunning = true;

	while (isRunning) {
		if (!pollFds.empty()) {
			int eventCount = poll(pollFds.data(), pollFds.size(), POLL_TIMEOUT_MS);
			if (eventCount == -1 && isRunning) {
				throw std::runtime_error("poll() error");
			}

			iteratePollFds(eventCount);
		}

		updatePollFds();
	}

	LOGT(Log::INFO, "ServerEngine stopped, shutting down webserv.");
}

void ServerEngine::iteratePollFds(int eventCount)
{
	int counter = 0;

	for (size_t i = 0; i < pollFds.size() && counter < eventCount; ++i) {
		if (pollFds[i].revents & POLLIN) {
			LOG("POLLIN event, fd = " << pollFds[i].fd);
			int serverSocketIdx = findServerSocketIndexByFd(pollFds[i].fd);
			if (serverSocketIdx == -1) {
				readClientIncomingData(pollFds[i].fd);
			} else {
				acceptNewClientConnection(serverSockets[serverSocketIdx]);
			}
		}

		if (pollFds[i].revents & POLLOUT) {
			LOG("POLLOUT event, fd = " << pollFds[i].fd);
			writeToClient(pollFds[i].fd);
		}
		
		if (pollFds[i].revents & (POLLHUP | POLLERR | POLLNVAL | POLLPRI)) {
			if (pollFds[i].revents & POLLHUP) {
				LOG("POLLHUP event, fd = " << pollFds[i].fd);
				LOGT(Log::INFO, "ClientConnection fd = " << pollFds[i].fd << " disconnected");
			} else {
				LOG("POLLERR or POLLNVAL or POLLPRI event, fd = " << pollFds[i].fd);
				LOGT(Log::ERROR, "Socket error on fd = " << pollFds[i].fd);
			}
			
			int serverSocketIdx = findServerSocketIndexByFd(pollFds[i].fd);
			if (serverSocketIdx == -1) {
				disconnectClient(pollFds[i].fd);
			} else {
				stopServerSocket(pollFds[i].fd, serverSocketIdx);
			}
		}

		if (pollFds[i].revents)
			++counter;
	}
}

void ServerEngine::updatePollFds()
{
	for (size_t i = 0; i < pollFdsRemovalQueue.size(); ++i) {
		removeFromPollFds(pollFdsRemovalQueue[i]);
	}

	for (size_t i = 0; i < pollFdsRegisterQueue.size(); ++i) {
		addToPollFds(pollFdsRegisterQueue[i]);
	}

	if (pollFdsRegisterQueue.size() > 0 || pollFdsRegisterQueue.size() > 0) {
		std::cout << "pollFds updated: ";
		printPollFds();
	}

	pollFdsRemovalQueue.clear();
	pollFdsRegisterQueue.clear();
}

//=============================================================================
// poll() file descriptors' functions
//=============================================================================
void ServerEngine::addToPollFds(int fd)
{
	pollFds.push_back( pollfd{fd, POLLIN | POLLERR | POLLHUP, 0} );
}

void ServerEngine::removeFromPollFds(int fd)
{
	for (auto it = pollFds.begin(); it != pollFds.end(); ++it) {
		if (it->fd == fd) {
			pollFds.erase(it);
			break; // break after erasing to avoid invalidating the iterator
		}
	}
}

void ServerEngine::setPollFdEvents(int fd, short events)
{
	// TODO: (Optimization) This is called by readClientIncomingData().
	//       Keep the index of client inside pollFds as a member varaible
	//       in order to eliminate this iteration?
	for (size_t i = 0; i < pollFds.size(); ++i) {
		if (pollFds[i].fd == fd) {
			pollFds[i].events = events;
			return;
		}
	}
	throw std::runtime_error("Could not find fd in pollFds to set the event of.");
}

void ServerEngine::printPollFds()
{
	std::cout << "pollFds = <";
	
	for (size_t i = 0; i < pollFds.size() - 1; ++i)
		std::cout << pollFds[i].fd << ", ";
	
	std::cout << (pollFds.empty() ? "" : std::to_string(pollFds.back().fd))
	          << ">"
	          << std::endl;
}

//=============================================================================
// Connections & Disconnections
//=============================================================================
void ServerEngine::acceptNewClientConnection(ServerSocket& clientConnectedServerSocket)
{
	int clientFd = accept(clientConnectedServerSocket.getFd(), NULL, NULL);
	if (clientFd == -1) {
		throw std::runtime_error("Error accepting client connection");
	}

	clients.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(static_cast<int>(clientFd)),
		std::forward_as_tuple(static_cast<int>(clientFd), clientConnectedServerSocket));
	pollFdsRegisterQueue.push_back(clientFd);
	LOGT(Log::INFO, "New ClientConnection accepted, fd = " << clientFd);
}

void ServerEngine::disconnectClient(int clientFd)
{
	LOG("Disconnecting ClientConnection fd = " << clientFd);
	clients.erase(clientFd);
	pollFdsRemovalQueue.push_back(clientFd);
	LOG("ClientConnection is removed from the map and queued for removal from pollFds, fd = " << clientFd);
	close(clientFd);
	LOG("ClientConnection fd = " << clientFd << " is closed");
}

void ServerEngine::stopServerSocket(int serverSocketFd, int serverSocketIdx)
{
	if (serverSocketIdx < 0) {
		LOG("stopServer(): serverIdx not provided, attempting to find by server fd...");
		serverSocketIdx = findServerSocketIndexByFd(serverSocketFd);
		if (serverSocketIdx < 0) {
			throw std::runtime_error("stopServerSocket(): serverIdx not provided and not found");
		}
	}

	close(serverSocketFd);
	LOG("Server fd = " << serverSocketFd << " is closed");
	removeFromPollFds(serverSocketFd);
	serverSockets.erase(serverSockets.begin() + serverSocketIdx);
	LOG("ServerSocket is removed from the list and queued for removal from pollFds, fd = " << serverSocketFd);
}

//=============================================================================
// Reading & Writing
//=============================================================================
void ServerEngine::readClientIncomingData(int clientFd)
{
	ClientConnection& client = getClientConnectionByFd(clientFd);
	
	client.receiveRequest();

	if (client.getConnectionError() || client.getDisconnected()) {
		disconnectClient(clientFd);
		return;
	}

	if (client.getRequest().doneReceiving) {
		RequestHandler::handle(client.getRequest(), client.getResponse());
		
		setPollFdEvents(clientFd, POLLOUT | POLLERR | POLLHUP);
		LOG("Now listening to POLLOUT event for ClientConnection fd = " << clientFd << " socket");
	}
}

void ServerEngine::writeToClient(int clientFd)
{
	ClientConnection& client = getClientConnectionByFd(clientFd);

	client.sendResponse();

	if (client.getResponse().error()) {
		disconnectClient(clientFd);
		return;
	}

	if (client.getResponse().complete()) {
		disconnectClient(clientFd);
	}
}

//=============================================================================
// Utility functions
//=============================================================================
int ServerEngine::findServerSocketIndexByFd(int fd)
{
	for (size_t i = 0; i < serverSockets.size(); ++i)
		if (serverSockets[i].getFd() == fd)
			return i;
	return -1;
}

ClientConnection& ServerEngine::getClientConnectionByFd(int fd)
{
	auto it = clients.find(fd);
	if (it == clients.end()) { 
		throw std::runtime_error("getClientConnectionByFd(): fd was not found in clients map");
	}
	return it->second;
}
