
#include "Message.hpp"

Message::Message(std::string& message) : whole_msg_(message){
}

Message::~Message(){
}


void	Message::parseMessage(){
 // need implement the function
 // for testing
	cmd_type_ = COMMANDTYPE::JOIN;
	cmd_str_type_ = "JOIN";
}



COMMANDTYPE	Message::getCmdType() const{
	return cmd_type_;
}

const std::string&	Message::getCmdStrType() const{
	return cmd_str_type_;
}


const std::vector<std::string>&	Message::getMsgParams() const{
	return msg_params_;
}
