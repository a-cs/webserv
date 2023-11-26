NAME = webserv
CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck -q ./$(NAME)

SRC_DIR := ./src
SRC := config.cpp \
		epollHandler.cpp \
		error.cpp \
		location.cpp \
		loopHandler.cpp \
		main.cpp \
		parseConfig.cpp \
		request.cpp \
		response.cpp \
		server.cpp \
		cgi.cpp \
		utils.cpp

OBJ_DIR := ./objs
OBJS := $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

INC := ./include

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(FLAGS) -I $(INC) -c $< -o $@

all: hosts $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $@

$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

hosts:
	@if [ ! -e /etc/hosts ]; then \
		sudo touch /etc/hosts; \
	fi
	sudo cp /etc/hosts ./hosts_backup

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)
	sudo rm -rf /etc/hosts
	sudo cp ./hosts_backup /etc/hosts
	rm -rf ./hosts_backup

re: fclean all

valgrind: all
	@$(VALGRIND)

v: valgrind

