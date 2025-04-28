
#pragma once

#include <string>
#include <iostream>

class Message{
	public:
	Message(std::string& message);
	~Message();

	private:
	std::string	msg_;

};
