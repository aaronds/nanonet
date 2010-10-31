/**
 * NanoEthernetInterface.
 *
 * An ethernet interface for micronet.
 */

#ifndef __NANO_ETHERNET_INTERFACE
#define __NANO_ETHERNET_INTERFACE

#include <Ethernet.h>
#include <NanoNet.h>

class NanoEthernetInterface : public NanoInterface {
	public:
		Client *cli;
		NanoEthernetInterface();
		NanoEthernetInterface(Client *c,uint8_t *addr,uint8_t *msk,uint8_t *identy);
		void init(Client *c,uint8_t *addr,uint8_t *msk,uint8_t *identity);
		uint8_t available();
		uint8_t *recv(uint8_t size);
		uint8_t send(NanoSocket *to,uint8_t *data,uint8_t size);
		uint8_t curAddr[NANO_ADDR_SIZE];
		uint8_t *identity;
		
		bool connect(void);
};

#endif
