NAME			= ircserv
CC 				= c++
CFLAGS			= -Wall -Werror -Wextra -std=c++98
FILE_EXTENSION	= .cpp

SRCS			= main.cpp Client.cpp Server.cpp Channel.cpp Other.cpp
 		
################################################################################
#                                  Makefile  objs                              #
################################################################################

SHELL 				:= /bin/bash

OBJS 				= $(patsubst %.cpp, objs/%.o, $(SRCS))

# Couleurs
RED 		= \\033[0;31m
GREEN 		= \\033[0;32m
YELLOW 		= \\033[0;33m
BLUE 		= \\033[0;34m
MAGENTA 	= \\033[0;35m
CYAN 		= \\033[0;36m
WHITE 		= \\033[0;37m
RESET 		= \\033[0m

all:		start $(NAME) end

start:
			@printf "%b" "$(GREEN)"
			@echo "+-----------------------+"
			@echo "|	       		|"
			@echo "|	$(NAME)	|"
			@echo "|	       		|"
			@echo "+-----------------------+"
			@echo "-------------------------"
			

$(NAME):	${OBJS}
			@($(CC) $(CFLAGS) -o $@ ${OBJS})

objs/%.o: 	$/%.cpp
			@mkdir -p $(dir $@)
			@printf "$(GREEN)Compiling %-*s$(RESET)" 20 "$(notdir $<)"
			@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ && printf "$(GREEN) [OK]$(RESET)\n" || printf "$(RED) [FAILED]$(RESET)\n"

end:
			@printf "%b" "$(GREEN)"
			@printf "%b" "$(GREEN)Compilation success$(RESET)\n"
			@printf "%b" "$(GREEN)-------------------------$(RESET)\n"
			@printf "%b" "$(GREEN)$(NAME) [OK]$(RESET)\n"

clean:
			@rm -rf objs
			@printf "%b" "$(RED)DELETE objs$(RESET)\n"
			
fclean:		clean
			@rm -rf $(NAME)
			@printf "%b" "$(RED)DELETE $(NAME)$(RESET)\n"
			
re:			fclean all

.PHONY:		all clean fclean re start end