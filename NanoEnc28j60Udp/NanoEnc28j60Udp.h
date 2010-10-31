/**
 * A UDP NanoNet interface for the Enc28j60 Ethernet controller.
 */

#ifndef __NANO_ENC28J60_UDP
#define __NANO_ENC28J60_UDP

#include <NanoNet.h>
#include <EtherShield.h>
#include <WProgram.h>

#define ENC28_MAC_LENGTH 6
#define ENC28_BUFFER_SIZE 500

class NanoEnc28j60Udp : public NanoInterface {

	public:
		NanoEnc28j60Udp();
		NanoEnc28j60Udp(EtherShield *e,uint8_t *macAddr,uint8_t *ipAddr,uint16_t lport,uint8_t *gatewayIp,uint8_t *serverIpAddr,uint16_t rport,uint8_t *identity,uint8_t *nanoAddr,uint8_t *nanoMaskAddr);
		void init(EtherShield *e,uint8_t *macAddr,uint8_t *ipAddr,uint16_t lport,uint8_t *gatewayIpAddr,uint8_t *serverIpAddr,uint16_t rport,uint8_t *id,uint8_t *nanoAddr,uint8_t *nanoMaskAddr);
		bool connect();
		void disconnect();
		uint8_t getState();
		uint8_t available();
		uint8_t *recv(uint8_t size);
		uint8_t send(NanoSocket *to,uint8_t *data,uint8_t size);
		
		uint8_t *recvUdpData(uint8_t *size);
		uint8_t recvUdpDataLength();
		void sendUdpData(uint8_t *data,uint8_t size);
		void writeData(uint8_t offset,uint8_t *data,uint8_t size);
		void sendUdpPacket(uint8_t size);

		void arpResolveAddress(Enc28j60UdpSocket *sock);

		static const int NO_NET = 0;
		static const int IDLE = 1;
		static const int ARP_SENT = 2;
		static const int CONNECTED = 4;

	private:
		void handleArp();
		EtherShield *es;
		uint8_t *mac;
		uint8_t *ip;
		uint16_t localPort;
		uint8_t serverMac[ENC28_MAC_LENGTH];
		uint8_t *gateway;
		uint8_t *server;
		uint16_t remotePort;
		uint8_t *identity;
		uint8_t state;
		uint8_t netBuffer[ENC28_BUFFER_SIZE];
		uint8_t availableSize;
		uint16_t ip_identifier;
};

#endif
