
#pragma once

#include <string>
#include <iostream>

class Message{
	public:
	Message(std::string& message);
	~Message();

	void	parseMessage();

	private:
	std::string	msg_;

};
