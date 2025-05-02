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
    // check stuff before
    users_.insert(&user);
}

void    Channel::removeUser(Client& user){
    operators_.erase(&user);
    users_.erase(&user);
}

void    Channel::addNewOperator(Client& user){
    operators_.insert(&user);
}

void    Channel::removeOperator(Client& user){
    operators_.erase(&user);
}

bool    Channel::isChannelOperator(Client& user){
    return operators_.find(&user) != operators_.end();
}

// Channel Operators commands:

// ERR_NOSUCHNICK (401)
// ERR_NOSUCHCHANNEL (403)
// ERR_USERNOTINCHANNEL (441)
// ERR_NOTONCHANNEL (442)
// ERR_BADCHANMASK (476)
// ERR_CHANOPRIVSNEEDED (482)
// ERR_CHANOPRIVSNEEDED (482)

// Check if user is an operator.
// Ensure target is in the channel.
// Remove target from the user set.
// Possibly notify both users (depending on how your client/server logic is structured).
// void    Channel::kickUser(Client& user, Client& target, const std::string& reason){

// }

// // Invite a user to join an invite-only channel
// // Check if user is an operator.
// // Ensure target is NOT in the channel.
// void    Channel::inviteUser(Client& user, Client& target) {

// }