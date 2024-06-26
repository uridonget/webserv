/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 14:57:20 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/25 17:13:18 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

int main(int ac, char *av[]) {
    Webserv webserv;

    try {
        if (ac != 2)
            throw RuntimeException("Invalid argument number");
        webserv.makeServerConfigStringList(av[1]);
        webserv.makeServerList();
        // 이제 시작이야!
        
    } catch (const RuntimeException& e) {
        std::cout << "Error : " << e.what() << std::endl;
    }
    


    
}
