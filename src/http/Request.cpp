#include "Request.hpp"
#include <algorithm>
#include "Utils.hpp"

Request::Request(const ServerBlock& _serverBlock)
	:	parsingState(ParsingState::REQUEST_LINE),
		doneReceiving(false),
		method(HTTP::Method::GET),
		errorStatusCode(std::nullopt),
		serverBlock(_serverBlock),
		resolvedLocationBlock(nullptr)
{
}

// TODO: This function should not be called until request is complete
// it needs the correct serverBlock found in order to access the locationBlock
// 
// Move this as a validation to the correct place (after resolving request and matching serverBlock)
bool	Request::isAllowedMethod(void)
{

	if (resolvedLocationBlock == nullptr)
		throw std::runtime_error("something went terrible wrong: reached isAllowedmethod() with unresolved locationBlock.");

	//	TODO:	should we protect this methode here and only accept it if the
	//			request is complete or has an error????
	return std::find(
				resolvedLocationBlock->allowMethods.begin(),
				resolvedLocationBlock->allowMethods.end(),
				method
			) != resolvedLocationBlock->allowMethods.end();
}
