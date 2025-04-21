# ft_irc

## Stages for Server

The server is created using the following steps:

	1. Socket Createtion(using socket());
	2. Setsockopt(set socket option, using setsockopt());
	3. Bind(using bind());
	4. Listen(using listen());
	5. Accept(using accept());

### 1. socket()

🔧 Purpose:
The socket() function is used in network programming to create a communication endpoint, known as a socket.

🧱 Syntax:
```
int socket(int domain, int type, int protocol);
```
It returns an integer file descriptor (like a handle) that represents the socket.

🧩 Parameter Breakdown:

1. domain — Communication domain (or address family):

	- AF_INET: IPv4 Internet protocols

	- AF_INET6: IPv6 Internet protocols

	- AF_UNIX: Local communication (between processes on the same machine)

2. type — Communication type:

	- SOCK_STREAM: Stream-based (TCP) — reliable, connection-oriented

	- SOCK_DGRAM: Datagram-based (UDP) — fast, connectionless

3. protocol — Usually set to 0 to select the default protocol for the given domain and
   type (e.g., TCP for SOCK_STREAM).

✅ Example:
```
int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
```
This creates a TCP socket using IPv4 and stores the socket file descriptor in serverSocket.

### 2. setsockopt()

setsockopt() – Optional, but recommended.

🔧 Purpose:
Set options on the socket — for example, allow port reuse so you can restart the server
without "address already in use" errors.

🧱 Syntax:
```
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```
🧩 Parameter Breakdown:

	Parameter | Description
	sockfd    | The socket file descriptor
	level     | Level of the option (typically SOL_SOCKET)
	optname   | The specific option (e.g. SO_REUSEADDR)
	optval    | Pointer to the option value
	optlen    | Size of the option value

✅ Example:
```
int opt = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```
This allows the socket to reuse the port even if it's in TIME_WAIT state after being closed.


### 3. bind()

🔧 Purpose:
Bind your socket to a specific IP address and port so it can receive incoming connections.

🧱 Syntax:
```
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
🧩 Parameter Breakdown:

	Parameter |  Description
	sockfd    |  The socket descriptor created by socket()
	addr      |  Pointer to a struct specifying IP address and port
	addrlen   |  Size of that address structure

✅ Example:
```
struct sockaddr_in address;
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(8080);  // Convert port to network byte order

bind(server_fd, (struct sockaddr *)&address, sizeof(address));
```
This binds the server to all available IPs on port 8080.

### 4. listen()

🔧 Purpose:
Put the socket into passive mode, ready to accept connection requests.

🧱 Syntax:
```
int listen(int sockfd, int backlog);
```
🧩 Parameter Breakdown:

	Parameter |  Description
	sockfd    |  The socket descriptor
	backlog   |  Max number of pending connections allowed in the queue

✅ Example:
```
listen(server_fd, 10);  // Can queue up to 10 pending clients
```
### 5. accpt()
🔧 Purpose:
Accept a new client connection and return a new socket for communication with that client.

🧱 Syntax:
```
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
🧩 Parameter Breakdown:

	Parameter |  Description
	sockfd    |  Listening socket
	addr      |  Will be filled with client’s address info
	addrlen   |  Input: size of addr; Output: actual size used

✅ Example:
```
int client_fd;
struct sockaddr_in client_addr;
socklen_t addrlen = sizeof(client_addr);

client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);
```


## References

https://www.geeksforgeeks.org/socket-programming-cc/

https://datatracker.ietf.org/doc/html/rfc1459
