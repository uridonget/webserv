/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   llParser.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 13:37:19 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/29 13:45:35 by sangyhan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LLPARSER_HPP
# define LLPARSER_HPP

#include <cctype>
#include <stdexcept>
#include "../include/Library.hpp"

const char NULL_CHAR = '\0';
const char CR = '\r';
const char LF = '\n';
const char SP = ' ';

/** Context Free Grammer
 * NULL = "\0"
 * RL = "\r"
 * NL = "\n"
 * CRLF = "\r\n"
 
 * HTTP-message = request-line CRLF field-lines CRLF

 * request-line = METHOD SP TARGET SP VER

 * field-lines = field-line CRLF field-lines | ε

 * field-line = field-name ":" OWS field-value OWS

 * field-value = field-content more-field-content

 * more-field-content = field-content more-field-content | ε

 * field-content = vchar additional-field-content

 * additional-field-content = 1*(SP / HTAB / vchar) vchar | ε

 * vchar = CHAR
*/

class llParser{
	public:
		llParser(const std::vector<char> &buffer, size_t endHeader);
		HttpRequest parse();

	private:
		char currentChar;
		size_t end;
		size_t bufIndex;
		const std::vector<char> &buf;

		void nextChar();
		void consumeSP();
		void consumeCRLF();
		std::string parseToken();
		std::pair<std::string, std::string> fieldline();
		std::string parseFieldName();
		std::string parseFieldValue();
		void consumeOWS();
		void expect(char expected);
};

#endif
