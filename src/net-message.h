#pragma once
#include "net-common.h"

// This is template for message
namespace olc
{
	namespace net
	{
		/*
		 * Message Header is sent at start of all messages.
		 * The tempalte allows us to use "enum class" to ensure
		 * that the message are valid at compile time.
		 */
		template <typename T>
		struct message_header
		{
			T id{};
			uint32_t size = 0;
		};

		template <typename T>
		struct message
		{
			message_header<T> header{};
			std::vector<uint8_t> body;

			// returns size of entire messsage packet in bytes
			size_t size() const
			{
				return sizeof(message_header<T>) + body.size();
			}

			// Override for std::cout compatibility - produces friendly description of message
			friend std::ostream &operator<<(std::ostream &os, const message<T> &msg)
			{
				os << "ID" << int(msg.header.id) << "Size:" << msg.header.size;
				return os;
			}

			// Pushes any POD-like data into message buffer
			template <typename DataType>
			friend message<T> &operator<<(message<T> &msg, const DataType &data)
			{
				// Check that the type of the data being pushed is trivially copyable
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

				// Cache current size of vector, as this will be the point we insert the data
				size_t i = msg.body.size();

				// Resize the vector by the size of the data being pushed
				msg.body.resize(msg.body.size() + sizeof(DataType));

				// Physically cope the data into the newly allocated vector space
				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

				// Recalculate the message size
				msg.header.size = msg.size();

				// Return the target message so it can be chained
				return msg;
			}
			template <typename DataType>
			friend message<T> &operator>>(message<T> &msg, const DataType data)
			{
				// Check that the type of the data being pushed is trivially copyable
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

				// Cache current size of vector, as this will be the point we insert the data
				size_t i = msg.body.size() - sizeof(DataType);

				// Physically cope the data into the newly allocated vector space
				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

				// Shrink the vector to remove read bytes,and reset end position.
				msg.body.resize(i);

				// Recalculate the message size
				msg.header.size = msg.size();

				// Return the target message so it can be chained
				return msg;
			}
		};

		// Forward declare the connection
		template <typename T>
		class connection;

		template <typename T>
		struct owned_message
		{
			std::shared_ptr<connection<T>> remote = nullptr;
			message<T> msg;

			// Again,a friendly string maker
			friend std::ostream &operator<<(std::ostream &os, const owned_message<T> &msg)
			{
				os << msg.msg;
				return os;
			}
		};
	}

}
