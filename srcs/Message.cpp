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


bool Message::parseMessage(){
    std::cout << "Received message: " << whole_msg_ << std::endl;
    std::istringstream input_stream(whole_msg_);
    std::string command;
    std::string     word;

    input_stream >> command;
    if (command == "JOIN" || command == "KICK" || command == "INVITE" || command == "TOPIC" || command == "MODE") {
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

        return true;
    }
    else
    {
        std::cout << "No valid command" << std::endl;
        return false;
    }
}

const std::string& Message::getRawMessage() const{
    return whole_msg_;
}

const std::vector<std::string>& Message::getParameters() const{
    return parameters_;
}
