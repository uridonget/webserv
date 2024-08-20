/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:27:21 by heolee            #+#    #+#             */
/*   Updated: 2024/08/09 20:08:44 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_HPP
# define FILE_HPP

#include <string>
#include <unistd.h>
#include "Buffer.hpp"

class File : public Buffer {
	private:
		std::string filename;
		int serverFd;
		bool error;

	public:
		File(int fd);
		File(int fd, std::string filename);
		~File();

		int whoAmI();
		const int &getServerFd();
		void setServerFd(int fd);
		const std::string &getFilename() const;
		void setError(bool code);
		const bool &getError();
};

#endif



