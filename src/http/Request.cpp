#include "Request.hpp"
#include <algorithm>
#include "Utils.hpp"

Request::Request(const ServerBlock& _serverBlock)
	:	parsingState(ParsingState::REQUEST_LINE),
		doneReceiving(false),
		method(HTTP::Method::GET),
		errorStatusCode(std::nullopt),
		serverBlock(_serverBlock),
		m_locationBlock(nullptr)
{
}

// TODO: This function should not be called until request is complete
// it needs the correct serverBlock found in order to access the locationBlock
// 
// Move this as a validation to the correct place (after resolving request and matching serverBlock)
bool	Request::isAllowedMethod(void)
{
	const LocationBlock* bestMatch = nullptr;
	for (const LocationBlock& loc : serverBlock.locationBlocks) {
		if (Utils::startsWith(URI, loc.route)) {
			if (bestMatch == nullptr || bestMatch->route.length() < loc.route.length())
				bestMatch = &loc;
		}
	}
	if (bestMatch == nullptr) {
		auto it = std::find_if(serverBlock.locationBlocks.begin(), serverBlock.locationBlocks.end(),
		                     [](const LocationBlock& loc){ return loc.route == "/"; });
		if (it == serverBlock.locationBlocks.end())
			throw std::runtime_error("something went terrible wrong whyle matching locations, failed to find default \"/\" location.");
		bestMatch = &(*it);
	}
	m_locationBlock = const_cast<LocationBlock*>(bestMatch);


	//	TODO:	should we protect this methode here and only accept it if the
	//			request is complete or has an error????
	return std::find(m_locationBlock->allowMethods.begin(), m_locationBlock->allowMethods.end(), method) != m_locationBlock->allowMethods.end();
}
