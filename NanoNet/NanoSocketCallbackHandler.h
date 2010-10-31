
#ifndef _NANO_SOCKET_CALLBACK_HANDLER
#define _NANO_SOCKET_CALLBACK_HANDLER

#include <NanoNet.h>
#include <NanoSocketHandler.h>

/**
 * The simplest possible socket handler.
 *
 * handle,open,close and error all deligate to c functions.
 */

class NanoSocketCallbackHandler : public NanoSocketHandler {
	public:
		/**
		 * The function to handle data.
		 */
		void (*callback) (NanoSocket *sock);

		/**
		 * open a new socket.
		 */
		void (*openCallback) (NanoSocket *sock);

		/**
		 * close a socket.
		 */
		void (*closeCallback) (NanoSocket *sock);

		/**
		 * error callback.
		 */
		void (*errorCallback) (NanoSocket *sock,uint8_t error);

		/**
		 * ack callback.
		 */

		void (*ackCallback) (NanoSocket *sock);

		/**
		 * Construct a callback handler with only a handle function.
		 */
		NanoSocketCallbackHandler(void (*cb) (NanoSocket *sock));

		/**
		 * Construct a callback handler with a handle and error function.
		 */

		NanoSocketCallbackHandler(void (*cb) (NanoSocket *sock),void (*er) (NanoSocket *sock,uint8_t error));

		/**
		 * Handle new data.
		 * See NanoSocketHandler.h
		 */
		void handle(NanoSocket *socket);

		/**
		 * Handle errors.
		 *
		 * See NanoSocketHandler.h
		 */
		void error(NanoSocket *socket,uint8_t error);

		/**
		 * Ack.
		 */

		void ack(NanoSocket *sock);

		/**
		 * Open a new socket.
		 *
		 * See NanoSocketHandler.h
		 */
		void open(NanoSocket *socket);

		/**
		 * Socket closing.
		 *
		 * See NanoSocketHandler.h
		 */
		void close(NanoSocket *socket);

		/**
		 * Legacy.
		 */
		bool timeout(NanoSocket *socket);
};

#endif
