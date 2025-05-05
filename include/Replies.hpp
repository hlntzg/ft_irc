/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:44:37 by jingwu            #+#    #+#             */
/*   Updated: 2025/05/05 14:55:18 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#define SERVER ":irc.ircserv.com"
#define SUPPORTUSERMODE "o"
#define SUPPORTCHANNELMODE "itkol"

/**
 * The keyword inline suggests to the compiler:
"Instead of calling this function normally, try inserting its code directly
where it's used."
This is like a copy-paste optimization at compile time to avoid function call
overhead for small functions.
 */

/*.............................Registeration Replies.........................*/

// 001 RPL_WELCOME
inline std::string rplWelcome(const std::string& nick){
	return (std::string(SERVER) + " 001 " + nick + " : Welcome to the IRC Network, " + nick + "\r\n");
}

// 002 RPL_YOURHOST
inline std::string rplYourHost(const std::string& nick){
	return (std::string(SERVER) + " 002 " + nick + " : Your host is, " + std::string(SERVER) +
			", running version ircserv-1.0\r\n");
}

// 003 RPL_CREATED
inline std::string rplCreated(const std::string& nick){
	return (std::string(SERVER) + " 003 " + nick + " :This server was created on May 10 2025\r\n");
}

// 004 RPL_MYINFO
inline std::string rplMyInfo(const std::string& nick){
	return (std::string(SERVER) + " 004 " + nick + " " + std::string(SERVER) +
	        + " 1.0 " + std::string(SUPPORTUSERMODE) + " "
			+ std::string(SUPPORTCHANNELMODE) +"\r\n");
}


/*...................................Error Replies.................................*/

// 401 ERR_NOSUCHNICK
/**
 * @brief Used to indicate the nickname parameter supplied to a command is currently unused.
 */
inline std::string errNoSuchNick(const std::string& nick, const std::string& wrong_nick){
	return (std::string(SERVER) + " 401 " + nick + " " + wrong_nick+" :No such nick\r\n");
}

// 403 ERR_NOSUCHCHANNEL
/**
 * @brief Used to indicate the given channel name is invalid
 */
inline std::string errNoSuchChannel(const std::string& nick, const std::string& wrong_channel){
	return (std::string(SERVER) + " 403 " + nick + " " + wrong_channel+" :No such channel\r\n");
}

// 404 ERR_CANNOTSENDTOCHAN
/**
 * @brief Sent to a user who is either (a) not on a channel which is mode +n or (b)
 * not a chanop (or mode +v) on a channel which has mode +m set and is trying to send
 * a PRIVMSG message to that channel.
 */
inline std::string canNotSendToChan(const std::string& nick, const std::string& channel_name){
	return (std::string(SERVER) + " 404 " + nick + " " + channel_name+" :Cannot send to channel\r\n");
}

// 405 ERR_TOOMANYCHANNELS
/**
 * @brief Sent to a user when they have joined the maximum number of allowed channels
 * and they try to join another channel.
 */
inline std::string tooManyChannels(const std::string& nick, const std::string& channel_name){
	return (std::string(SERVER) + " 405 " + nick + " " + channel_name
			+ " :You have joined too many channels\r\n");
}

// 421 ERR_UNKNOWNCOMMAND
/**
 * @brief Returned to a registered client to indicate that the command sent is unknown
 * by the server.
 */
inline std::string unknowCommand(const std::string& nick, const std::string& wrong_command){
	return (std::string(SERVER) + " 421 " + nick + " " + wrong_command
			+ " :Unknow command\r\n");
}

// 431 ERR_NONICKNAMEGIVEN
/**
 * @brief Returned when a nickname parameter expected for a command and isn't found.
 */
inline std::string nonNickNameGiven(const std::string& nick){
	return (std::string(SERVER) + " 431 " + nick + " :No nickname given\r\n");
}

// 432 ERR_ERRONEUSNICKNAME
/**
 * @brief Returned after receiving a NICK message which contains characters which do
 * not fall in the defined set.
 */
inline std::string erroneusNickName(const std::string& nick){
	return (std::string(SERVER) + " 432 " + nick + " :Erroneus nickname\r\n");
}

// 433 ERR_NICKNAMEINUSE
/**
 * @brief Returned when a NICK message is processed that results in an attempt to change
 * to a currently existing nickname.
 */
inline std::string nickNameinuse(const std::string& nick){
	return (std::string(SERVER) + " 433 " + nick + " :Nickname is already in use\r\n");
}

// 441 ERR_USERNOTINCHANNEL
/**
 * @brief Returned by the server to indicate that the target user of the command is
 * not on the given channel.
 */
inline std::string userNotInChannel(const std::string& nick, const std::string& targets,
const std::string& channel){
	return (std::string(SERVER) + " 441 " + nick  + " "  + targets + " "+ channel
		     + " :They aren't on that channel\r\n");
}

// 442 ERR_NOTONCHANNEL
/**
 * @brief Returned by the server whenever a client tries to perform a channel effecting
 * command for which the client isn't a member.
 */
inline std::string notOnChannel(const std::string& nick, const std::string& channel){
	return (std::string(SERVER) + " 442 " + nick  + " " + channel
		     + " :You're not on that channel\r\n");
}

// 443 ERR_USERONCHANNEL
/**
 * @brief Returned when a client tries to invite a user to a channel they are already on.
 *
 * @param nick the user who try to operate the command
 * @param target: the user who is added into a channel
 * @param channel: the channel which adds a new user in
 */
inline std::string userOnChannel(const std::string& nick, const std::string& target,
const std::string& channel){
	return (std::string(SERVER) + " 443 " + nick  + " " + target + " " + channel
		     + " :is already on channel\r\n");
}

// 451 ERR_NOTREGISTERED
/**
 * @brief Returned by the server to indicate that the client must be registered before
 * the server will allow it to be parsed in detail.
 */
inline std::string NotRegistered(const std::string& nick){
	return (std::string(SERVER) + " 451 " + nick  + " :You have not registered\r\n");
}

// 461 ERR_NEEDMOREPARAMS
/**
 * @brief Returned by the server by numerous commands to indicate to the client that
 * it didn't supply enough parameters.
 */
inline std::string needMoreParams(const std::string& command){
	return (std::string(SERVER) + " 461 " + command  + " :Not enough parameters\r\n");
}


// PASS and USER
ERR_ALREADYREGISTRED


// CHANNEL
ERR_BANNEDFROMCHAN
ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
ERR_CHANNELISFULL               ERR_BADCHANMASK


RPL_CHANNELMODEIS
           ERR_CHANOPRIVSNEEDED
             ERR_KEYSET
           RPL_BANLIST                     RPL_ENDOFBANLIST
           ERR_UNKNOWNMODE

           ERR_USERSDONTMATCH              RPL_UMODEIS
           ERR_UMODEUNKNOWNFLAG
		   RPL_NOTOPIC
		   RPL_INVITING    ERR_USERONCHANNEL
		   ERR_BADCHANMASK
