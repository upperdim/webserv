
NAME		=	webserv
CPP			=	c++
CPPFLAGS	=	-Wall -Wextra -Werror -std=c++11#-g -fsanitize=address

VPATH		=	src/ src/utils src/http src/config

SRCS		=	main.cpp \
				EventManager.cpp \
				Server.cpp \
				Connection.cpp Request.cpp Router.cpp FileBufferReader.cpp Response.cpp Validate.cpp \
				AHandler.cpp ErrorHandler.cpp GetHandler.cpp \
				signal.cpp Http.cpp Log.cpp \
				startsEndsWith.cpp trimWhitespaces.cpp \
				Parser.cpp Config.cpp

OBJS_DIR	=	obj
OBJS		=	$(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))

all: $(NAME)
	@echo "\033[92mexecute with: \033[1;92m"./$(NAME)"\033[0m"

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) -Iinc -Iinc/http -Iinc/config $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -Iinc -Iinc/http -Iinc/config -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

t: $(NAME)
	@./$(NAME)

ret: re t