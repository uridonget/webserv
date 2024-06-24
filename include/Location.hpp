/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 18:34:42 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/24 18:39:01 by haejeong         ###   ########.fr       */
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
		bool 							autoIndex;
		std::string						redirection; // return ...
		
	public:
		Location();
		~Location();
		
};

#endif