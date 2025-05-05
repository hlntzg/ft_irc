
#include "Message.hpp"

Message::Message(std::string& message) : whole_msg_(message){
}

Message::~Message(){
}


void	Message::parseMessage(){
 // need implement the function
 // for testing
	msg_type_ = COMMANDTYPE::JOIN;
}



void	Message::pass(Client& cli){
 // need implement the function
}

void	Message::quit(Client& cli){
	// need implement the function
   }


COMMANDTYPE	Message::getType(){
	return msg_type_;
}
