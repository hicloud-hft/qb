/*
 * qb - C++ Actor Framework
 * Copyright (C) 2011-2019 isndev (www.qbaf.io). All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *         limitations under the License.
 */

#include <gtest/gtest.h>
#include <qb/main.h>
#include <qb/actor.h>
#include <qb/network/ip.h>
#include <qb/network/tcp.h>
#include <qb/network/udp.h>

constexpr const unsigned short port = 64322;
constexpr const unsigned short port2 = 9090;

TEST(TCP, Blocking) {
    std::thread tlistener([]() {
        qb::network::tcp::Listener listener;
        EXPECT_FALSE(listener.listen(port) != qb::network::SocketStatus::Done);
        EXPECT_TRUE(listener.good());
        qb::network::tcp::Socket sock;
        EXPECT_FALSE(listener.accept(sock) != qb::network::SocketStatus::Done);
        sock.setBlocking(true);
        char buffer[512];
		*buffer = 0;
        std::size_t received;

        EXPECT_FALSE(sock.receive(buffer, 512, received) != qb::network::SocketStatus::Done);
        EXPECT_EQ(std::string(buffer), "Hello Test !");
    });
    std::thread tsender([]() {
        qb::network::tcp::Socket sock;
        EXPECT_FALSE(sock.connect("127.0.0.1", port, 10) != qb::network::SocketStatus::Done);
        EXPECT_TRUE(sock.good());
		std::this_thread::sleep_for(std::chrono::seconds(1));
        const char msg[] = "Hello Test !";
        EXPECT_FALSE(sock.send(msg, sizeof(msg)) != qb::network::SocketStatus::Done);
        sock.disconnect();
    });

    tlistener.join();
    tsender.join();
}

TEST(TCP, NonBlocking) {
	std::thread tlistener([]() {
		qb::network::tcp::Listener listener;
		EXPECT_FALSE(listener.listen(port) != qb::network::SocketStatus::Done);
		EXPECT_TRUE(listener.good());
		qb::network::tcp::Socket sock;
		EXPECT_FALSE(listener.accept(sock) != qb::network::SocketStatus::Done);
		sock.setBlocking(false);
		char buffer[512];
		*buffer = 0;
		std::size_t received;

		EXPECT_FALSE(sock.receive(buffer, 512, received) != qb::network::SocketStatus::NotReady);
		EXPECT_EQ(std::string(buffer), "");
	});
	std::thread tsender([]() {
		qb::network::tcp::Socket sock;
		EXPECT_FALSE(sock.connect("127.0.0.1", port, 10) != qb::network::SocketStatus::Done);
		EXPECT_TRUE(sock.good());
		sock.setBlocking(false);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		const char msg[] = "Hello Test !";
		EXPECT_FALSE(sock.send(msg, sizeof(msg)) != qb::network::SocketStatus::Done);
		sock.disconnect();
	});

	tlistener.join();
	tsender.join();
}

TEST(UDP, Blocking) {
    std::thread tlistener([]() {
        qb::network::udp::Socket listener;
        EXPECT_FALSE(listener.bind(port2) != qb::network::SocketStatus::Done);
        EXPECT_TRUE(listener.good());
        char buffer[512];
		*buffer = 0;
        std::size_t received;

        qb::network::ip from;
        unsigned short from_port;
        EXPECT_FALSE(listener.receive(buffer, 512, received, from, from_port) != qb::network::SocketStatus::Done);
        EXPECT_EQ(std::string(buffer), "Hello Test !");
        listener.unbind();
    });
    std::thread tsender([]() {
        qb::network::udp::Socket sock;
        EXPECT_TRUE(sock.good());
        std::this_thread::sleep_for(std::chrono::seconds(1));
        const char msg[] = "Hello Test !";
        EXPECT_FALSE(sock.send(msg, sizeof(msg), "127.0.0.1", port2) != qb::network::SocketStatus::Done);
        sock.close();
    });

    tlistener.join();
    tsender.join();
}

TEST(UDP, NonBlocking) {
    std::thread tlistener([]() {
        qb::network::udp::Socket listener;
        EXPECT_FALSE(listener.bind(port2) != qb::network::SocketStatus::Done);
        EXPECT_TRUE(listener.good());
        listener.setBlocking(false);
        char buffer[512];
        *buffer = 0;
        std::size_t received;

        qb::network::ip from;
        unsigned short from_port;
        EXPECT_FALSE(listener.receive(buffer, 512, received, from, from_port) != qb::network::SocketStatus::NotReady);
        EXPECT_EQ(std::string(buffer), "");
        listener.unbind();
    });
    std::thread tsender([]() {
        qb::network::udp::Socket sock;
        EXPECT_TRUE(sock.good());
        sock.setBlocking(false);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        const char msg[] = "Hello Test !";
        EXPECT_FALSE(sock.send(msg, sizeof(msg), "127.0.0.1", port2) != qb::network::SocketStatus::Done);
        sock.close();
    });

    tlistener.join();
    tsender.join();
}


TEST(Example, Equals) {
  qb::network::ip ip("192.168.0.123");
  EXPECT_EQ(ip.toString(), "192.168.0.123");
}