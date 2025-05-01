/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:17:32 by jingwu            #+#    #+#             */
/*   Updated: 2025/05/01 14:43:33 by jingwu           ###   ########.fr       */
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
#include <sys/epoll.h>
#include <signal.h>
#include <cstring> //for memset
#include <fcntl.h>  // for fcntl()
#include <set> // for std::set

class Client;
class Channel;
class Message;


#define SPECIAL_CHARS "!@#$%^&*()-_=+[]{}|;:'\",.<>?/\\~`"
#define PASSWORD_RULE "Allow contain:\n1.Letters\n2.Digits\n3.Characters in\"!@#$%^&*()-_=+[]{}|;:'\",.<>?/\\~`\""

enum COMMANDTYPE{
	PASS,
	NICK,
	USER,
	PRIVMSG,
	JOIN,
	PART,
	OPER,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	QUIT,
	NONE
};


class Server{
	public:
		Server(std::string port, std::string password);
		~Server();

		void	startServer();
		static int		responseToClient(Client& cli, const std::string& response);

	private:
		// Private attributes
		int					serv_port_;
		std::string			serv_passwd_;
		static Server*		server_;
		int					serv_fd_;
		int					epoll_fd_;
		struct sockaddr_in	serv_addr_;
		static constexpr int	MAX_EVENTS = 1024;

		// std::shared_ptr<T> is a smart pointer introduced in C++11 that manages the
		// lifetime of a dynamically allocated object. It does so using reference
		// counting — multiple shared_ptr instances can share ownership of the same object.
		// When the last shared_ptr pointing to that object is destroyed or reset,
		// the object is automatically deleted.
		std::unordered_map<int, std::shared_ptr<Client>> clients_; // the key is client socket (client_fd)
		std::unordered_map<std::string, std::shared_ptr<Channel>> channels_; // string is the channel name
		std::vector<struct epoll_event>	events_; // using for saving the clients' fds

		static volatile sig_atomic_t	keep_running_; // internal flag
		static const std::set<COMMANDTYPE> pre_registration_allowed_commands_;



		// private functions
		Server() = delete;
		Server(const Server&) = delete;
		Server& operator=(const Server&) = delete;

		void	setupSignalHandlers();
		static void	signalHandler(int signum);
		void	setupServSocket();
		void	acceptNewClient();
		void	processDataFromClient(int idx);
		void	removeClient(int fd, std::string reason);
		void	executeCommand(Message& msg, Client& cli);
};

#include "Logger.hpp"
#include "Client.hpp"
#include "Message.hpp"
// #include "Channel.hpp"
