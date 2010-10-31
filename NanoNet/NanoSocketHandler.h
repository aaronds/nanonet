/**
 * NanoSocketHandler.
 *
 * A virtual class for handeling sockets.
 */

#ifndef _NANO_SOCKET_HANDLER
#define _NANO_SOCKET_HANDLER

#include <NanoNet.h>

/**
 * NanoSocketHandler.
 *
 * A class for librarys responding to data arriving at sockets.
 */

class NanoSocketHandler {

	public:
		/**
		 * Constructor
		 *
		 * Adding 'timeout' switch requires constructor :(
		 */

		NanoSocketHandler();

		/**
		 * Legacy.
		 */
		static const int SOCK_AUTO_CLOSE = 1;

		/**
		 * Legacy.
		 */
		static const int SOCK_SERVER = 2;

		/**
		 * If a response is not recived before sock->timeout reaches zero.
		 */
		static const int SOCK_ERROR_TIMEOUT = 1;

		/**
		 * If recipt of a packet is not acknolaged.
		 *
		 * Currently only implmented on Nrf24l01 interface.
		 */

		static const int SOCK_ERROR_NO_ACK = 2;

		/** 
		 * Packet is acknolaged.
		 */

		 static const int SOCK_ACTION_ACK = 3;

		/**
		 * A new socket is opened.
		 *
		 * When a listening socket responds to a packet this method is 
		 * called with the new socket.
		 */

		virtual void open(NanoSocket *socket);

		/**
		 * New data is available for this socket.
		 *
		 * When this method returns NanoNet assumes the socket no longer
		 * needs the data from the packet and the memory can be re-used.
		 */

		virtual void handle(NanoSocket *socket);

		/**
		 * An acknolagement has been recived.
		 *
		 */

		virtual void ack(NanoSocket *socket);

		/**
		 * An error has occurred.
		 * See SOCK_ERROR_TIMEOUT and SOCK_ERROR_TIMEOUT for error types.
		 */

		virtual void error(NanoSocket *socket,uint8_t error);

		/**
		 * After handle has been called for a listening socket before addr,and rport are reset.
		 */
		virtual void close(NanoSocket *socket);

		/**
		 * Legacy.
		 */
		uint8_t mode;

		/**
		 * Timeout.
		 *
		 * Should NanoNet assume that every outbound packet requires a response?
		 */

		bool timeout;
};

#endif
