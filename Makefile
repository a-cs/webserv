NAME = webserv
CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck -q ./$(NAME) a

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
		utils.cpp

OBJ_DIR := ./objs
OBJS := $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

INC := ./include

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(FLAGS) -I $(INC) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $@

$(OBJS): | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

valgrind: all
	@$(VALGRIND)

v: valgrind