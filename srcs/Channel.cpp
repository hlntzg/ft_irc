/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hutzig <hutzig@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 14:42:06 by hutzig            #+#    #+#             */
/*   Updated: 2025/04/30 14:42:07 by hutzig           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(){
}

Channel::Channel(std::string& name, Client& user) : channel_name_(name) {
    channel_passwd_ = "";
    channel_topic_ = "";
    channel_invite_only_ = false;
    channel_restric_topic_ = false;
    channel_with_passwd_ = false;
    channel_user_limit_ = false;
    user_limit_ = USER_MAX;
    addNewUser(user);
    addNewOperator(user);
}

Channel::~Channel(){
}

std::string Channel::getName(){
    return channel_name_;
}

std::string Channel::getPassword(){
    return channel_passwd_;
}

std::string Channel::getTopic(){
    return channel_topic_;
}

bool    Channel::getInviteMode(){
    return channel_invite_only_;
}

bool    Channel::getTopicMode(){
    return channel_restric_topic_;
}

bool    Channel::getPasswdMode(){
    return channel_with_passwd_;
}

bool    Channel::getLimitMode(){
    return channel_user_limit_;
}

size_t  Channel::channelSize(){
    return users_.size();
}

// Channel's mode:
void    Channel::setInviteOnly(){
    channel_invite_only_ = true;
}

void    Channel::unsetInviteOnly(){
    channel_invite_only_ = false;
}

void    Channel::setTopicRestrictions(){
    channel_restric_topic_ = true;
}

void    Channel::unsetTopicRestrictions(){
    channel_restric_topic_ = false;
}

void    Channel::addNewTopic(const std::string& topic){
    channel_topic_ = topic;
}

void    Channel::setPassword(){
    channel_with_passwd_ = true;
}

void    Channel::unsetPassword(){
    channel_with_passwd_ = false;
}

void    Channel::addNewPassword(const std::string& key){
    channel_passwd_ = key;
    channel_with_passwd_ = true;
}

void    Channel::setLimit(){
    channel_user_limit_ = true;
}

void    Channel::unsetLimit(){
    channel_user_limit_ = false;
    user_limit_ = USER_MAX;
}

void    Channel::addLimit(int limit){
    if (channel_user_limit_ && limit < USER_MAX)
		user_limit_ = limit;
}

// Regular user and Channel Operator:
void    Channel::addNewUser(Client& user){
    // if (&user == nullptr) { // in C++, a reference (Client& user) can never be null.
	// 	Logger::log(Logger::ERROR, "Attempted to add null user to " + channel_name_);
	// 	return;
	// }
    int fd = user.getSocketFD();
	if (users_.find(&user) == users_.end()){
		users_.insert(&user);
		Logger::log(Logger::INFO, "User " + std::to_string(fd) + " joined " + channel_name_);
	}
	else
		Logger::log(Logger::INFO, "User " + std::to_string(fd) + " is already in " + channel_name_);
}

void    Channel::removeUser(Client& user){
    int fd = user.getSocketFD();
    if (users_.erase(&user) > 0){
        operators_.erase(&user);
        invited_users_.erase(&user);
		Logger::log(Logger::INFO, "User " + std::to_string(fd) + " left " + channel_name_);
	}
	else
		Logger::log(Logger::WARNING, "Attempted to remove user " + std::to_string(fd) + " who is not in " + channel_name_);
}

void    Channel::addNewOperator(Client& user){
    operators_.insert(&user);
}

void    Channel::removeOperator(Client& user){
    operators_.erase(&user);
}

bool    Channel::isChannelUser(Client& user){
    return users_.find(&user) != users_.end();
}

bool    Channel::isChannelOperator(Client& user){
    return operators_.find(&user) != operators_.end();
}

bool    Channel::isInvitedUser(Client& user){
    return invited_users_.find(&user) != invited_users_.end();
}

// General:
void    Channel::notifyChannelUsers(Client& target, std::string& msg){
    for (auto user : users_){
        if (user == &target) // do not notify target
            continue ;
        Server::responseToClient(*user, msg);
    }
}

// Channel Operators commands:

/**
 * KICK <channel> <nick> [<reason>]
 * 
 * Verifies that:
 * - The source user is in the channel.
 * - The source user is a channel operator.
 * - The target user exists and is in the channel.
 * Sends correct error replies (401, 403, 441, 442, 476, 482).
 * Prevents kicking IRC operators (optional but reasonable).
 * Broadcasts a kick message to the channel.
 * Logs the kick action.
 * */
void    Channel::kickUser(Client& user, Client& target, const std::string& reason){
    if (channel_name_.empty() || channel_name_[0] != '#'){
        Server::responseToClient(user, "ERR_BADCHANMASK (476) or ERR_NOSUCHCHANNEL (403)"); 
        return ;
    }
    if (!isChannelUser(user)){
        Server::responseToClient(user, "ERR_NOTONCHANNEL (442)");
        return ;
    }
    if (!isChannelOperator(user)){
        Server::responseToClient(user, "ERR_CHANOPRIVSNEEDED (482)");
        return ;
    }
    if (!isChannelUser(target)){
        Server::responseToClient(user, "ERR_USERNOTINCHANNEL (441)");
        return ;
    }
    // // Optional: Protect IRC operators
    // if (isChannelOperator(target)){
    //     std::string errCannotKickOperator = ":" + SERVER + " " + channel_name_ + " :Cannot kick an IRC operator\r\n";
    //     Server::responseToClient(user, errCannotKickOperator);
    //     return ;
    // }

    std::string message = "message from kickUser()"; // get this message from replyKick() with specific arguments
    // Notify all users in the channel except the target
    notifyChannelUsers(target, message); 
    // Notify the target separately (optional)
    Server::responseToClient(target, message);

    Logger::log(Logger::INFO, "User " + target.getNick() + " was kicked from channel " + channel_name_ + " by " + user.getNick());

    removeUser(target);
}

// // // Invite a user to join an invite-only channel
// // // Check if user is an operator.
// // // Ensure target is NOT in the channel.
void    Channel::inviteUser(Client& user, Client& target) {
    if (!isChannelUser(user)){
        Server::responseToClient(user, "ERR_NOTONCHANNEL (442)");
        return ;
    }
    if (getInviteMode() && !isChannelOperator(user)){
        Server::responseToClient(user, "ERR_CHANOPRIVSNEEDED (482)");
        return ;
    }
    if (isChannelUser(target)){
        Server::responseToClient(user, "ERR_USERONCHANNEL (443)");
        return ;
    }

    invited_users_.insert(&target);

    std::string message = "message from inviteUser()"; // get this message from replyInvite() with specific arguments
    Server::responseToClient(user, message); // RPL_INVITING (341)
    // Send the actual INVITE message to target
    std::string inviteMessage = ":" + user.getNick() + " INVITE " + target.getNick() + " " + channel_name_ + "\r\n";
    Server::responseToClient(target, inviteMessage);

    // RPL_AWAY (301) — if the target is away, notify the inviter
//     std::string targetAwayMessage = "rplAway(SERVER_NAME, inviter.getNick(), target.getNick(), target.getAwayMsg())";
//     if (!target.getAwayMsg().empty()) {
//         Server::responseToClient(user, targetAwayMessage);
//     }
}

/**
 * TOPIC <channel> [:topic]
 */
// // RPL_NOTOPIC (331)
// // RPL_TOPIC (332)
// // RPL_TOPICTIME (333)
// // ERR_NOSUCHCHANNEL (403)
// // ERR_NOTONCHANNEL (442)
// // ERR_NEEDMOREPARAMS (461)
// // ERR_CHANOPRIVSNEEDED (482)
void    Channel::topic(Client& user, const std::string& topic){
    if (!isChannelUser(user)){
        Server::responseToClient(user, "ERR_NOTONCHANNEL (442)");
        return ;
    }
    if (topic.empty()) {
        if (channel_topic_.empty())
            Server::responseToClient(user, "RPL_NOTOPIC (331)");
        else
            Server::responseToClient(user, "RPL_TOPIC (332)");
        return ;
    }
    if (getTopicMode() && !isChannelOperator(user)) {
        Server::responseToClient(user, "ERR_CHANOPRIVSNEEDED (482)");
        return ;
    }

    // Unset topic
    if (topic == ":") {
        addNewTopic("");  // or channel_topic_.clear();
        Logger::log(Logger::INFO, "User " + user.getNick() + " cleared topic in channel " + channel_name_);
        return;
    }

    addNewTopic(topic);

    std::string message = "message from rplTopic()";
    notifyChannelUsers(user, message);
    Server::responseToClient(user, message);

    Logger::log(Logger::INFO, "User " + user.getNick() + " set new topic in channel " + channel_name_ + ": " + topic);
}
