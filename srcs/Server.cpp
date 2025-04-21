/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 12:38:40 by jingwu            #+#    #+#             */
/*   Updated: 2025/04/21 14:28:24 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::string serv_pass, int serv_port):serv_pass_(serv_pass),
serv_port_(serv_port){
}

Server::Server(const Server& o):serv_pass_(o.serv_pass_),
serv_port_(o.serv_port_){
}

Server& Server::operator=(const Server& o){
	if (this != &o){
		serv_pass_ = o.serv_pass_;
		serv_port_ = o.serv_port_;
	}
	return *this;
}

Server*	Server::server_ = nullptr;

Server::~Server(){
	for (auto&	ch : channels_){
		ch.second.shutDownChannel();
	}
}

/**
 * 1. Stages for Server
 * 	The server is created using the following steps:
 * 		1) Socket Createtion(using socket());
 * 		2) Setsockopt(set socket option, using setsockopt());
 * 			This helps in manipulating options for the socket referred by the file
 * 			descriptor sockfd. This is completely optional, but it helps in reuse
 * 			of address and port. Prevents error such as: “address already in use”.
 * 		3) Bind(using bind());
 * 		4) Listen(using listen());
 * 		5) Accept(using accept());
 *
 * 2. What is socket()?
 * The socket() function is used in network programming to create a communication
 * endpoint, known as a socket.
 *
 * Syntax:  int socket(int domain, int type, int protocol);
 * It returns an integer file descriptor (like a handle) that represents the socket.
 *
 *  Parameter breakdown:
 *  	1. domain — Communication domain (or address family):
 * 			- AF_INET: IPv4 Internet protocols
 * 			- AF_INET6: IPv6 Internet protocols
 * 			- AF_UNIX: Local communication (between processes on the same machine)
 * 		2. type — Communication type:
 * 			- SOCK_STREAM: Stream-based (TCP) — reliable, connection-oriented
 * 			- SOCK_DGRAM: Datagram-based (UDP) — fast, connectionless
 * 		2. protocol — Usually set to 0 to select the default protocol for the given
 * 		   domain and type (e.g., TCP for SOCK_STREAM).
 *
 *
 *
 *
 */
void	Server::initServer(){
	// 1. Socket createtion
	serv_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_fd_ == -1){
		throw std::runtime_error("Error: failed to create socket for the server");
	}
	// 2. enable port reuse
	int opt = 1;
	// pass the value of opt to "const void* optval", in this case it is "SO_REUSEADDR"
	// or "SO_REUSEPORT", when the value is 1, it means turn it on; when the value is '0'
	// it means turn it off
	if (setsockopt(serv_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt)) < 0){
		throw std::runtime_error("Error: setsockopt");
	}
	// 3. Bind
	serv_addr_.sin_family = AF_INET;
	serv_addr_.sin_addr.s_addr = INADDR_ANY;
	serv_addr_.sin_port = htons(this->serv_port_);
	if (bind(serv_fd_, (sockaddr*)&serv_addr_, sizeof(serv_addr_)) < 0){
		throw std::runtime_error("Error: bind failed");
	}
}

void	Server::StartServer(){
	initServer();
}
