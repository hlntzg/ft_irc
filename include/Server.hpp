/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:17:32 by jingwu            #+#    #+#             */
/*   Updated: 2025/05/13 10:23:36 by jingwu           ###   ########.fr       */
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
#include <arpa/inet.h> // for inet_ntop

class Client;
class Channel;
class Message;


#define SPECIAL_CHARS_NAMES "[]{}^-'\\"
#define SPECIAL_CHARS_PASSWD "!@#$%^&*()-_=+[]{}|;:'\",.<>?/\\~`"
#define PASSWORD_RULE "Allow contain:\n1.Letters\n2.Digits\n3.Characters in\"!@#$%^&*()-_=+[]{}|;:'\",.<>?/\\~`\""
#define TARGET_LIM_IN_ONE_CMD (4)

enum COMMANDTYPE{
	PASS,
	NICK,
	USER,
	PRIVMSG,
	JOIN,
	PART,
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
		static int	responseToClient(Client& cli, const std::string& response);

	private:
		int					serv_port_;
		std::string			serv_passwd_;
		static Server*		server_;
		int					serv_fd_;
		int					epoll_fd_;
		struct sockaddr_in	serv_addr_;

		static constexpr int			MAX_EVENTS = 1024;
		static volatile sig_atomic_t	keep_running_; // internal flag

		// std::shared_ptr<T> is a smart pointer introduced in C++11 that manages the
		// lifetime of a dynamically allocated object. It does so using reference
		// counting — multiple shared_ptr instances can share ownership of the same object.
		// When the last shared_ptr pointing to that object is destroyed or reset,
		// the object is automatically deleted.
		std::unordered_map<int, std::shared_ptr<Client>>			clients_; // the key is client socket (client_fd)
		std::unordered_map<std::string, std::shared_ptr<Channel>>	channels_; // string is the channel name
		std::vector<struct epoll_event>								events_; // using for saving the clients' fds
		static const std::set<COMMANDTYPE>							pre_registration_allowed_commands_;
		static const std::set<COMMANDTYPE>							operator_commands_;

		// this defines executeFunc is a pointer to a function inside the Message class
		// that takes two reference arguments and returns void.
		// Using in the server class
		using executeFunc = void (Server::*)(Message& msg, Client& cli);
		static const std::unordered_map<COMMANDTYPE, Server::executeFunc> execute_map_;


		Server() = delete;
		Server(const Server&) = delete;
		Server& operator=(const Server&) = delete;

		void		setupSignalHandlers();
		static void	signalHandler(int signum);
		void		setupServSocket();
		void		acceptNewClient();
		void		processDataFromClient(int idx);
		void		removeClient(Client& usr, std::string reason);
		void		removeChannel(const std::string& channel_name);
		void		executeCommand(Message& msg, Client& cli);

		std::shared_ptr<Channel>		getChannelByName(const std::string& channel_name) const;
		std::shared_ptr<Client>			getUserByNick(const std::string& user_nick) const;
		// Client* getClientByNick(const std::string& nick) const;

		// commands
		void		passCommand(Message& msg, Client& cli);
		void		nickCommand(Message& msg, Client& cli);
		void		userCommand(Message& msg, Client& cli);
		void		privmsgCommand(Message& msg, Client& cli);
		void		joinCommand(Message& msg, Client& cli);
		void		partCommand(Message& msg, Client& cli);
		void		quitCommand(Message& msg, Client& cli);

		// Commands specific to channel operators:
		void		kickUser(Message& msg, Client& cli);
		void		inviteUser(Message& msg, Client& cli);
		void		topic(Message& msg, Client& cli);
		void		mode(Message& msg, Client& cli);

		// command helper functions
		bool		isPasswordMatch(const std::string& password);
		void		attempRegisterClient(Client& cli);
		bool		isNickInUse(const std::string& nick);
		bool		isExistedChannel(const std::string& channel_name);

		// for testing
		void	printUsers() const;
		void	printChannels() const;
};

#include "Logger.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Channel.hpp"
#include "Replies.hpp"


/**
 * Compare std::shared_ptr and Raw pointer(Client*)
 *  1. Ownership
 *  std::shared_ptr: Shared ownership — multiple shared_ptrs can point to the same object.
 *                    Object is destroyed when the last shared_ptr is destroyed.
 *  Raw Pointer: No ownership semantics — you manually manage the memory.
 *
 * 2. Memory Management
 *	 std::shared_ptr: Automatically deletes the object when the reference count reaches
                      zero.
     Raw Pointer: Must manually call delete. Easy to forget, leading to memory leaks.
 *
 * 3. Safety
 *  std::shared_ptr: Safer. Prevents double delete and dangling pointers if used correctly.
 *  Raw Pointer: Dangerous. Easy to cause memory leaks, dangling pointers, and undefined
 *               behavior.
 *
 * 4. Copyable
 *  std::shared_ptr: Can be copied and passed around safely; internally manages reference count.
 *  Raw Pointer: Can be copied, but no automatic tracking of who owns the memory.
 *
 *  5. Overhead
 *  std::shared_ptr: Slightly more overhead due to reference counting mechanism.
 *  Raw Pointer: No overhead, but higher risk of bugs.
 *
 * 6. Usage Scenario
 *  std::shared_ptr: Recommended when multiple parts of code share ownership of a
 *                   dynamically allocated object.
 *  Raw Pointer: Use when performance is critical and ownership is clear.
 *               Often used for low-level or temporary purposes.
 */
