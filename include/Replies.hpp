/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 10:44:37 by jingwu            #+#    #+#             */
/*   Updated: 2025/05/20 14:40:23 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#define SERVER "irc.ircserv.com"
#define SUPPORTUSERMODE "o"
#define SUPPORTCHANNELMODE "itkol"
#define CRLF "\r\n" // Carriage Return - Line Feed

// const std::string SERVER = "irc.ircserv.com"; option to replace std::string(SERVER)

/**
 * What is "inline" for?
 * The keyword inline suggests to the compiler:
 * "Instead of calling this function normally, try inserting its code directly
 * where it's used."
 * This is like a copy-paste optimization at compile time to avoid function call
 * overhead for small functions.
 */


/*...................................General Replies..............................*/

// NOTICE message to a user
inline std::string noticeToUser(const std::string& nick, const std::string& message) {
    return (":" + std::string(SERVER) + " NOTICE " + nick + " :" + message + CRLF);
}

/*...................................Commands Replies..............................*/

// 001 RPL_WELCOME
// inline std::string rplWelcome(const std::string& nick){
// 	return (":" + std::string(SERVER) + " 001 " + nick + " :Welcome to the IRC Network, " + nick + "\r\n");
// }
// 001 RPL_WELCOME
// "Welcome to the Internet Relay Network <nick>!<user>@<host>"

inline std::string rplWelcome(const std::string& nick, const std::string& prefix){
	return ":" + std::string(SERVER) + " 001 " + nick + " :Welcome to the Internet Relay Network "
				+ prefix + CRLF;
}

// 002 RPL_YOURHOST
inline std::string rplYourHost(const std::string& nick){
	return (":" + std::string(SERVER) + " 002 " + nick + " :Your host is " + std::string(SERVER) +
			", running version ircserv 1.0" + CRLF);
}

// 003 RPL_CREATED
inline std::string rplCreated(const std::string& nick){
	return (":" + std::string(SERVER) + " 003 " + nick + " :This server was created on May 10 2025" + CRLF);
}

// 004 RPL_MYINFO
inline std::string rplMyInfo(const std::string& nick){
	return (":" + std::string(SERVER) + " 004 " + nick + " :" + std::string(SERVER) +
	        + " 1.0 " + std::string(SUPPORTUSERMODE) + " "
			+ std::string(SUPPORTCHANNELMODE) + CRLF);
}

// 221 RPL_UMODEIS
inline std::string rplUserModeIs(const std::string& nick, const std::string& modes) {
    return (":" + std::string(SERVER) + " 221 " + nick + " " + modes + CRLF);
}

// 318 RPL_ENDOFWHOIS
inline std::string rplEndOfWhois(const std::string& requestorNick, const std::string& targetNick) {
	return ":" + std::string(SERVER) + " 318 " + requestorNick + " " + targetNick + " :End of /WHOIS list" + CRLF;
}

// 311 RPL_WHOISUSER
inline std::string rplWhoisUser(const std::string& requestorNick,
								const std::string& targetNick,
								const std::string& user,
								const std::string& host,
								const std::string& realName){
	return ":" + std::string(SERVER) + " 311 " + requestorNick + " " + targetNick + " " +
		   user + " " + host + " * :" + realName + CRLF;
}

/*"<nick> <server> :<server info>"
  "<client> <nick> <server> :<server info>"
 Sent as a reply to the WHOIS command, this numeric shows which server
 the client with the nickname <nick> is connected to. <server> is the name of
 the server (as used in message prefixes). <server info> is a string containing a description of that server.
  */
// 312 RPL_WHOISSERVER
inline std::string rplWhoIsServer(const std::string& requestorNick,
							   const std::string& targetNick)
{
	return ":" + std::string(SERVER) + " 312 " + requestorNick + " " + targetNick + std::string(SERVER) +
		   " :Your IRC Server :)" + CRLF;
}

/*Sent as a reply to the WHO command, this numeric indicates the end of a WHO response for the mask <mask>.

<mask> MUST be the same <mask> parameter sent by the client in its WHO message, but MAY be casefolded.

This numeric is sent after all other WHO response numerics have been sent to the client.

"<client> <mask> :End of WHO list"
*/
// 315 RPL_ENDOFWHO
inline std::string rplEndOfWho(const std::string& requester, const std::string& target) {
	return ":" + std::string(SERVER) + " 315 " + requester + " " + target +
		" :End of /WHO list." + CRLF;
}


// 319 RPL_WHOISCHANNELS
inline std::string rplWhoIsChannels(const std::string& requesterNick,
									const std::string& targetNick,
									const std::string& channels){
	return ":" + std::string(SERVER) + " 319 " + requesterNick + " " + targetNick + " :" + channels + CRLF;
}

// 324 RPL_CHANNELMODEIS
inline std::string ChannelModeIs(const std::string& nick, const std::string& channel,
const std::string& mode){
	return (":" + std::string(SERVER) + " 324 " + nick + " " + channel + " " + mode + CRLF);
}

// 331 RPL_NOTOPIC
inline std::string NoTopic(const std::string& nick, const std::string& channel){
	return (":" + std::string(SERVER) + " 331 " + nick + " " + channel + " :No topic is set" + CRLF);
}

// 332 RPL_TOPIC
inline std::string Topic(const std::string& nick, const std::string& channel,
const std::string& topic){
	return (":" + std::string(SERVER) + " 332 " + nick + " " + channel + " :" + topic + CRLF);
}

// 341 RPL_INVITING
/**
 * @brief Returned by the server to indicate that the attempted INVITE message was
 * successful and is being passed onto the end client.
 */
inline std::string Inviting(const std::string& nick, const std::string& channel,
const std::string& target){
	return (":" + std::string(SERVER) + " 341 " + nick + " " + target + " " + channel + CRLF);
}

/*
Sent as a reply to the WHO command, this numeric gives information about the client with the nickname <nick>.
If the WHO command was given a channel as the <mask> parameter, then the same channel MUST be returned in <channel>.
Otherwise <channel> is an arbitrary channel the client is joined to or a literal asterisk character ('*', 0x2A)
if no channel is returned. <hopcount> is the number of intermediate servers between the client issuing
the WHO command and the client <nick>, it might be unreliable so clients SHOULD ignore it.

<flags> contains the following characters, in this order:

Away status: the letter H ('H', 0x48) to indicate that the user is here, or the letter G ('G', 0x47) to indicate that the user is gone.
Optionally, a literal asterisk character ('*', 0x2A) to indicate that the user is a server operator.
Optionally, the highest channel membership prefix that the client has in <channel>, if the client has one.
Optionally, one or more user mode characters and other arbitrary server-specific flags.

"<client> <channel> <username> <host> <server> <nick> <flags> :<hopcount> <realname>"
*/
// 352 RPL_WHOREPLY
inline std::string rplWhoReply(const std::string& requester,
							const std::string& channel,
							const std::string& user,
							const std::string& host,
							const std::string& nick,
							const std::string& status,
							const std::string& realname){
	return ":" + std::string(SERVER) + " 352 " + requester + " " + channel + " " +
		user + " " + host + " " + std::string(SERVER) + " " + nick + " " + status + " :0 " + " " + realname + CRLF;
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
	return msg + CRLF;
}

// /**
//  * @brief Constructs a MODE message indicating mode changes on a channel.
//  *
//  * This message is sent to all users in the channel to inform them of a mode change.
//  * It follows the IRC format: ":<nick> MODE <channel> <modes> [<args>...]"
//  */
// inline std::string rplMode(const std::string& nick, const std::string& channel,
// 	const std::string& mode_flags, const std::vector<std::string>& args) {
// 	std::string msg = ":" + nick + " MODE " + channel + " " + mode_flags;
// 	for (const std::string& arg : args) {
// 		msg += " " + arg;
// 	}
// 	return msg + CRLF;
// }
// Broadcast MODE change message
inline std::string rplMode(const std::string& prefix, const std::string& channelName,
	const std::string& modes, const std::string& params){
	return prefix + " MODE " +
		channelName + " " + modes + " " + params + CRLF;
}

// RPL_JOINCHANNEL
/**
 * @brief Broadcast JOIN message
 */
inline std::string rplJoin(const std::string& prefix, const std::string& channel)
{
	return prefix + " JOIN " + channel + CRLF;
}

/**
 * @brief Constructs a PART message indicating that a user has left a channel.
 *
 * This message is sent to all users in the channel, including the parting user.
 * It follows the IRC format: ":<nick> PART <channel> [:reason]"
 */
inline std::string rplPart(const std::string& prefix, const std::string& channelName, const std::string& partMessage){
	return prefix + " PART " + channelName + " :" + partMessage + CRLF;
}

/**
 * @brief Constructs a QUIT message indicating that a user has left a channel.
 *
 * This message is sent to all users in the channel, including the parting user.
 * It follows the IRC format: ":<nick> PART <channel> [:reason]"
 */
inline std::string rplQuit(const std::string& prefix, const std::string& message)
{
	return prefix + " QUIT :" + message + CRLF;
}

// inline std::string rplResetNick(const std::string& old_nick, const std::string& new_nick) {
// 	return (":" + std::string(SERVER) + old_nick + " reset nickname to " + new_nick + CRLF);
// }
inline std::string rplResetNick(const std::string& prefix, const std::string& newNick){
	return prefix + " NICK :" + newNick + CRLF;
}

/**
 * @brief send a message to a user or channel users
 *
 * @param source: message sender
 * @param target: message receiveer, can be a user or a channel
 * @param message: message that sender input
 */
inline std::string rplPrivMsg(const std::string& source, const std::string& target,
const std::string& message){
	return ":" + source + " PRIVMSG " + target + " :" + message + CRLF;
}


/*...................................Error Replies.................................*/

// 400 ERR_UNKNOWNERROR
/**
 * @brief Using for the errors haven't defined in IRC doucuments
 *
 * @param nick: the nickname of the user you're sending the error to
 * @param cmd:  the command that triggered the error (e.g., JOIN, MODE)
 * @param err_msg: a human-readable error message
 */
inline std::string unknowError(const std::string& nick, const std::string& cmd, const std::string& err_msg){
	return (":" + std::string(SERVER) + " 400 " + nick + " " + cmd + " :" + err_msg + CRLF);
}

// 401 ERR_NOSUCHNICK
/**
 * @brief Used to indicate the nickname parameter supplied to a command is currently unused.
 */
inline std::string errNoSuchNick(const std::string& nick, const std::string& wrong_nick){
	return (":" + std::string(SERVER) + " 401 " + nick + " " + wrong_nick + " :No such nick" + CRLF);
}

// 403 ERR_NOSUCHCHANNEL
/**
 * @brief Used to indicate the given channel name is invalid
 */
inline std::string errNoSuchChannel(const std::string& nick, const std::string& wrong_channel){
	return (":" + std::string(SERVER) + " 403 " + nick + " " + wrong_channel+" :No such channel" + CRLF);
}

// 404 ERR_CANNOTSENDTOCHAN
/**
 * @brief Sent to a user who is either (a) not on a channel which is mode +n or (b)
 * not a chanop (or mode +v) on a channel which has mode +m set and is trying to send
 * a PRIVMSG message to that channel.
 */
inline std::string canNotSendToChan(const std::string& nick, const std::string& channel_name){
	return (":" + std::string(SERVER) + " 404 " + nick + " " + channel_name + " :Cannot send to channel" + CRLF);
}

// 405 ERR_TOOMANYCHANNELS
/**
 * @brief Sent to a user when they have joined the maximum number of allowed channels
 * and they try to join another channel.
 */
inline std::string tooManyChannels(const std::string& nick, const std::string& channel_name){
	return (":" + std::string(SERVER) + " 405 " + nick + " " + channel_name
			+ " :You have joined too many channels" + CRLF);
}

// 407 ERR_TOOMANYTARGETS
/**
 * @brief Sent when a command is given with too many targets (e.g., multiple channels or users),
 *        but only one is allowed (like in MODE or PRIVMSG).
 */
inline std::string tooManyTargets(const std::string& nick) {
	return (":" + std::string(SERVER) + " 407 " + nick + " :Too many targets" + CRLF);
}

// inline std::string tooManyTargets(const std::string& nick, const std::string& target, int max) {
// 	return (":" + std::string(SERVER) + " 407 " + nick + " " + target
// 			+ " :Too many targets. Only " + std::to_string(max) + " is allowed." + CRLF);
// }

// 409 ERR_NOORIGIN
inline std::string noOrigin(const std::string& nick){
	return ":" + std::string(SERVER) + " 409 " + nick + " :No origin specified" + CRLF;
}

// 421 ERR_UNKNOWNCOMMAND
/**
 * @brief Returned to a registered client to indicate that the command sent is unknown
 * by the server.
 */
inline std::string unknowCommand(const std::string& nick, const std::string& wrong_command){
	return (":" + std::string(SERVER) + " 421 " + nick + " " + wrong_command
			+ " :Unknown command" + CRLF);
}

// 431 ERR_NONICKNAMEGIVEN
/**
 * @brief Returned when a nickname parameter expected for a command and isn't found.
 */
inline std::string nonNickNameGiven(const std::string& nick){
	return (":" + std::string(SERVER) + " 431 " + nick + " :No nickname given" + CRLF);
}

// 432 ERR_ERRONEUSNICKNAME
/**
 * @brief Returned after receiving a NICK message which contains characters which do
 * not fall in the defined set.
 */
inline std::string erroneusNickName(const std::string& nick){
	return (":" + std::string(SERVER) + " 432 " + nick + " :Erroneus nickname/username" + CRLF);
}

// 433 ERR_NICKNAMEINUSE
/**
 * @brief Returned when a NICK message is processed that results in an attempt to change
 * to a currently existing nickname.
 */
inline std::string nickNameInUse(const std::string& nick, const std::string& new_nick){
	return (":" + std::string(SERVER) + " 433 " + nick + " " + new_nick +" :Nickname is already in use" + CRLF);
}


// 441 ERR_USERNOTINCHANNEL
/**
 * @brief Returned by the server to indicate that the target user of the command is
 * not on the given channel.
 */
inline std::string userNotInChannel(const std::string& nick, const std::string& targets,
const std::string& channel){
	return (":" + std::string(SERVER) + " 441 " + nick  + " "  + targets + " "+ channel
		     + " :They aren't on that channel" + CRLF);
}

// 442 ERR_NOTONCHANNEL
/**
 * @brief Returned by the server whenever a client tries to perform a channel effecting
 * command for which the client isn't a member.
 */
inline std::string notOnChannel(const std::string& nick, const std::string& channel){
	return (":" + std::string(SERVER) + " 442 " + nick  + " " + channel
		     + " :You're not on that channel" + CRLF);
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
		return (":" + std::string(SERVER) + " 443 " + nick  + " " + target + " " + channel
		     + " :is already on channel" + CRLF);
	return (":" + std::string(SERVER) + " 443 " + nick  + " " + channel
		     + " :is already on channel" + CRLF);
}

// 451 ERR_NOTREGISTERED
/**
 * @brief Returned by the server to indicate that the client must be registered before
 * the server will allow it to be parsed in detail.
 */
inline std::string NotRegistered(const std::string& command){
	return (":" + std::string(SERVER) + " 451 " + command  + " :You have not registered" + CRLF);
}

// 353 RPL_NAMREPLY
inline std::string	rplNamReply(const std::string &nick, const std::string &channel, const std::string &namesList)
{
	return (":" + std::string(SERVER) + " 353 " + nick + " = " + channel + " :" + namesList + CRLF);
}

// 366 RPL_ENDOFNAMES
inline std::string	rplEndOfNames(const std::string &nick, const std::string &channel)
{
	return (":" + std::string(SERVER) + " 366 " + nick + " " + channel + " :End of /NAMES list." + CRLF);
}
// 368 RPL_ENDOFBANLIST
inline std::string rplEndOfBanList(const std::string& nick, const std::string& channel) {
    return ":" + std::string(SERVER) + " 368 " + nick + " " + channel + " :End of channel ban list" + CRLF;
}

// 461 ERR_NEEDMOREPARAMS
/**
 * @brief Returned by the server by numerous commands to indicate to the client that
 * it didn't supply enough parameters.
 */
inline std::string needMoreParams(const std::string& command){
	return (":" + std::string(SERVER) + " 461 " + command  + " :Not enough parameters" + CRLF);
}

// 462 ERR_ALREADYREGISTRED
/**
 * @brief Returned by the server to any link which tries to change part of the
 * registered details (such as password or user details from second USER message).
 */
inline std::string alreadyRegistred(const std::string& nick){
	return (":" + std::string(SERVER) + " 462 " + nick  + " :You may not reregister" + CRLF);
}

// 464 ERR_PASSWDMISMATCH
/**
 * @brief Returned to indicate a failed attempt at registering a connection for
 * which a password was required and was either not given or incorrect.
 */
inline std::string passwdMismatch(const std::string& nick){
	return (":" + std::string(SERVER) + " 464 " + nick  + " :Password incorrect" + CRLF);
}

// 467 ERR_KEYSET
inline std::string keySet(const std::string& nick, const std::string& channel){
	return (":" + std::string(SERVER) + " 467 " + nick + " " + channel + " :Channel key already set" + CRLF);
}

// 471 ERR_CHANNELISFULL
inline std::string channelIsFull(const std::string& nick, const std::string& channel){
	return (":" + std::string(SERVER) + " 471 " + nick + " " + channel + " :Cannot join channel (+l)" + CRLF);
}

// 472 ERR_UNKNOWNMODE
inline std::string unknownMode(const std::string& nick, const std::string& unknown_mode, const std::string& channel){
	return (":" + std::string(SERVER) + " 472 " + nick + " " + unknown_mode + " :is unknown mode char to me for " + channel + CRLF);
}

// 473 ERR_INVITEONLYCHAN
inline std::string inviteOnlyChan(const std::string& nick, const std::string& channel){
	return (":" + std::string(SERVER) + " 473 " + nick + " " + channel + " :Cannot join channel (+i)" + CRLF);
}

// 475 ERR_BADCHANNELKEY
inline std::string badChannelKey(const std::string& nick, const std::string& channel){
	return (":" + std::string(SERVER) + " 475 " + nick + " " + channel + " :Cannot join channel (+k)" + CRLF);
}

// 478 ERR_INVITE_SYNTAX
/**
 * @brief Any command requiring 'chanop' privileges (such as MODE messages) must return this
 * error if the client making the attempt is not a chanop on the specified channel
 */
inline std::string InviteSyntaxErr(const std::string& nick){
	return (":" + std::string(SERVER) + " 478 " + nick + ":Invalid Invite command" + CRLF);
}

// 479 ERR_BADCHANNELNAME
inline std::string badChannelName(const std::string& nick, const std::string& channel){
	return (":" + std::string(SERVER) + " 479 " + nick + " " + channel + " :Illegal channel name" + CRLF);
}


// 482 ERR_CHANOPRIVSNEEDED
/**
 * @brief Any command requiring 'chanop' privileges (such as MODE messages) must return this
 * error if the client making the attempt is not a chanop on the specified channel
 */
inline std::string ChanoPrivsNeeded(const std::string& nick, const std::string& channel){
	return (":" + std::string(SERVER) + " 482 " + nick + " " + channel + " :You're not channel operator" + CRLF);
}

// 501 ERR_UMODEUNKNOWNFLAG
inline std::string umodeUnknownFlag(const std::string& nick) {
    return (":" + std::string(SERVER) + " 501 " + nick + " :Unknown MODE flag" + CRLF);
}

// 502 ERR_USERSDONTMATCH
inline std::string usersDontMatch(const std::string& nick) {
    return (":" + std::string(SERVER) + " 502 " + nick + " :Cannot change mode for other users" + CRLF);
}

// 696 ERR_INVALIDMODEPARAM
// 696 is a non-standard but widely adopted IRC numeric that means: "You tried to apply a mode
// (+k, +l, etc.) but the parameter you gave is invalid for that mode."
/**
 * @brief Returned when a user provides an invalid parameter for a channel mode.
 * Commonly used for +k (key), +l (limit), etc.
 */
inline std::string InvalidModeParamErr(const std::string& nick, const std::string& channel,
char mode, const std::string& param, const std::string& reason) {
    return ":" + std::string(SERVER) + " 696 " + nick + " " + channel + " " + mode + " " + param + " :" + reason + CRLF;
}
