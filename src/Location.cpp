#include "../include/Location.hpp"

Location::Location() {}

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

void Location::setRedirection(std::string redirection) {
	this->redirection = redirection;
}