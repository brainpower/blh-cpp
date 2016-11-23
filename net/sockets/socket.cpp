/*
 * Copyright (c) 2013-2016 brainpower <brainpower at mailbox dot org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#include "socket.hpp"
#include <netinet/tcp.h>
#include <fcntl.h>
#include <iostream>
using namespace std;

Socket::Socket(const uint32_t addr, const uint16_t port){
	_sock = socket(AF_INET, SOCK_STREAM, 0); // sock_dgram is udp automatically

	connect(addr, port);
}

Socket::Socket(const std::string &addr, const uint16_t port){
	_sock = socket(AF_INET, SOCK_STREAM, 0);

	connect(stoaddr(addr), port);
}

Socket::Socket(const uint32_t addr, const uint16_t port, const int timeout){
	_sock = socket(AF_INET, SOCK_STREAM, 0); // sock_dgram is udp automatically

	connect(addr, port, timeout);
}

Socket::Socket(const std::string &addr, const uint16_t port, const int timeout){
	_sock = socket(AF_INET, SOCK_STREAM, 0);

	connect(stoaddr(addr), port, timeout);
}

void Socket::connect(const uint32_t addr, const uint16_t port, const int timeout){
	if(_connected) return;
	if(_sock < 0) {
		perror("[Socket] Error creating socket");
		return;
	}

	int flags = 0, error = 0, ret = 0;
	fd_set  rset, wset;
	socklen_t   len = sizeof(error);
	struct timeval  ts;

	_addr = addr;
	_port = port;

	sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = _addr;
	serv.sin_port = _port;

	ts.tv_sec = timeout;

	//clear out descriptor sets for select
	//add socket to the descriptor sets
	FD_ZERO(&rset);
	FD_SET(_sock, &rset);
	wset = rset;    //structure assignment ok

	//set socket nonblocking flag
	if( (flags = fcntl(_sock, F_GETFL, 0)) < 0){
		perror("Cannot get socket flags");
		return;
	}

	if(fcntl(_sock, F_SETFL, flags | O_NONBLOCK) < 0){
		perror("Cannot set socket flags");
		return;
	}

	int reuse = 1;
	if(setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <0)
		perror("[Socket] setsockopt reuse");
	setsockopt(_sock,IPPROTO_TCP,TCP_NODELAY,&reuse, sizeof(reuse) );


	//initiate non-blocking connect
	if( (ret = ::connect(_sock, (const sockaddr *)&serv, sizeof(serv)) < 0 ) )
		if (errno != EINPROGRESS){
			goto error;
		}

	if(ret == 0)    //then connect succeeded right away
		goto done;

	//we are waiting for connect to complete now
	if( (ret = select(_sock + 1, &rset, &wset, NULL, (timeout) ? &ts : NULL)) < 0){
		goto error;
	}

	if(ret == 0){   //we had a timeout
		errno = ETIMEDOUT;
		goto error;
	}

	//we had a positivite return so a descriptor is ready
	if (FD_ISSET(_sock, &rset) || FD_ISSET(_sock, &wset)){
		if(getsockopt(_sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			goto error; // socopt not readable
	} else
		goto error; // fd not set

	if(error){  //check if we had a socket error
		errno = error;
		goto error;
	}

done:
	//put socket back in blocking mode
	if(fcntl(_sock, F_SETFL, flags) < 0)
		goto error;

	_connected = true;
	return;

error:
	perror("[Socket] Error connecting");
	return;
}

/// expects addr and port to be in network byte order
void Socket::connect(const uint32_t addr, const uint16_t port){
	if(!_connected){
		if(_sock < 0) {
			perror("[Socket] Error creating socket");
			return;
		}
		_addr = addr;
		_port = port;

		sockaddr_in serv;
		serv.sin_family = AF_INET;
		serv.sin_addr.s_addr = _addr;
		serv.sin_port = _port;

		int reuse = 1;
		if(setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <0)
			perror("[ServSocket] setsockopt reuse");
		setsockopt(_sock,IPPROTO_TCP,TCP_NODELAY,&reuse, sizeof(reuse) );

		if(::connect(_sock, (const sockaddr *)&serv, sizeof(serv)) <0){
			perror("[Socket] Error connecting");
			return;
		}
		_connected = true;
		printf("[Socket] connected to %d on port %d...\n", ntohl(_addr), ntohs(_port));
	}
}

void Socket::connect(const string &addr, const uint16_t port){
	connect(stoaddr(addr), port);
}

void Socket::connect(const string &addr, const uint16_t port, const int timeout){
	connect(stoaddr(addr), port, timeout);
}

/*int Socket::sendto(const void *data, const size_t len, const uint32_t addr, const uint16_t port){
	if(_connected){

		sockaddr_in to;
		to.sin_family = AF_INET;
		to.sin_addr.s_addr = _addr;
		to.sin_port = _port;

		return ::sendto(_sock, data, len, 0, (const sockaddr *)&to, sizeof(to));
	}
	return -1;
}

int Socket::sendto(const void *data, const size_t len, const string &addr, const uint16_t port){
	sendto(data, len, stoaddr(addr), port);
}*/

ssize_t Socket::send(const void *data, const size_t len){
	if(_connected){
		char* it = (char*) data;
		ssize_t l    = len,
						sent = 0;

		while( l > 0 ) {
			sent = ::send(_sock, it, l, 0);
			if( sent < 1 ) return sent;
			it += sent;
			l  -= sent;
		}
		return len;
	}
	return -1;
}

ssize_t Socket::receive(void *data, const size_t len){
	if(_connected){
		char *it = (char*) data;
		ssize_t l = len,
						rcvt = 0;

		while( l > 0 ){
			rcvt = ::recv(_sock, it, l, 0);
			if( rcvt < 1 ) return rcvt;
			it += rcvt;
			l  -= rcvt;
		}
		return len;
	}
	return -1;
}

ssize_t Socket::write(const void *data, const size_t len){
	if(_connected)
		return ::write(_sock, data, len);
}

ssize_t Socket::read(void *data, const size_t len){
	if(_connected)
		return ::read(_sock, data, len);
}


uint32_t Socket::stoaddr(const string &str){
	in_addr addr;
	int n = inet_pton(AF_INET, str.c_str(), &addr); // converts ipv4 addr's from string "x.x.x.x" to uint32_t
	return n == 1 ? addr.s_addr : nametoaddr(str);
}

uint32_t Socket::nametoaddr(const string &str){
	struct hostent *he;
	struct in_addr **addr_list;

	if ( (he = gethostbyname( str.c_str() ) ) == NULL){
		//FIXME: get rid of the deprecated gethostbyname!
		herror("[Socket] ");
		return 0;
	}

	addr_list = (struct in_addr **) he->h_addr_list;

	return addr_list[0]->s_addr;
}

Socket& Socket::operator<< (const std::string &lhs){
	write(lhs.c_str(), lhs.size());
	return *this;
}

template <>
Socket& Socket::operator>> <>(std::string &lhs){
	char cbuff[1024];

	auto len = read(&cbuff, sizeof(cbuff));
	lhs = std::string (cbuff, len);

	while(len == sizeof(cbuff)){
		len = read(&cbuff, sizeof(cbuff));
		lhs.append( std::string(cbuff, len) );
	}
	return *this;
}
