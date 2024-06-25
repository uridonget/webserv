/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 18:34:42 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/25 15:37:35 by haejeong         ###   ########.fr       */
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
		bool 							autoindex;
		std::string						redirection; // return ...
		
	public:
		Location();
		~Location();
		
		void setPath(std::string path);
		void setRoot(std::string root);
		void setIndex(std::string index);
		void setAutoIndex(bool autoindex);
		void setRedirection(std::string redirection);
		
};

#endif