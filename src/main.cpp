/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 14:57:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/09 14:22:46 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

int main(int ac, char *av[]) {
	Webserv webserv;

	try {
		if (ac != 2)
			throw RuntimeException("Invalid argument number");
		webserv.configurationParsing(av[1]);
		webserv.initKqueue();
		webserv.makeServerList();
		
		webserv.registerServerWithChangeList();

		webserv.runServers();

	} catch (const RuntimeException& e) {
		std::cerr << "Error : " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
