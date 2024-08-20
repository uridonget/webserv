/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiParser.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/19 15:19:29 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/19 16:56:06 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIPARSER_HPP
# define CGIPARSER_HPP

#include <cctype>
#include <stdexcept>
#include "Library.hpp"

class CgiParser{
	public:
		CgiParser(const std::vector<char> &buffer, size_t endHeader);
		HttpRequest parse(int &code, std::string &reason);

	private:
		std::istringstream stream;
		char currentChar;
		static const char NULL_CHAR = '\0';
		static const char CR = '\r';
		static const char LF = '\n';
		static const char SP = ' ';

		CgiParser() {};
		void nextChar();
		void consumeSP();
		void consumeNL();
		std::string parseToken();
		std::pair<std::string, std::string> fieldline();
		std::string parseFieldName();
		std::string parseFieldValue();
		void consumeOWS();
		void expect(char expected);
};

#endif
