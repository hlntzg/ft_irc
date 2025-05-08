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

}


void	quitCommand(Message& msg, Client& cli){

}

// From Channel


void    Server::inviteUser(Message& msg, Client& user)
//Client& user, Client& target)
 {
	std::unordered_map<std::string, std::shared_ptr<Channel>> channels = msg.getChannelList();
	for (const auto& [name, channelPtr] : channels_ ){
		if (!channelPtr->isChannelUser(user)){
        // ERR_NOTONCHANNEL (442)
			Server::responseToClient(user, notOnChannel(user.getNick(), name));
			return ;
		}
		if (channelPtr->getInviteMode() && !channelPtr->isChannelOperator(user)){
			// ERR_CHANOPRIVSNEEDED (482)
			Server::responseToClient(user, ChanoPrivsNeeded(user.getNick(), name));
			return ;
		}

		if (channelPtr->isChannelUser(target)){
			// ERR_USERONCHANNEL (443)
			Server::responseToClient(user, userOnChannel(user.getNick(), target.getNick(), name));
			return ;
		}
		invited_users_.insert(&target);
	}



    // RPL_INVITING (341)
    Server::responseToClient(user, Inviting(user.getNick(), channel_name_, target.getNick()));
    // Send the actual INVITE message to target
    std::string inviteMessage = ":" + user.getNick() + " INVITE " + target.getNick() + " " + channel_name_ + "\r\n";
    Server::responseToClient(target, inviteMessage);

    // Not sure if we will implement that:
    // RPL_AWAY (301) — if the target is away, notify the inviter
    // std::string targetAwayMessage = "rplAway(SERVER_NAME, inviter.getNick(), target.getNick(), target.getAwayMsg())";
    // if (!target.getAwayMsg().empty()) {
    //     Server::responseToClient(user, targetAwayMessage);
    // }
}
