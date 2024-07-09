# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/12 10:20:27 by haejeong          #+#    #+#              #
#    Updated: 2024/07/09 13:07:01 by sangyhan         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	 = webserv

CXX		 = c++

CXXFLAGS = -g # -Wall -Wextra -Werror -std=c++98

SRCS	 = $(wildcard src/*.cpp)

INCS	 = $(wildcard include/*.hpp)

OBJS	 = $(SRCS:.cpp=.o)

all	: $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o : %.cpp $(INCS)
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean 	:
	rm -rf $(OBJS)

fclean  : clean
	rm -rf $(NAME)

re		: fclean all

.PHONY : all clean fclean re
