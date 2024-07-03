/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:23:48 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/03 17:59:51 by sangyhan         ###   ########.fr       */
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
		bool 							autoindex;
		std::map<int, std::string>		errorPages; // <errorNum, path>
		std::pair<int, std::string>		redirection; // return ...
		std::vector<Location> 			locationList; // locaiton list
		std::set<METHOD> 				allowedMethods; // allowed method

		unsigned long					clientMaxBodySize;
		
	public:
		ServerConfig();
		~ServerConfig();

		void parseConfig(const std::string& configStr);
		void printConfig();
		size_t getListen();
		const std::string &getRoot() const;
};

#endif