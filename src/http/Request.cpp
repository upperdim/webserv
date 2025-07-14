#include "Request.hpp"
#include <algorithm>
#include "Utils.hpp"

Request::Request(std::vector<ServerBlock>& _serverBlocks)
	:	parsingState(ParsingState::REQUEST_LINE),
		storeBodyInFile(false),
		doneReceiving(false),
		method(HTTP::Method::GET),
		errorStatusCode(std::nullopt),
		serverBlocks(_serverBlocks),
		resolvedServerBlock(nullptr),
		resolvedLocationBlock(nullptr),
		isChunkedBodyTransfer(false)
{
}
