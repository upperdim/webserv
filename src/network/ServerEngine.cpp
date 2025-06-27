#include <iostream>
#include <unistd.h>  // close()
#include "ServerEngine.hpp"
#include "ClientConnection.hpp"
#include "HTTP.hpp"
#include "Logger.hpp"

ServerEngine::ServerEngine(Config config) {
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
			servers.push_back(Server(config.serverBlocks[i]));
		}
	}

	// Register servers to pollfds
	for (size_t i = 0; i < servers.size(); ++i) {
		addToPollFds(servers[i].getFd());
	}
}

ServerEngine::~ServerEngine() {
	log("ServerEngine destructor");

	for (size_t i = 0; i < servers.size(); ++i) {
		close(servers[i].getFd());
		log("closed server fd = " << servers[i].getFd());
	}
	
	for (auto it = clients.begin(); it != clients.end(); ++it) {
		close(it->second.getFd());
		log("closed ClientConnection fd = " << it->second.getFd());
	}
}

void ServerEngine::run() {
	while (true) {
		// if (pollFds.empty())
		// 	continue; // TODO: (Optimization) sleep instead?

		int eventCount = poll(pollFds.data(), pollFds.size(), POLL_TIMEOUT_MS);
		if (eventCount == -1) {
			throw std::runtime_error("poll() error");
		}
		
		iteratePollFds(eventCount);
		updatePollFds();
	}
}

void ServerEngine::iteratePollFds(int eventCount) {
	int counter = 0;

	for (size_t i = 0; i < pollFds.size() && counter < eventCount; ++i) {
		if (pollFds[i].revents & POLLIN) {
			log("POLLIN event, fd = " << pollFds[i].fd);
			int serverIdx = findServerIndexByFd(pollFds[i].fd);
			if (serverIdx == -1) {
				readClientIncomingData(pollFds[i].fd);
			} else {
				acceptNewClientConnection(servers[serverIdx]);
			}
		}

		if (pollFds[i].revents & POLLOUT) {
			log("POLLOUT event, fd = " << pollFds[i].fd);
			writeToClient(pollFds[i].fd);
		}
		
		if (pollFds[i].revents & (POLLHUP | POLLERR | POLLNVAL | POLLPRI)) {
			if (pollFds[i].revents & POLLHUP) {
				log("POLLHUP event, fd = " << pollFds[i].fd);
				logl(INFO, "ClientConnection fd = " << pollFds[i].fd << " disconnected");
			} else {
				log("POLLERR or POLLNVAL or POLLPRI event, fd = " << pollFds[i].fd);
				logl(ERROR, "Socket error on fd = " << pollFds[i].fd);
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

void ServerEngine::updatePollFds() {
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

void ServerEngine::addToPollFds(int fd) {
	pollFds.push_back( pollfd{fd, POLLIN | POLLERR | POLLHUP, 0} );
}

void ServerEngine::removeFromPollFds(int fd) {
	for (auto it = pollFds.begin(); it != pollFds.end(); ++it) {
		if (it->fd == fd) {
			pollFds.erase(it);
			break; // break after erasing to avoid invalidating the iterator
		}
	}
}

void ServerEngine::setPollFdEvents(int fd, short events) {
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

void ServerEngine::printPollFds() {
	std::cout << "pollFds = <";
	
	for (size_t i = 0; i < pollFds.size() - 1; ++i)
		std::cout << pollFds[i].fd << ", ";
	
	std::cout << (pollFds.empty() ? "" : std::to_string(pollFds.back().fd))
	          << ">"
	          << std::endl;
}

void ServerEngine::acceptNewClientConnection(Server& clientConnectedServer) {
	int clientFd = accept(clientConnectedServer.getFd(), NULL, NULL);
	if (clientFd == -1) {
		throw std::runtime_error("Error accepting client connection");
	}

	clients.emplace(clientFd, ClientConnection(clientFd, clientConnectedServer));
	pollFdsRegisterQueue.push_back(clientFd);
	logl(INFO, "New ClientConnection accepted, fd = " << clientFd);
}

void ServerEngine::disconnectClient(int clientFd) {
	log("Disconnecting ClientConnection fd = " << clientFd);
	clients.erase(clientFd);
	pollFdsRemovalQueue.push_back(clientFd);
	log("ClientConnection is removed from the map and queued for removal from pollFds, fd = " << clientFd);
	close(clientFd);
	log("ClientConnection fd = " << clientFd << " is closed");
}

void ServerEngine::stopServer(int serverFd, int serverIdx) {
	if (serverIdx < 0) {
		serverIdx = findServerIndexByFd(serverFd);
	}

	close(serverFd);
	log("Server fd = " << serverFd << " is closed");
	removeFromPollFds(serverFd);
	servers.erase(servers.begin() + serverIdx);
	log("Server is removed from the list and queued for removal from pollFds, fd = " << serverFd);
}

void ServerEngine::readClientIncomingData(int clientFd) {
	ClientConnection& client = getClientConnectionByFd(clientFd);
	
	client.receiveRequest();

	if (client.getRequest().isComplete) {
		client.setResponse(HTTP::handleHTTPRequest(client.getRequest()));
		setPollFdEvents(clientFd, POLLOUT | POLLERR | POLLHUP);
		log("Now listening to POLLOUT event for ClientConnection fd = " << clientFd << " socket");
	}

	if (client.getConnectionError()) {
		logl(ERROR, "Connection error on ClientConnection fd = " << clientFd);
		disconnectClient(clientFd);
	}
}

void ServerEngine::writeToClient(int clientFd) {
	ClientConnection& client = getClientConnectionByFd(clientFd);

	client.sendResponse();

	if (client.getResponse().isComplete) {
		disconnectClient(clientFd);
	}
}

int ServerEngine::findServerIndexByFd(int fd) {
	for (size_t i = 0; i < servers.size(); ++i)
		if (servers[i].getFd() == fd)
			return i;
	return -1;
}

ClientConnection& ServerEngine::getClientConnectionByFd(int fd) {
	auto it = clients.find(fd);
	if (it == clients.end()) { 
		throw std::runtime_error("getClientConnectionByFd(): fd was not found in clients map");
	}
	return it->second;
}
