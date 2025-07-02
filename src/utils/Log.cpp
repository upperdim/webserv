#include "Log.hpp"

Log::~Log()
{
}

const Log::t_type Log::LOG_TYPES[6] = {
	{"DEBUG",   RESET,       RESET},
	{"INFO",    LIGHTBLUE,   BLUE},
	{"WARNING", LIGHTYELLOW, YELLOW},
	{"ERROR",   LIGHTRED,    RED},
	{"SUCCESS", LIGHTGREEN,  GREEN},
	{"CUSTOM",  RESET,       RESET}
};


/* ************************************************************************** */
/* ************************************************************************** */


void	Log::log(size_t typeIdx, const std::ostringstream& oss)
{
	if (typeIdx > 5) typeIdx = 5;
	std::cout	<< logLabelColor(typeIdx) << BOLD    << "[" << LOG_TYPES[typeIdx].name << "] "
				<< logMsgColor(typeIdx)   << REGULAR << oss.str() << std::endl;
}

void	Log::log(size_t typeIdx, const std::ostringstream& label, const std::ostringstream& oss)
{
	if (typeIdx > 5) typeIdx = 5;
	std::cout	<< logLabelColor(typeIdx) << BOLD    << "[" << label.str() << "] "
				<< logMsgColor(typeIdx)   << REGULAR << oss.str() << std::endl;
}

void	Log::log(const std::ostringstream& label, const std::ostringstream& oss, const char* col1, const char* col2)
{
	std::cout	<< col1 << BOLD    << "[" << label.str() << "] "
				<< col2 << REGULAR << oss.str() << std::endl;
}

void	Log::debug(const std::string msg)
{
	print(LOG_TYPES[DEBUG], LOG_TYPES[DEBUG].name, msg);
}

void	Log::debug(const std::string msg, const size_t val)
{
	print(LOG_TYPES[DEBUG], LOG_TYPES[DEBUG].name, msg + std::to_string(val));
}

void	Log::info(const std::string msg)
{
	print(LOG_TYPES[INFO], LOG_TYPES[INFO].name, msg);
}

void	Log::info(const std::string label, const std::string msg)
{
	print(LOG_TYPES[INFO], label, msg);
}

void	Log::warning(const std::string msg)
{
	print(LOG_TYPES[WARNING], LOG_TYPES[WARNING].name, msg);
}

void	Log::warning(const std::string label, const std::string msg)
{
	print(LOG_TYPES[WARNING], label, msg);
}

void	Log::error(const std::string msg)
{
	print(LOG_TYPES[ERROR], LOG_TYPES[ERROR].name, msg);
}

void	Log::error(const std::string label, const std::string msg)
{
	print(LOG_TYPES[ERROR], label, msg);
}

void	Log::success(const std::string msg)
{
	print(LOG_TYPES[SUCCESS], LOG_TYPES[SUCCESS].name, msg);
}

void	Log::success(const std::string label, const std::string msg)
{
	print(LOG_TYPES[SUCCESS], label, msg);
}

void	Log::msg(const std::string label, const std::string msg, const std::string col1, const std::string col2)
{
	std::cerr	<< col1 << BOLD << label 
				<< col2 << REGULAR << msg
				<< RESET << std::endl;
}

void	Log::raw(const std::string msg, size_t split)
{
	#if ENABLE_LOG && PRINT_RAW
		std::cerr	<< DEFAULT << BOLD << "[RAW]";
		std::cerr	<< std::hex << std::setfill('0');

		size_t c = 0;

		for (auto it = msg.begin(); it < msg.end(); ++it)
		{
			std::isprint(*it)	? std::cerr << DEFAULT	: std::cerr << LIGHTMAGENTA;
			(c == 0)			? std::cerr << "\n"		: std::cerr << " ";
			std::cerr << std::setw(2) << static_cast<unsigned int>(*it);
			if (split == 0)
				(*it == '\n') ? c = 0 : ++c;
			else
				(*it == '\n' || c == split - 1) ? c = 0 : ++c;
		}
		std::cerr << std::endl;
	#else
		(void)msg;
		(void)split;
	#endif
}

const char*	Log::logLabelColor(size_t typeIdx)
{
	return LOG_TYPES[typeIdx].col1;
}
const char*	Log::logMsgColor(size_t typeIdx)
{
	return LOG_TYPES[typeIdx].col2;
}

void	Log::print(const t_type type, const std::string label, const std::string msg)
{
	std::cerr	<< type.col1 << BOLD << "[" << label << "] "
				<< type.col2 << REGULAR << msg
				<< RESET << std::endl;
}
