#include "../include/Location.hpp"

Location::Location()
	: path(""),
	root(""),
	index(""),
	autoindex(false),
	clientMaxBodySize(INT_MAX) {
}

Location::~Location() {}

void Location::setPath(std::string path) {
	this->path = path;
}

void Location::setRoot(std::string root) {
	this->root = root;
}

void Location::setIndex(std::string index) {
	this->index = index;
}

void Location::setAutoindex(bool autoindex) {
	this->autoindex = autoindex;
}

void Location::setRedirection(int errorCode, std::string path) {
	redirection.first = errorCode;
	redirection.second = path;
}

void Location::setAllowedMethods(std::set<METHOD> methods) {
	allowedMethods.clear();
	allowedMethods = methods;
}

void Location::setClientMaxBodySize(unsigned int maxBodySize) {
	this->clientMaxBodySize = maxBodySize;
}

std::string Location::getPath() const {
	return (path);
}

std::string Location::getRoot() const {
	return (root);
}

std::string Location::getIndex() const {
	return (index);
}

bool Location::getAutoindex() const {
	return (autoindex);
}

std::map<int, std::string> & Location::getErrorPages() {
	return errorPages;
}

std::pair<int, std::string> Location::getRedirection() const {
	return (redirection);
}

std::set<METHOD> Location::getAllowedMethods() const {
	return (allowedMethods);
}

unsigned long Location::getClientMaxBodySize() const {
    return (clientMaxBodySize);
}

void Location::showLocation() const {
	std::cout << "-----------------------\n";
	if (path.length())
		std::cout << "path : " << getPath() << std::endl;
	if (root.length())
		std::cout << "root : " << getRoot() << std::endl;
	if (index.length())
		std::cout << "index : " << getIndex() << std::endl;
	if (autoindex) {
		std::cout << "AutoIndex: on" << std::endl;
	} else {
		std::cout << "AutoIndex: off" << std::endl;
	}
	if (getRedirection().first != 0)
		std::cout << "return : " << getRedirection().first << ", " << getRedirection().second << std::endl;
	if (allowedMethods.size()) {
		std::cout << "Allowed Methods : ";
		for (std::set<METHOD>::iterator it = allowedMethods.begin(); it != allowedMethods.end(); it++) {
			if (*it == GET) {
				std::cout << "GET ";
			} else if (*it == POST) {
				std::cout << "POST ";
			} else if (*it == DELETE) {
				std::cout << "DELETE ";
			}
		}
		std::cout << std::endl;
	}
}
