/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 12:38:40 by jingwu            #+#    #+#             */
/*   Updated: 2025/05/01 14:09:49 by jingwu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/**
 * Parsing the parameters and initilize the pass and port variables
 */
Server::Server(std::string port, std::string password){
	// 1. parsing for port
	for (auto it : port){
		if (!isdigit(it)){
			throw std::invalid_argument("Error: non-digit character found in port");
		}
	}
	int port_num;
	try{
		port_num = std::stoi(port);
		if (port_num < 1 || port_num > 65535){
			throw std::overflow_error("Error: port is out of range 1-65535");
		}
	} catch(const std::exception& e){
		throw std::runtime_error("Error: port is out of range 1-65535");
	}

	// 2. paring for password
	if (password.size() < 4){
		throw std::overflow_error("Error: password is less than 4 characters");
	}
	// if we don't cast the c to unsigned char, it can crash with UTF-8
	for (auto c : password){
		if (!isalnum(static_cast<unsigned char>(c)) && std::string(SPECIAL_CHARS).find(c) == std::string::npos){
			throw std::invalid_argument("Error: invalid character in password: '"
				+ std::string(1,c) + "'\n" + PASSWORD_RULE);
		}
	}
	serv_port_ = port_num;
	serv_passwd_ = password;
	epoll_fd_ = epoll_create1(0);
	if (epoll_fd_ == -1){
		throw std::runtime_error("Error: epoll_create1 failed");
	}
}

Server*	Server::server_ = nullptr;

volatile sig_atomic_t	Server::keep_running_ = 1;



/**
 * @brief Define the commands that an unregistered client can execute. Currenctly
 * set to PASS, NICK, USER and QUIT
 */
const std::set<COMMANDTYPE> Server::pre_registration_allowed_commands_ = {
	PASS,
	NICK,
	USER,
	QUIT
};




Server::~Server(){
}

void	Server::signalHandler(int signum){
	if (signum == SIGINT || signum == SIGTERM){
		Server::keep_running_ = 0;
	}
}

void	Server::setupSignalHandlers(){
	struct sigaction sa;
	sa.sa_handler = signalHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	signal(SIGPIPE, SIG_IGN);
}

/**
 * Stages for Server
 * 	The server is created using the following steps:
 * 		1) Socket Createtion;
 * 		2) Setsockopt;;
 * 		3) Bind;
 * 		4) Listen;
 */
void	Server::setupServSocket(){
	// 1. Socket createtion
	Logger::log(Logger::INFO, "initServer::Socket createtion ");
	serv_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_fd_ == -1){
		throw std::runtime_error("Error: failed to create socket for the server");
	}
	// 2. enable port reuse
	int opt = 1;
	// pass the value of opt to "const void* optval", in this case it is "SO_REUSEADDR"
	// or "SO_REUSEPORT", when the value is 1, it means turn it on; when the value is '0'
	// it means turn it off
	Logger::log(Logger::INFO, "initServer::Set socket option");
	if (setsockopt(serv_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt)) < 0){
		throw std::runtime_error("Error: setsockopt");
	}
	// 3. Bind to all the avaiable IPs and server port
	Logger::log(Logger::INFO, "initServer::Binding on port " + std::to_string(serv_port_));
	memset(&serv_addr_, 0, sizeof(serv_addr_)); // zero out everyting before use
	serv_addr_.sin_family = AF_INET;
	serv_addr_.sin_addr.s_addr = INADDR_ANY;
	serv_addr_.sin_port = htons(this->serv_port_);
	if (bind(serv_fd_, (sockaddr*)&serv_addr_, sizeof(serv_addr_)) < 0){
		throw std::runtime_error("Error: bind failed");
	}
	// 4. listen, the backlog number(now set to 10) can be changed based on the
	// performance during testing
	// After do listen(fd, backlog), now the "fd" become a listening fd.
	if (listen(serv_fd_, 10) == -1){
		throw std::runtime_error("Error: something wrong happended on listen");
	}

	// 5. register listeing socket for read(EPOLLIN)
	int flags = fcntl(serv_fd_, F_GETFL, 0);
	fcntl(serv_fd_, F_SETFL, flags | O_NONBLOCK);

	struct epoll_event ev{};
	// EPOLLIN for read events + EPOLLET for edge-triggered
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = serv_fd_;
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, serv_fd_, &ev) == -1){
		throw std::runtime_error("Error: epoll_ctl ADD listen_fd failed");
	}
	// prepare event buffer
	events_.resize(MAX_EVENTS);

	// add log message
	Logger::log(Logger::INFO, "Server listening on port " + std::to_string(serv_port_));
}

void	Server::startServer(){
	setupSignalHandlers();
	setupServSocket();
	while (keep_running_){
		// Wait indefinitely for events
		int nready = epoll_wait(epoll_fd_, events_.data(), events_.size(), -1);
		if (nready < 0){
			if (errno == EINTR){
				continue; // restart on signal
			}
			throw std::runtime_error("Error:" + std::string("epoll_wait: ") + strerror(errno));
		}
		for (int i = 0; i < nready; i++){
			int		fd = events_[i].data.fd;
			auto	evs = events_[i].events;
			// 1) new connections on listening socket, accept it
			if (fd == serv_fd_){
				acceptNewClient();
				Logger::log(Logger::DEBUG, "Active clients: " +
							std::to_string(clients_.size()));
				continue;
			}
			// 2) check for error or hang-up
			else if (evs & (EPOLLERR | EPOLLHUP)){
				removeClient(fd, "disconnected");
				continue;
			}
			// 3) date to read
			else if (evs & EPOLLIN){
				try {
					processDataFromClient(i);
				}catch (std::invalid_argument& e){
					Logger::log(Logger::WARNING, e.what());
				} catch (std::exception& e){
					Logger::log(Logger::ERROR, e.what());
				}
			}
		}
	}
	Logger::log(Logger::INFO, "Shutting down Server");
	for (auto const& [fd, cli] : clients_) {
		epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
		close(fd);
	}
	clients_.clear();
	close(epoll_fd_);
	close(serv_fd_);
	return;
}

/**
 * @brief This function will accept all the pending connections at once.
 */
void	Server::acceptNewClient(){
	// Process all pending connections at once before processing other events
	while (true) {
        sockaddr_in client_addr;
        socklen_t  clientLen = sizeof(client_addr);
        int client_fd = accept(serv_fd_,
                               reinterpret_cast<sockaddr*>(&client_addr),
                               &clientLen);
        if (client_fd < 0) {
            // No more pending connections
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return ;
            }
            // real errors
            throw std::runtime_error("accept failed: " + std::string(strerror(errno)));
        }

        // setting non-blockning mode
        int flags = fcntl(client_fd, F_GETFL, 0);
        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

        // Register new client into epoll
        epoll_event ev{};
        ev.events = EPOLLIN;
        ev.data.fd = client_fd;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
            close(client_fd);
            throw std::runtime_error("epoll_ctl ADD client failed");
        }
        clients_[client_fd] = Client(client_fd);
        Logger::log(Logger::INFO, "New client " + std::to_string(client_fd));
    }
}

/**
 * @brief This function will first try to receive the data from client socket first. If
 * receive successfully, then store it in client instantiation; otherwise, it means
 * the client wants to close the connection. Second, if the receive the client data
 * succeffuly, then get the line separate by CRLF, saving into buffer. then parse it,
 * execute it.
 */
void	Server::processDataFromClient(int idx){
	int	client_fd = events_[idx].data.fd;
	Client* client = &(clients_[client_fd]);
	if (!client->receiveRawData()){
		Logger::log(Logger::INFO, "Client '" + std::to_string(client_fd) + "' disconnected");
		removeClient(client_fd, "Client disconnect");
		return;
	}
	std::string	buffer;
	// extract one line command/message that separate by CRLF
	while (client->getNextMessage(buffer)){
		try{
			Message	msg(buffer);
			msg.parseMessage();
			executeCommand(msg, *client);
		} catch (std::exception& e){
			Logger::log(Logger::WARNING, e.what());
		}
	}
}

// Not finish yet
void	Server::removeClient(int fd, std::string reason){
	// for testing only now, latter will implement
	Logger::log(Logger::INFO, "Removing client " + std::to_string(fd) + ": " + reason);
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
    close(fd);
    clients_.erase(fd);
	?????????
}

/**
 * @brief If the client hasn't finished registration process, and execute no-permission-
 * command, then just return. Otherwise, call Message::execute to execute the command
 *
 */
void	Server::executeCommand(Message& msg, Client& cli){
	COMMANDTYPE	type = msg.getType();
	// If the client hasn't finished registration, then the commands are limited
	if (!cli.isRegistered() && pre_registration_allowed_commands_.find(type)
		== pre_registration_allowed_commands_.end() ){
		Logger::log(Logger::INFO, "Unregistered client can't execute the command");
		return;
	}
	msg.execute(cli);
}

/**
 * @brief Send response message to client
 *
 * @param cli: the response message receiver
 * @param repsonse: the reponse message
 *
 * @return bytes written or throw error(negative value)
 */
int	Server::responseToClient(Client& cli, const std::string& response){
	int	n_bytes = send(cli.getSocketFD(), response.c_str(), response.length(), MSG_DONTWAIT);
	if (n_bytes < 0){
		Logger::log(Logger::WARNING, "Failed to send data to user " + cli.getNick() +
		": " + response);
	} else {
		Logger::log(Logger::DEBUG, "Sent successfully "+ cli.getNick() + ": " + response);
	}
	return (n_bytes);
}
