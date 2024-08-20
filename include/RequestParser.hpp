/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 21:10:04 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/25 13:18:05 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

#include "Library.hpp"
#include "llParser.hpp"
#include "Message.hpp"

class RequestParser {

	private:
		static int trie[10][26];
		HttpRequest request;

	public:
		RequestParser() {};
		~RequestParser() {};

		static const size_t npos = 18446744073709551615UL;

		size_t	kmp(std::vector<char> &buf, std::string &target, size_t start);
		size_t	findEnd(std::vector<char> &buf, char *append, size_t size);
		bool	chunkParsing(std::vector<char> &buf, Message *client, bool & end);
		size_t	checkEnd(Message *client, char *append, size_t size, size_t & endHeader);
};

#endif
