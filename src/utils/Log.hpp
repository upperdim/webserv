#ifndef LOG_HPP
#define LOG_HPP

#include <string>
#include <sstream>

# define ENABLE_LOG		false
# define PRINT_DEBUG	false
# define PRINT_INFO		false
# define PRINT_WARNING	true
# define PRINT_ERROR	true
# define PRINT_SUCCESS 	false
# define PRINT_MSG		false
# define PRINT_RAW		false

# if ENABLE_LOG

# define LOG(msg) \
	do { std::ostringstream _oss_; _oss_ << msg; Log::log(Log::DEBUG, _oss_); } while(0)
# define LOGT(type, msg) \
	do { std::ostringstream _oss_; _oss_ << msg; Log::log(type, _oss_); } while(0)
# define LOGTL(type, label, msg) \
	do { std::ostringstream _label_, _oss_; _label_ << label; _oss_ << msg; Log::log(type, _label_, _oss_); } while(0)
# define LOGC(label, msg, col1, col2) \
	do { std::ostringstream _label_, _oss_; _label_ << label; _oss_ << msg; Log::log(_label_, _oss_, col1, col2); } while(0)

#  if PRINT_DEBUG
#   define LOG_DEBUG(msg)					Log::debug(msg)
#   define LOG_DEBUG_MV(msg, val)			Log::debug(msg, val)
#  else
#   define LOG_DEBUG(msg)
#   define LOG_DEBUG_MV(msg)
#  endif
#  if PRINT_INFO
#   define LOG_INFO(msg)						Log::info(msg)
#   define LOG_INFO_LM(lab, msg)				Log::info(lab, msg)
#  else
#   define LOG_INFO(msg)
#   define LOG_INFO_LM(lab, msg)
#  endif
#  if PRINT_WARNING
#   define LOG_WARNING(msg)					Log::warning(msg)
#   define LOG_WARNING_LM(lab, msg)			Log::warning(lab, msg)
#  else
#   define LOG_WARNING(msg)
#   define LOG_WARNING_LM(lab, msg)
#  endif
#  if PRINT_ERROR
#   define LOG_ERROR(msg)					Log::error(msg)
#   define LOG_ERROR_LM(lab, msg)			Log::error(lab, msg)
#  else
#   define LOG_ERROR(msg)
#   define LOG_ERROR_LM(lab, msg)
#  endif
#  if PRINT_SUCCESS
#   define LOG_SUCCESS(msg)					Log::success(msg)
#   define LOG_SUCCESS_LM(lab, msg)			Log::success(lab, msg)
#  else
#   define LOG_SUCCESS(msg)
#   define LOG_SUCCESS_LM(lab, msg)
#  endif
#  if PRINT_MSG
#   define LOG_MSG(...)						Log::msg(__VA_ARGS__)
#  else
#   define LOG_MSG(...)
#  endif
#  if PRINT_RAW
#   define LOG_RAW(msg, split)				Log::raw(msg, split)
#  else
#   define LOG_RAW(msg, split)
#  endif
# else
#  define LOG(msg)
#  define LOGT(type, msg)
#  define LOGTL(type, label, msg)
#  define LOGC(label, msg, col1, col2)
#  define LOG_DEBUG(msg)
#  define LOG_DEBUG_MV(msg, val)
#  define LOG_INFO(msg)
#  define LOG_INFO_LM(lab, msg)
#  define LOG_WARNING(msg)
#  define LOG_WARNING_LM(lab, msg)
#  define LOG_ERROR(msg)
#  define LOG_ERROR_LM(lab, msg)
#  define LOG_SUCCESS(msg)
#  define LOG_SUCCESS_LM(lab, msg)
#  define LOG_MSG(lab, msg, col1, col2)
#  define LOG_RAW(msg, split)
# endif

class Log
{
public:
	~Log();

	enum Level {
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		SUCCESS,
		CUSTOM
	};

	static void	log(size_t typeIdx, const std::ostringstream& oss);
	static void	log(size_t typeIdx, const std::ostringstream& label, const std::ostringstream& oss);
	static void	log(const std::ostringstream& label, const std::ostringstream& oss, const char* col1, const char* col2);

	static void	debug(const std::string msg);
	static void	debug(const std::string msg, const size_t val);
	static void	info(const std::string msg);
	static void	info(const std::string label, const std::string msg);
	static void	warning(const std::string msg);
	static void	warning(const std::string label, const std::string msg);
	static void	error(const std::string msg);
	static void	error(const std::string label, const std::string msg);
	static void	success(const std::string msg);
	static void	success(const std::string label, const std::string msg);
	static void	msg(const std::string label, const std::string msg, const std::string col1, const std::string col2);
	static void	raw(const std::string msg, size_t split);

	typedef struct s_type
	{
		const char*	name;
		const char*	col1;
		const char*	col2;
	} t_type;

private:
	Log();
	Log(const Log& orher);
	Log&	operator=(const Log& rhs);

	static const t_type LOG_TYPES[6];

	static const char*	logLabelColor(size_t typeIdx);
	static const char*	logMsgColor(size_t typeIdx);

	static void	print(const t_type type, const std::string label, const std::string msg);
};

#endif
