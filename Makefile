CC = c++
FLAGS = -std=c++98 -Wall -Wextra -g #-fsanitize=address #-Werror 
RM = rm -f

NAME = webserv

SRCS = srcs/main.cpp \
	   srcs/Config/Config.cpp \
	   srcs/ServerWeb/ServerWeb.cpp \
	   srcs/ServerWeb/Socket/Socket.cpp \

	

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re