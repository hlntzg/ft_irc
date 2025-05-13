/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 09:15:08 by arissane          #+#    #+#             */
/*   Updated: 2025/05/13 14:15:23 by jingwu           ###   ########.fr       */
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

void Message::initCommandHandlers(){
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
    return true;
}

bool Message::handleNICK(){
    return true;
}

bool Message::handleTOPIC(){
    return true;
}

bool Message::handleUSER(){
    return true;
}

bool Message::handlePRIVMSG(){
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
    for (const std::string& param : parameters_){
        if (!param.empty() && param[0] == '#') {
            msg_channels_.push_back(param.substr(1));
        } else {
            msg_users_.push_back(param);
        }
    }
    return true;
}

bool Message::handleJOIN(){
    for (const std::string& param : parameters_){
        if (!param.empty() && param[0] == '#') {
            msg_channels_.push_back(param.substr(1));
        } else {
            passwords_.push_back(param);
        }
    }
    return true;
}

bool Message::handleQUIT(){
    return true;
}

bool Message::handleINVITE(){
    for (const std::string& param : parameters_){
        if (!param.empty() && param[0] == '#') {
            msg_channels_.push_back(param.substr(1));
        } else {
            msg_users_.push_back(param);
        }
    }
    return true;
}

bool Message::handleMODE(){
    msg_channels_.push_back(parameters_[0].substr(1));
    parameters_[0] = parameters_[0].substr(1);
    return true;
}

bool Message::handleKICK(){
    for (size_t i = 1; i < parameters_.size(); ++i){
        if (parameters_[i][0] == '#') {
            msg_channels_.push_back(parameters_[i].substr(1));
        } else {
            msg_users_.push_back(parameters_[i]);
        }
    }
    return true;
}

bool Message::validateParameters(const std::string& command){
    auto it = command_handlers_.find(command);
    if (it != command_handlers_.end()){
        return it->second();
    }
    Logger::log(Logger::ERROR, "Unknown command: " + command);
    return false;
}

/**
 * Transforms the entire message received into a istringstream and extracts
 * one by one all the parts of it separated by spaces, then further separates each
 * word by commas if any are found. The command is saved in thecmd_string_ and the
 * trailing message in msg_trailing, everything else is pushed to the parameters_ vector
 * and then validated separately for each command.
 */
bool Message::parseMessage(){
    std::istringstream input_stream(whole_msg_);
    std::string command;
    std::string     word;

    input_stream >> command;
    cmd_string_ = command;
    cmd_type_ = INVALID;
    while (input_stream >> word){
        if (!word.empty() && word[0] == ':'){
            std::string rest_of_line;
            std::getline(input_stream, rest_of_line);
            //rest_of_line.erase(0, rest_of_line.find_first_not_of(" \t\r\n"));
            if (!rest_of_line.empty()){
            msg_trailing_ = word.substr(1) + rest_of_line;
            }
            else{
            msg_trailing_ = word.substr(1);
            }
            break;
        }
        std::stringstream string_stream(word);
        std::string token;
        while (std::getline(string_stream, token, ',')){
            if (!token.empty()){
                ++number_of_parameters_;
                parameters_.push_back(token);
            }
        }
    }
    if (validateParameters(command) == false){
        Logger::log(Logger::ERROR, "Validation failed");
        return false;
    }
    return true;
}

const std::string& Message::getWholeMessage() const{
    return whole_msg_;
}

int Message::getNumberOfParameters() const{
    return number_of_parameters_;
}

const std::string& Message::getTrailing() const{
	return msg_trailing_;
}

const std::vector<std::string>& Message::getParameters() const{
	return parameters_;
}

const std::vector<std::string>& Message::getUsers() const{
	return msg_users_;
}

const std::vector<std::string>& Message::getChannels() const{
	return msg_channels_;
}

COMMANDTYPE Message::getCommandType() const{
    return cmd_type_;
}

const std::string& Message::getCommandString() const{
	return cmd_string_;
}

const std::vector<std::string>& Message::getPasswords() const{
	return passwords_;
}
