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

#ifndef SOCKET_HXX
#define SOCKET_HXX

/// This is a TCP socket!


// POSIX / Linux headers
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
// C headers
#include <cstdint> // uint32_t,uint16_t
// C++ headers
#include <string>


class Socket{
public:

	Socket() {}
	Socket(const uint32_t addr, const uint16_t port);
	Socket(const uint32_t addr, const uint16_t port, const int timeout);
	Socket(const std::string &addr, const uint16_t port);
	Socket(const std::string &addr, const uint16_t port, const int timeout);
	~Socket() { }

	void connect(const std::string &addr, const uint16_t port);
	void connect(const std::string &addr, const uint16_t port, const int timeout);
	void connect(const uint32_t addr, const uint16_t port);
	void connect(const uint32_t addr, const uint16_t port, const int timeout);

	ssize_t  send(const void *data, const size_t len);
	ssize_t  receive( void *data, const size_t len);

	void close() { ::close(_sock); _connected = false; }
	bool isConnected() const { return _connected; }

	uint32_t getAddr() const { return _addr; }
	uint32_t getPort() const { return _port; }

	static uint32_t stoaddr(const std::string &saddr);

	template <class T>
	Socket& operator<<(const T &lhs);
	Socket& operator<<(const std::string &lhs);

	template <class T>
	Socket& operator>>(T &lhs);

private:
	static uint32_t nametoaddr(const std::string &saddr);

	uint32_t _addr = 0;
	uint16_t _port = 0;
	bool _connected = false;
	int _sock = 0;

	friend class ServSocket;
};

template<class T>
Socket& Socket::operator<<(const T &lhs){
	write(lhs, sizeof(T));
	return *this;
}
template <class T>
Socket& Socket::operator>>(T &lhs) {
	read(&lhs, sizeof(T));
	return *this;
}
#endif //SOCKET_HXX
