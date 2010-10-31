/**
 * NanoInterface.
 *
 * A very small interface for micro/nano net.
 */

#ifndef _NANO_INTERFACE
#define _NANO_INTERFACE

#include <NanoNet.h>

class NanoInterface {
	public:
		/**
		 * Default constructor.
		 */
		NanoInterface();

		/**
		 * Construct and call init(...).
		 */

		NanoInterface(uint8_t *addr,uint8_t *mask);
		
		/**
		 * Initalize the interface.
		 */

		virtual void init(uint8_t *addr,uint8_t *imask);

		/**
		 * Retun the number of bytes available to be read from this interface.
		 */

		virtual uint8_t available();

		/**
		 * Return a pointer containing size bytes from the interface.
		 * NB: NanoNet will only ever request NANO_PACKET_SIZE bytes.
		 */

		virtual uint8_t *recv(uint8_t size);

		/**
		 * Send size bytes in data from this interface to to.
		 * Size should be <= NANO_DATA_SIZE.
		 */

		virtual uint8_t send(NanoSocket *to,uint8_t *data,uint8_t size);

		/**
		 * Try to return half-duplex interfaces to reciving mode if possible.
		 * NB: I havn't had any luck calling this function from an interupt handler.
		 */

		virtual void interupt();

		/**
		 * The current sending socket.
		 * Normaly used for error reporting.
		 */

		NanoSocket *sock;

		/**
		 * The network this interface is attached to.
		 */
		NanoNet *net;

		/**
		 * The next in the list of interfaces.
		 */
		NanoInterface *next;
		
		/**
		 * The address which this interface responds to.
		 */
		uint8_t *addr;

		/**
		 * The ''netmask'' for this interface.
		 */

		uint8_t *mask;

		/**
		 * Input Buffer
		 *
		 * The net->addInterface method sets this pointer.
		 *
		 * This buffer is often shared with other interfaces
		 * therfore no data should be written to it unless net explicitly 
		 * requests it via this->recv.
		 */

		uint8_t *inBuffer;

		/**
		 * Output buffer
		 *
		 * The net->addInterface method sets this pointer.
		 *
		 * This buffer is often shared with other interfaces.
		 * NanoNet assumes that by the time this->send returns this buffer
		 * is not used any further and could be used by another interface.
		 */

		uint8_t *outBuffer;
};

#endif
