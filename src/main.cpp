/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 14:57:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/27 16:26:11 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

int main(int ac, char *av[]) {
	Webserv webserv;

	try {
		if (ac != 2)
			throw RuntimeException("Invalid argument number");
		webserv.makeServerConfigStringList(av[1]);
		
		webserv.initKqueue();
		
		webserv.makeServerList();
		
		webserv.connectKqueueToServer();

		webserv.runServers();
		
		
	} catch (const RuntimeException& e) {
		std::cerr << "Error : " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
