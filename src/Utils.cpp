/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:46:15 by heolee            #+#    #+#             */
/*   Updated: 2024/07/04 17:33:12 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Library.hpp"

void setNonblock(int fd) {
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		throw RuntimeException("fcntl");
	}
}

std::string readFile(const char* filename) {
	std::ifstream file(filename);
	if (!file) {
		return "";
	}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}
