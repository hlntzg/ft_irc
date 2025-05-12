#include "Server.hpp"

// void	Server::privmsgCommand(Message& msg, Client& cli){
// 	std::vector<std::string>	channels = msg.getChannelList();
// 	//??????
// }

/**
 * @brief Join a user into a channel
 *  1. valid if there is channel argument;
 *  2. If input too many channels;
 *  3. Loop the channel list, check if each channel exists already:
 * 	    3.1 If Not exist:
 *            Create a new channel, set the user as the operator.Notify the user
 *            who has joined
 *      3.2 If exist :
 * 	        -- Checking if the channel is full. If yes, can't join(ERR_CHANNELISFULL).
 *          -- Checking if the channel is invite_only. If yes, checking if the user
 *              on the invitee list. If no, can't join(ERR_INVITEONLYCHAN)
 * 			-- Checking if the channel needs a key. If yes, checking if the user provide
 * 	           the matched key.
 *
 */
// void	Server::joinCommand(Message& msg, Client& cli){
// 	std::vector<std::string>	channels = msg.getChannelList();
// 	if (channels.size() == 0){
// 		responseToClient(cli, needMoreParams("JOIN"));
// 		return;
// 	} else if (channels.size() > TARGET_LIM_IN_ONE_CMD){
// 		responseToClient(cli, tooManyTargets(cli.getNick()));
// 		return;
// 	}
// 	for (const auto& chan_name : channels){
// 		std::shared_ptr<Channel> channel = getChannelByName(chan_name);
// 		if (channel == nullptr){
// 			Channel new_channel(chan_name, cli);
// 			responseToClient(cli, rplJoinChannel(cli, chan_name));
// 		} else {
// 			const std::unordered_set<Client*>& user_list = channel->getChannelUsers();
// 			channel->addNewUser(cli);
// 			for (const auto& user : user_list){
// 				responseToClient(user, )
// 			}
// 		}
// 	}
// }

// bool	Server::isExistedChannel(const std::string& channel_name){
// 	for (const auto& [name, channelPtr] : channels_){
// 		if (name == channel_name){
// 			return true;
// 		}
// 	}
// 	return false;
// }


// {PART, &Server::partCommand},


