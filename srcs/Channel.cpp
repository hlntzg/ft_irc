/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 14:42:06 by hutzig            #+#    #+#             */
/*   Updated: 2025/05/20 14:34:11 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string& name, Client& user) : channel_name_(name) {
    channel_passwd_ = "";
    channel_topic_ = "";
    channel_invite_only_ = false;
    channel_restric_topic_ = false;
    channel_with_passwd_ = false;
    channel_user_limit_ = false;
    user_limit_ = 0;
    addNewUser(user);
    addNewOperator(user);
}

Channel::~Channel(){
}

const std::string& Channel::getName() const{
    return channel_name_;
}

const std::string& Channel::getPassword() const{
    return channel_passwd_;
}

const std::string& Channel::getTopic() const{
    return channel_topic_;
}

bool    Channel::getInviteMode() const{
    return channel_invite_only_;
}

bool    Channel::getTopicMode() const{
    return channel_restric_topic_;
}

bool    Channel::getPasswdMode() const{
    return channel_with_passwd_;
}

bool    Channel::getLimitMode() const{
    return channel_user_limit_;
}

size_t  Channel::channelSize(){
    return users_.size();
}

const std::unordered_set<Client*>&  Channel::getChannelUsers() const{
    return users_;
}

// Channel's set mode:
void    Channel::setInviteOnly(){
    channel_invite_only_ = true;
}

void    Channel::unsetInviteOnly(){
    channel_invite_only_ = false;
}

void    Channel::addNewInviteUser(Client& user){
    invited_users_.insert(&user);
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
}

void    Channel::setLimit(){
    channel_user_limit_ = true;
}

void    Channel::unsetLimit(){
    channel_user_limit_ = false;
}

void    Channel::addLimit(int limit){
	user_limit_ = limit;
}

// Regular user and Channel Operator:

/**
 * @brief Adds a user to the channel, handling mode restrictions.
 *
 * This function attempts to add the given user to the channel.
 * It enforces the following channel modes:
 * - `+i` (invite-only): The user must be in the invited users list. Once joined, the user is removed from that list.
 * - `+l` (user limit): The number of users in the channel must not exceed the set limit.
 *
 * If the user cannot join due to mode restrictions, the appropriate numeric error replies are sent.
 * If the user is already in the channel, a log entry is made but no changes are applied.
 *
 * @param user The client attempting to join the channel.
 */
void    Channel::addNewUser(Client& user){
    int fd = user.getSocketFd();

    if (channel_invite_only_) {
        invited_users_.erase(&user);
    }
    users_.insert(&user);
    Logger::log(Logger::INFO, "User " + std::to_string(fd) + " joined " + channel_name_);
}

/**
 * @brief Removes a user from the channel.
 *
 * This function removes the given user from the set of current channel members.
 * If the user is also listed as an operator or in the invite list, those entries are cleared as well.
 *
 * @param user The client to be removed from the channel.
 */
void    Channel::removeUser(Client& user){
    int fd = user.getSocketFd();
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

/**
 * @brief Check if channel has operators. Using when a member leave
 * or remove from a channel, if there is no operator in the channel,
 * the server needs to grand the operator privilege to the user who joined
 * channel earliest
 */
bool    Channel::isThereOperatorInChannel(){
    if (operators_.empty() == true){
        return false;
    } else {
        return true;
    }
}

// General:

/**
 * @brief Sends a message to all users in the channel except the target user.
 *
 * @param target The user to exclude from receiving the message.
 * @param msg The message to send to the other users in the channel.
 */
void    Channel::notifyChannelUsers(Client& target, const std::string& msg){
    for (auto user : users_){
        if (user == &target) // do not notify target
            continue ;
        Server::responseToClient(*user, msg);
    }
}

bool    Channel::isEmptyChannel(){
    return users_.empty();
}

bool    Channel::isFullChannel(){
    if (channelSize() >= user_limit_)
        return true;
    return false;
}

/**
 * @brief Adds a user to the specified user type list in the channel.
 *
 * @param user The user to add.
 * @param type The user type (regular, operator, or invited).
 */
void    Channel::insertUser(std::shared_ptr<Client> user, USERTYPE type){
    if (type == USERTYPE::REGULAR){
        addNewUser(*user);
    }
    if (type == USERTYPE::OPERATOR){
        addNewOperator(*user);
    }
    if (type == USERTYPE::INVITE){
        addNewInviteUser(*user);
    }
}

/**
 * @brief Checks if a user belongs to a specific user type list in the channel.
 *
 * @param user The user to check.
 * @param type The channel list to check the user type (regular, operator, or invited).
 * @return True if the user is in the specified list, false otherwise.
 */
bool    Channel::isUserInList(Client& user, USERTYPE type){
    if (type == USERTYPE::REGULAR){
        if (isChannelUser(user))
            return (true);
    }
    if (type == USERTYPE::OPERATOR){
        if (isChannelOperator(user))
            return (true);
    }
    if (type == USERTYPE::INVITE){
        if (isInvitedUser(user))
            return (true);
    }
    return (false);
}

/**
 * @brief using to get the firt user saving in the user container. Used when
 * the last operator is removed from the channel, tranfer the operator
 * role to the first user
 */
Client*     Channel::getTheFirstUser() const{
    if (users_.empty() == false){
        return *(users_.begin());
    } else {
        return nullptr;
    }
}


// for testing only
#if 0
void    Channel::printChannelInfo() const{
    std::cout << "Channel Info:\n";
    std::cout << "  name:" << channel_name_ << std::endl;
    std::cout << "  password:" << channel_passwd_ << std::endl;
    std::cout << "  topic:" << channel_topic_ << std::endl;
    std::cout << "  invite_only:" << (channel_invite_only_ ? "true" : "false") << std::endl;
    std::cout << "  restric_topic:" << (channel_restric_topic_ ? "true" : "false") << std::endl;
    std::cout << "  with_password_mode:" << (channel_with_passwd_ ? "true" : "false") << std::endl;
    std::cout << "  user_limit_mode:" << (channel_user_limit_ ? "true" : "false") << std::endl;
    std::cout << "  user_limit:" << std::to_string(user_limit_) << std::endl;
}

void    Channel::printUsers(USERTYPE type) const{
    std::unordered_set<Client*> list;
    switch (type){
    case USERTYPE::REGULAR:
        std::cout << "channel users are: ";
         list = users_;
        break;
    case USERTYPE::OPERATOR:
        std::cout << "channel operators are: ";
        list = operators_;
        break;
    case USERTYPE::INVITE:
        std::cout << "channel invitees are: ";
        list = invited_users_;
        break;
    }
    for (const auto& user : list){
        std::cout << user->getNick() << ", ";
    }
    std::cout << std::endl;
}
#endif
