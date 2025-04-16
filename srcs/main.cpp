/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 10:32:32 by jingwu            #+#    #+#             */
/*   Updated: 2025/04/16 12:58:05 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

int main(int ac, char** av){
    if (ac != 3){
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        exit(EXIT_FAILURE);
    }
    try{
        ?????????
        exit (EXIT_SUCCESS);
    }catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
