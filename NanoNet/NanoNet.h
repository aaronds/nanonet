/**
 * NanoNet
 *
 * A small library to provide network stack like functionality for arduinos.
 * Author: Aaron Shrimpton <aaronds@gmail.com>
 * Date: 16-1-2010
 *
 */

#ifndef _NANO_NET
#define _NANO_NET

/**
 * Packet format:
 *
 * +-------------------------------------------------------------+
 * | To Port | From Port | Data Length | From Address | Data ... |
 * +-------------------------------------------------------------+
 *
 * To Port, From Port and Data length are all one byte long.
 *
 * The length of From Address and Data are tuned with the defines below.
 * NB: The size of the packet is fixed.
 */

/**
 * The maximum payload size.
 *
 * Note this must not be larger than the smallest maximum packet size of all
 * the interfaces.
 *
 * E.g: The Nrf24l01 has a maximum payload of 32 bytes
 * .:. NANO_DATA_SIZE + NANO_ADDR_SIZE + 3 <= 32
 * Assuming 5 byte address implies NANO_DATA_SIZE <= 24. 
 */

#ifndef NANO_DATA_SIZE
#define NANO_DATA_SIZE 16
#endif

/**
 * The number of address bytes. Again this must not be larger than the smallest maximum address size.
 *
 * E.g: The Nrf24l01 has a maximum address size of 5 bytes.
 */

#ifndef NANO_ADDR_SIZE
#define NANO_ADDR_SIZE 5
#endif

/**
 * The total packet size including address, source and destination ports.
 * Specific interfaces may pre/post fix additional bytes.
 *
 * E.g: The ethernet interface prefixes NANO_ADDR_SIZE bytes of the to address.
 */ 

#ifndef NANO_PACKET_SIZE
#define NANO_PACKET_SIZE NANO_DATA_SIZE + NANO_ADDR_SIZE + 3 
#endif


extern "C" {
	#include <inttypes.h>
	#include <stdlib.h>
	#include <string.h>
}

/**
 * NanoInterface - The phisical interfaces Nrf24l01/Ethernet/...
 * See NanoInterface.h
 */

class NanoInterface;

/**
 * NanoNet - The network class.
 */

class NanoNet;

/**
 * NanoSocket - The endpoints in the network.
 * See NanoSocket.h
 */

class NanoSocket;

/**
 * NanoSocketHandler - A class handeling socket activity.
 * See NanoSocketHandler.h
 */

class NanoSocketHandler;

#include <NanoInterface.h>
#include <NanoSocket.h>
#include <NanoSocketHandler.h>

/**
 * NanoNet.
 *
 * The network class.
 */

class NanoNet {
	public:
		/**
		 * Interfaces are stored in a list.
		 * This is the first one.
		 */
		NanoInterface *primaryIf;

		/**
		 * Connecte and listening sockets are stored in a list.
		 * The first one.
		 */

		NanoSocket *sock;

		/**
		 * Default constructor
		 */

		NanoNet();
		
		/**
		 * Construct then call init.
		 */

		NanoNet(NanoInterface *pri);

		/**
		 * Initalize network with primary interface.
		 */

		void init(NanoInterface *pri);

		/**
		 * Add an interface to the list of interfaces.
		 */
		
		void addInterface(NanoInterface *intf);

		/**
		 * Open a outbound socket.
		 */
		void connect(NanoSocket *socket);

		/**
		 * Disconnect an outbound socket.
		 */

		void disconnect(NanoSocket *socket);

		/**
		 * Open a listening socket.
		 */

		void bind(NanoSocket *socket);

		/**
		 * Resolve the interface to use with a socket.
		 * Uses sock->addr, intf->addr and intf->mask in a similar way
		 * to ip.
		 */

		NanoInterface *resolveInterface(uint8_t *addr);

		/**
		 * Allocate a local port for a outbound connection.
		 */

		uint8_t allocatePort();

		/**
		 * Poll all interfaces for activity.
		 * This should be called frequently to ensure a timely response.
		 */
		
		void poll();

		/**
		 * Handle packet.
		 *
		 * To aid non-polling design, proceses this packet here.
		 */

		void handlePacket(NanoInterface *intf,uint8_t *packet);

		/**
		 * Call interupt() on all interfaces.
		 *
		 * Can be used to return half-duplex interfaces (Nrf24l01) to 
		 * listening mode. Use this function in cases where calling 
		 * poll() is likely to either take to long or cause a stack
		 * heep colission.
		 *
		 * In most cases the socket responsible is accessable so the 
		 * same can be achived with sock->intf->interupt().
		 */

		void tickle();

		/**
		 * isWaiting
		 *
		 * Scann though sockets to see if any is waiting for data.
		 * Usefull if you want to turn off the an interface.
		 */

		bool isWaiting();

		/**
		 * processSocketState.
		 *
		 * Dispatch ack / error events.
		 */

		void processSocketState(NanoSocket *sock);

		/**
		 * processStateChanges.
		 *
		 * Action any state changes in the socket list.
		 */

		void processStateChanges();

		/**
		 * processTimeout.
		 *
		 * Dispatch any timeout errors due.
		 */

		void processTimeout();

		/**
		 * The inbound data buffer, shared between all interfaces.
		 */
		uint8_t inBuffer[NANO_PACKET_SIZE];

		/**
		 * The outbound data buffer, shared between all interfaces.
		 */

		uint8_t outBuffer[NANO_PACKET_SIZE];

		/**
		 * Default timeout value.
		 */

		uint16_t timeout;

	private:

		/**
		 * Where to start trying to allocate ports for outbound sockets
		 * from.
		 */

		uint8_t nextPort;
};

#endif
