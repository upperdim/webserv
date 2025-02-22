
NAME		=	webserv
CPP			=	c++
CPPFLAGS	=	-Wall -Wextra -Werror -std=c++11

VPATH		=	src/

SRCS		=	main.cpp EventLoop.cpp Server.cpp Connection.cpp signal.cpp Log.cpp
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