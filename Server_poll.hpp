/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:17:32 by jingwu            #+#    #+#             */
/*   Updated: 2025/04/25 13:53:19 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <unordered_map> //  don’t care about the order and want better performance
#include <regex>
#include <vector>
#include <stdexcept>
#include <netinet/in.h> // for struct sockaddr_in
#include <poll.h>
#include <signal.h>
#include <cstring> //for memset

// #include <unistd.h> // for sleep() for testing,



#define SPECIAL_CHARS "!@#$%^&*()-_=+[]{}|;:'\",.<>?/\\~`"
#define PASSWORD_RULE "Allow contain:\n1.Letters\n2.Digits\n3.Characters in\"!@#$%^&*()-_=+[]{}|;:'\",.<>?/\\~`\""

class Client;
class Channel;

class Server{
	public:
		Server(std::string port, std::string password);
		~Server();

		void	startServer();

	private:
		// Private attributes
		int					serv_port_;
		std::string			serv_passwd_;
		static Server*		server_;
		int					serv_fd_;
		struct sockaddr_in	serv_addr_;
		std::unordered_map<int, Client> clients_; // the key is client socket (client_fd)
		// std::unordered_map<int, Channel> channels_;
		std::vector<struct pollfd>	poll_fds_; // using for saving the clients' fds

		static volatile sig_atomic_t	keep_running_; // internal flag

		// private functions
		Server() = delete;
		Server(const Server&) = delete;
		Server& operator=(const Server&) = delete;

		void	setupSignalHandlers();
		static void	signalHandler(int signum);
		void	setupServSocket();
		void	acceptNewClient();
		void	processDataFromClient(size_t idx);
};

#include "Logger.hpp"
#include "Client.hpp"
// #include "Channel.hpp"
