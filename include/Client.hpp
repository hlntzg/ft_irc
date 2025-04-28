/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 12:55:41 by jingwu            #+#    #+#             */
/*   Updated: 2025/04/28 14:37:36 by jingwu           ###   ########.fr       */
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

		bool	receiveRawData();
		bool	getNextMessage(std::string& buffer);

	private:
		int	socket_fd_;
		std::string	raw_data_;

		Client(const Client&) = delete;


};
