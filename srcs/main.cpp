/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 10:32:32 by jingwu            #+#    #+#             */
/*   Updated: 2025/04/22 14:33:57 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"

int main(int ac, char** av){
    if (ac != 3){
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        exit(EXIT_FAILURE);
    }
    try{
        Server serv(av[1], av[2]);
        serv.start();
        exit(EXIT_SUCCESS);
    }catch(const std::exception& e){
        std::cerr << RED << e.what() << RESET << std::endl;
        exit(EXIT_FAILURE);
    }
}
