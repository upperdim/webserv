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

// TODO: This function should not be called until request is complete
// it needs the correct serverBlock found in order to access the locationBlock
// 
// Move this as a validation to the correct place (after resolving request and matching serverBlock)
bool	Request::isAllowedMethod(void)
{
	for (const LocationBlock& locationBlock : serverBlock.locationBlocks) {
		if (locationBlock.route == URI) {
			m_locationBlock = const_cast<LocationBlock*>(&locationBlock);
		}
	}

	if (m_locationBlock == nullptr) {
		throw std::runtime_error("locationblock is null");
	}

	//	TODO:	should we protect this methode here and only accept it if the
	//			request is complete or has an error????
	return std::find(m_locationBlock->allowMethods.begin(), m_locationBlock->allowMethods.end(), method) != m_locationBlock->allowMethods.end();
}
