#include "Request.hpp"
#include <algorithm>
#include "Utils.hpp"

Request::Request(std::vector<ServerBlock>& _serverBlocks)
	:	parsingState(ParsingState::REQUEST_LINE),
		doneReceiving(false),
		method(HTTP::Method::GET),
		errorStatusCode(std::nullopt),
		bodyBytesStored(0),
		isChunkedBodyTransfer(false),
		currentChunkSize(0),
		currentChunkBytesReceived(0),
		awaitingChunkSize(true),
		serverBlocks(_serverBlocks),
		resolvedServerBlock(nullptr),
		resolvedLocationBlock(nullptr)
{
}
