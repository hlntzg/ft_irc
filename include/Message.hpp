/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:36:39 by arissane          #+#    #+#             */
/*   Updated: 2025/05/20 14:56:14 by arissane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <iostream>
#include <unordered_set>
#include <vector>
#include "Server.hpp"

class Client;

class Message{
	public:
		Message(std::string& message);
		~Message();

		bool					parseMessage();
		const std::string& 		getWholeMessage() const;
		int 					getNumberOfParameters() const;
		const std::string&		getTrailing() const;
		const std::vector<std::string>&	getParameters() const;
		const std::vector<std::string>&	getUsers() const;
		const std::vector<std::string>&	getChannels() const;
		COMMANDTYPE 			getCommandType() const;
		const std::string& 		getCommandString() const;
		const std::vector<std::string>& getPasswords() const;
		bool getTrailingEmpty() const;

		// for testing only
		// void	printMsgInfo() const;
		// void	printUserList() const;

	private:
		using CommandHandler = std::function<bool()>;
		std::unordered_map<std::string, CommandHandler> command_handlers_;
		void				initCommandHandlers();
		bool				handleGeneric();
		bool				handleCAP();
		bool 				handlePASS();
		bool				handleKICK();
		bool				handleJOIN();
		bool				handleMODE();
		bool				handleNoParse();
		bool 				validateParameters(const std::string& command);
		std::string			whole_msg_;
		int					number_of_parameters_;
		std::string			msg_trailing_;//everything found after :
		std::vector<std::string>	parameters_;//All the parameters, except commandtype or trailing message
		std::vector<std::string>	msg_users_;//parameters that should be users in the message
		std::vector<std::string>	msg_channels_;//parameters that should be channels in the message
		COMMANDTYPE			cmd_type_;//type of the command as defined in the server.hpp
		std::string			cmd_string_;//string version of the given command
		std::vector<std::string>	passwords_;//passwords for the JOIN command if it exists
		bool				msg_trailing_empty_;//set to true only if the msg_trailing_ was ":"
};

#include "Client.hpp"
