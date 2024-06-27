/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: heolee <heolee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:27:38 by heolee            #+#    #+#             */
/*   Updated: 2024/06/25 16:27:39 by heolee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include "Buffer.hpp"

class Message : public Buffer {
	private:


	public:
		Message();
		~Message();

		int whoAmI();

};

#endif



