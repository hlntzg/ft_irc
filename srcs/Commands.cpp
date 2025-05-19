#include "Server.hpp"

// Here will implements all the related commands functions


/**
 * @brief PASS is used to set a 'connection password'
 *
 * @param msg: the command full message object
 * @param cli: the command operator
 *
 * Numeric repiles:
 *  ERR_NEEDMOREPARAMS(461)     ERR_ALREADYREGISTRED(462)
 */
void	Server::passCommand(Message& msg, Client& cli){
	std::string	nick = cli.getNick();
	if (msg.getParameters().empty()){
		responseToClient(cli, needMoreParams("PASS"));
		return;
	} else if (cli.isRegistered()){
		responseToClient(cli, alreadyRegistred(nick));
		return ;
	}
	// vector.at() is safer than vector.at[0] to access the element.
	// at() will do the bounds checking
	std::string	password = msg.getParameters().at(0);
	if (isPasswordMatch(password) == false){
		Logger::log(Logger::ERROR, "Password doesn't match");
		responseToClient(cli, passwdMismatch(nick));
	}
	cli.setPassword(password);
	attempRegisterClient(cli);
	Logger::log(Logger::DEBUG, "Set " + nick + "'s password to " + password);
}

bool	Server::isPasswordMatch(const std::string& password){
	return (serv_passwd_ == password);
}

void	Server::attempRegisterClient(Client& cli){
	if (cli.getPassword().size() == 0 || cli.getNick().size() == 0 ||
		cli.getUsername().size() == 0){
		return;
	}
	const std::string& nick = cli.getNick();
	if (isNickInUse(nick, &cli)){
		responseToClient(cli, nickNameInUse(nick, nick));
		return;
	}
	if (cli.getPassword() != serv_passwd_){
		responseToClient(cli, passwdMismatch(nick));
		return;
	}
	cli.setRegistrationStatus(true);
	responseToClient(cli, rplWelcome(nick, cli.getPrefix()));
	responseToClient(cli,rplYourHost(nick));
	responseToClient(cli,rplCreated(nick));
	responseToClient(cli,rplMyInfo(nick));
	std::cout << "Print from attempRegiser function\n";
	cli.printInfo(); // for testing only
}

/**
 * @brief Checking if the passed nick is in use
 *
 * @param nick The nickname to be checked
 *
 * @return
 * - `true`: The nickname is already in use
 * - `false`: The nickname is available
 */
bool Server::isNickInUse(const std::string& nick, const Client* requesting_client){
	for (auto& [fd, user] : clients_){
		// why do we added second condition here??
		if (user->getNick() == nick && user->isRegistered() && user.get() != requesting_client){
			return true;
		}
	}
	return false;
}

/**
 * @brief Using to set a nick for a client
 * syntax:
 *    nickname = ( letter / special ) *8( letter / digit / special / "-" )
 * special    =  %x5B-60 / %x7B-7D
 *                 ; "[", "]", "\", "`", "_", "^", "{", "|", "}"
 */
void	Server::nickCommand(Message& msg, Client& cli){
	std::vector<std::string>	params = msg.getParameters();
	std::string	usr_nick = cli.getNick().empty() ? "*" : cli.getNick();
	// 1. should contain at least one parameter
	if (params.size() == 0){
		responseToClient(cli, nonNickNameGiven(usr_nick));
		Logger::log(Logger::ERROR, "no nickname is given");
		return;
	}
	const std::string& nick = params.at(0);
	// 2. nickname length checking (between 1~9 characters)
	if (nick.size() > 20 || nick.size() < 1){
		responseToClient(cli, erroneusNickName(usr_nick));
		Logger::log(Logger::ERROR, "nickname is too long");
		return;
	}
	// 3. nickname inuse checking
	if (isNickInUse(nick, &cli) == true){
		responseToClient(cli, nickNameInUse(usr_nick, nick));
		Logger::log(Logger::ERROR, "nickname is in use");
		return;
	}
	// 4. first letter checking, should be just letter or special character
	if (!isalpha(static_cast<unsigned char>(nick.at(0)))
			&& std::string(SPECIAL_CHARS_NAMES).find(nick.at(0)) == std::string::npos){
		responseToClient(cli, erroneusNickName(usr_nick));
		Logger::log(Logger::ERROR, "nickname's first letter is invalid");
		return;
	}
	// 5. checking invalid characters in the rest nickname
	for (auto c : nick){
		if (!isalnum(static_cast<unsigned char>(c))
			&& std::string(SPECIAL_CHARS_NAMES).find(c) == std::string::npos){
			responseToClient(cli, erroneusNickName(usr_nick));
			Logger::log(Logger::ERROR, "nickname contains invalid characters");
			return;
		}
	}
	const std::string&	old_prefix = cli.getPrefix();
	cli.setNick(nick);
	if (cli.isRegistered() == false){ // first time registeration
		attempRegisterClient(cli);
	} else{ // reset nickname
		responseToClient(cli, rplResetNick(old_prefix, nick));
		Logger::log(Logger::INFO, "Send reset nick notification to userself");
		// notice channels users who are joined the same channel with the user
		for (const auto& [name, channel_ptr] : channels_){
			if (channel_ptr->isUserInList(cli, USERTYPE::REGULAR) == true){
				std::string	message = rplResetNick(old_prefix, nick);
				channel_ptr->notifyChannelUsers(cli, message);
				Logger::log(Logger::INFO, "Send reset nick notification to channel users");
			}
		}
	}
}

/**
 * @brief The USER command is used at the beginning of connection to specify
 * the username, hostname and realname of a new user.
 *
 *  Syntax:
 *   USER <user> <mode> <unused> <realname>
 *
 *  The <realname> may contain space characters.
 *
 */
void	Server::userCommand(Message& msg, Client& cli){
	std::vector<std::string>	params = msg.getParameters();

	if (params.size() < 3 || msg.getTrailing().empty()){
		responseToClient(cli, needMoreParams("USER"));
		return;
	}
	const std::string& username = params.at(0);
	const std::string& realname = msg.getTrailing();

	for (auto c : username){
		if (!isalnum(static_cast<unsigned char>(c))
			&& std::string(SPECIAL_CHARS_NAMES).find(c) == std::string::npos){
			responseToClient(cli, erroneusNickName(""));
			Logger::log(Logger::INFO, "username is invalid");
			return;
		}
	}

	for (auto c : realname){
		if (!isalnum(static_cast<unsigned char>(c))
			&& std::string(SPECIAL_CHARS_NAMES).find(c) == std::string::npos &&
			c != ' '){
			responseToClient(cli, erroneusNickName(""));
			Logger::log(Logger::INFO, "Realname is invalid");
			return;
		}
	}
	cli.setUsername(username);
	cli.setRealname(realname);
	cli.setHostname(params.at(1));
	cli.setServername(params.at(2));
	if (cli.isRegistered() == false){
		attempRegisterClient(cli);
	}
}

void Server::quitCommand(Message& msg, Client& cli){
	std::string reason = "Client quit";
	if (!msg.getParameters().empty()){
		reason = msg.getParameters()[0];
	}
	removeClient(cli, reason);
}

/**
 * PART <channel>{,<channel>} [:message]
 *
 * @brief Handles the IRC PART command, allowing a user to leave one or more channels.
 *
 * This function removes the user from each specified channel (if they are a member),
 * and notifies all users in the channel of the departure. If a channel becomes empty
 * after the departure, it is deleted from the server.
 *
 * Syntax:
 *   - PART #chan1,#chan2 [:optional parting message]
 *
 * Behavior:
 *   - Sends a PART message to all users in the channel
 *   - If the channel becomes empty, it is destroyed
 *
 * Error Replies:
 *   - ERR_NOSUCHCHANNEL (403) – If the specified channel does not exist
 *   - ERR_NOTONCHANNEL (442) – If the user is not a member of the channel
 *   - ERR_TOOMANYTARGETS (407) – If too many channels are specified in one command
 *
 * @param msg  The parsed Message object containing the list of channels and optional message.
 * @param cli  The client issuing the PART command.
 */
void	Server::partCommand(Message& msg, Client& cli){
	std::vector<std::string> channel_list = msg.getChannels();
	std::vector<std::string> target_list = msg.getUsers();

	if (channel_list.size() == 0 || (target_list.size() > 0)){
		responseToClient(cli, needMoreParams("PART"));
		return;
	} else if (channel_list.size() > TARGET_LIM_IN_ONE_CMD){
		responseToClient(cli, tooManyTargets(cli.getNick()));
		return;
	}

	for(const auto& channel_name : channel_list){
		std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_name);
		if (!channel_ptr) {
			responseToClient(cli, errNoSuchChannel(cli.getNick(), channel_name));
			continue;
		}
		if (!channel_ptr->isChannelUser(cli)) {
			responseToClient(cli, notOnChannel(cli.getNick(), channel_name));
			continue;
		}

		std::string message = rplPart(cli.getPrefix(), channel_name, msg.getTrailing());

		channel_ptr->notifyChannelUsers(cli, message);
		responseToClient(cli, message);

		channel_ptr->removeUser(cli);

		Logger::log(Logger::INFO, "User " + cli.getNick() + " left channel " + channel_name);

		if (channel_ptr->isEmptyChannel()) {
			removeChannel(channel_name);
		}
	}
}

// Commands specific to channel operators:

/**
 * KICK <channel> <nick> [:<reason>]
 *
 * @brief Handles the IRC KICK command to forcibly remove users from one or more channels.
 * This function allows an IRC operator or channel operator to remove target users from channels.
 *
 * KICK command support:
 *   1. Kick multiple users from a single channel
 *      - Example: KICK #chan user1,user2
 *   2. Kick a single user from multiple channels
 *      - Example: KICK #chan1,#chan2 user1
 *   3. Kick N users from N channels (one-to-one mapping)
 *      - Example: KICK #chan1,#chan2 user1,user2
 *        This kicks:
 *          - user1 from #chan1
 *          - user2 from #chan2
 *
 * The function performs permission checks:
 *   - Whether the kicking user is in the channel :: ERR_NOTONCHANNEL (442)
 *   - Whether the kicking user has operator privileges :: ERR_CHANOPRIVSNEEDED (482)
 *   - Whether the target user is present in the specified channel :: ERR_USERNOTINCHANNEL (441)
 *
 * On success:
 *   - Notifies all channel members of the kick
 *   - Sends the KICK message directly to the kicked user (target)
 *   - Removes the target user from the channel
 *
 * @param msg   The parsed Message object containing the KICK command with channel(s), target(s), and optional comment.
 * @param user  The client issuing the KICK command.
 */
void	Server::kickUser(Message& msg, Client& user){
	std::vector<std::string> channel_list = msg.getChannels();
	std::vector<std::string> target_list = msg.getUsers();
	size_t	n_channel = channel_list.size();
	size_t 	n_target = target_list.size();

	if (channel_list.size() == 0 || target_list.size() == 0){
		responseToClient(user, needMoreParams("KICK"));
		return;
	} else if (channel_list.size() > TARGET_LIM_IN_ONE_CMD){
		responseToClient(user, tooManyTargets(user.getNick()));
		return;
	}

	if (n_channel == 1 && n_target > 0){
		std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_list.at(0));
		if (!channel_ptr) {
			responseToClient(user, errNoSuchChannel(user.getNick(), channel_list.at(0)));
			return ;
		}
		if (!channel_ptr->isChannelUser(user)){
			responseToClient(user, notOnChannel(user.getNick(), channel_list.at(0)));
			return;
		}
		if (!channel_ptr->isChannelOperator(user)){
			responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_list.at(0)));
			return ;
		}
		for(const auto& target_nick : target_list){
			std::shared_ptr<Client> target_ptr = getUserByNick(target_nick);
			if (!target_ptr) {
				responseToClient(user, errNoSuchNick(user.getNick(), target_nick));
				continue ;
			}
			if (!channel_ptr->isChannelUser(*getUserByNick(target_nick))){
				responseToClient(user, userNotInChannel(user.getNick(), target_nick, channel_list.at(0)));
				continue;
			}
			if (target_nick == user.getNick()){
				responseToClient(user, noticeToUser(user.getNick(), "You cannot kick yourself from a channel."));
				continue;
			}

			std::string message = rplKick(user.getNick(), target_nick, channel_list.at(0), msg.getTrailing());
    		channel_ptr->notifyChannelUsers(*getUserByNick(target_nick), message);
    		responseToClient(*getUserByNick(target_nick), message);

    		Logger::log(Logger::INFO, "User " + target_nick + " was kicked from channel " + channel_list.at(0) + " by " + user.getNick());
    		channel_ptr->removeUser(*getUserByNick(target_nick));
		}
	} else if (n_target == 1 && n_channel > 0){
		for(const auto& channel_name : channel_list){
			std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_name);
			if (!channel_ptr) {
				responseToClient(user, errNoSuchChannel(user.getNick(), channel_list.at(0)));
				continue ;
			}
			if (!channel_ptr->isChannelUser(user)){
				responseToClient(user, notOnChannel(user.getNick(), channel_name));
				continue;;
			}
			if (!channel_ptr->isChannelOperator(user)){
				responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name));
				continue;
			}
			std::shared_ptr<Client> target_ptr = getUserByNick(target_list.at(0));
			if (!target_ptr) {
				responseToClient(user, errNoSuchNick(user.getNick(), target_list.at(0)));
				continue ;
			}
			if (!channel_ptr->isChannelUser(*getUserByNick(target_list.at(0)))){
				responseToClient(user, userNotInChannel(user.getNick(), target_list.at(0), channel_name));
				continue;
			}
			if (target_list.at(0) == user.getNick()){
				responseToClient(user, canNotSendToChan(user.getNick(), "You cannot kick yourself from a channel."));
				continue;
			}

			std::string message = rplKick(user.getNick(), target_list.at(0), channel_name, msg.getTrailing());
			channel_ptr->notifyChannelUsers(*getUserByNick(target_list.at(0)), message);
			responseToClient(*getUserByNick(target_list.at(0)), message);

			Logger::log(Logger::INFO, "User " + target_list.at(0) + " was kicked from channel " + channel_name + " by " + user.getNick());
    		channel_ptr->removeUser(*getUserByNick(target_list.at(0)));
		}
	} else if (n_channel == n_target) {
		for (size_t i = 0; i < n_channel; ++i) {
			const std::string& channel_name = channel_list[i];
			const std::string& target_nick = target_list[i];
			std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_name);

			if (!channel_ptr) {
				responseToClient(user, errNoSuchChannel(user.getNick(), channel_list.at(0)));
				return ;
			}
			if (!channel_ptr->isChannelUser(user)) {
				responseToClient(user, notOnChannel(user.getNick(), channel_name));
				continue;
			}
			if (!channel_ptr->isChannelOperator(user)) {
				responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name));
				continue;
			}
			std::shared_ptr<Client> target_ptr = getUserByNick(target_nick);
			if (!target_ptr){
				responseToClient(user, errNoSuchNick(user.getNick(), target_list.at(0)));
				continue;
			}
			if (!channel_ptr->isChannelUser(*target_ptr)) {
				responseToClient(user, userNotInChannel(user.getNick(), target_nick, channel_name));
				continue;
			}
			if (target_nick == user.getNick()){
				responseToClient(user, canNotSendToChan(user.getNick(), "You cannot kick yourself from a channel."));
				continue;
			}

			std::string message = rplKick(user.getNick(), target_nick, channel_name, msg.getTrailing());
			channel_ptr->notifyChannelUsers(*target_ptr, message);
			responseToClient(*target_ptr, message);

			Logger::log(Logger::INFO, "User " + target_nick + " was kicked from channel " + channel_name + " by " + user.getNick());
			channel_ptr->removeUser(*target_ptr);
		}
	} else {
		responseToClient(user,InviteSyntaxErr(user.getNick()));
	}
}

/**
 * INVITE <nickname> <channel>
 *
 * @brief Handles the IRC INVITE command to invite user(s) to one or more channels.
 * This function allows a user (typically an operator in invite-only channels) to invite
 * other users to join channels they are in.
 *
 * INVITE command support:
 *   1. Invite a single user to multiple channels
 *      - Example: INVITE user1 #chan1,#chan2
 *   2. Invite multiple users to a single channel
 *      - Example: INVITE user1,user2 #chan
 *
 * The function performs permission and validity checks:
 *   - Whether the user who is inviting in the channel :: ERR_NOTONCHANNEL (442)
 *   - Whether the channel is invite-only and if the user who is inviting has operator privileges :: ERR_CHANOPRIVSNEEDED (482)
 *   - Whether the target user is already in the channel :: ERR_USERONCHANNEL (443)
 *
 * On success:
 *   - Sends RPL_INVITING (341) to the inviting user
 *   - Sends the INVITE command message to the invited user
 *   - Marks the target user as invited in the channel's state
 *
 * @param msg   The parsed Message object containing the INVITE command with channel(s) and target(s).
 * @param user  The client issuing the INVITE command.
 */
void    Server::inviteUser(Message& msg, Client& user){
	std::vector<std::string> channel_list = msg.getChannels();
	std::vector<std::string> target_list = msg.getUsers();
	size_t	n_channel = channel_list.size();
	size_t 	n_target = target_list.size();

	if (channel_list.size() == 0 || target_list.size() == 0){
		responseToClient(user, needMoreParams("INVITE"));
		return;
	} else if (channel_list.size() > TARGET_LIM_IN_ONE_CMD){
		responseToClient(user, tooManyTargets(user.getNick()));
		return;
	}

	if (n_channel == 1 && n_target > 0){
		std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_list.at(0));
		if (!channel_ptr) {
			responseToClient(user, errNoSuchChannel(user.getNick(), channel_list.at(0)));
			return ;
		}
		if (!channel_ptr->isChannelUser(user)){
			responseToClient(user, notOnChannel(user.getNick(), channel_list.at(0)));
			return;
		}
		if (channel_ptr->getInviteMode() && !channel_ptr->isChannelOperator(user)){
			responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_list.at(0)));
			return ;
		}
		for(const auto& target_nick : target_list){
			std::shared_ptr<Client> target_ptr = getUserByNick(target_nick);
			if (!target_ptr) {
				responseToClient(user, errNoSuchNick(user.getNick(), target_nick));
				continue ;
			}
			if (channel_ptr->isChannelUser(*getUserByNick(target_nick))){
				responseToClient(user, userOnChannel(user.getNick(), target_nick, channel_list.at(0)));
				continue;
			}
			channel_ptr->insertUser(getUserByNick(target_nick), USERTYPE::INVITE);
			responseToClient(user, Inviting(user.getNick(), channel_list.at(0), target_nick));
			std::string inviteMessage = ":" + user.getNick() + " INVITE " + target_nick + " " + channel_list.at(0) + "\r\n";
    		responseToClient(*getUserByNick(target_nick), inviteMessage);
		}
	}  else if (n_target == 1 && n_channel > 0){
		for(const auto& channel_name : channel_list){
			std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_name);
			if (!channel_ptr) {
				responseToClient(user, errNoSuchChannel(user.getNick(), channel_list.at(0)));
				continue ;
			}
			if (!channel_ptr->isChannelUser(user)){
				responseToClient(user, notOnChannel(user.getNick(), channel_name));
				continue;;
			}
			if (channel_ptr->getInviteMode() && !channel_ptr->isChannelOperator(user)){
				responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name));
				continue;
			}
			std::shared_ptr<Client> target_ptr = getUserByNick(target_list.at(0));
			if (!target_ptr) {
				responseToClient(user, errNoSuchNick(user.getNick(), target_list.at(0)));
				return ;
			}
			channel_ptr->insertUser(target_ptr, USERTYPE::INVITE);
			responseToClient(user, Inviting(user.getNick(), channel_name, target_ptr->getNick()));
			std::string inviteMessage = ":" + user.getNick() + " INVITE " + target_ptr->getNick() + " " + channel_name + "\r\n";
    		responseToClient(*getUserByNick(target_ptr->getNick()), inviteMessage);
		}
	} else {
		responseToClient(user,InviteSyntaxErr(user.getNick()));
	}
}

/**
 * TOPIC <channel> [:topic]
 *
 * @brief Handles the IRC TOPIC command to set or view a channel's topic.
 *
 * TOPIC command support:
 *   1. Only supports changing or viewing the topic of a single channel at a time.
 *   2. If no trailing parameter is provided:
 *      - Returns the current topic with RPL_TOPIC (332) if one is set.
 *      - Otherwise, returns RPL_NOTOPIC (331) indicating no topic is set.
 *   3. If the channel is in topic-protected mode (+t), only channel operators may set the topic.
 *   4. If the trailing parameter is a colon (`:`) with no content, the topic will be cleared.
 *
 * The function performs the following checks:
 *   - Ensures the user is a member of the channel :: ERR_NOTONCHANNEL (442)
 *   - Checks topic-change privileges if the channel is protected :: ERR_CHANOPRIVSNEEDED (482)
 *
 * On success:
 *   - Updates or clears the channel topic.
 *   - Notifies all users in the channel of the topic change.
 *   - Responds to the user with the updated topic message.
 *
 * @param msg   The parsed Message object containing the TOPIC command and parameters.
 * @param user  The client issuing the TOPIC command.
 */
void	Server::topic(Message& msg, Client& user){
	std::vector<std::string> channel_list = msg.getChannels();
	std::vector<std::string> target_list = msg.getUsers();
	size_t	n_channel = channel_list.size();

	if (channel_list.size() == 0 || target_list.size() != 0){
		responseToClient(user, needMoreParams("TOPIC"));
		return;
	}
	if (n_channel > 1){
		responseToClient(user,InviteSyntaxErr(user.getNick()));
		return ;
	}

	std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_list.at(0));
	if (!channel_ptr) {
		responseToClient(user, errNoSuchChannel(user.getNick(), channel_list.at(0)));
		return ;
	}
    if (!channel_ptr->isChannelUser(user)){
        Server::responseToClient(user, notOnChannel(user.getNick(), channel_list.at(0)));
        return ;
    }

	// No trailing parameter present AND there was no explicit ':' → just display topic (or no topic)
	if (msg.getTrailing().empty() && msg.getTrailingEmpty() == false){
		if (channel_ptr->getTopic().empty())
			responseToClient(user, NoTopic(user.getNick(), channel_list.at(0)));
		else
			responseToClient(user, Topic(user.getNick(), channel_list.at(0), channel_ptr->getTopic()));
		return ;
	}
	if (channel_ptr->getTopicMode() && !channel_ptr->isChannelOperator(user)){
		responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_list.at(0)));
		return ;
	}

    if (msg.getTrailingEmpty() == true) {
		channel_ptr->addNewTopic("");
    	Logger::log(Logger::INFO, "User " + user.getNick() + " cleared topic in channel " + channel_list.at(0));

   		std::string message = Topic(user.getNick(), channel_list.at(0), "");
    	channel_ptr->notifyChannelUsers(user, message);
    	responseToClient(user, message);
        return;
    }

	channel_ptr->addNewTopic(msg.getTrailing());
	std::string message = Topic(user.getNick(), channel_list.at(0), msg.getTrailing());
	channel_ptr->notifyChannelUsers(user, message);
	responseToClient(user, message);

    Logger::log(Logger::INFO, "User " + user.getNick() + " set new topic in channel " + channel_list.at(0) + ": " + msg.getTrailing());
}

/**
 * MODE <channel> [modes-flags [mode-params]]
 *
 * @brief Handles the IRC MODE command for viewing or modifying a channel's modes.
 *
 * MODE command support:
 *   1. Only supports modifying modes for one channel at a time.
 *   2. Supports multiple mode flags in a single command (e.g., "+itlk key 10").
 *   3. Parameters must be provided in the correct order and quantity as required by the flags.
 *      - Flags that require arguments: 'k' (password), 'l' (user limit), 'o' (operator toggle).
 *      - Flags without arguments: 'i' (invite-only), 't' (topic restrictions).
 *
 * Function behavior:
 *   - If no flags are specified, the current channel modes are returned using RPL_CHANNELMODEIS (324).
 *   - Validates that the user is a member of the channel :: ERR_NOTONCHANNEL (442)
 *   - Validates operator privileges for modifying modes :: ERR_CHANOPRIVSNEEDED (482)
 *   - Applies mode changes as specified, including:
 * 	 | Set mode     | Unset mode   | Description                                       |
 * 	 |--------------|--------------|---------------------------------------------------|
 * 	 | `+i`         | `-i`         | Allow only invited users to join the channel      |
 * 	 | `+t`         | `-t`         | Restrict/allow topic changes to operators only    |
 * 	 | `+k <key>`   | `-k`         | Set/unset a password required to join the channel |
 * 	 | `+o <nick>`  | `-o <nick>`  | Grant/revoke operator status to a user            |
 * 	 | `+l <limit>` | `-l`         | Set/unset a maximum user limit in the channel     |
 *
 *   - Notifies all users in the channel of the mode changes.
 *   - Returns error replies for invalid flags, missing parameters, or permission issues.
 *
 * @param msg   The parsed Message object containing the MODE command and parameters.
 * @param user  The client issuing the MODE command.
 */
// MODE #ch +i al
//  MODE alic +i
//  MODE +i
//  MODE #a
void	Server::mode(Message& msg, Client& user){

	std::vector<std::string> params_list = msg.getParameters();
	std::vector<std::string> target_list = msg.getUsers();
	std::vector<std::string> channel_list = msg.getChannels();

	if (!target_list.empty() && params_list.at(0) == target_list.at(0)){
		if (user.getNick() != target_list.at(0)){
			responseToClient(user, usersDontMatch(user.getNick())); // ERR_USERSDONTMATCH
			return;
		}
		if (params_list.size() == 1){
			responseToClient(user, rplUserModeIs(user.getNick(), user.getUserMode()));
			return ;
		}
		const std::string mode = params_list.at(1);
		// Validate: must start with + or - followed by valid mode chars
		std::regex mode_regex("^([+-][iworO]*)+$");
		if (!std::regex_match(mode, mode_regex)){
			responseToClient(user, umodeUnknownFlag(user.getNick())); // ERR_UMODEUNKNOWNFLAG
			return;
		}
		user.setUserMode(mode);
		responseToClient(user, rplUserModeIs(user.getNick(), mode)); // RPL_UMODEIS
		return;
	}

	if (channel_list.empty()) {
		responseToClient(user, needMoreParams("MODE"));
		return;
	}
	if (channel_list.size() > 1){
		responseToClient(user, tooManyTargets(user.getNick()));
		return;
	}

	std::string	channel_name = channel_list.at(0);//params_list.at(0);
	std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_name);
	if (!channel_ptr) {
        responseToClient(user, errNoSuchChannel(user.getNick(), channel_name));
		return;
    }
	if (!channel_ptr->isChannelUser(user)) {
        responseToClient(user, notOnChannel(user.getNick(), channel_name));
        return;
    }

	std::string	mode_flags = "";
	if (params_list.size() > 1)
		mode_flags = params_list.at(1);

	std::vector<std::string> args;
	if (params_list.size() > 2)
    	args.assign(params_list.begin() + 2, params_list.end());

	if (mode_flags.empty()) { // display only active modes
        std::string status = "";
        if (channel_ptr->getInviteMode()) status += "i";
        if (channel_ptr->getTopicMode()) status += "t";
        if (channel_ptr->getPasswdMode()) status += "k";
        if (channel_ptr->getLimitMode()) status += "l";
		// Prepend '+' only if at least one mode is active
    	if (!status.empty())
        	status = "+" + status;
        responseToClient(user, ChannelModeIs(user.getNick(), channel_name, status));
        return;
    }

	if (!channel_ptr->isChannelOperator(user)) {
        responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name));
        return;
    }

	bool adding = true;
	size_t arg_index = 0;
	std::string update_modes;

	for (size_t i = 0; i < mode_flags.size(); ++i) {
		char c = mode_flags[i];

		if (c == '+') {
			adding = true;
		}
		else if (c == '-') {
			adding = false;
		}
		else if (c == 'i') {
			adding ? channel_ptr->setInviteOnly() : channel_ptr->unsetInviteOnly();
			update_modes += (adding ? "+i" : "-i");
		}
		else if (c == 't') {
			adding ? channel_ptr->setTopicRestrictions() : channel_ptr->unsetTopicRestrictions();
			update_modes += (adding ? "+t" : "-t");
		}
		else if (c == 'k') {
			if (adding) {
				if (arg_index >= args.size()) {
					responseToClient(user, needMoreParams("MODE"));
					return;
				}
				if (!isValidModePassword(args[arg_index])){
					responseToClient(user, InvalidModeParamErr(user.getNick(), channel_name, 'k', args[arg_index++], "Invalid channel key"));
					continue ;
				}
				channel_ptr->addNewPassword(args[arg_index++]);
				channel_ptr->setPassword();
				update_modes += "+k";
			} else {
				channel_ptr->unsetPassword();
				update_modes += "-k";
			}
		}
		else if (c == 'l') {
			if (adding) {
				if (arg_index >= args.size()) {
					responseToClient(user, needMoreParams("MODE"));
					return;
				}
				if (!isPositiveInteger(args[arg_index])) {
    				responseToClient(user, InvalidModeParamErr(user.getNick(), channel_name, 'l', args[arg_index++], "Limit must be a positive integer"));
            		continue;
        		}
				channel_ptr->addLimit(std::stoi(args[arg_index++]));
				channel_ptr->setLimit();
				update_modes += "+l";
			} else {
				channel_ptr->unsetLimit();
				update_modes += "-l";
			}
		}
		else if (c == 'o') {
			if (arg_index >= args.size()) {
				responseToClient(user, needMoreParams("MODE"));
				return;
			}
			std::string nick = args[arg_index++];
			std::shared_ptr<Client> target_ptr = getUserByNick(nick);
			if (!target_ptr) {
				responseToClient(user, errNoSuchNick(user.getNick(), nick));
				continue ;
			}
			if (!channel_ptr->isChannelUser(*target_ptr)) {
				responseToClient(user, userNotInChannel(user.getNick(), nick, channel_name));
				continue;
			}
			if (adding) {
				channel_ptr->addNewOperator(*target_ptr);
				update_modes += "+o";
			} else {
				channel_ptr->removeOperator(*target_ptr);
				update_modes += "-o";
			}
		}
		else {
			responseToClient(user, unknownMode(user.getNick(), std::string(1, c), channel_name));
		}
	}
	// Notify all users in the channel about the mode change
	if (arg_index > args.size())
		arg_index = args.size();
	std::vector<std::string> params(args.begin(), args.begin() + arg_index);
	std::string	params_str;
	for (const std::string& arg : params) {
		params_str += " " + arg;
	}
	std::string message = rplMode(user.getPrefix(), channel_name, update_modes, params_str);
	channel_ptr->notifyChannelUsers(user, message);
	responseToClient(user, message);
}

/**
 * @brief Join a user into a channel
 *  1. valid if there is channel argument;
 *  2. If input too many channels;
 *  3. Loop the channel list, check if each channel exists already:
 * 	    3.1 If Not exist:
 *            Create a new channel, set the user as the operator.Notify the user
 *            who has joined
 *      3.2 If exist :
 * 	        -- Checkingif the user exists in the channel already. If yes, then do nothing
 *          -- Checking if the channel is full. If yes, can't join(ERR_CHANNELISFULL).
 *          -- Checking if the channel is invite_only. If yes, checking if the user
 *              on the invitee list. If no, can't join(ERR_INVITEONLYCHAN)
 * 			-- Checking if the channel needs a key. If yes, checking if the user provide
 * 	           the matched key.
 * Command syntax:
 * 		JOIN #channel1 [<#channel2> <#channel3> <#channel4>] [:password]
 *
 */
// continue when we decide how to get the users list/invite list....

// OBS to JOIN by Helena: Consider switching from 342 to 353 and 366 for names list
// consistency (unless you're intentionally simplifying).

// checking if channel reaches the server/user limit logic
void	Server::joinCommand(Message& msg, Client& cli){
	const std::string&	nick = cli.getNick();
	std::vector<std::string>	channels = msg.getChannels();
	std::vector<std::string>	passwds = msg.getPasswords();
	size_t	index = 0;

	// checking if the arguments number is valid
	if (channels.size() == 0){
		responseToClient(cli, needMoreParams("JOIN"));
		return;
	} if (channels.size() > TARGET_LIM_IN_ONE_CMD){
		responseToClient(cli, tooManyTargets(nick));
		Logger::log(Logger::ERROR, "too many target");
		return;
	}
	size_t passwds_index = 0;
	for (const auto& chan_name : channels){
		std::shared_ptr<Channel> channel = getChannelByName(chan_name);
		if (channel == nullptr){
			// Checking if server has reached its maximum channel
			if (n_channel_ >= SERVER_CHANNEL_LIMIT){
				responseToClient(cli, unknowError(nick, "JOIN", "Cannot create new channel — server has reached its maximum"));
				Logger::log(Logger::WARNING, "the server has reached its maximum number of allowed channels");
				continue;
			}
			// checking if the channel name is valid
			if (chan_name.size() > 50 || !isChannelValid(chan_name)){
				responseToClient(cli, badChannelName(nick, chan_name));
				Logger::log(Logger::ERROR, "Channel name is invalid");
				continue;
			}
			// checking if the ammout of channels that user joined has reached its maximum
			if (cli.getUserNChannel() >= USER_CHANNEL_LIMIT){
				responseToClient(cli, unknowError(nick, "JOIN", "The user has reached its maximum channel"));
				Logger::log(Logger::WARNING, "the user has reached its maximum number of allowed channels");
				return;
			}
			//channels_[chan_name] = std::make_shared<Channel>(chan_name, cli);
			// Create the new channel and add the user as operator
			channel = std::make_shared<Channel>(chan_name, cli);
			channels_[chan_name] = channel;
			n_channel_++;
			std::cout << "n_channel=" << n_channel_ << std::endl; // for testing
			cli.increaseUserNchannel();
			if (passwds_index < passwds.size()){
				const std::string& passwd = passwds[passwds_index++];
				if (!isValidModePassword(passwd)){
					responseToClient(cli, InvalidModeParamErr(nick, chan_name, 'k', passwd, "Invalid channel key"));
					continue ;
				}
				channel->addNewPassword(passwd);
				channel->setPassword();
			}
			responseToClient(cli, rplJoin(cli.getPrefix(), chan_name));
			std::string	message = chan_name + " has been created. Now server has " + std::to_string(n_channel_) + " channels";
			Logger::log(Logger::INFO, message);
			// std::cout << "call from joincommand\n";// for testing only
			// printChannels(); // for testing only
		} else {
			// checking if the user is in the channel already. If yes, then return without
			// doing anything
			if (channel->isUserInList(cli, USERTYPE::REGULAR) == true){
				// :server 443 hele #test3 :is already on channel
				responseToClient(cli, userOnChannel(nick, "", chan_name));
				continue;
			}
			// checking if the channel is full, only if flag user_limit_ is true
			if (channel->getLimitMode() && channel->isFullChannel() == true){
				responseToClient(cli, channelIsFull(nick, chan_name));
				continue;
			}
			// If the channel needs a password, but the client doesn't provide it
			if (channel->getPasswdMode() == true){
				if (passwds.size() == 0){
					responseToClient(cli, badChannelKey(nick,chan_name));
					continue;
				}
				if (channel->getPassword() != passwds.at(index++)){
					responseToClient(cli, badChannelKey(nick,chan_name));
					continue;
				}
			}
			// If the channel is invite_only but the client is not on the invitee list
			if (channel->getInviteMode() == true && channel->isUserInList(cli, USERTYPE::INVITE) == false){
				responseToClient(cli, inviteOnlyChan(nick, chan_name));
				continue ;
			}
			channel->addNewUser(cli);
			cli.increaseUserNchannel(); // increase the channel number that the user joined
			std::string	message = rplJoin(cli.getPrefix(), chan_name);
			channel->notifyChannelUsers(cli, message);
			responseToClient(cli, message);
		}
	}
}

bool	Server::isChannelValid(const std::string& channel_name){
	char	first_char = channel_name[0];
	if (std::string(SUPPORTCHANNELPREFIX).find(first_char) == std::string::npos){ // doesn't find
		return false;
	}
	// if the channel starts with '!', the size of the channel name should be 6;
	// channelid only contains uppercase letters and digits
	if (first_char == '!'){
		if (channel_name.size() != 6
			|| channel_name.substr(1,5).find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")){
			return false;
		}
	}
	// channel name can't contain: NUL, BEL, \r, \n, space, comma, and :
	for (const char& c : channel_name){
		if (c == 0 || c == 7 || c == 13 || c == 10 || c == 32 || c == 44 || c == 58){
			return false;
		}
	}
	return true;
}


bool	Server::isExistedChannel(const std::string& channel_name){
	for (const auto& [name, channelPtr] : channels_){
		if (name == channel_name){
			return true;
		}
	}
	return false;
}

/**
 * @brief Send a message to a user or a channel if they exist.
 * Can be used to message multiple users and/or channels at the same time
 */
void Server::privmsgCommand(Message& msg, Client& cli){
    std::vector<std::string> channels = msg.getChannels();
    std::vector<std::string> users = msg.getUsers();
	std::vector<std::string> params_list = msg.getParameters();
	const std::string& message = msg.getTrailing();

    if (channels.empty() && users.empty()){
		responseToClient(cli, needMoreParams("PRIVMSG"));
		Logger::log(Logger::ERROR, "No user/channel in the arguments");
		return;
	}
    if (msg.getTrailingEmpty() == true /*|| message == "\r\n"*/){
		responseToClient(cli, "No text to send\r\n");
		Logger::log(Logger::ERROR, "there is no message to be sent");
		return;
	}
    //if (channels.size() + users.size() > TARGET_LIM_IN_ONE_CMD){
	if (params_list.size() > TARGET_LIM_IN_ONE_CMD) {
		responseToClient(cli, tooManyTargets(cli.getNick()));
		Logger::log(Logger::ERROR, "too many target");
		return;
	}
    for (const auto& channel_name : channels){
        std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_name);
        if (!channel_ptr) {
            responseToClient(cli, errNoSuchChannel(cli.getNick(), channel_name));
			Logger::log(Logger::ERROR, "No such channel");
            continue;
        }
        if (!channel_ptr->isChannelUser(cli)){
            responseToClient(cli, notOnChannel(cli.getNick(), channel_name));
			Logger::log(Logger::ERROR, "User isn't on the channel");
            continue;
        }
		channel_ptr->notifyChannelUsers(cli, rplPrivMsg(cli.getNick(), channel_name, message));
		Logger::log(Logger::INFO, "send message to channel users");
    }
    for (const auto& target_nick : users){
        std::shared_ptr<Client> target_client = getUserByNick(target_nick);
        if (!target_client){
            responseToClient(cli, errNoSuchNick(cli.getNick(), target_nick));
			Logger::log(Logger::ERROR, "no such nick");
            continue;
        }
		responseToClient(*target_client, rplPrivMsg(cli.getNick(), target_client->getNick(), message));
		Logger::log(Logger::INFO, "send message to a user");
    }
}

/**
 * Helper for an old version of the capCommand(), can probably be removed if parsing
 * handles it for msg_trailing_
 */
std::string trim(const std::string& str) {
    size_t start = 0;
    while (start < str.length() && std::isspace(str[start])) ++start;
    size_t end = str.length();
    while (end > start && std::isspace(str[end - 1])) --end;
    return str.substr(start, end - start);
}

/**
 * @brief starts "Client Capability Communication" between the client and the server.
 * This is done automatically on startup by irssi, so we only handle the responses irssi
 * expects to hear in order to set up a successful connection.
 */
void Server::capCommand(Message& msg, Client& cli){
	std::vector<std::string> parameters = msg.getParameters();
	std::string subcmd = parameters.empty() ? "" : parameters[0];

	if (subcmd == "LS"){
		std::string response = "CAP * LS :multi-prefix\r\n";
		responseToClient(cli, response);
	} else if (subcmd == "REQ"){
		std::string requested_caps = trim(msg.getTrailing());
		if (!requested_caps.empty()){
			std::string response = "CAP * ACK :" + requested_caps + "\r\n";
			responseToClient(cli, response);
		} else{
			Logger::log(Logger::ERROR, "CAP REQ missing capability list");
		}
	} else if (subcmd == "END"){
		// No response needed — just move on to registration
	} else{
		Logger::log(Logger::WARNING, "Unhandled CAP subcommand: " + subcmd);
	}
}

bool Server::isValidModePassword(const std::string& key) {
    if (key.empty() || key.length() > 23) {
        return false;
    }
    for (unsigned char ch : key) {
        // Allow if in the specified ASCII ranges
        if ((ch >= 0x01 && ch <= 0x05) || (ch >= 0x07 && ch <= 0x08) || ch == 0x0C ||
            (ch >= 0x0E && ch <= 0x1F) || (ch >= 0x21 && ch <= 0x7F)) {
            continue;
        } else {
            return false;
        }
    }
    return true;
}

bool Server::isPositiveInteger(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

/**
 * @brief Used by irssi to make sure the connection to the server still exists.
 * Responds to PING commmand with a PONG reply
 */
void Server::pingCommand(Message& msg, Client& cli){
	if (msg.getParameters().empty()){
		responseToClient(cli, noOrigin(cli.getNick()));
		return;
	}
	const std::string& origin = msg.getParameters().at(0);
	responseToClient(cli, "PONG :" + origin + "\r\n");
}

std::string rplWhoisChannels(const std::string& requesterNick, const std::string& targetNick, const std::string& channels){
	return ":irc.ircserv.com 319 " + requesterNick + " " + targetNick + " :" + channels + "\r\n";
}

/**
 * @brief Used by irssi when multiple users try to connect with the same information.
 * Confirms whether the user information is the exact same or not
 */
void Server::whoisCommand(Message& msg, Client& cli){
	std::vector<std::string> params = msg.getParameters();
	if (params.empty()){
		responseToClient(cli, nonNickNameGiven(cli.getNick()));
		return;
	}
	std::string targetNick = params[0];
	std::shared_ptr<Client> target = getUserByNick(targetNick);
	if (!target){
		responseToClient(cli, errNoSuchNick(cli.getNick(), targetNick));
		responseToClient(cli, rplEndOfWhois(cli.getNick(), targetNick));
		return;
	}
	std::string channels = getChannelsOfUser(*target);
	if (!channels.empty()){
		std::string r319 = rplWhoisChannels(cli.getNick(), target->getNick(), channels);
		responseToClient(cli, r319);
	}
	//if there are weird issues, try removing this r312
	std::string	r312 = ":irc.ircserv.com 312 " + cli.getNick() + " " + target->getNick() + " irc.ircserv.com :Your IRC Server\r\n";
	responseToClient(cli, r312);
	std::string r311 = rplWhoisUser(cli.getNick(), target->getNick(), target->getUsername(), target->getHostname(), target->getRealname());
	responseToClient(cli, r311);
	std::string r318 = rplEndOfWhois(cli.getNick(), targetNick);
	responseToClient(cli, r318);
}
