/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IoMultiplexing.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: heolee <heolee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:46:15 by heolee            #+#    #+#             */
/*   Updated: 2024/06/25 16:46:15 by heolee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// this file is main loop 

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <map>
#include <vector>
#include "../include/Webserv.hpp"

