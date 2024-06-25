/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:23:48 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/25 15:49:09 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

#include "Library.hpp"
#include "Location.hpp"

class ServerConfig {
	private:
		size_t							listen; // port
		std::string						serverName; // localhost, 127.0.0.1
		std::string 					root; // root path
		std::string						index;
		std::map<int, std::string>		errorPages; // <errorNum, path>
		std::string						redirection; // return ...
		std::vector<Location> 			locationList; // locaiton list

		unsigned long					clientMaxBodySize;
		
	public:
		ServerConfig();
		~ServerConfig();

		void parseConfig(const std::string& configStr);
		void printConfig();
};

#endif