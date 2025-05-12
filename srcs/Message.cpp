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
#include "Logger.hpp"
#include <string>
#include <sstream>

Message::Message(std::string& message) : whole_msg_(message), number_of_parameters_(0){
    initCommandHandlers();
}

Message::~Message(){
}

void Message::initCommandHandlers() {
    command_handlers_ = {
        {"PASS",   [this](){ cmd_type_ = PASS; return handlePASS(); }},
        {"NICK",   [this](){ cmd_type_ = NICK; return handleNICK(); }},
        {"TOPIC",  [this](){ cmd_type_ = TOPIC; return handleTOPIC(); }},
        {"USER",   [this](){ cmd_type_ = USER; return handleUSER(); }},
        {"PRIVMSG",[this](){ cmd_type_ = PRIVMSG; return handlePRIVMSG(); }},
        {"PART",   [this](){ cmd_type_ = PART; return handlePART(); }},
        {"JOIN",   [this](){ cmd_type_ = JOIN; return handleJOIN(); }},
        {"QUIT",   [this](){ cmd_type_ = QUIT; return handleQUIT(); }},
        {"INVITE", [this](){ cmd_type_ = INVITE; return handleINVITE(); }},
        {"MODE",   [this](){ cmd_type_ = MODE; return handleMODE(); }},
        {"KICK",   [this](){ cmd_type_ = KICK; return handleKICK(); }}
    };
}

bool Message::handlePASS(){
    if (parameters_.size() != 1) {
        Logger::log(Logger::ERROR, "PASS requires exactly one parameter.");
        return false;
    }
    return true;
}

bool Message::handleNICK(){
    if (parameters_.size() != 1) {
        Logger::log(Logger::ERROR, "NICK requires exactly one parameter.");
        return false;
    }
    return true;
}

bool Message::handleTOPIC(){
    if (parameters_.size() > 1) {
        Logger::log(Logger::ERROR, "TOPIC requires exactly one parameter or none.");
        return false;
    }
    return true;
}

bool Message::handleUSER(){
    if (parameters_.size() != 3) {
        Logger::log(Logger::ERROR, "USER command requires exactly 3 parameters.");
        return false;
    }
    if (msg_trailing_.empty()) {
        Logger::log(Logger::ERROR, "USER command requires a trailing message");
        return false;
    }
    return true;
}

bool Message::handlePRIVMSG(){
    if (parameters_.empty()){
        Logger::log(Logger::ERROR, "PRIVMSG requires at least one parameter.");
        return false;
    }
    if (msg_trailing_.empty()){
        Logger::log(Logger::ERROR, "PRIVMSG no trailing message.");
        return false;
    }
    for (const std::string& param : parameters_){
        if (!param.empty() && param[0] == '#') {
            msg_channels_.push_back(param.substr(1));
        } else {
            msg_users_.push_back(param);
        }
    }
    return true;
}

bool Message::handlePART(){
    if (parameters_.size() != 1) {
        Logger::log(Logger::ERROR, "PART requires exactly one channel parameter.");
        return false;
    }
    if (parameters_[0].empty() || parameters_[0][0] != '#') {
        Logger::log(Logger::ERROR, "PART channel must begin with '#'.");
        return false;
    }
    msg_channels_.push_back(parameters_[0].substr(1));
    parameters_[0] = parameters_[0].substr(1);
    return true;
}

bool Message::handleJOIN(){
    if (parameters_.empty()) {
        Logger::log(Logger::ERROR, "JOIN requires at least one parameter.");
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
            Logger::log(Logger::ERROR, "JOIN requires all parameters to begin with '#' or exactly one channel and one key.");
            return false;
        }
        msg_channels_.push_back(parameters_[i].substr(1));
        parameters_[i] = parameters_[i].substr(1);
    }
    return true;
}

bool Message::handleQUIT(){
    if (!parameters_.empty()) {
        Logger::log(Logger::ERROR, "QUIT must not have parameters before trailing message.");
        return false;
    }
    return true;
}

bool Message::handleINVITE(){
    if (parameters_.size() < 2) {
        Logger::log(Logger::ERROR, "INVITE requires at least one user and one channel.");
        return false;
    }

    for (const std::string& param : parameters_) {
        if (!param.empty() && param[0] == '#') {
            msg_channels_.push_back(param.substr(1));
        } else {
            msg_users_.push_back(param);
        }
    }

    if (msg_users_.empty() || msg_channels_.empty()) {
        Logger::log(Logger::ERROR, "INVITE must include at least one user and one channel.");
        return false;
    }

    if (msg_channels_.size() > 1 && msg_users_.size() > 1) {
        Logger::log(Logger::ERROR, "INVITE can only have multiple users OR multiple channels, not both.");
        return false;
    }
    return true;
}

bool Message::handleMODE(){
    if (parameters_.size() < 2) {
        Logger::log(Logger::ERROR, "MODE should contain more than one parameter");
        return false;
    }
    if (parameters_[0][0] != '#') {
        Logger::log(Logger::ERROR, "MODE first parameter must be a channel name");
        return false;
    }
    if (!msg_trailing_.empty()) {
        Logger::log(Logger::ERROR, "MODE doesn't accept a trailing message");
        return false;
    }
    msg_channels_.push_back(parameters_[0].substr(1));
    parameters_[0] = parameters_[0].substr(1);
    return true;
}

bool Message::handleKICK(){
    if (parameters_.size() < 2) {
        Logger::log(Logger::ERROR, "KICK requires at least one channel and one user.");
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
        Logger::log(Logger::ERROR, "KICK must include at least one channel and one user.");
        return false;
    }

    bool valid = false;
    if (msg_channels_.size() == 1 || msg_users_.size() == 1) {
        valid = true;
    } else if (msg_channels_.size() == msg_users_.size()) {
        valid = true;
    }
    if (!valid) {
        Logger::log(Logger::ERROR, "Invalid KICK: mismatch in number of channels and users.");
        return false;
    }
    return true;
}

bool Message::validateParameters(const std::string& command) {
    auto it = command_handlers_.find(command);
    if (it != command_handlers_.end()) {
        return it->second();
    }
    Logger::log(Logger::ERROR, "Unknown command: " + command);
    return false;
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
        std::stringstream string_stream(word);
        std::string token;
        while (std::getline(string_stream, token, ',')) {
            if (!token.empty()) {
                std::cout << "Adding \"" << token << "\" to parameters_" << std::endl;
                ++number_of_parameters_;
                parameters_.push_back(token);
            }
        }
    }
    if (validateParameters(command) == false){
        Logger::log(Logger::ERROR, "Validation failed");
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

COMMANDTYPE Message::getCommandType() const {
    return cmd_type_;
}

const std::string& Message::getCommandString() const {
	return cmd_string_;
}
