#include <iostream>
#include <unistd.h> // close()
#include <thread>   // std::this_thread::sleep_for
#include <chrono>   // std::chrono::seconds
#include "ServerEngine.hpp"
#include "ClientConnection.hpp"
#include "HTTP.hpp"
#include "HTTPMethodHandler.hpp"
#include "Log.hpp"

volatile std::sig_atomic_t ServerEngine::isRunning = false;

// Server creation:
//   - Same      host:port => same ServerSocket, insert to map as <server_name, serverBlock>
//   - Different host:port => create a new ServerSocket, insert to map as <server_name, serverBlock>
//
// map < host:port , serverBlock >
//
ServerEngine::ServerEngine(Config config)
{
	if (config.serverBlocks.size() == 0)
		return;

	// Create servers
	for (size_t i = 0; i < config.serverBlocks.size(); ++i) {
		bool isDuplicatePort = false;

		for (size_t j = 0; j < i; ++j) {
			if (config.serverBlocks[i].listenPort == config.serverBlocks[j].listenPort) {
				isDuplicatePort = true;
				break;
			}
		}

		if (!isDuplicatePort) {
			servers.push_back(ServerSocket(config.serverBlocks[i]));
		}
	}

	// Register servers to pollfds
	for (size_t i = 0; i < servers.size(); ++i) {
		addToPollFds(servers[i].getFd());
	}
}

ServerEngine::~ServerEngine()
{
	LOG("ServerEngine destructor");

	for (size_t i = 0; i < servers.size(); ++i) {
		close(servers[i].getFd());
		LOG("closed server fd = " << servers[i].getFd());
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
		if (pollFds.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(EMPTY_POLLFDS_SLEEP_TIME_MS));
		}

		int eventCount = poll(pollFds.data(), pollFds.size(), POLL_TIMEOUT_MS);
		if (eventCount == -1 && isRunning) {
			throw std::runtime_error("poll() error");
		}
		
		iteratePollFds(eventCount);
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
			int serverIdx = findServerIndexByFd(pollFds[i].fd);
			if (serverIdx == -1) {
				readClientIncomingData(pollFds[i].fd);
			} else {
				acceptNewClientConnection(servers[serverIdx]);
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
			
			int serverIdx = findServerIndexByFd(pollFds[i].fd);
			if (serverIdx == -1) {
				disconnectClient(pollFds[i].fd);
			} else {
				stopServer(pollFds[i].fd, serverIdx);
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
void ServerEngine::acceptNewClientConnection(ServerSocket& clientConnectedServer)
{
	int clientFd = accept(clientConnectedServer.getFd(), NULL, NULL);
	if (clientFd == -1) {
		throw std::runtime_error("Error accepting client connection");
	}

	clients.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(static_cast<int>(clientFd)),
		std::forward_as_tuple(static_cast<int>(clientFd), clientConnectedServer));
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

void ServerEngine::stopServer(int serverFd, int serverIdx)
{
	if (serverIdx < 0) {
		LOG("stopServer(): serverIdx not provided, attempting to find by server fd...");
		serverIdx = findServerIndexByFd(serverFd);
		if (serverIdx < 0) {
			throw std::runtime_error("stopServer(): serverIdx not provided and not found");
		}
	}

	close(serverFd);
	LOG("Server fd = " << serverFd << " is closed");
	removeFromPollFds(serverFd);
	servers.erase(servers.begin() + serverIdx);
	LOG("Server is removed from the list and queued for removal from pollFds, fd = " << serverFd);
}

//=============================================================================
// Reading & Writing
//=============================================================================
void ServerEngine::readClientIncomingData(int clientFd)
{
	ClientConnection& client = getClientConnectionByFd(clientFd);
	
	client.receiveRequest();

	if (client.getRequest().doneReceiving) {
		HTTPMethodHandler::handle(client.getRequest(), client.getResponse());
		setPollFdEvents(clientFd, POLLOUT | POLLERR | POLLHUP);
		LOG("Now listening to POLLOUT event for ClientConnection fd = " << clientFd << " socket");
	}

	if (client.getConnectionError()) {
		LOGT(Log::ERROR, "Connection error on ClientConnection fd = " << clientFd);
		disconnectClient(clientFd);
	}
}

void ServerEngine::writeToClient(int clientFd)
{
	ClientConnection& client = getClientConnectionByFd(clientFd);

	client.sendResponse();

	if (client.getResponse().error()) {
		disconnectClient(clientFd);
	}

	if (client.getResponse().complete()) {
		disconnectClient(clientFd);
	}
}

//=============================================================================
// Utility functions
//=============================================================================
int ServerEngine::findServerIndexByFd(int fd)
{
	for (size_t i = 0; i < servers.size(); ++i)
		if (servers[i].getFd() == fd)
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
