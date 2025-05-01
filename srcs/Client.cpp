/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 12:59:33 by jingwu            #+#    #+#             */
/*   Updated: 2025/05/01 13:56:01 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : socket_fd_(0), isRegistered_(0){}

Client::Client(int fd) : socket_fd_(fd), isRegistered_(0){
	// for testing
	// std::cout << "New client has been create\n";
}

Client&	Client::operator=(const Client& other){
	if (this != & other){
		Client(other.socket_fd_);
	}
	return *this;
}

Client::~Client(){
}


int	Client::getSocketFD() const{
	return socket_fd_;
}

const std::string&	Client::getNick() const{
	return nick_;
}

/**
 * @brief Receive the raw data from socket, filling/saving into receive buffer.
 *
 * @return
 *  True, read successful;
 *  False, some error;
 *
 */
bool	Client::receiveRawData(){
	// remeber we use edge_trigger mode, so we need to receive all the massage at once
}

/**
 * @brief Gets the next CRLF separated(IRC rule) message from the receive raw data
 * and stores it into passed paramenter buffer.
 *
 * @param
 * buffer to store the message in
 *
 * @return
 * true: if successfully get the message
 * false: raw data is empty.
 */
bool	Client::getNextMessage(std::string& buffer){

}

bool	Client::isRegistered(){
	return isRegistered_;
}
