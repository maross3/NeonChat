#pragma once
#include <iostream>
#include <ostream>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/ip/tcp.hpp>


typedef void (*OnNewMessageReceived)(const char* userName, const char* chatMessage);

class NeonChatClient
{
	// Client side context and socket
	boost::asio::io_context io_context;

public:
	OnNewMessageReceived onNewMessageReceivedCallback = nullptr;
	boost::asio::ip::tcp::socket socket;

	NeonChatClient() : socket(io_context)
	{
	}

	void RegisterOnNewMessageReceivedCallback(OnNewMessageReceived callback)
	{
		onNewMessageReceivedCallback = callback;
	}
	// Connect to the server
	void ConnectToNeonChatServer(const char* serverAddress, const char* port)
	{
		try
		{
			boost::asio::ip::tcp::resolver resolver(io_context);
			boost::asio::connect(socket, resolver.resolve(serverAddress, port));
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << std::endl;
		}
	}

	// Send a chat message to the server
	void SendChatToServer(const char* userName, const char* chatMessage)
	{
		std::string message(chatMessage);
		boost::system::error_code ec;

		try
		{
			// invoke event if handler is registered
			if (onNewMessageReceivedCallback != nullptr) onNewMessageReceivedCallback(userName, chatMessage);
			else std::cout << "No callback registered for new message received." << std::endl;

			// send the message
			boost::asio::async_write(socket, boost::asio::buffer(message),
			                         [this](boost::system::error_code ec, std::size_t /*length*/)
			                         {
				                         if (!ec)
				                         {
					                         std::cout << "Message sent successfully." << std::endl;
				                         }
				                         else
				                         {
					                         std::cerr << "Error occurred while sending message: " << ec.message() <<
						                         std::endl;
				                         }
			                         });
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << std::endl;
		}
	}
};

std::unique_ptr<NeonChatClient> client;
// make sure you set these to your server address and port number
const char* serverAddress = "127.0.0.1";
const char* port = "8888";


extern "C" {
inline __declspec(dllexport) void ConnectToNeonChat()
{
	client = std::make_unique<NeonChatClient>();
	client->ConnectToNeonChatServer(serverAddress, port);
}

// todo, remove username and make it a member on client
inline __declspec(dllexport) void SendNeonChat(const char* userName, const char* chatMessage)
{
	if (client) client->SendChatToServer(userName, chatMessage);
}

inline __declspec(dllexport) void DisconnectFromNeonChat()
{
	client.reset();
}

inline __declspec(dllexport) void RegisterOnNewMessageReceivedCallback(OnNewMessageReceived callback)
{
	if (client) client->onNewMessageReceivedCallback = callback;
}
}
