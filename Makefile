
NAME		=	webserv
CPP			=	c++
CPPFLAGS	=	-Wall -Wextra -Werror -std=c++11 #-g -fsanitize=address

VPATH		=	src/

SRCS		=	main.cpp EventManager.cpp Server.cpp Connection.cpp Request.cpp Response.cpp Validate.cpp signal.cpp trimWhitespaces.cpp StatusCodes.cpp Log.cpp
OBJS		=	$(SRCS:.cpp=.o)

all: $(NAME)
	@echo "\033[92mexecute with: \033[1;92m"./$(NAME)"\033[0m"

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) -Iinc $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -Iinc -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

t: $(NAME)
	@./$(NAME)

ret: re t