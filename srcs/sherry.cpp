#include "Server.hpp"

/**
 * @brief User can use this PRIVMSG to send a message to the whole channel users
 * or send it to single user
 *
 * Syntax: (and the limisted of the arguments except tailing message is 4)
 *    PRIVMSG #42Helsinki :Hello everyone!
 *    PRIVMSG john_doe :Hey, how are you?
 */
// void	Server::privmsgCommand(Message& msg, Client& cli){
// 	std::vector<std::string>	channels = msg.getChannels();
// 	std::vector<std::string>	users = msg.getUsers();
// 	if (channels.size() + users.size() > 4){
// 		responseToClient(cli, tooManyTargets(cli.getNick()));
// 		return;
// 	}

// }

#if 0
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
			const std::unordered_set<Client*>& user_list = channel->getChannelUsers();
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
#endif


