
#include "Message.hpp"

Message::Message(std::string& message) : whole_msg_(message){
}

/**
 * @brief Define the commands that only an operator can execute. Currenctly
 * set to: OPER, KICK, INVITE, TOPIC and MODE
 */
const std::set<COMMANDTYPE> Message::operator_commands_ = {
	KICK,
	INVITE,
	TOPIC,
	MODE
};

/**
 * @brief Setup the map for commands and execute functions
 */
const std::unordered_map<COMMANDTYPE, Message::executeFunc> Message::execute_map_ = {
	{PASS, &Message::passCommand},
	// {NICK, &Message::nickCommand},
	// {USER, &Message::userCommand},
	// {PRIVMSG, &Message::privmsgCommand},
	// {JOIN, &Message::joinCommand},
	// {PART, &Message::partCommand},
	// {OPER, &Message::operCommand},
	// {KICK, &Message::kickCommand},
	// {INVITE, &Message::inviteCommand},
	// {TOPIC, &Message::topicCommand},
	// {MODE, &Message::modeCommand},
	{QUIT, &Message::quitCommand}
};

Message::~Message(){
}


void	Message::parseMessage(){
 // need implement the function
 // for testing
	msg_type_ = COMMANDTYPE::JOIN;
}



void	Message::passCommand(Client& cli){
 // need implement the function
}

void	Message::quitCommand(Client& cli){
	// need implement the function
   }


COMMANDTYPE	Message::getType(){
	return msg_type_;
}


void	Message::execute(Client& cli){
	// If an not-operator client try to execute operator commands, then reject.
	if (operator_commands_.find(msg_type_) != operator_commands_.end()){
		std::unordered_set<std::shared_ptr<Channel>>::iterator it = msg_channels_.begin();
		for (; it != msg_channels_.end(); it++){
			// need add a checking user's permisson in channle class
			if (!it->isClientOperator(cli)){
				Server::responseToClient(// need add response message here);
			}
		}
	}
// How should I know if the client has permission on all the channels???

	auto it = execute_map_.find(msg_type_);
	if (it != execute_map_.end()){
		(this->*it->second)(cli);
	}
}
