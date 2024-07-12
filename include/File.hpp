/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:27:21 by heolee            #+#    #+#             */
/*   Updated: 2024/07/11 15:47:54 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_HPP
# define FILE_HPP

#include <string>
#include "Buffer.hpp"

class File : public Buffer {
	private:
		std::string filename;

	public:
		File(int fd);
		File(int fd, std::string filename);
		~File();

		int whoAmI();
		const std::string &getFilename() const;
};

#endif



