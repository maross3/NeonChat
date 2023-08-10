#include <iostream>
#include <chrono>
#include <thread>

#include "NeonChatClient.h"
#include "NeonChatServer.h"

int main()
{
	// Define your message callback
	OnNewMessageReceived messageCallback = [](const char* userName, const char* chatMessage)
	{
		std::cout << "[" << userName << "]: " << chatMessage << std::endl;
	};

	// Start the chat server
	std::string line;
	
	while (true)
	{
		std::cout << "Enter command: ";

		std::getline(std::cin, line);

		if (line == "exit")
			break;

		// todo, create a commands class with help text
		if (line == "start") StartNeonChatServer();
		else std::cout << "Unknown command: " << line << std::endl;
	}

	DisconnectFromNeonChat();

	std::this_thread::sleep_for(std::chrono::seconds(2)); 
}
