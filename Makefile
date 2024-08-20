# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/11 15:42:22 by sangyhan          #+#    #+#              #
#    Updated: 2024/08/20 16:05:43 by sangyhan         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME     = webserv

CXX      = c++
CXXFLAGS = -O2 -Wall -Wextra -Werror -std=c++98

SRCS     = $(wildcard src/*.cpp)
INCS     = $(wildcard include/*.hpp)
OBJDIR   = obj
OBJS     = $(SRCS:src/%.cpp=$(OBJDIR)/%.o)

all: $(OBJDIR) $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: src/%.cpp $(INCS)
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re