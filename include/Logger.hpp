/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 09:34:26 by jingwu            #+#    #+#             */
/*   Updated: 2025/05/13 08:34:26 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <iostream>

// Define colours
#define RESET "\033[0;0m"
#define RED "\033[0;31m"
#define ORANGE "\033[0;91m"
#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"

#ifndef LOG_LEVEL
 #define LOG_LEVEL Logger::INFO
#endif // LOG_LEVEL

class Logger{
	public:
		enum LEVEL{
			DEBUG,
			INFO,
			WARNING,
			ERROR,
		};
		static const std::chrono::system_clock::time_point start_time;

		~Logger();
		static void log(enum LEVEL level, std::string msg);

	private:
		Logger() = delete;
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		static void cleanMessage(std::string& msg);
};
