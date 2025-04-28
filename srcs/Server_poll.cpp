/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jingwu <jingwu@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 12:38:40 by jingwu            #+#    #+#             */
/*   Updated: 2025/04/25 13:53:15 by jingwu           ###   ########.fr       */
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
}

Server*	Server::server_ = nullptr;

volatile sig_atomic_t	Server::keep_running_ = 1;

Server::~Server(){
	// for (auto&	ch : channels_){
	// 	ch.second.shutDownChannel();
	// }
}

void	Server::signalHandler(int signum){
	(void)signum;
	Server::keep_running_ = 0;
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
 * 		5) Accept;
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
	// add log message
	Logger::log(Logger::INFO, "Server listening on port " + std::to_string(serv_port_));
	// POLLIN: 	We're interested in readable events (e.g., new client trying to connect)
	// 0: Set to 0 now; will be filled by poll() later to tell what actually happened
	poll_fds_.push_back({serv_fd_, POLLIN, 0});
}

void	Server::startServer(){
	setupSignalHandlers();
	setupServSocket();
	while (keep_running_){
		// poll_fds_.data() points to the first element of your std::vector<pollfd>,
		// it is the address of the vector too.
		if(poll(poll_fds_.data(), poll_fds_.size(), -1) < 0){
			// errno: a global variable that records the error code when the last
			// system call failed
			// strerror: convert the errno into a readable strin message
			throw std::runtime_error(strerror(errno)); // What is strerror and errno
		}
		for (size_t i = 0; i < poll_fds_.size(); i++){
			// Find the pollfd events is POLLIN
			// "POLLIN" means “ready to read” — e.g., new data available or,
			// on a listening socket, a new connection is pending.
			// Bitwise test, "revents & POLLIN" checks whether the POLLIN bit is set.
			if (poll_fds_[i].events & POLLIN){
				try {
					// When there is POLLIN event happens,
					// for the listeing socket, it means there is new connection;
					// for the client socket, it means there is new data to be read;
					if (poll_fds_[i].fd == serv_fd_){
						acceptNewClient();
						Logger::log(Logger::DEBUG, "Amount of clients' requests: " +
									std::to_string(poll_fds_.size() - 1)); // minus 1, because there is on it listening socket
					} else {
						processDataFromClient(i);
					}
				} catch (std::invalid_argument& e){
					Logger::log(Logger::WARNING, e.what());
				} catch (std::exception& e){
					Logger::log(Logger::ERROR, e.what());
				}
			}
		}
	}
	Logger::log(Logger::INFO, "Shutting down Server");
	if (server_){
		close (server_->serv_fd_);
		delete server_; // safely delete, calls destructor. Don't call destructor
						// directly like "server_->~Server()"
	}
	return;
}

/**
 * @brief Saving the new clinet socket into client_fd.
 * If client_fd > 0, add the client_fd into poll_fds_ vector for the future communation.
 * 	And save the client into clients_ map.
 * If client_fd < 0, it means there is an error generated, then throw the error.
 */
void	Server::acceptNewClient(){
	struct sockaddr_in	client_addr;
	socklen_t	clinetLen = sizeof(client_addr);
	int	client_fd = accept(serv_fd_, (sockaddr*)&serv_addr_, &clinetLen);
	if (client_fd > 0){
		Logger::log(Logger::INFO, "New client '" + std::to_string(client_fd)
					+ "' has connected");
		poll_fds_.push_back({client_fd, POLLIN, 0});
		this->clients_[client_fd] = Client(client_fd);
	}else{
		throw std::runtime_error(strerror(errno));
	}
}

void	Server::processDataFromClient(size_t idx){
	std::string	buff;
	Client*	client = &(clients_[poll_fds_[idx].fd]);

}
