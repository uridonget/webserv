/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   llParser.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
<<<<<<< HEAD
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 13:37:19 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/02 16:50:18 by haejeong         ###   ########.fr       */
=======
/*   By: sangyhan <sangyhan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 13:37:19 by sangyhan          #+#    #+#             */
/*   Updated: 2024/07/03 18:13:46 by sangyhan         ###   ########.fr       */
>>>>>>> 96092613a3aa9553f841c45c77c7753d66017939
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

<<<<<<< HEAD
class llParser{
	public:
		llParser(std::vector<char> buffer, size_t endHeader);
		HttpRequest parse();

	private:
		std::istringstream stream;
		char currentChar;

		llParser() {};
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
=======
class llParser
{
public:
    llParser(std::vector<char> buffer, size_t endHeader);
    HttpRequest parse();

private:
    std::istringstream stream;
    char currentChar;

    llParser() {};
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
>>>>>>> 96092613a3aa9553f841c45c77c7753d66017939
