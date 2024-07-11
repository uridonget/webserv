/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:27:38 by heolee            #+#    #+#             */
/*   Updated: 2024/07/11 15:29:00 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include "Buffer.hpp"

class Message : public Buffer {
	private:
		bool	headerFlag;
		size_t	headerEnd;
		size_t	contentLength; // 0 : 아직 안찾아봄, npos : 찾아봤는데 없음. 그 외 : 찾았음. 

	public:
		Message(int fd);
		~Message();
		void setHeaderFlag(bool flag);
		const bool getHeaderFlag() const;
		void setHeaderEnd(size_t pos);
		const size_t getHeaderEnd() const;
		void setContentLength(size_t len); // content length 저장
		const size_t getContentLength() const; // content length 가져오기
		
		int whoAmI();

};

#endif



