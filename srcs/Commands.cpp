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
