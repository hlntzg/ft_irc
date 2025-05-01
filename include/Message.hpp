
#pragma once

#include <string>
#include <iostream>
#include <unordered_set>

class Client;
class Server;

/**
 * General IRC Message Format:
 *        [:prefix] <command> <params> [:trailing]
 * In this project, we don't conside prefix commands.
 *
 * For the command KICK, INVITE, TOPIC and MODE, only KICK command can accept
 * more than one channels, such as "KICK #chan1,#chan2 user1,user2 :reason" *
 *
 * Examples of commands:
 *    PASS mysecretpassword
 *    NICK sherry
 *    USER sherry 0 * :Sherry Zhang
 *    PRIVMSG #42Helsinki :Hello everyone!
 *    PRIVMSG john_doe :Hey, how are you?
 *    JOIN #team1,#team2
 *    JOIN #secretChannel mykey123  // join a channel with keys
 *    PART #42Helsinki
 *    PART #team2 :See you later!
 *    KICK #42Helsinki user123 :Spamming is not allowed
 *    KICK #channel user1,user2 :Clean up!
 *    INVITE alice #42Helsinki
 *    TOPIC #42Helsinki // get current topic
 *    TOPIC #42Helsinki :Welcome to 42 Helsinki IRC!   // set a new topic
 *    MODE sherry     // query user mode
 *    MODE #42Helsinki +i  // Set channel to invite-only
 *    MODE #42Helsinki +o alice  // give operator status
 *    QUIT :Goodbye, folks!
 *    QUIT
 *
 */

class Message{
	public:
		Message(std::string& message);
		~Message();

		void	parseMessage();

		COMMANDTYPE	getType();
		void	execute(Client& cli);

	private:
		std::string	whole_msg_;
		// Elements contain in a message
		COMMANDTYPE	msg_type_;
		std::unordered_set<std::shared_ptr<Channel>>	msg_channels_;

		// msg_params used to store the content other than command, channels
		// and trailing in a message, like "user1", "user2", "alice"
		std::vector<std::string>	msg_params_;
		std::string	msg_trailing_;

		static const std::set<COMMANDTYPE> operator_commands_;
		// this defines executeFunc is a pointer to a function inside the Message class
		// that takes two reference arguments and returns void.
		// Using in the server class
		using executeFunc = void (Message::*)(Client& cli);
		static const std::unordered_map<COMMANDTYPE, Message::executeFunc> execute_map_;

		void	passCommand(Client& cli);
		void	quitCommand(Client& cli);

};

#include "Client.hpp"
#include "Server.hpp"
