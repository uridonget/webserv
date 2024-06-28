/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Buffer.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:27:05 by heolee            #+#    #+#             */
/*   Updated: 2024/06/28 19:46:12 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUFFER_HPP
# define BUFFER_HPP

#include <vector>

class Buffer {

	protected:
		int fd;
		std::vector<char> buffer;

	public:
		Buffer(int fd);
		virtual ~Buffer();

		Buffer(const Buffer &other);
        Buffer &operator = (const Buffer& other);

		virtual int whoAmI();
		int getFd() const;
		std::vector<char>& getBuffer();


};

#endif




