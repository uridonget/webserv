/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParsing.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 15:11:00 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/24 15:48:03 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSING_HPP
# define CONFIG_PARSING_HPP

#include "../include/Library.hpp"

class ConfigParsing {
	private:
		std::vector<std::string> serverConfigs;

	public:
		ConfigParsing();
		~ConfigParsing();
		void removeComment(std::string & fileInput);
		void removeEmptyLine(std::string & fileInput);
		void checkComment(std::string & fileInput);
};

#endif



