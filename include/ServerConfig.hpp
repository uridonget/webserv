/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:23:48 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/24 14:41:36 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

#include "Library.hpp"

class ServerConfig {
	private:
		size_t							listen; // port
		std::string						serverName; // localhost, 127.0.0.1
		std::string 					root; // root path
		unsigned long					clientMaxBodySize;
		std::string						index;
		std::map<int, std::string>		errorPages; // <errorNum, path>
		std::string						redirection; // return ...
		std::vector<ServerConfig> 		chilren; // locaiton
		
	public:
		ServerConfig();
		~ServerConfig();
		
};

#endif