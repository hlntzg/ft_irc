/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 09:53:01 by jingwu            #+#    #+#             */
/*   Updated: 2025/04/22 10:29:01 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

Logger::~Logger(){
}

/**
 * start_time{}: uniform initialization, added in c++11
 * For example:
 * 	int x1 = 3.14; // it can be compiled, but the fraction part will be lost
 *  int x1{3.14}; // Can't compile, more safer
 */
const std::chrono::system_clock::time_point Logger::start_time{
	std::chrono::system_clock::now()};

void Logger::log(enum LEVEL level, std::string msg){
	if (level >= LOG_LEVEL){
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::system_clock::now() - start_time).count();
		if (level == DEBUG){
			std::cout << GREEN << diff << " {DEBUG} ";
		} else if (level == INFO){
			std::cout << BLUE << diff << " {INFO} ";
		} else if (level == WARNING){
			std::cout << ORANGE << diff << " {WARNING} ";
		} else if (level == ERROR){
			std::cout << RED << diff << " {ERROR} ";
		}
		cleanMessage(msg);
		std::cout << msg << RESET << std::endl;
	}
}

void Logger::cleanMessage(std::string& msg){
	while (msg.back() == '\n' || msg.back() == '\r'){
		msg.pop_back();
	}
}
