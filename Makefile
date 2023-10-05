NAME = webserv
CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIR := ./src
SRC := main.cpp

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
