/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 12:55:41 by jingwu            #+#    #+#             */
/*   Updated: 2025/05/06 14:50:25 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>

class Client{
	public:
		Client();
		Client(int fd, std::string host);
		Client&	operator=(const Client& other);
		~Client();

		// getters
		int					getSocketFD() const;
		const std::string&	getNick() const;
		bool				getNextMessage(std::string& buffer);
		const std::string&	getPrefix() const;


		// setters
		void	setPassword(const std::string& passwd);


		bool	receiveRawData();

		bool	isRegistered();


	private:
		int	socket_fd_;
		std::string	nick_;
		std::string	name_;
		std::string	fullname_;
		std::string	host_;
		std::string password_;
		std::string	raw_data_;
		bool	isRegistered_;

		Client(const Client&) = delete;
};
