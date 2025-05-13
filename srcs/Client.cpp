/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 12:59:33 by jingwu            #+#    #+#             */
/*   Updated: 2025/05/13 08:54:47 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : socket_fd_(0), isRegistered_(0){}

Client::Client(int fd, std::string host) : socket_fd_(fd), hostname_(host),
isRegistered_(0){
}

Client&	Client::operator=(const Client& other){
	if (this != & other){
		socket_fd_ = other.socket_fd_;
        nick_ = other.nick_;
        username_ = other.username_;
        realname_ = other.realname_;
        hostname_ = other.hostname_;
        servername_ = other.servername_;
        password_ = other.password_;
        raw_data_ = other.raw_data_;
        isRegistered_ = other.isRegistered_;
	}
	return *this;
}

Client::~Client(){
}

int Client::getSocketFd() const{
	return socket_fd_;
}

const std::string&	Client::getUsername() const{
	return username_;
}

const std::string&	Client::getNick() const{
	return nick_;
}

const std::string&	Client::getPassword() const{
    return password_;
}

/**
 * @brief Used when the server responding to a command issued by client.
 * For example:
 *    PRIVMSG bob :Hello, Bob!
 *
 * Server responds with:
 *    :alice!alice@hostname PRIVMSG bob :Hello, Bob!
 */
std::string	Client::getPrefix() const{
	return (":" + nick_ + "!" + username_ + "@" + hostname_);
}


void	Client::setNick(const std::string& nick){
    nick_ = nick;
}

void	Client::setUsername(const std::string& username){
    username_ = username;
}

void	Client::setRealname(const std::string& realname){
    realname_ = realname;
}

void	Client::setHostname(const std::string& hostname){
    hostname_ = hostname;
}

void	Client::setServername(const std::string& servername){
    servername_ = servername;
}

void	Client::setPassword(const std::string& passwd){
    password_ = passwd;
}

void	Client::setRegistrationStatus(bool	status){
    isRegistered_ = status;
}


/**
 * @brief Receive the raw data from socket, filling/saving into receive buffer.
 *
 * @return
 *  True, read successful;
 *  False, some error;
 *
 */
bool	Client::receiveRawData(){
	char buffer[BUFFER_SIZE + 1];
    ssize_t bytes_read;

    while (true) {
        bytes_read = recv(socket_fd_, buffer, BUFFER_SIZE, 0);

        if (bytes_read > 0) {
            raw_data_.append(buffer, bytes_read);
        } else if (bytes_read == 0) {
            // Connection closed
            return false;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // All data read
                break;
            } else {
                // Other errors
                perror("recv");
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief Gets the next CRLF separated(IRC rule) message from the receive raw data
 * and stores it into passed paramenter buffer.
 *
 * @param
 * buffer to store the message in
 *
 * @return
 * true: if successfully get the message
 * false: raw data is empty.
 */
bool	Client::getNextMessage(std::string& buffer){
	// Find the position of CRLF ("\r\n") in raw_data_
    size_t crlf_pos = raw_data_.find("\n");

    if (crlf_pos == std::string::npos) {
        // No complete message yet
        return false;
    }

    // Extract the message (including CRLF)
    buffer = raw_data_.substr(0, crlf_pos + 2);

    // Remove the processed message from raw_data_
    raw_data_.erase(0, crlf_pos + 2);

    return true;
}

bool	Client::isRegistered(){
	return isRegistered_;
}

// for testing only
void	Client::printInfo() const{
    std::cout << "User info:\n";
    std::cout << "  fd:" << socket_fd_ << std::endl;
    std::cout << "  nick:" << nick_ << std::endl;
    std::cout << "  username:" << username_ << std::endl;
    std::cout << "  realname:" << realname_ << std::endl;
    std::cout << "  hostname:" << hostname_ << std::endl;
    std::cout << "  servername:" << servername_ << std::endl;
    std::cout << "  password:" << password_ << std::endl;
    std::cout << "  isRegistered:" << isRegistered_ << std::endl;
}

void    Client::printRawData() const{
    std::cout << "rawdata:" << raw_data_ << std::endl;
}
