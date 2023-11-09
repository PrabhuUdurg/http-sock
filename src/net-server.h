#pragma once
#include "net-common.h"

// TODO: Custom C sockets.
namespace olc
{
    namespace net
    {
        template <typename T>
        class server_interface
        {
        public:
            server_interface(uint16_t port)
                // Listening users, IPv4 version, and context
                : m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
            {
            }

            virtual ~server_interface()
            {
                Stop();
            }

            bool Start()
            {
                try
                {
                    // Gice some work for io_context before the thread
                    WaitForClientConnection();

                    // Start Context
                    m_threadContext = std::thread([this]()
                                                  { m_asioContext.run(); });
                }
                catch (std::exception &e)
                {
                    // Something prohibited the server from listening
                    std::cerr << "[SERVER] Exception: " << e.what() << "\n";
                    return false;
                }

                std::cout << "[SERVER] Started!\n";
                return true;
            }

            void Stop()
            {
                // Request the context to close
                m_asioContext.stop();

                // Tidy up context thread
                if (m_threadContext.joinable())
                    m_threadContext.join();

                // Inform someone, anybody, if they care ...
                std::cout << "[SERVER] Stopped!\n";
            }

            // ASYNC - Instruct asio to wait for connection
            void WaitForClientConnection()
            {
                m_asioAcceptor.async_accept(
                    // Receive error code and socket
                    [this](std::error_code ec, asio::ip::tcp::socket socket)
                    {
                        if (!ec)
                        {
                            // If access Successfull
                            std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << "\n";

                            // When something connected create a new object
                            std::shared_ptr<connection<T>> newconn =
                                std::make_shared<connection<T>>(connection<T>::owner::server,
                                                                m_asioContext, std::move(socket), m_qMessageIn);

                            // Give the user servrer a chance to deny connection
                            if (OnClientConnect(newconn))
                            {
                                // Connection allowed, so add to container of new conection
                                m_deqConnections.push_back(std::move(newconn));

                                // Assign ID to connections
                                m_deqConnections.back()->ConnectToClient(nIDCounter++);

                                std::cout << "[" << m_deqConnections.back()->GetID() << "] Connection Approved\n";
                            }
                            else
                            {
                                // If user denied connection
                                std::cout << "[------] Connection Denied\n";
                            }
                        }
                        else
                        {
                            std::cout << "[SERVER] New Connection Error: " << ec.message() << "\n";
                        }

                        // Giving work to asio context
                        WaitForClientConnection();
                    });
            }

            // Send a message to a specific client
            // shared_ptr can keep ownership over couple objects
            void MessageClient(std::shared_ptr<connection<T>> client, const message<T> &msg)
            {
                // Checking if client pointer is correct, and client connected
                if (client && client->IsConnected())
                {
                    client->Send(msg);
                }
                else
                {
                    // Remove a client
                    OnClientDisconnect(client);
                    client.reset();
                    m_deqConnections.erase(
                        std::remove(m_deqConnections.begin(), m_deqConnections.end(), client), m_deqConnections.end());
                }
            }

            // Send message to all client
            void MessageAllClients(const message<T> &msg, std::shared_ptr<connection<T>> pIgnoreClient = nullptr)
            {
                bool bInvalidClientExists = false;

                for (auto &client : m_deqConnections)
                {
                    // Check client connected
                    if (client && client->IsConnected())
                    {
                        // .. it is!
                        if (client != pIgnoreClient)
                            client->Send(msg);
                    }
                    else
                    {
                        OnClientDisconnect(client);
                        client.reset();

                        bInvalidClientExists = true;
                    }
                }

                if (bInvalidClientExists)

                    // Entirely remove client from deque of connections
                    m_deqConnections.erase(
                        std::remove(m_deqConnections.begin(), m_deqConnections.end(), nullptr), m_deqConnections.end());
            }

            // Constrain number of messages
            void Update(size_t nMaxMessages = -1, bool bWait = false)
            {
                if (bWait)
                    m_qMessageIn.wait();

                size_t nMessageCount = 0;
                while (nMessageCount < nMaxMessages && !m_qMessageIn.empty())
                {
                    // Grab the front message
                    auto msg = m_qMessageIn.pop_front();

                    // Pass to message handler
                    OnMessage(msg.remote, msg.msg);

                    nMessageCount++;
                }
            }

        protected:
            // Called when a client connects, you can veto the connection by returning false
            virtual bool OnClientConnect(std::shared_ptr<connection<T>> client)
            {

                return false;
            }

            // Called when a client appears to have disconnected
            virtual void OnClientDisconnect(std::shared_ptr<connection<T>> client)
            {
            }

            // Called when a message arrives. Help server to deal with specific messages.
            virtual void OnMessage(std::shared_ptr<connection<T>> client, message<T> &msg)
            {
            }

        protected:
            // Thread Safe Queue for incoming message packets
            tsqueue<owned_message<T>> m_qMessageIn;

            // Container of active validated connections
            std::deque<std::shared_ptr<connection<T>>> m_deqConnections;

            // Order of declaration is important - it is also the order of initialisation
            asio::io_context m_asioContext;
            std::thread m_threadContext;

            // Sockets of connected clients
            asio::ip::tcp::acceptor m_asioAcceptor;

            // ID which assigns to user
            uint32_t nIDCounter = 10000;
        };
    }
}