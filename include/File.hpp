/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   File.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: heolee <heolee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 16:27:21 by heolee            #+#    #+#             */
/*   Updated: 2024/06/25 16:27:22 by heolee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILE_HPP
# define FILE_HPP

#include "Buffer.hpp"

class File : public Buffer {
	private:


	public:
		File(int fd);
		~File();

		int whoAmI();

};

#endif



