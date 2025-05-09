#include "Server.hpp"

/**
 * @brief Checking if the passed nick is in use
 *
 * @param nick The nickname to be checked
 *
 * @return
 * - `true`: The nickname is already in use
 * - `false`: The nickname is available
 */
// bool	Server::isNickInUse(const std::string& nick){
// 	for (auto& [fd, user] : clients_){
// 		if (user->getNick() == nick){
// 			return true;
// 		}
// 	}
// 	return false;
// }

// void	Server::nickCommand(Message& msg, Client& cli){
// 	std::vector<std::string>	params = msg.getMsgParams();
// 	if (params.size() == 0){
// 		responseToClient(cli, nonNickNameGiven());
// 		return;
// 	}
// 	const std::string& nick = params.at(0);
// 	if (isNickInUse(nick) == true){
// 		responseToClient(cli, nickNameinuse(""));
// 	}
// 	// checking if nick string contains only valid characters
// 	for (auto c : nick){
// 		if (!isalnum(static_cast<unsigned char>(c)) && std::string(SPECIAL_CHARS).find(c) == std::string::npos){
// 			responseToClient(cli, erroneusNickName(""));
// 			return;
// 		}
// 	}
// 	cli.setNick(nick);
// 	attempRegisterClient(cli);
// }

// void	Server::userCommand(Message& msg, Client& cli){
// 	std::vector<std::string>	params = msg.getMsgParams();
// 	if (params.size() < 4){
// 		responseToClient(cli, needMoreParams("USER"));
// 		return;
// 	}
// 	const std::string& username = params.at(0);
// 	const std::string& realname = params.at(3);
// 	for (auto c : username){
// 		if (!isalnum(static_cast<unsigned char>(c)) && std::string(SPECIAL_CHARS).find(c) == std::string::npos){
// 			responseToClient(cli, erroneusNickName(""));
// 			return;
// 		}
// 	}
// 	for (auto c : realname){
// 		if (!isalnum(static_cast<unsigned char>(c)) && std::string(SPECIAL_CHARS).find(c) == std::string::npos){
// 			responseToClient(cli, erroneusNickName(""));
// 			return;
// 		}
// 	}
// 	cli.setUsername(username);
// 	cli.setRealname(realname);
// 	cli.setHostname(params.at(1));
// 	cli.setServername(params.at(2));
// 	attempRegisterClient(cli);
// }

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


// void	Server::quitCommand(Message& msg, Client& cli){
// 	std::vector<std::string>	params = msg.getMsgParams();
// 	removeClient(cli, params.at(0));
// }

