/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 10:49:19 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/25 15:53:41 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "Server.hpp"
#include "ConfigParsing.hpp"
#include "ServerConfig.hpp"

class Webserv {
	private:
		ConfigParsing						configParsing;
		std::vector<std::string> 			configStrings;
		std::map<int, Server> 				serverList;
		std::map<int, std::vector<char> > 	clients;
		int kq;

	public:
		Webserv();
		~Webserv();

		void makeServerConfigStringList(const std::string & configPath);
};

#endif