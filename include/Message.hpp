/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:27:38 by heolee            #+#    #+#             */
/*   Updated: 2024/07/11 19:46:01 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include "Buffer.hpp"

class Message : public Buffer {
	private:
		bool	headerFlag;
		size_t	headerEnd;
		size_t	contentLength; // npos : 없음. 그 외 : 찾았음.
		bool	chunkFlag;
		size_t	chunkStart; // chunk 일 때 버퍼를 어디서부터 읽어야하는지에 대한 변수

	public:
		Message(int fd);
		~Message();
		void setHeaderFlag(bool flag);
		const bool getHeaderFlag() const;
		void setHeaderEnd(size_t pos);
		const size_t getHeaderEnd() const;
		void setContentLength(size_t len);
		const size_t getContentLength() const;
		void setChunkFlag(bool flag);
		const bool getChunkFlag() const;
		void setChunkStart(size_t pos);
		const size_t getChunkStart() const;

		int whoAmI();

};

#endif



