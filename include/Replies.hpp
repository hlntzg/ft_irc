/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:44:37 by jingwu            #+#    #+#             */
/*   Updated: 2025/05/13 11:16:41 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#define SERVER "irc.ircserv.com"
#define SUPPORTUSERMODE "o"
#define SUPPORTCHANNELMODE "itkol"

/**
 * What is "inline" for?
 * The keyword inline suggests to the compiler:
 * "Instead of calling this function normally, try inserting its code directly
 * where it's used."
 * This is like a copy-paste optimization at compile time to avoid function call
 * overhead for small functions.
 */

/*...................................Commands Replies..............................*/

// 001 RPL_WELCOME
inline std::string rplWelcome(const std::string& nick){
	return (std::string(SERVER) + " 001 " + nick + " : Welcome to the IRC Network, " + nick + "\r\n");
}

// 002 RPL_YOURHOST
inline std::string rplYourHost(const std::string& nick){
	return (std::string(SERVER) + " 002 " + nick + " : Your host is " + std::string(SERVER) +
			", running version ircserv 1.0\r\n");
}

// 003 RPL_CREATED
inline std::string rplCreated(const std::string& nick){
	return (std::string(SERVER) + " 003 " + nick + " :This server was created on May 10 2025\r\n");
}

// 004 RPL_MYINFO
inline std::string rplMyInfo(const std::string& nick){
	return (std::string(SERVER) + " 004 " + nick + " :" + std::string(SERVER) +
	        + " 1.0 " + std::string(SUPPORTUSERMODE) + " "
			+ std::string(SUPPORTCHANNELMODE) +"\r\n");
}

// 324 RPL_CHANNELMODEIS
inline std::string ChannelModeIs(const std::string& nick, const std::string& channel,
const std::string& mode_params){
	return (std::string(SERVER) + " 324 " + nick + " " + channel +
	        + " mode " + mode_params + " \r\n");
}

// 331 RPL_NOTOPIC
inline std::string NoTopic(const std::string& nick, const std::string& channel){
	return (std::string(SERVER) + " 331 " + nick + " " + channel +": No topic is set \r\n");
}

// 332 RPL_TOPIC
inline std::string Topic(const std::string& nick, const std::string& channel,
const std::string& topic){
	return (std::string(SERVER) + " 332 " + nick + " " + channel + " " + topic +"\r\n");
}

// 341 RPL_INVITING
/**
 * @brief Returned by the server to indicate that the attempted INVITE message was
 * successful and is being passed onto the end client.
 */
inline std::string Inviting(const std::string& nick, const std::string& channel,
const std::string& invitee_nick){
	return (std::string(SERVER) + " 341 " + nick + " " + channel + " " + invitee_nick +"\r\n");
}

/**
 * @brief Constructs a KICK message indicating that a user has been removed from a channel.
 *
 * This message is sent to all users in the channel (except the target) and directly to the kicked user.
 * It follows the IRC format: ":<kicker> KICK <channel> <target> [:reason]"
 */
inline std::string rplKick(const std::string& nick, const std::string& target,
const std::string& channel, const std::string& reason) {
	std::string msg = ":" + nick + " KICK " + channel + " " + target;
	if (!reason.empty()) {
		msg += " :" + reason;
	}
	return msg + "\r\n";
}

/**
 * @brief Constructs a MODE message indicating mode changes on a channel.
 *
 * This message is sent to all users in the channel to inform them of a mode change.
 * It follows the IRC format: ":<nick> MODE <channel> <modes> [<args>...]"
 */
inline std::string rplMode(const std::string& nick, const std::string& channel,
                           const std::string& mode_flags,
                           const std::vector<std::string>& args) {
	std::string msg = ":" + nick + " MODE " + channel + " " + mode_flags;
	for (const std::string& arg : args) {
		msg += " " + arg;
	}
	return msg + "\r\n";
}

/**
 * @brief Constructs a MODE message indicating mode changes on a channel.
 *
 * This message is sent to all users in the channel to inform them of a mode change.
 * It follows the IRC format: ":<nick> MODE <channel> <modes> [<args>...]"
 */
inline std::string rplJoinChannel(const std::string& nick, const std::string& prefix, const std::string& channel_name) {
	 return prefix + " " + nick + " has joined #" + channel_name + "\r\n";
}

/**
 * @brief Constructs a PART message indicating that a user has left a channel.
 *
 * This message is sent to all users in the channel, including the parting user.
 * It follows the IRC format: ":<nick> PART <channel> [:reason]"
 */
inline std::string rplPart(const std::string& nick, const std::string& channel, const std::string& reason = "") {
	std::string msg = ":" + nick + " PART " + channel;
	if (!reason.empty()) {
		msg += " :" + reason;
	}
	return msg + "\r\n";
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

// 405 ERR_TOOMANYTARGETS
/**
 * @brief Sent to a user when they have joined the maximum number of allowed channels
 * and they try to join another channel.
 */
inline std::string tooManyTargets(const std::string& nick){
	return (std::string(SERVER) + nick + " :too many targets\r\n");
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
inline std::string nonNickNameGiven(){
	return (std::string(SERVER) + " 431 :No nickname given\r\n");
}

// 432 ERR_ERRONEUSNICKNAME
/**
 * @brief Returned after receiving a NICK message which contains characters which do
 * not fall in the defined set.
 */
inline std::string erroneusNickName(const std::string& nick){
	return (std::string(SERVER) + " 432 " + nick + " :Erroneus nickname/username\r\n");
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
	if (target != "")
		return (std::string(SERVER) + " 443 " + nick  + " " + target + " " + channel
		     + " :is already on channel\r\n");
	return (std::string(SERVER) + " 443 " + nick  + " " + channel
		     + " :is already on channel\r\n");
}

// 451 ERR_NOTREGISTERED
/**
 * @brief Returned by the server to indicate that the client must be registered before
 * the server will allow it to be parsed in detail.
 */
inline std::string NotRegistered(const std::string& command){
	return (std::string(SERVER) + " 451 " + command  + " :You have not registered\r\n");
}

// 461 ERR_NEEDMOREPARAMS
/**
 * @brief Returned by the server by numerous commands to indicate to the client that
 * it didn't supply enough parameters.
 */
inline std::string needMoreParams(const std::string& command){
	return (std::string(SERVER) + " 461 " + command  + " :Not enough parameters\r\n");
}

// 462 ERR_ALREADYREGISTRED
/**
 * @brief Returned by the server to any link which tries to change part of the
 * registered details (such as password or user details from second USER message).
 */
inline std::string alreadyRegistred(const std::string& nick){
	return (std::string(SERVER) + " 462 " + nick  + " :You may not reregister\r\n");
}

// 464 ERR_PASSWDMISMATCH
/**
 * @brief Returned to indicate a failed attempt at registering a connection for
 * which a password was required and was either not given or incorrect.
 */
inline std::string passwdMismatch(const std::string& nick){
	return (std::string(SERVER) + " 464 " + nick  + " :Password incorrect\r\n");
}

// 467 ERR_KEYSET
inline std::string keySet(const std::string& nick, const std::string& channel){
	return (std::string(SERVER) + " 467 " + nick + " " + channel + " :Channel key already set\r\n");
}

// 471 ERR_CHANNELISFULL
inline std::string channelIsFull(const std::string& nick, const std::string& channel){
	return (std::string(SERVER) + " 471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n");
}

// 472 ERR_UNKNOWNMODE
inline std::string unknownMode(const std::string& nick, const std::string& unknown_mode){
	return (std::string(SERVER) + " 472 " + nick + " " + unknown_mode + " :is unknown mode char to me\r\n");
}

// 473 ERR_INVITEONLYCHAN
inline std::string inviteOnlyChan(const std::string& nick, const std::string& channel){
	return (std::string(SERVER) + " 473 " + nick + " " + channel + " :Cannot join channel (+i)\r\n");
}

// 474 ERR_BADCHANNELKEY
inline std::string badChannelKey(const std::string& nick, const std::string& channel){
	return (std::string(SERVER) + " 474 " + nick + " " + channel + " :Cannot join channel (+k)\r\n");
}

// 478 ERR_INVITE_SYNTAX
/**
 * @brief Any command requiring 'chanop' privileges (such as MODE messages) must return this
 * error if the client making the attempt is not a chanop on the specified channel
 */
inline std::string InviteSyntaxErr(const std::string& nick){
	return (std::string(SERVER) + " 478 " + nick + ":Invalid Invite command\r\n");
}

// 482 ERR_CHANOPRIVSNEEDED
/**
 * @brief Any command requiring 'chanop' privileges (such as MODE messages) must return this
 * error if the client making the attempt is not a chanop on the specified channel
 */
inline std::string ChanoPrivsNeeded(const std::string& nick, const std::string& channel){
	return (std::string(SERVER) + " 482 " + nick + " " + channel + " :You're not channel operator\r\n");
}


// ERR_NOCHANOPASSWD
/**
 * @brief Any command requiring 'chanop' privileges (such as MODE messages) must return this
 * error if the client making the attempt is not a chanop on the specified channel
 */
inline std::string noChanoPasswd(const std::string& nick, const std::string& channel){
	return (std::string(SERVER) + nick + " " + channel + " :Channel password doesn't be provided\r\n");
}
