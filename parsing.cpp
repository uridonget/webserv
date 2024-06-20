/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haejeong <haejeong@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/20 11:31:32 by haejeong          #+#    #+#             */
/*   Updated: 2024/06/20 12:56:38 by haejeong         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

static std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::map<std::string, std::string> webserv::parseRequest(const std::string & request) {
    std::map<std::string, std::string> env_vars;
    std::vector<std::string> lines = split(request, '\n');
    std::string request_line = lines[0];
    std::vector<std::string> request_parts = split(request_line, ' ');

    // Set REQUEST_METHOD, REQUEST_URI, and SERVER_PROTOCOL
    env_vars["REQUEST_METHOD"] = request_parts[0];
    env_vars["REQUEST_URI"] = request_parts[1];
    env_vars["SERVER_PROTOCOL"] = request_parts[2];

    // Parse headers
    for (size_t i = 1; i < lines.size(); ++i) {
        std::string line = lines[i];
        if (line == "\r" || line.empty()) {
            break;
        }
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 2); // Skip ": "
            // Convert header names to uppercase and replace '-' with '_'
            for (size_t i = 0; i < key.size(); ++i) {
                if (key[i] == '-') {
                    key[i] = '_';
                } else {
                    key[i] = toupper(key[i]);
                }
            }
            env_vars["HTTP_" + key] = value;
        }
    }

    // Set CONTENT_LENGTH and CONTENT_TYPE if present
    if (env_vars.find("HTTP_CONTENT_LENGTH") != env_vars.end()) {
        env_vars["CONTENT_LENGTH"] = env_vars["HTTP_CONTENT_LENGTH"];
    }
    if (env_vars.find("HTTP_CONTENT_TYPE") != env_vars.end()) {
        env_vars["CONTENT_TYPE"] = env_vars["HTTP_CONTENT_TYPE"];
    }

    // Get the body of the request
    std::string body;
    size_t body_pos = request.find("\r\n\r\n");
    if (body_pos != std::string::npos) {
        body = request.substr(body_pos + 4);
    }
    env_vars["BODY"] = body;

    return env_vars;
}

std::vector<char*> webserv::createEnvp(const std::map<std::string, std::string> &env_vars) {
    std::vector<char*> envp;
    for (std::map<std::string, std::string>::const_iterator it = env_vars.begin(); it != env_vars.end(); ++it) {
        std::string env_string = it->first + "=" + it->second;
        char* env_cstr = new char[env_string.size() + 1];
        std::strcpy(env_cstr, env_string.c_str());
        envp.push_back(env_cstr);
    }
    envp.push_back(NULL);
    return envp;
}
