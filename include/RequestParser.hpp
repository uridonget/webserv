/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 21:10:04 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/03 18:17:58 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

#include "Library.hpp"
#include "llParser.hpp"
<<<<<<< HEAD

class RequestParser {

	private:
		static int trie[10][26];
		HttpRequest request;

	public:
		RequestParser() {};
		~RequestParser() {};

		static const size_t npos = 18446744073709551615UL;

		size_t kmp(std::vector<char> &buf, std::string &target, size_t start);
		size_t findEnd(std::vector<char> &buf, char *append, size_t size);
		size_t checkEnd(std::vector<char> &buf, char *append, size_t size, size_t & endHeader);
		struct HttpRequest requestParsing(std::vector<char> fullRequest, size_t endIndex, size_t & endHeader);
		void setBody(HttpRequest & request, std::vector<char> & buf, size_t & endHeader, size_t & endIndex);
=======
>>>>>>> 96092613a3aa9553f841c45c77c7753d66017939

};

#endif
