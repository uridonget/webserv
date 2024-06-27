/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Library.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:19:03 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/27 11:48:25 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDES_HPP
# define INCLUDES_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <fcntl.h>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>

#define PORT 8080
#define BUFFER_SIZE 100
#define MAX_BODY_SIZE 30000000
#define READ_END "\r\n\r\n"

class RuntimeException : public std::runtime_error {
public:
    RuntimeException(const std::string& message) 
        : std::runtime_error(message) {}
};

#endif