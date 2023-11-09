#include <iostream>
#include <chrono> //Delay

#define ASIO_STANDALONE

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

// Buffer to store respond from server.
std::vector<char> vBuffer(20 * 1024);

// Reading data from the socket and adding to buffer(Asynchronous)
// Basically this is recursive function will always call itself untill the data will end
void GrabSomeData(asio::ip::tcp::socket &socket)
{
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),

						   // Lambds checking errors and how much we read with  async function
						   [&](std::error_code ec, std::size_t length)
						   {
							   if (!ec)
							   {
								   std::cout << "\n\nRead " << length << " bytes\n\n";

								   for (int i = 0; i < length; i++)
									   std::cout << vBuffer[i];

								   GrabSomeData(socket);
							   }
						   });
}
int main()
{
	// Error Handling variable
	asio::error_code ec;

	// Create a "context" - essentially the platform specific interface
	asio::io_context context;

	/*
	   Give some fake tasks to asio so the context doesn't finish
	   because if asio context doesn't have anything to do, it exits
	   immediately. Also we allow it to run in its own thread "Start
	   Context"
	*/
	asio::io_context::work idleWork(context);

	// Start the context
	std::thread thrContext = std::thread([&]()
										 { context.run(); });

	// Get the address of somewhere we wish to connect to
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);

	// Create a socket, the context will deliver the implmentation
	asio::ip::tcp::socket socket(context);

	// Tell socket to try and connect
	socket.connect(endpoint, ec);

	if (!ec)
	{
		std::cout << "Connected!" << std::endl;
	}
	else
	{
		std::cout << "Failed to connect to address:\n"
				  << ec.message() << std::endl;
	}

	if (socket.is_open())
	{
		GrabSomeData(socket);

		std::string sRequest =
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n";
		// Sending request
		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);
	}
	return 0;
}
