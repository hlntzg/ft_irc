
#pragma once

#include <string>
#include <iostream>
#include <unordered_set>
#include <vector>

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
 *    MODE #42Helsinki +it  // Set channel to invite-only
 *    MODE #42Helsinki +ioltk alice 10 passwd // give operator status
 *    QUIT :Goodbye, folks!
 *    QUIT
 *
 *   
 * 
 * In the parsing, we need to check the order of the parameters, and if the channel and
 * user is valid.
 * 
 * SET LLIMITED MAX CHANNEL to 4, apply to all the commmand related with multiple
 * channels and client
 * 
 * JOIN only supprot below two conditions:
 *    1. multiple channels without password, limited max 4 channel, like "JOIN #team1,#team2 ...";
 *    2. one channel only with password; like "JOIN #secretChannel mykey123", and
 *       like "JOIN #secretChannel #test1 mykey123" is wrong command
 * 
 * PART support
 *    1. multiple channels, limited max 4 channel.
 * 
 * KICK support: 
 *   1. kick one user from multiple channels;
 *   2. kick multiple users from one channel;
 *   3. kick N users from N channels in order, it means the amount of the user
 *      and channel should equal. 
 *      Exmaple: KICK #chann1,#chann2 user1,user2
 *      It means: - Kick user1 from #chann1
 *                - Kick user2 from #chann2
 * 
 * INVITE support
 *   1. invite one user to multiple channels;
 *   2. invite multiple users to one channel;
 * 
 * TOPIC support
 *   1. only can change one channel's topic at one time;
 *   2. If there is no paramter, then just display the current topic if it has one,
 *      otherwise, it show NOTopic reply(331);
 * 
 * MODE support
 *   1. change one channel's mode at one time, but can contain multiple supported
 *      flags.  the parameters must be given in the order that the flags require
 *      them, and only flags that need arguments will consume arguments from the list.
 * 
 * QUIT support (Leave from the IRC network)
 *    1. with quiting reason or without
 */

class Message{
	public:
		Message(std::string& message);
		~Message();

		bool				parseMessage();
		const std::string& 		getWholeMessage() const;
		int 				getNumberOfParameters() const;
		const std::string&		getTrailing() const;
		const std::vector<std::string>&	getParameters() const;
		const std::vector<std::string>&	getUsers() const;
		const std::vector<std::string>&	getChannels() const;
		const std::vector<std::string>&	getFlags() const;
		COMMANDTYPE 			getCommandType() const;

		/*
		COMMANDTYPE						getCmdType() const;
		*/
	private:
		bool 				validateParameters(const std::string& command);
		std::string			whole_msg_;
		int				number_of_parameters_;
		std::string			msg_trailing_;//everything found after :
		std::vector<std::string>	parameters_;//all the parameters in a vector
		std::vector<std::string>	msg_users_;//parameters that should be users in the message
		std::vector<std::string>	msg_channels_;//parameters that should be channels in the message
		std::vector<std::string>	flags_;//flags from mode
		COMMANDTYPE			cmd_type_;

		// std::string stores channel name;
		// std::shared_ptr<Channel> stores Channel objects;
		// std::unordered_map<std::string, std::shared_ptr<Channel>>	msg_channel_;
		std::vector<std::string> msg_channels_;

		// msg_params used to store the content other than command, channels
		// and trailing in a message, like "user1", "user2", "alice"
		std::vector<std::string>	msg_params_;
		*/
};

#include "Client.hpp"
#include "Server.hpp"
