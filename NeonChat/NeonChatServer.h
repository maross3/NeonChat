#pragma once
#include <iostream>
#include <string>

#include "Session.h"
#include "boost/asio.hpp"

using boost::asio::ip::tcp;

// We'll use a simple class to manage our chat server
class NeonChatServer
{
private:
	boost::asio::io_service io_service;
	tcp::acceptor acceptor;
	std::unique_ptr<std::thread> service_thread; // A thread to run the io_service
	std::vector<std::shared_ptr<session>> sessions; // Stores active sessions

public:
	NeonChatServer(short port)
		: acceptor(io_service, tcp::endpoint(tcp::v4(), port))
	{
	}


	// Ensure the io_service is stopped and the thread is joined on destruction
	~NeonChatServer()
	{
		stop();
	}

	void start()
	{
		// Start accepting connections
		acceptor.async_accept(
			[this](const boost::system::error_code ec, tcp::socket socket)
			{
				if (!ec)
				{
					const auto newSession = std::make_shared<session>(std::move(socket));
					newSession->start();
					sessions.push_back(newSession);
					std::cout << "Server session started. todo: add client name \n";

					cleanUpSessions(); // clean up any completed sessions
				}
				else
				{
					std::cerr << "Error accepting connection: " << ec.message() << std::endl;
				}
				start(); // Accept the next connection
			});

		if (service_thread) return;
		std::cout << "Server starting...\n";

		service_thread = std::make_unique<std::thread>(
			[this]()
			{
				io_service.run();
			});
		std::cout << "Server started!\n";
	}

	void stop() {
		// Stops the io_service.
		io_service.stop();

		// Waits for the service thread to finish.
		if (service_thread && service_thread->joinable()) {
			service_thread->join();
		}

		// Now that the thread is done, we can get rid of it.
		service_thread = nullptr;
	}


	void cleanUpSessions()
	{
		sessions.erase(std::remove_if(sessions.begin(), sessions.end(),
			[](const std::shared_ptr<session>& sess)
			{
				return sess->isComplete();
			}), sessions.end());
	}
};

// The server will be a global variable, since we only need one
std::unique_ptr<NeonChatServer> server;

extern "C" {
inline __declspec(dllexport) void StartNeonChatServer()
{
	try
	{
		// We create the server here when we're ready to start it
		server = std::make_unique<NeonChatServer>(8888);
		server->start();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}
}
