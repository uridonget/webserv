#include "../include/Location.hpp"

Location::Location()
	: path(""),
	root(""),
	index(""),
	autoindex(false) {	
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

void Location::setAutoIndex(bool autoindex) {
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

std::string Location::getPath() const {
	return (path);
}

std::string Location::getRoot() const {
	return (root);
}

std::string Location::getIndex() const {
	return (index);
}

bool Location::getAutoIndex() const {
	return (autoindex);
}

std::pair<int, std::string> Location::getRedirection() const {
	return (redirection);
}

std::set<METHOD> Location::getAllowedMethods() const {
	return (allowedMethods);
}

void Location::showLocation() const {
	std::cout << "-----------------------\n";
	if (path.length())
		std::cout << "path : " << getPath() << std::endl;
	if (root.length())
		std::cout << "root : " << getRoot() << std::endl;
	if (index.length())
		std::cout << "index : " << getIndex() << std::endl;
	std::cout << "index : ";
	if (getAutoIndex())
		std::cout << "on" << std::endl;
	else
		std::cout << "off" << std::endl;
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
