# Makeflags
# JOBS ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
# MAKEFLAGS += -j$(JOBS)

# Sources
SRC_FOLDERS	=	src \
				src/http \
				src/utils \
				src/config \
				src/network \
				src/handler  \


VPATH		=	$(SRC_FOLDERS)


SRCS_NO_MAIN=	Config.cpp \
				Token.cpp \
				Lexer.cpp \
				Validator.cpp \
				Throw.cpp \
				Parser.cpp \
				ServerEngine.cpp \
				ServerSocket.cpp \
				ClientConnection.cpp \
				Request.cpp \
				RequestParser.cpp \
				Response.cpp \
				FileBufferReader.cpp \
				RequestHandler.cpp \
				GetHandler.cpp \
				PostHandler.cpp \
				DeleteHandler.cpp \
				HTTP.cpp \
				Log.cpp \
				Utils.cpp \
				CGIHandler.cpp \



SRCS		=	main.cpp $(SRCS_NO_MAIN)

# Includes
INCLUDES_DIRS	=	$(SRC_FOLDERS)
INCLUDE_FLAG	=	$(addprefix -I, $(INCLUDES_DIRS))

# Compilation
NAME		=	webserv
CPP			=	c++
CPPFLAGS	=	-Wall -Wextra -Werror -std=c++17 $(INCLUDE_FLAG) #-g -fsanitize=address

OBJS_DIR	=	obj
OBJS		=	$(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))

all: $(NAME)
	@echo "\033[92mexecute with: \033[1;92m"./$(NAME)"\033[0m"

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

t: $(NAME)
	@./$(NAME) default.conf

ret: re t

debug: CPPFLAGS += -g
debug: $(NAME)

fdebug: fclean
fdebug: CPPFLAGS += -g
fdebug: $(NAME)
