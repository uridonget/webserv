/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pipe.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/19 17:58:34 by sangyhan          #+#    #+#             */
/*   Updated: 2024/08/20 15:23:35 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPE_HPP
# define PIPE_HPP

#include <string>
#include <unistd.h>
#include "Buffer.hpp"

class Pipe : public Buffer {
	private:
		int serverFd;
		int inputFd;
		bool error;
		bool procEnd;
		bool readEnd;
		pid_t pid;
		bool inClosed;
		bool outClosed;

	public:
		Pipe(int inputFd, int outputFd);
		~Pipe();

		void closeInput();
		void closeOutput();
		int whoAmI();
		const bool &getError();
		void setError(bool code);
		const int &getServerFd();
		virtual int autoWrite(size_t size);
		const pid_t &getPid();
		const int &getInputFd();
		void setPid(pid_t pid);
		void setServerFd(int fd);
		bool End();
		void setProcEnd(bool code);
		void setReadEnd(bool code);
};

#endif
