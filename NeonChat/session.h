#pragma once
#include <iostream>
#include <memory>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

class session : public std::enable_shared_from_this<session>
{
private:
	boost::asio::ip::tcp::socket socket;
	boost::asio::streambuf buffer;

public:
	session(boost::asio::ip::tcp::socket socket) : socket(std::move(socket))
	{
	}

	bool isComplete() const
	{
		return !socket.is_open();  // session is complete if the socket is not open
	}

	void start()
	{
		auto self(shared_from_this());
		boost::asio::async_read_until(socket, buffer, "\n",
		                              [this, self](boost::system::error_code ec, std::size_t length)
		                              {
			                              if (!ec)
			                              {
				                              std::string message(buffers_begin(buffer.data()),
				                                                  buffers_begin(buffer.data()) + length);
				                              std::cout << "Received: " << message;
				                              buffer.consume(length); // Remove the processed data from the buffer
				                              start(); // Wait for the next message
			                              }
		                              });
	}
};
