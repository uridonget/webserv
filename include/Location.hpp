/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 18:34:42 by haejeong          #+#    #+#             */
/*   Updated: 2024/07/09 13:13:20 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "Library.hpp"

class Location {
	private:
		std::string						path; // location /path {}
		std::string 					root; // root path
		std::string						index;
		std::pair<int, std::string>		redirection;
		std::set<METHOD> 				allowedMethods; // allowed method
		
	public:
		Location();
		~Location();
		
		void setPath(std::string path);
		void setRoot(std::string root);
		void setIndex(std::string index);
		void setRedirection(int errorCode, std::string path);
		void setAllowedMethods(std::set<METHOD>);
		
		std::string getPath() const;
		std::string getRoot() const;
		std::string getIndex() const;
		std::pair<int, std::string> getRedirection() const;
		std::set<METHOD> getAllowedMethods() const;

		void showLocation() const;
};

#endif