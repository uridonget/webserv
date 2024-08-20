/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Buffer.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:27:05 by heolee            #+#    #+#             */
/*   Updated: 2024/08/20 11:45:58 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUFFER_HPP
# define BUFFER_HPP

#include "Library.hpp"
#include <vector>

class Buffer {

	protected:
		int fd;
		std::vector<char> readBuffer;
		std::vector<char> writeBuffer;
		size_t index;
		char cBuffer[BUFFER_SIZE + 1];

	public:
		Buffer(int fd);
		virtual ~Buffer();

		Buffer(const Buffer &other);
		Buffer &operator = (const Buffer& other);

		virtual int whoAmI() = 0;
		int getFd() const;
		virtual int autoWrite(size_t size);
		bool writeBufEmpty();
		std::vector<char>& getReadBuffer();
		std::vector<char>& getWriteBuffer();

};

#endif




