/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 16:23:48 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/22 13:36:19 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP
# define SERVER_CONFIG_HPP

#include "Library.hpp"
#include "Location.hpp"

class ServerConfig {
    private:
        size_t                          listen;
        std::string                     serverName;
        std::string                     root;
        std::string                     index;
        bool                            autoindex;
        std::map<int, std::string>      errorPages;
        std::pair<int, std::string>     redirection;
        std::set<METHOD>                allowedMethods;
        std::vector<Location>           locationList;
        std::vector<Location>           extensionList;
        unsigned long                   clientMaxBodySize;
        
    public:
        ServerConfig();
        ~ServerConfig();
        void parseConfig(const std::string& configStr);
        void printConfig();
        size_t getListen() const;
        std::string getServerName() const;
        std::string getRoot() const;
        std::string getIndex() const;
        bool getAutoindex() const;
        std::map<int, std::string> & getErrorPages();
        std::pair<int, std::string> getRedirection() const;
        std::vector<Location> getLocationList() const;
        std::vector<Location> getExtensionList() const;
        std::set<METHOD> getAllowedMethods() const;
        unsigned long getClientMaxBodySize() const;
};

#endif