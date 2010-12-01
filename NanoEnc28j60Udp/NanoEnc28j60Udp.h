/**
 * A UDP NanoNet interface for the Enc28j60 Ethernet controller.
 *
 * Depends on Enc28j60Udp
 * http://github.com/aaronds/arduino-enc28j60udp
 *
 * This is somewhat more complicated than it needs to be due to supporting both 
 * polling loop and interupt driven operation. TODO: Different drivers for different
 * modes of opperation.
 *
 * In polling mode (poll = true) calls to available() will trigger Enc28j60Udp.poll()
 * if there is a packet waiting for the NanoEnc29j60Udp will call the serviceUdp()
 * method which will copy the data and set availableSize, the original available() can
 * then return availableSize.
 *
 * In interupt mode (poll = false) the interupt should be configured to call 
 * Enc28j60Udp.poll() if data is available this will trigger the NanoNet interfaces 
 * serviceUdp() method which will directly inject the packet into NanoNet.
 */

#ifndef __NANO_ENC28J60_UDP
#define __NANO_ENC28J60_UDP

#include <NanoNet.h>
#include <Enc28j60Udp.h>
#include <WProgram.h>

class NanoEnc28j60Udp : public NanoInterface, public Enc28j60UdpSocket, public Enc28j60UdpSocketHandler {

	public:
		NanoEnc28j60Udp();
		NanoEnc28j60Udp(uint8_t *identity,uint8_t *nanoAddr,uint8_t *nanoMaskAddr);
		void init(uint8_t *id,uint8_t *nanoAddr,uint8_t *nanoMaskAddr);
		uint8_t available();
		uint8_t *recv(uint8_t size);
		uint8_t send(NanoSocket *to,uint8_t *data,uint8_t size);
		void serviceUdp(uint8_t *data,uint16_t size);

		/*
		 * If you are using net.poll() instead of interupts set this to true.
		 */

		bool poll;
		bool connect();

	private:
		uint8_t *identity;
		uint8_t availableSize;
		uint8_t pendingBuffer[NANO_PACKET_SIZE];
};

#endif
