/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 10:49:19 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/25 16:13:42 by haejeong         ###   ########.fr       */
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
		std::map<int, Server> 				serverList; // <port, Server>
		std::map<int, std::vector<char> > 	clients;
		int kq;

	public:
		Webserv();
		~Webserv();

		void makeServerConfigStringList(const std::string & configPath);
		std::vector<ServerConfig> getServerConfigs();
		void makeServerList();
};

#endif