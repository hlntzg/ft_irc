/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 12:59:33 by jingwu            #+#    #+#             */
/*   Updated: 2025/04/25 13:32:16 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : socket_fd_(0){}

Client::Client(int fd) : socket_fd_(fd){
	// for testing
	std::cout << "New client has been create\n";
}

Client&	Client::operator=(const Client& other){
	if (this != & other){
		Client(other.socket_fd_);
	}
	return *this;
}

Client::~Client(){
}
