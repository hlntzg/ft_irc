/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 12:55:41 by jingwu            #+#    #+#             */
/*   Updated: 2025/05/01 13:55:24 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>

class Client{
	public:
		Client();
		Client(int fd);
		Client&	operator=(const Client& other);
		~Client();

		int		getSocketFD() const;
		const std::string&	getNick() const;
		bool	receiveRawData();
		bool	getNextMessage(std::string& buffer);
		bool	isRegistered();

	private:
		int	socket_fd_;
		std::string	nick_;
		std::string	raw_data_;
		bool	isRegistered_;

		Client(const Client&) = delete;


};
