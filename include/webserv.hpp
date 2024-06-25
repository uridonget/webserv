#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "parser.hpp"
#include "server.hpp"

class webserv {
	private:
		parser parser;
		std::vector<server> serverList;
		int kq; // kqueue

	public:
		webserv();
		~webserv();
		

		
};

#endif