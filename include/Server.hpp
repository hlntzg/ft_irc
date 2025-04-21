/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:17:32 by jingwu            #+#    #+#             */
/*   Updated: 2025/04/21 14:10:29 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <unordered_map> //  don’t care about the order and want better performance
#include <regex>
#include <vector>
#include <stdexcept>

#include <netinet/in.h>
#include <poll.h>

class Client;
class Channel;

class Server{
	public:
		Server(std::string serv_pass, int serv_port);
		Server(const Server& o);
		Server& operator=(const Server& o);
		~Server();

		void	StartServer();

	private:
		// Private attributes
		std::string	serv_pass_;
		int			serv_port_;
		static Server*	server_;
		int			serv_fd_;
		struct sockaddr_in	serv_addr_;
		std::unordered_map<int, Client> clients_;
		std::unordered_map<int, Channel> channels_;
		std::vector<struct pollfd>	poll_fds_;

		// private functions
		Server();
		void	initServer();


};
