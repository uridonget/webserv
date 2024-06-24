/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 10:49:19 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/24 15:21:06 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "Server.hpp"
#include "ConfigParsing.hpp"

class Webserv {
	private:
		ConfigParsing						configParsing;
		std::vector<std::string> 			serverConfigs;
		std::map<int, Server> 				serverList;
		std::map<int, std::vector<char> > 	clients;
		int kq;

	public:
		Webserv();
		~Webserv();

		void makeServerList(const std::string & configPath);

};

#endif