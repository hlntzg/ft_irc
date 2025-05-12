/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 14:42:06 by hutzig            #+#    #+#             */
/*   Updated: 2025/05/09 14:56:36 by jingwu           ###   ########.fr       */
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


// Channel's mode:
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
bool    Channel::addNewUser(Client& user){
    int fd = user.getSocketFd();

    if (channel_invite_only_) {
        if (!isInvitedUser(user)) {
            // 473 ERR_INVITEONLYCHAN
            Server::responseToClient(user, inviteOnlyChan(user.getNick(), channel_name_));
            return (false);
        }
        invited_users_.erase(&user);
    }
    if (channel_user_limit_ && channelSize() >= user_limit_) {
        Logger::log(Logger::WARNING, "User " + std::to_string(fd) + " cannot join " + channel_name_ + ": channel is full");
        // 471 ERR_CHANNELISFULL
        Server::responseToClient(user, channelIsFull(user.getNick(), channel_name_));
        return (false);
    }
	if (users_.find(&user) == users_.end()){
		users_.insert(&user);
		Logger::log(Logger::INFO, "User " + std::to_string(fd) + " joined " + channel_name_);
        return(true);
	}
	else {
		Logger::log(Logger::INFO, "User " + std::to_string(fd) + " is already in " + channel_name_);
        return (false);
    }
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

// General:

/**
 * @brief Sends a message to all users in the channel except the target user.
 *
 * @param target The user to exclude from receiving the message.
 * @param msg The message to send to the other users in the channel.
 */
void    Channel::notifyChannelUsers(Client& target, std::string& msg){
    for (auto user : users_){
        if (user == &target) // do not notify target
            continue ;
        Server::responseToClient(*user, msg);
    }
}

bool    Channel::isEmptyChannel(){
    return users_.empty();
}

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

// Channel Operators commands:

/**
 * Added by Sherry
 * More explaination about KICK command:
 * Exmaple 1: KICK #chann1,#chann2 user1,user2
 *          It means: - Kick user1 from #chann1
 *                    - Kick user2 from #chann2
 *
 * Example 2: KICK #chann1,#chann2 user1
 *          It means: Kick user1 from #chann1 and #chann2
 *
 * Example 3: KICK #chann1 user1,user2
 *          It means: kick user1 and user2 from chann1
 */

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
// void    Channel::kickUser(Client& user, Client& target, const std::string& reason){
//     if (channel_name_.empty() || channel_name_[0] != '#'){
//         //"ERR_BADCHANMASK (476) or ERR_NOSUCHCHANNEL (403)"
//         Server::responseToClient(user, errNoSuchChannel(user.getNick(), channel_name_));
//         return ;
//     }
//     if (!isChannelUser(user)){
//         //"ERR_NOTONCHANNEL (442)"
//         Server::responseToClient(user, notOnChannel(user.getNick(), channel_name_));
//         return ;
//     }
//     if (!isChannelOperator(user)){
//         //"ERR_CHANOPRIVSNEEDED (482)"
//         Server::responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name_));
//         return ;
//     }
//     if (!isChannelUser(target)){
//         //"ERR_USERNOTINCHANNEL (441)"
//         Server::responseToClient(user, userNotInChannel(user.getNick(), target.getNick(), channel_name_));
//         return ;
//     }
//     // // Optional: Protect IRC operators
//     // if (isChannelOperator(target)){operators_
//     //     std::string errCannotKickOperator = ":" + SERVER + " " + channel_name_ + " :Cannot kick an IRC operator\r\n";
//     //     Server::responseToClient(user, errCannotKickOperator);
//     //     return ;
//     // }

//     std::string message = "message from kickUser()"; // get this message from replyKick() with specific arguments
//     // Notify all users in the channel except the target
//     notifyChannelUsers(target, message);
//     // Notify the target separately (optional)
//     Server::responseToClient(target, message);

//     Logger::log(Logger::INFO, "User " + target.getNick() + " was kicked from channel " + channel_name_ + " by " + user.getNick());

//     removeUser(target);
// }

/**
 * INVITE <nickname> <channel>
 *
 * @brief Implements the IRC INVITE command: Invites a user to join an invite-only channel.
 *
 * Grants one-time access to the channel by adding the target user to the invited users list.
 * This access is cleared once the target successfully joins the channel.
 *
 * This method performs the following checks:
 * - Verifies the inviter is currently in the channel.
 * - Ensures the inviter is a channel operator if the channel is in invite-only mode.
 * - Ensures the target user is not already in the channel.
 *
 * If all conditions are met, the target user is added to the invited list, and
 * an INVITE message is sent to them.
 *
 * @param user The client sending the invite (the inviter).
 * @param target The client being invited to the channel.
 */
// void    Channel::inviteUser(Client& user, Client& target) {
//     if (!isChannelUser(user)){
//         // ERR_NOTONCHANNEL (442)
//         Server::responseToClient(user, notOnChannel(user.getNick(), channel_name_));
//         return ;
//     }
//     if (getInviteMode() && !isChannelOperator(user)){
//         // ERR_CHANOPRIVSNEEDED (482)
//         Server::responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name_));
//         return ;
//     }
//     if (isChannelUser(target)){
//         // ERR_USERONCHANNEL (443)
//         Server::responseToClient(user, userOnChannel(user.getNick(), target.getNick(), channel_name_));
//         return ;
//     }

//     invited_users_.insert(&target);

//     // RPL_INVITING (341)
//     Server::responseToClient(user, Inviting(user.getNick(), channel_name_, target.getNick()));
//     // Send the actual INVITE message to target
//     std::string inviteMessage = ":" + user.getNick() + " INVITE " + target.getNick() + " " + channel_name_ + "\r\n";
//     Server::responseToClient(target, inviteMessage);

//     // Not sure if we will implement that:
//     // RPL_AWAY (301) — if the target is away, notify the inviter
//     // std::string targetAwayMessage = "rplAway(SERVER_NAME, inviter.getNick(), target.getNick(), target.getAwayMsg())";
//     // if (!target.getAwayMsg().empty()) {
//     //     Server::responseToClient(user, targetAwayMessage);
//     // }
// }

/**
 * TOPIC <channel> [:topic]
 *
 * @brief Implements the IRC TOPIC command: Handles viewing or setting the topic of the channel.
 *
 * Validates that the user is a member of the channel.
 * Depending on the parameters and user privileges:
 * - If no topic is provided, responds with the current topic (or lack thereof).
 * - If the channel is in topic-protected mode (+t), only operators can set the topic.
 * - If the topic parameter is ":", the current topic is cleared.
 * - Otherwise, sets the new topic and notifies all channel users.
 *
 * @param user The client requesting or changing the topic.
 * @param topic The new topic to be set, or empty to query the current topic.
 */
// void    Channel::topic(Client& user, const std::string& topic){
//     if (!isChannelUser(user)){
//         //"ERR_NOTONCHANNEL (442)"
//         Server::responseToClient(user, notOnChannel(user.getNick(), channel_name_));
//         return ;
//     }
//     if (topic.empty()) {
//         if (channel_topic_.empty())
//             //"RPL_NOTOPIC (331)"
//             Server::responseToClient(user, NoTopic(user.getNick(), channel_name_));
//         else
//             //"RPL_TOPIC (332)"
//             Server::responseToClient(user, Topic(user.getNick(), channel_name_, topic));
//         return ;
//     }
//     if (getTopicMode() && !isChannelOperator(user)) {
//         //"ERR_CHANOPRIVSNEEDED (482)"
//         Server::responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name_));
//         return ;
//     }

//     // Unset topic
//     if (topic == ":") {
//         addNewTopic("");  // or channel_topic_.clear();
//         Logger::log(Logger::INFO, "User " + user.getNick() + " cleared topic in channel " + channel_name_);
//         return;
//     }

//     addNewTopic(topic);

//     //332 RPL_TOPIC
//     std::string message = Topic(user.getNick(), channel_name_, topic);
//     notifyChannelUsers(user, message);
//     Server::responseToClient(user, message);

//     Logger::log(Logger::INFO, "User " + user.getNick() + " set new topic in channel " + channel_name_ + ": " + topic);
// }

/**
 * MODE <channel> [modes-flags [mode-params]]
 *
 *
| Mode | Name             | Description                                                       |
| ---- | ---------------- | ----------------------------------------------------------------- |
| `+i` | Invite-only      | Only invited users may join the channel.                          |
| `+t` | Topic protection | Only channel operators can set the topic.                         |
| `+k` | Key (password)   | Users must supply the correct password (key) to join the channel. |
| `+o` | Operator         | Gives or removes operator status from a user in the channel.      |
| `+l` | Limit            | Sets a user limit for the number of clients in the channel.       |
 *
 * Only channel operators can set/unset mode
 * +k and +l require arguments when setting, not when unsetting.
 * +o always requires a nickname argument (set and unset)
 *
 * When using multiple mode flags in a single MODE command, the parameters must be given
 * in the order that the flags require them, and only flags that need arguments will
 * consume arguments from the list.
 * In IRC, mode strings must be part of a single flag sequence, and you cannot have + and -
 * for the same flag type mixed with others in a single string.
 *
 * ERR_NEEDMOREPARAMS (461) if argument is missing
 * ERR_CHANOPRIVSNEEDED (482) if user isn’t a channel operator
 * ERR_UNKNOWNMODE (472) for unsupported mode
 */
// void    Channel::mode(Server& server, Client& user, const std::string& mode_flags, std::vector<std::string> args){
//     if (!isChannelUser(user)) {
//         //ERR_NOTONCHANNEL (442)
//         Server::responseToClient(user, notOnChannel(user.getNick(), channel_name_));
//         return;
//     }
//     if (mode_flags.empty()) {
//         std::string mode_status = "+";
//         if (channel_invite_only_)
//             mode_status += "i";
//         if (channel_restric_topic_)
//             mode_status += "t";
//         if (channel_with_passwd_)
//             mode_status += "k";
//         if (channel_user_limit_)
//             mode_status += "l";
//         // RPL_CHANNELMODEIS (324)
//         Server::responseToClient(user, ChannelModeIs(user.getNick(), channel_name_, mode_status));
//         return;
//     }

//     if (!isChannelOperator(user)) {
//         // ERR_CHANOPRIVSNEEDED (482)
//         Server::responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name_));
//         return;
//     }

//     bool adding = true;
//     size_t arg_index = 0;
//     std::string update_modes;

//     for (size_t i = 0; i < mode_flags.size(); ++i) {
//         char c = mode_flags[i];

//         if (c == '+') {
//             adding = true;
//         }
//         else if (c == '-') {
//             adding = false;
//         }
//         else if (c == 'i') {
//             adding ? setInviteOnly() : unsetInviteOnly();
//             update_modes += (adding ? "+i" : "-i");
//         }
//         else if (c == 't') {
//             adding ? setTopicRestrictions() : unsetTopicRestrictions();
//             update_modes += (adding ? "+t" : "-t");
//         }
//         else if (c == 'k') {
//             if (adding) {
//                 if (arg_index >= args.size()) {
//                     Server::responseToClient(user, needMoreParams("MODE"));
//                     return;
//                 }
//                 addNewPassword(args[arg_index++]);
//                 setPassword();
//                 update_modes += "+k";
//             }
//             else {
//                 unsetPassword();
//                 update_modes += "-k";
//             }
//         }
//         else if (c == 'l') {
//             if (adding) {
//                 if (arg_index >= args.size()) {
//                     Server::responseToClient(user, needMoreParams("MODE"));
//                     return;
//                 }
//                 addLimit(std::stoi(args[arg_index++]));
//                 setLimit();
//                 update_modes += "+l";
//             }
//             else {
//                 unsetLimit();
//                 update_modes += "-l";
//             }
//         }
//         else if (c == 'o') {
//             if (arg_index >= args.size()) {
//                 Server::responseToClient(user, needMoreParams("MODE"));
//                 return;
//             }
//             std::string nick = args[arg_index++];
//             Client* target = Server::getClientByNick(nick);
//             if (!target || !isChannelUser(*target)) {
//                 Server::responseToClient(user, userNotInChannel(user.getNick(), nick, channel_name_));
//                 continue;
//             }
//             if (adding) {
//                 addNewOperator(*target);
//                 update_modes += "+o";
//             }
//             else {
//                 removeOperator(*target);
//                 update_modes += "-o";
//             }
//         }
//         else {
//             Server::responseToClient(user, unknownMode(user.getNick(), std::to_string(c)));
//         }
//     }

//     // Notify all users of the mode change
//     std::string message = ":" + user.getNick() + " MODE " + channel_name_ + " " + update_modes;
//     for (size_t i = 0; i < arg_index; ++i) {
//         message += " " + args[i];
//     }
//     message += "\r\n";

//     notifyChannelUsers(user, message);
//     Server::responseToClient(user, message);
// }
