# ft_irc

## IRC Message

### 1.1 Connection Resigstration

A "PASS" command is not required for either client or server connection to be registered, but it must precede the server message or the latter of the NICK/USER combination. It is strongly recommended that all server connections have a password in order to give some level of security to the actual connections. The recommended order for a client to
register is as follows:<br>
	1. Pass message <br>
	2. Nick message <br>
	3. User message <br>

#### 1.1.1 Password message

Command:   PASS <br>
Parameter: `<password>`

The PASS command is used to set a 'connection password'. The password can and must be set before any attempt to register the connection is made. Currently this requires that clients send a PASS command before sending the NICK/USER combination and servers *must*
send a PASS command before any SERVER command. The password supplied must match the one contained in the C/N lines (for servers) or I lines (for clients). It is possible to send multiple PASS commands before registering but only the last one sent is used for verification and it may not be changed once registered.
Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED

Example:
```bash
PASS secretpasswordhere
```
#### 1.1.2 NICK message

Command:   NICK <br>
Parameter: `<nickname> [ <hopcount> ]`

NICK message is used to give user a nickname or change the previous one. The <hopcount> parameter is only used by servers to indicate how far away a nick is from its home server.  A local connection has a hopcount of 0.  If supplied by a client, it must be ignored.

If a NICK message arrives at a server which already knows about an identical nickname for another client, a nickname collision occurs. As a result of a nickname collision, all
instances of the nickname are removed from the server's database, and a KILL command is issued to remove the nickname from all other server's database. If the NICK message causing the collision was a nickname change, then the original (old) nick must be removed as well.

If the server recieves an identical NICK from a client which is directly connected, it may issue an ERR_NICKCOLLISION to the local client, drop the NICK command, and not generate any kills.<br>
Numeric Replies:

           ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
           ERR_NICKNAMEINUSE               ERR_NICKCOLLISION

Example:
a. Introducing new nick "Wiz"
```bash
NICK Wiz
```
b. WiZ changed his nickname to Kilroy
```bash
:Wiz NICK Kilroy
```
#### 1.1.3. User message

Command:   USER <br>
Parameter: `<username> <hostname> <servername> <realname>`

The USER message is used at the beginning of connection to specify the username, hostname, servername and realname of s new user.  It is also used in communication between servers to indicate new user arriving on IRC, since only after both USER and NICK have been
received from a client does a user become registered.

Between servers USER must to be prefixed with client's NICKname. Note that hostname and servername are normally ignored by the IRC server when the USER command comes from a directly connected client (for security reasons), but they are used in server to server
communication.  This means that a NICK must always be sent to a remote server when a new user is being introduced to the rest of the network before the accompanying USER is sent.

It must be noted that realname parameter must be the last parameter, because it may contain space characters and must be prefixed with a colon (':') to make sure this is recognised as such.

Since it is easy for a client to lie about its username by relying solely on the USER message, the use of an "Identity Server" is recommended.  If the host which a user connects from has such a server enabled the username is set to that as in the reply from the
"Identity Server". <br>
Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED

Exmaple:

User registering themselves with a username of "guest" and real name "Ronnie Reagan".
```bash
USER guest tolmoon tolsun :Ronnie Reagan
```

message between servers with the nickname for which the USER command belongs to
```bash
:testnick USER guest tolmoon tolsun :Ronnie Reagan
```

#### 1.1.4. Oper

Command:   OPER <br>
Parameter: `<user> <password>`

OPER message is used by a normal user to obtain operator privileges. The combination of <user> and <password> are required to gain Operator privileges.

If the client sending the OPER command supplies the correct password for the given user, the server then informs the rest of the network of the new operator by issuing a "MODE +o" for the clients nickname.
The OPER message is client-server only.
Numeric Replies:

           ERR_NEEDMOREPARAMS              RPL_YOUREOPER
           ERR_NOOPERHOST                  ERR_PASSWDMISMATCH

Example:
Attempt to register as an operator using a username of "foo" and "bar" as the password.
```bash
OPER foo bar
```
#### 1.1.5. Quit

Command:   QUIT <br>
Parameter: [`<Quit message>`]

A client session is ended with a quit message.  The server must close the connection to a client which sends a QUIT message. If a "Quit Message" is given, this will be sent instead of the default message, the nickname.

When netsplits (disconnecting of two servers) occur, the quit message is composed of the names of two servers involved, separated by a space.  The first name is that of the server which is still connected and the second name is that of the server that has become
disconnected.

If, for some other reason, a client connection is closed without the client  issuing  a  QUIT  command  (e.g.  client  dies and EOF occurs on socket), the server is required to fill in the quit  message  with some sort  of  message  reflecting the nature of the event which caused it to happen.
Numeric Replies: None.

Example:
Preferred message format.
```bash
QUIT :Gone to have lunch
```

### 1.2 Join message

#### 1.2.1 Join message

Command:   JOIN <br>
Parameter: `<channel>{,<channel>} [<key>{,<key>}]`

Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_BANNEDFROMCHAN
           ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
           ERR_CHANNELISFULL               ERR_BADCHANMASK
           ERR_NOSUCHCHANNEL               ERR_TOOMANYCHANNELS
           RPL_TOPIC

Examples:

<pre>   JOIN #foobar                    ; join channel #foobar.

   JOIN &foo fubar                 ; join channel &foo using key "fubar".

   JOIN #foo,&bar fubar            ; join channel #foo using key "fubar"
                                   and &bar using no key.

   JOIN #foo,#bar fubar,foobar     ; join channel #foo using key "fubar".
                                   and channel #bar using key "foobar".

   JOIN #foo,#bar                  ; join channels #foo and #bar.

   :WiZ JOIN #Twilight_zone        ; JOIN message from WiZ </pre>

#### 1.2.2 Kick command

Command:    KICK<br>
Parameters: `<channel> <user> [<comment>]`

Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
           ERR_BADCHANMASK                 ERR_CHANOPRIVSNEEDED
           ERR_NOTONCHANNEL

Examples:

<pre>KICK &Melbourne Matthew         ; Kick Matthew from &Melbourne

KICK #Finnish John :Speaking English
                                ; Kick John from #Finnish using
                                "Speaking English" as the reason
                                (comment).

:WiZ KICK #Finnish John         ; KICK message from WiZ to remove John
                                from channel #Finnish </pre>

NOTE:
It is possible to extend the KICK command parameters to the following:

`<channel>{,<channel>} <user>{,<user>} [<comment>]`

#### 1.2.3 Invite message

Command:    INVITE<br>
Parameters: `<nickname> <channel>`

The INVITE message is used to invite users to a channel. The parameter <nickname> is the nickname of the person to be invited to the target channel <channel>.  There is no requirement that the channel the target user is being invited to must exist or be a valid
channel.  To invite a user to a channel which is invite only (MODE +i), the client sending the invite must be recognised as being a channel operator on the given channel.

Numeric Replies:

		ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
		ERR_NOTONCHANNEL                ERR_USERONCHANNEL
		ERR_CHANOPRIVSNEEDED
		RPL_INVITING                    RPL_AWAY

Examples:

<pre>:Angel INVITE Wiz #Dust         ; User Angel inviting WiZ to channel
                                     #Dust

INVITE Wiz #Twilight_Zone       ; Command to invite WiZ to
                                #Twilight_zone</pre>

#### 1.2.4 Topic message

Command:    TOPIC<br>
Parameters: `<channel> [<topic>]`

The TOPIC message is used to change or view the topic of a channel. The topic for channel <channel> is returned if there is no <topic> given.  If the <topic> parameter is present, the topic for that channel will be changed, if the channel modes permit this action.
Numeric Replies:

		ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
		RPL_NOTOPIC                     RPL_TOPIC
		ERR_CHANOPRIVSNEEDED


Examples:

<pre>:Wiz TOPIC #test :New topic     ;User Wiz setting the topic.

TOPIC #test :another topic      ;set the topic on #test to "another
                                   topic".

TOPIC #test                     ; check the topic for #test.</pre>

#### 1.2.5 Mode message (here just introduce channel mode)

Command: MODE
Parameters: `<channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>] [<ban mask>]`

The MODE command is provided so that channel operators may change the
characteristics of `their' channel.  It is also required that servers
be able to change channel modes so that channel operators may be
created.

The various modes available for channels are as follows:(in this prject, we just
need to implement modes: i, t, k, o and l)

           o - give/take channel operator privileges;
           p - private channel flag;
           s - secret channel flag;
           i - invite-only channel flag;
           t - topic settable by channel operator only flag;
           n - no messages to channel from clients on the outside;
           m - moderated channel;
           l - set the user limit to channel;
           b - set a ban mask to keep users out;
           v - give/take the ability to speak on a moderated channel;
           k - set a channel key (password).

When using the 'o' and 'b' options, a restriction on a total of three
per mode command has been imposed.  That is, any combination of 'o'
and

Examples:

<pre>MODE #Finnish +im               ; Makes #Finnish channel moderated and
                                'invite-only'.

MODE #Finnish +o Kilroy         ; Gives 'chanop' privileges to Kilroy on
                                channel #Finnish.

MODE #Finnish +v Wiz            ; Allow WiZ to speak on #Finnish.

MODE #Fins -s                   ; Removes 'secret' flag from channel
                                #Fins.

MODE #42 +k oulu                ; Set the channel key to "oulu".

MODE #eu-opers +l 10            ; Set the limit for the number of users
                                on channel to 10.

MODE &oulu +b                   ; list ban masks set for channel.

MODE &oulu +b *!*@*             ; prevent all users from joining.

MODE &oulu +b *!*@*.edu         ; prevent any user from a hostname
                                matching *.edu from joining.</pre>

###  1.3 Private message

Command:    PRIVMSG<br>
Parameters: `<receiver>{,<receiver>} <text to be sent>`

PRIVMSG is used to send private messages between users.  <receiver> is the nickname of the receiver of the message.  <receiver> can also be a list of names or channels separated with commas.

The <receiver> parameter may also me a host mask (#mask) or server mask ($mask). In both cases the server will only send the PRIVMSG to those who have a server or host matching the mask. The mask  must have at least 1  (one) "." in it and no wildcards following the
last ".". This requirement exists to prevent people sending messages to "#*" or "$*",  which  would  broadcast to all users; from experience, this is abused more than used responsibly and properly. Wildcards are the '*' and  '?' characters. This extension to the PRIVMSG command is only available to Operators.

Numeric Replies:

		ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
		ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
		ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
		ERR_NOSUCHNICK
		RPL_AWAY

Examples:

<pre>:Angel PRIVMSG Wiz :Hello are you receiving this message ?
                                ; Message from Angel to Wiz.

PRIVMSG Angel :yes I'm receiving it !receiving it !'u>(768u+1n) .br ;
                                Message to Angel.

PRIVMSG jto@tolsun.oulu.fi :Hello !
                                ; Message to a client on server
                                tolsun.oulu.fi with username of "jto".

PRIVMSG $*.fi :Server tolsun.oulu.fi rebooting.
                                ; Message to everyone on a server which
                                has a name matching *.fi.

PRIVMSG #*.edu :NSFNet is undergoing work, expect interruptions
                                ; Message to all users who come from a
                                host which has a name matching *.edu.</pre>


## Stages for Server

The server is created using the following steps:

	1. Socket Createtion(using socket());
	2. Setsockopt(set socket option, using setsockopt());
	3. Bind(using bind());
	4. Listen(using listen());
	5. Accept(using accept());

## Some Functions

### 1. socket()

🔧 Purpose:
The socket() function is used in network programming to create a communication endpoint, known as a socket.

🧱 Syntax:
```c
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
```c
int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
```
This creates a TCP socket using IPv4 and stores the socket file descriptor in serverSocket.

### 2. setsockopt()

setsockopt() – Optional, but recommended.

🔧 Purpose:
Set options on the socket — for example, allow port reuse so you can restart the server
without "address already in use" errors.

🧱 Syntax:
```c
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
```c
int opt = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```
This allows the socket to reuse the port even if it's in TIME_WAIT state after being closed.


### 3. bind()

🔧 Purpose:
Bind your socket to a specific IP address and port so it can receive incoming connections.

🧱 Syntax:
```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
🧩 Parameter Breakdown:

	Parameter |  Description
	sockfd    |  The socket descriptor created by socket()
	addr      |  Pointer to a struct specifying IP address and port
	addrlen   |  Size of that address structure

✅ Example:
```c
struct sockaddr_in address;
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(8080);  // Convert port to network byte order

bind(server_fd, (struct sockaddr *)&address, sizeof(address));
```
This binds the server to all available IPs on port 8080.

```c
struct sockaddr_in address;
// Set up the address structure
address.sin_family = AF_INET;
address.sin_port = htons(PORT);

// Convert and assign IP address "192.168.3.21"
if (inet_pton(AF_INET, "192.168.3.21", &address.sin_addr) <= 0) {
	perror("Invalid IP address");
	exit(EXIT_FAILURE);
}

// Bind to IP and port
if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
	perror("bind failed");
	exit(EXIT_FAILURE);
}
```
This binds a socket to the specific IP address 192.168.3.21 on port 8080.

#### 3.1 struct sockaddr_in
```struct sockaddr_in``` is a specialized structure used to define the IPv4 address and port when creating or connecting a socket.

It's used with functions like:

	- bind()
	- connect()
	- accept()
	- sendto(), recvfrom()

✅ Defined in <netinet/in.h>:
Here’s how it’s typically defined (simplified):
```c
struct sockaddr_in {
    sa_family_t    sin_family;   // Address family: AF_INET
    in_port_t      sin_port;     // Port number (network byte order)
    struct in_addr sin_addr;     // IP address
    char           sin_zero[8];  // Padding, not used
};
```
🔍 Field-by-field breakdown:

	Field      | Type            | Purpose
	sin_family | sa_family_t     | Always set to AF_INET for IPv4
	sin_port   | in_port_t       | Port number in network byte order (use htons())
	sin_addr   | struct in_addr  | IP address (use inet_pton() or inet_addr())
	sin_zero   | 8 bytes         | Padding to match sockaddr size — ignore it

✅ Example of setting up sockaddr_in:
```c
struct sockaddr_in address;

address.sin_family = AF_INET;                            // IPv4
address.sin_port = htons(8080);                          // Convert to network byte order
inet_pton(AF_INET, "192.168.1.100", &address.sin_addr);  // Convert string IP to binary
```
htons() = Host TO Network Short (for port)
inet_pton() = presentation-to-network (for IP)

#### 3.2 Comparison: sockaddr_in vs. sockaddr_in6
```sockaddr_in6``` is used for IPv6.

	Field          | sockaddr_in (IPv4)         | sockaddr_in6 (IPv6)
	Address family | AF_INET                    | AF_INET6
	Port           | in_port_t sin_port         | in_port_t sin6_port
	IP Address     | struct in_addr sin_addr    | struct in6_addr sin6_addr (128 bits)
	                 (32 bits)
	Scope ID       | (Not applicable)           | uint32_t sin6_scope_id (for link-local)
	Flow info      | (Not applicable)           | uint32_t sin6_flowinfo (optional QoS)
	Padding        | char sin_zero[8] (unused)  | (No padding)

✅ 2. IPv6 Example – sockaddr_in6
```c
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

struct sockaddr_in6 ipv6_addr;
memset(&ipv6_addr, 0, sizeof(ipv6_addr));

ipv6_addr.sin6_family = AF_INET6;
ipv6_addr.sin6_port = htons(8080);  // Same function as IPv4
inet_pton(AF_INET6, "fe80::1", &ipv6_addr.sin6_addr);
```
```fe80::1``` is a link-local IPv6 address.

```sin6_scope_id``` can be set for link-local addresses if needed (e.g., eth0 interface).

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
### 6. poll()

 Poll() is used for monitoring multiple file descriptors to see if I/O is possible on any
 of them. It’s especially useful in network programming and is commonly used in building
 scalable servers (like your IRC server project) where you want to handle many clients
 without blocking.

🔧 Header
```cpp
#include <poll.h>
```

📚 Function Signature
```cpp
int poll(struct pollfd fds[], nfds_t nfds, int timeout);
```

🧩 Parameters

	Parameter | Description
	fds       | Array of pollfd structures (each one represents a file descriptor you want
				to monitor).
	nfds      | Number of entries in the fds array.
	timeout   | Time in milliseconds to wait: - 0 = return immediately - -1 = wait
				indefinitely - >0 = wait for that many ms

🧱 pollfd Structure
```cpp
struct pollfd {
    int fd;         // File descriptor to monitor
    short events;   // Events to watch for
    short revents;  // Events that occurred (filled by poll)
};
```
Common events/revents flags:

	Flag     | Meaning

	POLLIN   | There is data to read

	POLLOUT  | Writing is possible

	POLLERR  | Error condition

	POLLHUP  | Hang up (disconnection)

	POLLNVAL | Invalid request (e.g., bad fd)

🔁 Return Value

	> 0: Number of fds with events

	0: Timeout

	-1: Error (check errno)

### 7. epoll()
`epoll` is Linux’s high-performance I/O multiplexing API designed to efficiently monitor
large numbers of file descriptors (typically sockets). It solves the O(N) scanning problem
of `select/poll` by letting the kernel maintain an “interest list” and only reporting FDs
that actually have events.

#### 7.1 Core Concepts
 1) Epoll Instance
 You create it with
 ```cpp
 int epfd = epoll_create1(0);
 ```
This returns a file descriptor (`epfd`) representing your interest list.

 2) Registering FDs
 Use `epoll_ctl()` to add, modify, or delete sockets you want to watch:
 ```cpp
 struct epoll_event ev;
 ev.events = EPOLLIN;      // e.g. want read-ready events
 ev.data.fd = listen_fd;
 epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);
 ```
 3) Waiting for Events
Call epoll_wait() to block until one or more FDs are ready:
```cpp
const int MAX_EVENTS = 1024;
std::vector<struct epoll_event> events(MAX_EVENTS);

int n = epoll_wait(epfd, events.data(), events.size(), timeout_ms);
for (int i = 0; i < n; ++i) {
    int fd = events[i].data.fd;
    uint32_t evs = events[i].events;
    // handle evs & EPOLLIN, EPOLLOUT, EPOLLERR, etc.
}
```
#### 7.2 Triggering Modes
 * Level-Triggered (LT) (default)
	As long as data remains to be read or the socket stays writable, epoll_wait will keep returning that FD. Simpler to program but can cause extra wakeups.

 * Edge-Triggered (ET) (set via EPOLLET)<br>
 	Only notifies you once when the state changes (e.g. from no data → data available). You must set the socket to non-blocking and then loop on read()/write() until they return EAGAIN, or you’ll miss further data.

#### 7.3 Common Event Flags

* `EPOLLIN` – data to read (or new connection on listen socket)
* `EPOLLOUT` – ready to write
* `EPOLLERR` – error condition (e.g. reset)
* `EPOLLHUP` – hangup (peer closed)
* `EPOLLRDHUP` – peer performed orderly shutdown (FIN)
* `EPOLLET` – enable edge-triggered mode
* `EPOLLONESHOT` – auto-disable after event, must rearm explicitly

#### 7.4 Why Use epoll?
 * Scalability: O(1) or O(active_fds) behavior → ideal for thousands of concurrent
 connections.
 * Low Overhead: Kernel only wakes your thread when there’s actual work, avoiding wasted
 scans or syscalls.

 * Flexibility: Choose LT or ET, combine flags for one-shot usage, and watch for
 half-closes via `EPOLLRDHUP`.

### 8. fcntl()
`fcntl()` (file control) is a system call in Unix/Linux that changes the behavior of an
already opened file descriptor.

A file descriptor can represent:

 * a regular file
 * a socket (for networking)
 * a pipe
 * a device

You can use `fcntl()` to:

 * Get or set the file descriptor flags (e.g., set non-blocking)
 * Get or set the file status flags (e.g., O_NONBLOCK, O_APPEND)
 * Lock a file
 * Duplicate a file descriptor
 * etc.

Its general syntax:
```cpp
int fcntl(int fd, int cmd, ... /* arg */ );
```
 * `fd` is the file descriptor you want to operate on.
 * `cmd` is what you want to do (like F_GETFL, F_SETFL, etc.)
 * `arg` is optional, depending on the command.

#### 8.1 What is Non-blocking I/O?
Normally, I/O is blocking.
For example:
 * When you `read()` from a socket, if there is no data available, the program will block (pause) and wait until data arrives.
 * When you `write()` to a socket, if the buffer is full, your program will block until it can send.

Non-blocking I/O changes this behavior:
 * When you `read()`, if there is no data available, it immediately returns -1 and sets errno = `EAGAIN`.
 * When you `write()`, if the buffer is full, it immediately returns -1 and sets errno = `EAGAIN`.
Your program doesn't get stuck anymore — it can do something else instead of waiting.
👉 In event-driven servers (epoll, kqueue, etc.), non-blocking is critical because you don't want your server to "freeze" when talking to slow clients.

#### 8.2 How to use fcntl() to set non-blocking mode
Step 1: Get the current flags
```cpp
int flags = fcntl(fd, F_GETFL, 0);
if (flags == -1) {
    perror("fcntl get");
    exit(EXIT_FAILURE);
}

```
Step 2: Add `O_NONBLOCK`
```cpp
flags |= O_NONBLOCK;
if (fcntl(fd, F_SETFL, flags) == -1) {
    perror("fcntl set");
    exit(EXIT_FAILURE);
}

```
After this, all `read()`, `write()` on this fd will become non-blocking.

## Testing

### Testing SIGTERM
1. Get the program PID :
 ```bash
 ps aux | grep irc_server
 ```
2. Kill it:
```bash
 kill <PID>
```


## References

https://www.geeksforgeeks.org/socket-programming-cc/

https://datatracker.ietf.org/doc/html/rfc2812#

http://chi.cs.uchicago.edu/chirc/irc.html
