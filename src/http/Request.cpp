#include "Request.hpp"
#include <algorithm>

Request::Request(const ServerBlock& _serverBlock)
	:	parsingState(ParsingState::REQUEST_LINE),
		doneReceiving(false),
		method(HTTP::Method::GET),
		errorStatusCode(std::nullopt),
		serverBlock(_serverBlock),
		m_locationBlock(nullptr)
{
}

const LocationBlock&	Request::locationBlock()
{
	if (m_locationBlock == nullptr) {
		// Matches the requestTaget to a serverBlock.locationBlock
		// TODO:	more needs o be done
		for (const LocationBlock& locationBlock : serverBlock.locationBlocks) {
			if (locationBlock.route == URI) {
				m_locationBlock = const_cast<LocationBlock*>(&locationBlock);
				return *m_locationBlock;
			}
		}
		m_locationBlock = const_cast<LocationBlock*>(&serverBlock.locationBlocks.front());
	}
	return *m_locationBlock;
}

bool	Request::isAllowedMethod(void)
{
	//	TODO:	should we protect this methode here and only accept it if the
	//			request is complete or has an error????
	return std::find(locationBlock().allowMethods.begin(), locationBlock().allowMethods.end(), method) != locationBlock().allowMethods.end();
}
