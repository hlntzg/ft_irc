NAME := ircserv
COMPILER := c++
FLAGS := -Wall -Wextra -Werror -std=c++11

#colour define
GREEN := \033[1;32m
PURPLE := \033[0;35m
RED := \033[0;31m
ORANGE := \033[0;91m
BLUE = \033[1;34m
RESET := \033[0;0m

# Directories
SRCS_DIR := srcs
OBJS_DIR := objs
INCLUDE := include

# Sources
SRCS := main.cpp Logger.cpp Server.cpp

#INCLUDE := $(INCLUDE_DIR)/Server.hpp

OBJS := $(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))

# Commands
RM := rm -rf
MKDIR := mkdir -p

# DEBUG/INFO/WARNING/ERROR
LOG_LEVEL := INFO

all: snippet $(NAME)
	@echo "$(BLUE)███████████████████████   Compiling is DONE  ███████████████████████$(RESET)"
	@echo "$(GREEN)$(NAME) has been generated$(RESET)"
	@echo "$(GREEN)Usage: ./ircserv <port> <password>$(RESET)"

head:
	@echo "$(BLUE)███████████████████████ Making ft_irc Server ███████████████████████$(RESET)"

$(NAME): head $(OBJS)
	@$(COMPILER) $(OBJS) -o $@

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp $(INCLUDE)
	@$(MKDIR) $(OBJS_DIR)
	@printf "$(BLUE)█ $(PURPLE)Compiling$(RESET) $<\r\t\t\t\t\t\t\t..."
	@$(COMPILER) -DLOG_LEVEL=$(LOG_LEVEL) $(FLAGS) -I$(INCLUDE) -o $@ -c $<
	@echo "\r\t\t\t\t\t\t\t$(GREEN)      DONE$(BLUE) █$(RESET)"

# Rules for cleant the project
clean:
	@$(RM) $(OBJS_DIR)
	@echo "$(RED)$(OBJS_DIR) have been cleaned$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)$(NAME) has been cleaned$(RESET)"

re: fclean all

# Print Credit
snippet:
	@echo "        $(GREEN)   ___   _____            ___     ___     ___$(RESET)"
	@echo "        $(GREEN)  | __| |_   _|          |_ _|   | _ \   / __|$(RESET)"
	@echo "        $(GREEN)  | _|    | |     ___     | |    |   /  | (__$(RESET)"
	@echo "        $(GREEN) _|_|_   _|_|_   |___|   |___|   |_|_\   \___|$(RESET)"
	@echo "        $(PURPLE)_| \"\"\" |_|\"\"\"\"\"|_|\"\"\"\"\"|_|\"\"\"\"\"|_|\"\"\"\"\"|_|\"\"\"\"\"|$(RESET)"
	@echo "        $(PURPLE) \`-0-0-''\`-0-0-''\`-0-0-''\`-0-0-''\`-0-0-''\`-0-0-''$(RESET)"
	@echo "$(BLUE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"
	@echo " Made by lovely souls: $(ORANGE)Helena Utzig, Anssi Rissanen and Jingjing Wu$(RESET)"
	@echo "$(BLUE)━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━$(RESET)"

.PHONY: all clean fclean re
