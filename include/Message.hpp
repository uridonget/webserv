/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:27:38 by heolee            #+#    #+#             */
/*   Updated: 2024/07/08 21:32:39 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include "Buffer.hpp"

class Message : public Buffer {
	private:
		bool headerFlag;
		size_t headerEnd;

	public:
		Message(int fd);
		~Message();
		void setHeaderFlag(bool flag);
		const bool getHeaderFlag() const;
		void setHeaderEnd(size_t pos);
		const bool getHeaderEnd() const;
		int whoAmI();

};

#endif



