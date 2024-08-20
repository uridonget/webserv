/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 18:34:42 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/22 13:37:59 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "Library.hpp"

class Location {
	private:
		std::string						path; 
		std::string 					root; 
		std::string						index;
		bool							autoindex;
		std::map<int, std::string>      errorPages; 
		std::pair<int, std::string>		redirection;
		std::set<METHOD> 				allowedMethods; 
		unsigned long                   clientMaxBodySize;
		
	public:
		Location();
		~Location();
		
		void setPath(std::string path);
		void setRoot(std::string root);
		void setIndex(std::string index);
		void setAutoindex(bool autoindex);
		void setRedirection(int errorCode, std::string path);
		void setAllowedMethods(std::set<METHOD>);
		void setClientMaxBodySize(unsigned int maxBodySize);
		
		std::string getPath() const;
		std::string getRoot() const;
		std::string getIndex() const;
		bool getAutoindex() const;
		std::map<int, std::string> & getErrorPages();
		std::pair<int, std::string> getRedirection() const;
		std::set<METHOD> getAllowedMethods() const;
		unsigned long getClientMaxBodySize() const;

		void showLocation() const;
};

#endif