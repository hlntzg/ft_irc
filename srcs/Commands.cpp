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
	if (!isPasswordMatch(password)){
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
	if (isNickInUse(nick)){
		responseToClient(cli, nickNameinuse(nick));
		return;
	}
	if (cli.getPassword() != serv_passwd_){
		responseToClient(cli, passwdMismatch(nick));
		return;
	}
	cli.setRegistrationStatus(true);
	responseToClient(cli, rplWelcome(nick));
	responseToClient(cli,rplYourHost(nick));
	responseToClient(cli,rplCreated(nick));
	responseToClient(cli,rplMyInfo(nick));
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
bool	Server::isNickInUse(const std::string& nick){
	for (auto& [fd, user] : clients_){
		if (user->getNick() == nick){
			return true;
		}
	}
	return false;
}

void	Server::nickCommand(Message& msg, Client& cli){
	std::vector<std::string>	params = msg.getParameters();
	if (params.size() == 0){
		responseToClient(cli, nonNickNameGiven());
		return;
	}
	const std::string& nick = params.at(0);
	if (isNickInUse(nick) == true){
		responseToClient(cli, nickNameinuse(""));
	}
	// checking if nick string contains only valid characters
	for (auto c : nick){
		if (!isalnum(static_cast<unsigned char>(c)) && std::string(SPECIAL_CHARS).find(c) == std::string::npos){
			responseToClient(cli, erroneusNickName(""));
			return;
		}
	}
	cli.setNick(nick);
	attempRegisterClient(cli);
}

void	Server::userCommand(Message& msg, Client& cli){
	std::vector<std::string>	params = msg.getParameters();
	if (params.size() < 4){
		responseToClient(cli, needMoreParams("USER"));
		return;
	}
	const std::string& username = params.at(0);
	const std::string& realname = params.at(3);
	for (auto c : username){
		if (!isalnum(static_cast<unsigned char>(c)) && std::string(SPECIAL_CHARS).find(c) == std::string::npos){
			responseToClient(cli, erroneusNickName(""));
			return;
		}
	}
	for (auto c : realname){
		if (!isalnum(static_cast<unsigned char>(c)) && std::string(SPECIAL_CHARS).find(c) == std::string::npos){
			responseToClient(cli, erroneusNickName(""));
			return;
		}
	}
	cli.setUsername(username);
	cli.setRealname(realname);
	cli.setHostname(params.at(1));
	cli.setServername(params.at(2));
	attempRegisterClient(cli);
}

void	Server::quitCommand(Message& msg, Client& cli){
	std::vector<std::string>	params = msg.getParameters();
	removeClient(cli, params.at(0));
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

	if (n_channel == 1){
		std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_list.at(0));
		if (!channel_ptr->isChannelUser(user)){
			responseToClient(user, notOnChannel(user.getNick(), channel_list.at(0)));
			return;
		}
		if (!channel_ptr->isChannelOperator(user)){
			responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_list.at(0)));
			return ;
		}
		for(const auto& target_nick : target_list){
			if (!channel_ptr->isChannelUser(*getUserByNick(target_nick))){
				responseToClient(user, userOnChannel(user.getNick(), target_nick, channel_list.at(0)));
				continue;
			}

			std::string message = rplKick(user.getNick(), target_nick, channel_list.at(0), msg.getTrailing());
    		channel_ptr->notifyChannelUsers(*getUserByNick(target_nick), message);
    		responseToClient(*getUserByNick(target_nick), message);

    		Logger::log(Logger::INFO, "User " + target_nick + " was kicked from channel " + channel_list.at(0) + " by " + user.getNick());
    		channel_ptr->removeUser(*getUserByNick(target_nick));
		}
	} else if (n_target == 1){
		for(const auto& channel_name : channel_list){
			std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_name);
			if (!channel_ptr->isChannelUser(user)){
				responseToClient(user, notOnChannel(user.getNick(), channel_name));
				continue;;
			}
			if (!channel_ptr->isChannelOperator(user)){
				responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name));
				continue;
			}
			if (!channel_ptr->isChannelUser(*getUserByNick(target_list.at(0)))){
				responseToClient(user, userOnChannel(user.getNick(), target_list.at(0), channel_name));
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

			if (!channel_ptr->isChannelUser(user)) {
				responseToClient(user, notOnChannel(user.getNick(), channel_name));
				continue;
			}
			if (!channel_ptr->isChannelOperator(user)) {
				responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name));
				continue;
			}
			std::shared_ptr<Client> target_user = getUserByNick(target_nick);
			if (!target_user || !channel_ptr->isChannelUser(*target_user)) {
				responseToClient(user, userOnChannel(user.getNick(), target_nick, channel_name));
				continue;
			}

			std::string message = rplKick(user.getNick(), target_nick, channel_name, msg.getTrailing());
			channel_ptr->notifyChannelUsers(*target_user, message);
			responseToClient(*target_user, message);

			Logger::log(Logger::INFO, "User " + target_nick + " was kicked from channel " + channel_name + " by " + user.getNick());
			channel_ptr->removeUser(*target_user);
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

	if (n_channel == 1){
		std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_list.at(0));
		if (!channel_ptr->isChannelUser(user)){
			responseToClient(user, notOnChannel(user.getNick(), channel_list.at(0)));
			return;
		}
		if (channel_ptr->getInviteMode() && !channel_ptr->isChannelOperator(user)){
			responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_list.at(0)));
			return ;
		}
		for(const auto& target_nick : target_list){
			if (channel_ptr->isChannelUser(*getUserByNick(target_nick))){
				responseToClient(user, userOnChannel(user.getNick(), target_nick, channel_list.at(0)));
				continue;
			}
			channel_ptr->insertUser(getUserByNick(target_nick), USERTYPE::INVITE);
			responseToClient(user, Inviting(user.getNick(), channel_list.at(0), target_nick));
			std::string inviteMessage = ":" + user.getNick() + " INVITE " + target_nick + " " + channel_list.at(0) + "\r\n";
    		responseToClient(*getUserByNick(target_nick), inviteMessage);
		}
	}  else if (n_target == 1){
		for(const auto& channel_name : channel_list){
			std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_name);
			if (!channel_ptr->isChannelUser(user)){
				responseToClient(user, notOnChannel(user.getNick(), channel_name));
				continue;;
			}
			if (channel_ptr->getInviteMode() && !channel_ptr->isChannelOperator(user)){
				responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name));
				continue;
			}
			std::shared_ptr<Client> target_ptr = getUserByNick(target_list.at(0));
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
	size_t	n_channel = channel_list.size();

	if (n_channel > 1){
		responseToClient(user,InviteSyntaxErr(user.getNick()));
		return ;
	}

	std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_list.at(0));
    if (!channel_ptr->isChannelUser(user)){
        Server::responseToClient(user, notOnChannel(user.getNick(), channel_list.at(0)));
        return ;
    }
	if (msg.getTrailing().empty()){
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

    // // Unset topic
	// [!!!] Comment by Helena: check with parsing, probably dont get ':' as msg_trailing,
	// so no need this check just if msg_training is "" somehow;
    // if (msg.getTrailing() == ":") {
    //     channel_ptr->getTopic().clear();
    //     Logger::log(Logger::INFO, "User " + user.getNick() + " cleared topic in channel " + channel_list.at(0));
    //     return;
    // }

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
void	Server::mode(Message& msg, Client& user){
	std::vector<std::string> params_list = msg.getParameters();
	std::string	channel_name = params_list.at(0);
	std::string	mode_flags = params_list.at(1);
	std::vector<std::string> args(params_list.begin() + 2, params_list.end());

	std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_name);
   	if (!channel_ptr->isChannelUser(user)) {
        responseToClient(user, notOnChannel(user.getNick(), channel_name));
        return;
    }
	if (mode_flags.empty()) {
        std::string status = "+";
        if (channel_ptr->getInviteMode())
            status += "i";
        if (channel_ptr->getTopicMode())
            status += "t";
        if (channel_ptr->getPasswdMode())
            status += "k";
        if (channel_ptr->getLimitMode())
            status += "l";
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
			std::shared_ptr<Client> target = getUserByNick(nick);
			if (!target || !channel_ptr->isChannelUser(*target)) {
				responseToClient(user, userNotInChannel(user.getNick(), nick, channel_name));
				continue;
			}
			if (adding) {
				channel_ptr->addNewOperator(*target);
				update_modes += "+o";
			} else {
				channel_ptr->removeOperator(*target);
				update_modes += "-o";
			}
		}
		else {
			responseToClient(user, unknownMode(user.getNick(), std::string(1, c)));
		}
	}
	// Notify all users in the channel about the mode change
	std::vector<std::string> params(args.begin(), args.begin() + arg_index);
	std::string message = rplMode(user.getNick(), channel_name, update_modes, params);

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
void	Server::joinCommand(Message& msg, Client& cli){
	const std::string&	nick = cli.getNick();
	std::vector<std::string>	channels = msg.getChannels();
	std::vector<std::string>	passwds = msg.getPasswords();
	size_t	index = 0;
	// checking if the arguments number is valid
	if (channels.size() == 0){
		responseToClient(cli, needMoreParams("JOIN"));
		return;
	} else if (channels.size() > TARGET_LIM_IN_ONE_CMD){
		responseToClient(cli, tooManyTargets(nick));
		return;
	}
	for (const auto& chan_name : channels){
		std::shared_ptr<Channel> channel = getChannelByName(chan_name);
		if (channel == nullptr){
			Channel new_channel(chan_name, cli);
			responseToClient(cli, rplJoinChannel(cli.getNick(), chan_name));
		} else {
			// checking if the user is in the channel already. If yes, then return without
			// doing anything
			if (channel->isUserInList(cli, USERTYPE::REGULAR) == true){
				return;
			}
			// checking if the channel is full
			if (channel->isFullChannel() == true){
				responseToClient(cli, channelIsFull(nick, chan_name));
			}
			// If the channel needs a password, but the client doesn't provide it
			if (channel->getPasswdMode() == true){
				if (passwds.size() == 0){
					responseToClient(cli, noChanoPasswd(nick,chan_name));
				}
				if (channel->getPassword() != passwds.at(index++)){
					responseToClient(cli, badChannelKey(nick,chan_name));
				}
			}
			// If the channel is invite_only but the client is not on the invitee list
			if (channel->getInviteMode() == true){
				if (channel->isUserInList(cli, USERTYPE::INVITE) == false){
					responseToClient(cli, inviteOnlyChan(nick, chan_name));
				}
			}
			channel->addNewUser(cli);
			std::string	message = rplJoinChannel(nick, chan_name);
			channel->notifyChannelUsers(cli, message);
			responseToClient(cli, message);
		}
	}
}

bool	Server::isExistedChannel(const std::string& channel_name){
	for (const auto& [name, channelPtr] : channels_){
		if (name == channel_name){
			return true;
		}
	}
	return false;
}
