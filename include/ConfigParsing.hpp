/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParsing.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 15:11:00 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/25 16:10:46 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSING_HPP
# define CONFIG_PARSING_HPP

#include "../include/Library.hpp"
#include "../include/ServerConfig.hpp"

class ServerConfig;

class ConfigParsing {
	private:
		std::vector<std::string> configStrings;
		std::vector<ServerConfig> ServerConfigs;

	public:
		ConfigParsing();
		~ConfigParsing();
		void removeComment(std::string & fileInput);
		void removeEmptyLine(std::string & fileInput);
		void insertServerConfig(int start, std::string &fileInput);
		void splitServer(std::string & fileInput);
		
		void setServerConfig();
		std::vector<ServerConfig> getServerConfigs();
};

#endif



