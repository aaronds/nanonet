/**
 * NanoSocket.
 *
 * A even smaller socket.
 */

#ifndef _NANO_SOCKET
#define _NANO_SOCKET

#include <NanoNet.h>

/**
 * NanoSocket.
 *
 * The end points of NanoNet.
 */

class NanoSocket {
	public:
		/**
		 * The local port.
		 */
		uint8_t lport;

		/**
		 * The remote port.
		 */

		uint8_t rport;

		/**
		 * The remote address of this socket.
		 * Will be NANO_ADDR_SIZE long.
		 *
		 * For listening sockets on 'normal' interfaces this will be set
		 * to net->inBuffer + 3
		 *
		 * DO NOT RELY ON THIS
		 */

		uint8_t *addr;

		/**
		 * The recived data for this socket.
		 * Will be this->dataLength long.
		 *
		 * For 'normal' interfaces this will be set to
		 * net->inBuffer + 3 + NANO_ADDR_SIZE.
		 *
		 * DO NOT RELY ON THIS.
		 *
		 * Aplications are discouraged from accessing this buffer directly,
		 * use this->recv(size).
		 *
		 */

		uint8_t *data;

		/**
		 * The length of data available.
		 *
		 * For 'normal' interfaces this will be equal to
		 * net->inBuffer[2]
		 *
		 * Applications are discouraged from accessing this variable directly,
		 * use dataLength = this->available() instead.
		 */

		uint8_t dataLength;

		/**
		 * The interface for this socket.
		 */

		NanoInterface *intf;

		/**
		 * The handler for this socket.
		 */

		NanoSocketHandler *handler;

		/**
		 * The next socket in the list of sockets.
		 */

		NanoSocket *next;
		
		/**
		 * State used to trigger error and ack.
		 */	
		
		uint8_t state;

		/**
		 * For outbound sockets sending data is presumed to require a response
		 * this is the number of times poll() has been called since data was 
		 * send, if it overflows before a response is recived an error is 
		 * triggered.
		 */
		uint16_t timeout;

		/**
		 * Send data out of this socket.
		 */

		virtual void send(uint8_t *data,uint8_t len);

		/**
		 * Receve data from this socket.
		 *
		 * Calling recive(size) will normaly return data and advance the
		 * data pointer by size bytes and reduce dataLength by size.
		 */

		virtual uint8_t* recv(uint8_t size);

		/**
		 * Return the number of bytes remaing to be recived.
		 */

		virtual uint8_t available();

		/**
		 * Set data pointer to NULL and dataLength = 0.
		 */

		virtual void close();

		/**
		 * Set data pointer to NULL and dataLength = 0.
		 * Use if a outbound socket is to be reassigned to a different address
		 * or port.
		 */

		virtual void flush();

		/**
		 * Initalize socket.
		 */

		void init(uint8_t lport,uint8_t rport,uint8_t *addr,NanoSocketHandler *h);

		/**
		 * Construct a socket and call init.
		 *
		 * Note for a listening socket set rport = 0 and addr = NULL.
		 */

		NanoSocket(uint8_t lport,uint8_t rport,uint8_t *addr,NanoSocketHandler *h);
		/**
		 * Construct an outbound socket.
		 */
		NanoSocket(uint8_t rport,uint8_t *addr,NanoSocketHandler *h);

		/**
		 * Default constructor.
		 */

		NanoSocket();

};

#endif
