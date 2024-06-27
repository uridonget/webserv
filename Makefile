# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/12 10:20:27 by haejeong          #+#    #+#              #
#    Updated: 2024/06/27 12:57:11 by haejeong         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	 = a.out

CXX		 = c++

CXXFLAGS = -fsanitize=address # -Wall -Wextra -Werror -std=c++98

SRCS	 = $(wildcard src/*.cpp)

INCS	 = $(wildcard include/*.hpp)

OBJS	 = $(SRCS:.cpp=.o)

all	: $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o : %.cpp $(INCS)
	$(CXX) -c $< -o $@

clean 	:
	rm -rf $(OBJS)

fclean  : clean
	rm -rf $(NAME)

re		: fclean all

.PHONY : all clean fclean re
