/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arissane <arissane@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 09:15:08 by arissane          #+#    #+#             */
/*   Updated: 2025/05/09 09:15:15 by arissane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include <string>
#include <sstream>

Message::Message(std::string& message) : whole_msg_(message), number_of_parameters_(0){
}

Message::~Message(){
}


bool Message::validateParameters(const std::string& command) {
    if (command == "PASS") {
	cmd_type_ = PASS;
        if (parameters_.size() != 1) {
            std::cout << "PASS requires exactly one parameter." << std::endl;
            return false;
        }
    }
    else if (command == "NICK") {
	cmd_type_ = NICK;
        if (parameters_.size() != 1) {
            std::cout << "NICK requires exactly one parameter." << std::endl;
            return false;
        }
    }
    else if (command == "TOPIC") {
	cmd_type_ = TOPIC;
        if (parameters_.size() > 1) {
            std::cout << "TOPIC requires exactly one parameter or none." << std::endl;
            return false;
        }
    }
    else if (command == "USER") {
	cmd_type_ = USER;
        if (parameters_.size() != 3) {
            std::cout << "USER command requires exactly 3 parameters." << std::endl;
            return false;
        }
        if (msg_trailing_.empty()) {
            std::cout << "USER command requires a trailing message" << std::endl;
            return false;
        }
    }
    else if (command == "PRIVMSG") {
	cmd_type_ = PRIVMSG;
        if (parameters_.size() != 1) {
            std::cout << "PRIVMSG requires exactly one parameter." << std::endl;
            return false;
        }
        if (msg_trailing_.empty())
        {
            std::cout << "PRIVMSG no trailing message." << std::endl;
            return false;
        }
    }
    else if (command == "PART") {
	cmd_type_ = PART;
        if (parameters_.size() != 1) {
            std::cout << "PART requires exactly one channel parameter." << std::endl;
            return false;
        }
        if (parameters_[0].empty() || parameters_[0][0] != '#') {
            std::cout << "PART channel must begin with '#'." << std::endl;
            return false;
        }
        msg_channels_.push_back(parameters_[0].substr(1));
        parameters_[0] = parameters_[0].substr(1);
    }
    else if (command == "JOIN") {
	cmd_type_ = JOIN;
        if (parameters_.empty()) {
            std::cout << "JOIN requires at least one parameter." << std::endl;
            return false;
        }

        if (parameters_.size() == 2 && parameters_[0][0] == '#' && parameters_[1][0] != '#') {
            msg_channels_.push_back(parameters_[0].substr(1));
            std::string user = parameters_[1];
            msg_users_.push_back(parameters_[1]);
            parameters_[0] = parameters_[0].substr(1);
            return true;
        }
        for (size_t i = 0; i < parameters_.size(); ++i){
            if (parameters_[i][0] != '#'){
                std::cout << "JOIN requires all parameters to begin with '#' or exactly one channel and one key." << std::endl;
                return false;
            }
            msg_channels_.push_back(parameters_[i].substr(1));
            parameters_[i] = parameters_[i].substr(1);
        }
    }
    else if (command == "QUIT") {
	cmd_type_ = QUIT;
        if (!parameters_.empty()) {
            std::cout << "QUIT must not have parameters before trailing message." << std::endl;
            return false;
        }
    }
    else if (command == "INVITE") {
	cmd_type_ = INVITE;
        if (parameters_.size() < 2) {
            std::cout << "INVITE requires at least one user and one channel." << std::endl;
            return false;
        }
    
        std::vector<std::string> users;
        std::vector<std::string> channels;
    
        for (const std::string& param : parameters_) {
            if (!param.empty() && param[0] == '#') {
                channels.push_back(param.substr(1));
            } else {
                users.push_back(param);
            }
        }
    
        if (users.empty() || channels.empty()) {
            std::cout << "INVITE must include at least one user and one channel." << std::endl;
            return false;
        }
    
        if (channels.size() > 1 && users.size() > 1) {
            std::cout << "INVITE can only have multiple users OR multiple channels, not both." << std::endl;
            return false;
        }
        msg_channels_ = channels;
        msg_users_ = users;
    }
    else if (command == "MODE") {
	cmd_type_ = MODE;
        if (parameters_.size() < 2) {
            std::cout << "MODE should contain more than one parameter" << std::endl;
            return false;
        }
        if (parameters_[0][0] != '#') {
            std::cout << "MODE first parameter must be a channel name" << std::endl;
            return false;
        }
        if (!msg_trailing_.empty()) {
            std::cout << "MODE doesn't accept a trailing message" << std::endl;
            return false;
        }
        msg_channels_.push_back(parameters_[0].substr(1));
        parameters_[0] = parameters_[0].substr(1);
        for (size_t i = 1; i < parameters_.size(); ++i){
            flags_.push_back(parameters_[i]);
        }
    }
    else if (command == "KICK") {
	cmd_type_ = KICK;
        if (parameters_.size() < 2) {
            std::cout << "KICK requires at least one channel and one user." << std::endl;
            return false;
        }
    
        for (size_t i = 1; i < parameters_.size(); ++i){
            if (parameters_[i][0] == '#') {
                msg_channels_.push_back(parameters_[i].substr(1));
            } else {
                msg_users_.push_back(parameters_[i]);
            }
        }
    
        if (msg_channels_.empty() || msg_users_.empty()) {
            std::cout << "KICK must include at least one channel and one user." << std::endl;
            return false;
        }
    
        bool valid = false;
        if (msg_channels_.size() == 1 || msg_users_.size() == 1) {
            valid = true;
        } else if (msg_channels_.size() == msg_users_.size()) {
            valid = true;
        }
        if (!valid) {
            std::cout << "Invalid KICK: mismatch in number of channels and users." << std::endl;
            return false;
        }
    }
    return true;
}

bool Message::parseMessage(){
    std::cout << "Received message: " << whole_msg_ << std::endl;
    std::istringstream input_stream(whole_msg_);
    std::string command;
    std::string     word;

    input_stream >> command;
    cmd_string_ = command;
    std::cout << "Command == " << command << std::endl;
    while (input_stream >> word) {
        if (!word.empty() && word[0] == ':') {
            std::string rest_of_line;
            std::getline(input_stream, rest_of_line);
            msg_trailing_ = word.substr(1) + rest_of_line;
            std::cout << "Trailing message: " << msg_trailing_ << std::endl;
            std::cout << "number of parameters == " << number_of_parameters_ << std::endl;
            break;
        }
        std::cout << "Adding \"" << word << "\" to parameters_" << std::endl;
        ++number_of_parameters_;
        parameters_.push_back(word);
    }
    if (validateParameters(command) == false){
        std::cout << "Validation failed" << std::endl;
        return false;
    }
    std::cout << "Success" << std::endl;
    return true;
}

const std::string& Message::getWholeMessage() const{
    return whole_msg_;
}

int Message::getNumberOfParameters() const{
    return number_of_parameters_;
}

const std::string& Message::getTrailing() const {
	return msg_trailing_;
}

const std::vector<std::string>& Message::getParameters() const {
	return parameters_;
}

const std::vector<std::string>& Message::getUsers() const {
	return msg_users_;
}

const std::vector<std::string>& Message::getChannels() const {
	return msg_channels_;
}

const std::vector<std::string>& Message::getFlags() const {
	return flags_;
}

COMMANDTYPE Message::getCommandType() const {
    return cmd_type_;
}

const std::string& Message::getCommandString() const {
	return cmd_string_;
}
