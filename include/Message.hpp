/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:27:38 by heolee            #+#    #+#             */
/*   Updated: 2024/08/01 14:13:41 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <list>
#include "Buffer.hpp"

enum CLIENT_STATUS{
	ALIVE,
	WILL_BE_CLOSE,
	CLOSE,
};

class Message : public Buffer {
	private:
		enum CLIENT_STATUS status;
		bool	headerFlag;
		size_t	headerEnd;
		size_t	contentLength; // npos : 없음. 그 외 : 찾았음.
		bool	chunkFlag;
		size_t	chunkStart; // chunk 일 때 버퍼를 어디서부터 읽어야하는지에 대한 변수
		std::vector<char> chunkBuffer;
		std::list<Buffer *> resourceList;

	public:
		Message(int fd);
		~Message();
		void clearFlag();
		void setHeaderFlag(bool flag);
		const bool &getHeaderFlag() const;
		void setHeaderEnd(size_t pos);
		const size_t &getHeaderEnd() const;
		void setContentLength(size_t len);
		const size_t &getContentLength() const;
		void setChunkFlag(bool flag);
		const bool &getChunkFlag() const;
		void setChunkStart(size_t pos);
		const size_t &getChunkStart() const;
		void addResource(Buffer *buf);
		void deleteResource(Buffer *buf);
		const std::list<Buffer *> &getResourceList() const;
		std::vector<char> & getChunkBuffer();
		const enum CLIENT_STATUS &getStatus();
		void setStatus(enum CLIENT_STATUS status);
		int whoAmI();

};

#endif



