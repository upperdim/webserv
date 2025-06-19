# Sources
SRC_FOLDERS	=	src \
				src/http \
				src/utils \
				src/config \
				src/network \


VPATH		=	$(SRC_FOLDERS)


SRCS_NO_MAIN=	Config.cpp \
				Token.cpp \
				Lexer.cpp \
				Validator.cpp \
				Throw.cpp \
				Parser.cpp \
				EventManager.cpp \
				Server.cpp \
				Connection.cpp \
				Request.cpp \
				FileBufferReader.cpp \
				Response.cpp \
				Validate.cpp \
				signal.cpp \
				HTTP.cpp \
				Log.cpp \
				trimWhitespaces.cpp \
				readFile.cpp \
				Utils.cpp \


SRCS		=	main.cpp $(SRCS_NO_MAIN)

# Includes
INCLUDES_DIRS	=	$(SRC_FOLDERS)
INCLUDE_FLAG	=	$(addprefix -I, $(INCLUDES_DIRS))

# Compilation
NAME		=	webserv
CPP			=	c++
CPPFLAGS	=	-Wall -Wextra -Werror -std=c++17 #-g -fsanitize=address

OBJS_DIR	=	obj
OBJS		=	$(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))

all: $(NAME)
	@echo "\033[92mexecute with: \033[1;92m"./$(NAME)"\033[0m"

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(INCLUDE_FLAG) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) $(INCLUDE_FLAG) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

t: $(NAME)
	@./$(NAME) default.conf

ret: re t
