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
	if (msg.getMsgParams().empty()){
		responseToClient(cli, needMoreParams("PASS"));
		return;
	} else if (cli.isRegistered()){
		responseToClient(cli, alreadyRegistred(nick));
		return ;
	}
	// vector.at() is safer than vector.at[0] to access the element.
	// at() will do the bounds checking
	std::string	password = msg.getMsgParams().at(0);
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
	cli.setRegisterStatus(); // set it to true
	responseToClient(cli, rplWelcome(nick));
	responseToClient(cli,rplYourHost(nick));
	responseToClient(cli,rplCreated(nick));
	responseToClient(cli,rplMyInfo(nick));
}

bool	Server::isNickInUse(const std::string& nick){

}

void	Server::quitCommand(Message& msg, Client& cli){

}

// Commands specific to channel operators:

void	Server::kickUser(Message& msg, Client& user){
	std::vector<std::string> channel_list = msg.getChannelList();
	std::vector<std::string> target_list = msg.getParamsList();
	size_t	n_channel = channel_list.size();
	size_t 	n_target = target_list.size();

	if (n_channel == 1){
		std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_list.at(0));
		if (!channel_ptr->isChannelUser(user)){
			// ERR_NOTONCHANNEL (442)
			responseToClient(user, notOnChannel(user.getNick(), channel_list.at(0)));
			return;
		}
		if (!channel_ptr->isChannelOperator(user)){
			// ERR_CHANOPRIVSNEEDED (482)
			responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_list.at(0)));
			return ;
		}
		for(const auto& target_nick : target_list){
			// ERR_USERNOTINCHANNEL (441)
			if (!channel_ptr->isChannelUser(*getUserByNick(target_nick))){
				responseToClient(user, userOnChannel(user.getNick(), target_nick, channel_list.at(0)));
				continue;
			}

			std::string message = rplKick(user.getNick(), target_nick, channel_list.at(0), msg.getTrailing());
    		// Notify all users in the channel except the target
    		channel_ptr->notifyChannelUsers(*getUserByNick(target_nick), message);
    		// Notify the target separately (optional)
    		responseToClient(*getUserByNick(target_nick), message);

    		Logger::log(Logger::INFO, "User " + target_nick + " was kicked from channel " + channel_list.at(0) + " by " + user.getNick());
    		channel_ptr->removeUser(*getUserByNick(target_nick));
		}
	} else if (n_target == 1){
		for(const auto& channel_name : channel_list){
			std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_name);
			if (!channel_ptr->isChannelUser(user)){
        	// ERR_NOTONCHANNEL (442)
				responseToClient(user, notOnChannel(user.getNick(), channel_name));
				continue;;
			}
			if (!channel_ptr->isChannelOperator(user)){
				// ERR_CHANOPRIVSNEEDED (482)
				responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name));
				continue;
			}
			if (!channel_ptr->isChannelUser(*getUserByNick(target_list.at(0)))){
				// ERR_USERNOTINCHANNEL (441)
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

void    Server::inviteUser(Message& msg, Client& user){
	std::vector<std::string> channel_list = msg.getChannelList();
	std::vector<std::string> target_list = msg.getParamsList();
	size_t	n_channel = channel_list.size();
	size_t 	n_target = target_list.size();

	if (n_channel == 1){
		std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_list.at(0));
		if (!channel_ptr->isChannelUser(user)){
        // ERR_NOTONCHANNEL (442)
			responseToClient(user, notOnChannel(user.getNick(), channel_list.at(0)));
			return;
		}
		if (channel_ptr->getInviteMode() && !channel_ptr->isChannelOperator(user)){
			// ERR_CHANOPRIVSNEEDED (482)
			responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_list.at(0)));
			return ;
		}
		for(const auto& target_nick : target_list){
			if (channel_ptr->isChannelUser(*getUserByNick(target_nick))){
				responseToClient(user, userOnChannel(user.getNick(), target_nick, channel_list.at(0)));
				continue;
			}
			channel_ptr->insertUser(getUserByNick(target_nick), INVITE);
			responseToClient(user, Inviting(user.getNick(), channel_list.at(0), target_nick));
			std::string inviteMessage = ":" + user.getNick() + " INVITE " + target_nick + " " + channel_list.at(0) + "\r\n";
    		responseToClient(*getUserByNick(target_nick), inviteMessage);
		}
	} else if (n_target == 1){
		for(const auto& channel_name : channel_list){
			std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_name);
			if (!channel_ptr->isChannelUser(user)){
        	// ERR_NOTONCHANNEL (442)
				responseToClient(user, notOnChannel(user.getNick(), channel_name));
				continue;;
			}
			if (channel_ptr->getInviteMode() && !channel_ptr->isChannelOperator(user)){
				// ERR_CHANOPRIVSNEEDED (482)
				responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_name));
				continue;
			}
			std::shared_ptr<Client> target_ptr = getUserByNick(target_list.at(0));
			channel_ptr->insertUser(target_ptr, INVITE);
			responseToClient(user, Inviting(user.getNick(), channel_name, target_ptr->getNick()));
			std::string inviteMessage = ":" + user.getNick() + " INVITE " + target_ptr->getNick() + " " + channel_name + "\r\n";
    		responseToClient(*getUserByNick(target_ptr->getNick()), inviteMessage);
		}
	} else {
		responseToClient(user,InviteSyntaxErr(user.getNick()));
	}
}

void	Server::topic(Message& msg, Client& user){
	std::vector<std::string> channel_list = msg.getChannelList();
	size_t	n_channel = channel_list.size();

	if (n_channel > 1){
		responseToClient(user,InviteSyntaxErr(user.getNick()));
		return ;
	}

	std::shared_ptr<Channel> channel_ptr = getChannelByName(channel_list.at(0));
    if (!channel_ptr->isChannelUser(user)){
        // ERR_NOTONCHANNEL (442)
        Server::responseToClient(user, notOnChannel(user.getNick(), channel_list.at(0)));
        return ;
    }
	if (msg.getTrailing().empty()){
		if (channel_ptr->getTopic().empty())
		 	// RPL_NOTOPIC (331)
			responseToClient(user, NoTopic(user.getNick(), channel_list.at(0)));
		else
			// RPL_TOPIC (332)
			responseToClient(user, Topic(user.getNick(), channel_list.at(0), channel_ptr->getTopic()));
		return ;
	}
	if (channel_ptr->getTopicMode() && !channel_ptr->isChannelOperator(user)){
		// ERR_CHANOPRIVSNEEDED (482)
		responseToClient(user, ChanoPrivsNeeded(user.getNick(), channel_list.at(0)));
		return ;
	}

    // Unset topic
    if (msg.getTrailing() == ":") {
        channel_ptr->getTopic().clear();
        Logger::log(Logger::INFO, "User " + user.getNick() + " cleared topic in channel " + channel_list.at(0));
        return;
    }

	channel_ptr->addNewTopic(msg.getTrailing());
    // 332 RPL_TOPIC
	std::string message = Topic(user.getNick(), channel_list.at(0), msg.getTrailing());
	channel_ptr->notifyChannelUsers(user, message);
	responseToClient(user, message);

    Logger::log(Logger::INFO, "User " + user.getNick() + " set new topic in channel " + channel_list.at(0) + ": " + msg.getTrailing());
}

// By Helena: maybe it's possible to have also mode() in here
// void	Server::mode(Message& msg, Client& user){
// 	std::vector<std::string> params_list = msg.getParamsList();
// 	std::string	mode_flags = params_list.at(0);
// 	1. add the rest of params_list on std::vector<std::string> args
// 	2. check if code from Channel::mode(...) can fit here :)
// }
