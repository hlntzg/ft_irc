/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 12:55:41 by jingwu            #+#    #+#             */
/*   Updated: 2025/05/16 08:34:03 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <sys/socket.h> // for recv()

#define BUFFER_SIZE (5000)

class Client{
	public:
		Client();
		Client(int fd, std::string host);
		Client&	operator=(const Client& other);
		~Client();

		// getters
		int					getSocketFd() const;
		const std::string&	getNick() const;
		const std::string&	getUsername() const;
		const std::string&	getRealname() const;
		const std::string&	getHostname() const;
		const std::string&	getPassword() const;
		bool				getNextMessage(std::string& buffer);
		std::string			getPrefix() const;
		const std::string&	getUserMode() const;

		// setters
		void	setNick(const std::string& nick);
		void	setUsername(const std::string& username);
		void	setRealname(const std::string& realname);
		void	setHostname(const std::string& hostname);
		void	setServername(const std::string& servername);
		void	setPassword(const std::string& passwd);
		void	setRegistrationStatus(bool	status);
		void	setUserMode(const std::string& mode);

		bool	receiveRawData();
		bool	isRegistered();

		// for testing
		void	printInfo() const;
		void    printRawData() const;

	private:
		int	socket_fd_;
		std::string	nick_;
		std::string	username_;
		std::string	realname_;
		std::string	hostname_;
		std::string	servername_;
		std::string password_;
		std::string	raw_data_;
		std::string	user_mode_;
		bool	isRegistered_;

		Client(const Client&) = delete;
};
