#include "NanoEnc28j60Udp.h"
#include <HardwareSerial.h>

NanoEnc28j60Udp::NanoEnc28j60Udp() : NanoInterface(NULL,NULL){
	mac = NULL;
	ip = NULL;
	localPort = 0;
	gateway = NULL;
	server = NULL;
	remotePort = 0;
	identity = NULL;
	availableSize = 0;
	ip_identifier = 0;
}

NanoEnc28j60Udp::NanoEnc28j60Udp(EtherShield *e,uint8_t *macAddr,uint8_t *ipAddr,uint16_t lport,uint8_t *gatewayIp,uint8_t *serverIpAddr,uint16_t rport,uint8_t *identity,uint8_t *nanoAddr,uint8_t *nanoMaskAddr) : NanoInterface(nanoAddr,nanoMaskAddr){
	init(e,macAddr,ipAddr,lport,gatewayIp,serverIpAddr,rport,identity,nanoAddr,nanoMaskAddr);
}

void NanoEnc28j60Udp::init(EtherShield *e,uint8_t *macAddr,uint8_t *ipAddr,uint16_t lport,uint8_t *gatewayIp,uint8_t *serverIpAddr,uint16_t rport,uint8_t *id,uint8_t *nanoAddr,uint8_t *nanoMaskAddr){
	NanoInterface::init(nanoAddr,nanoMaskAddr);
	es = e;
	mac = macAddr;
	ip = ipAddr;
	localPort = lport;
	gateway = gatewayIp;
	server = serverIpAddr;
	remotePort = rport;
	identity = id;
	state = NO_NET;
}

bool NanoEnc28j60Udp::connect(){
	switch(state){
		case NO_NET:
			if(net == NULL){
				return false;
			}else{
				state = IDLE;
			}
		case IDLE:
			if(gateway){
				es->ES_make_arp_request(outBuffer,gateway);
			}else{
				es->ES_make_arp_request(outBuffer,server);
			}
			state = ARP_SENT;
			return true;
			break;
		default:
			break;
	}

	return false;
}

void NanoEnc28j60Udp::disconnect(){
	state = IDLE;
}

uint8_t NanoEnc28j60Udp::getState(){
	return state;
}

uint8_t NanoEnc28j60Udp::available(){

	if(availableSize > 0){
		return availableSize;
	}

	uint8_t plen = es->ES_enc28j60PacketReceive(ENC28_BUFFER_SIZE,netBuffer);
	uint8_t dataLength = 0;

	if(plen){

		switch(state){
			case ARP_SENT:
				handleArp();
				break;
			case CONNECTED:
				if(es->ES_eth_type_is_arp_and_my_ip(netBuffer,plen)){
					es->ES_make_arp_answer_from_request(netBuffer);
					return 0;
				}
				if(es->ES_eth_type_is_ip_and_my_ip(netBuffer,plen)){
					dataLength = recvUdpDataLength();

					if(dataLength == NANO_PACKET_SIZE + NANO_ADDR_SIZE){
						availableSize = dataLength;
						return dataLength;
					}else{
						return 0;
					}
				}
				break;
			default:
				return 0;	
		}
	}
	return 0;
}

void NanoEnc28j60Udp::handleArp(){
	if(es->ES_arp_packet_is_myreply_arp(netBuffer)){
		memcpy(serverMac,netBuffer + (ETH_SRC_MAC),6);
		//writeData(0,addr,NANO_ADDR_SIZE);
		//writeData(NANO_ADDR_SIZE,identity,NANO_ADDR_SIZE);
		//sendUdpPacket(NANO_ADDR_SIZE * 2);
		state = CONNECTED;
	}
}

void NanoEnc28j60Udp::writeData(uint8_t offset,uint8_t *data,uint8_t size){
	memcpy(netBuffer + UDP_DATA_P + offset,data,size);
}

void NanoEnc28j60Udp::sendUdpData(uint8_t *data,uint8_t size){
	writeData(0,data,size);
	sendUdpPacket(size);
}

void NanoEnc28j60Udp::sendUdpPacket(uint8_t size){
	uint16_t ck = 0;

	netBuffer[ IP_P ] = IP_V4_V | IP_HEADER_LENGTH_V;

	// set TOS to default 0x00
	netBuffer[ IP_TOS_P ] = 0x00;

	
	// set packet identification
	netBuffer[ IP_ID_H_P ] = (ip_identifier >>8) & 0xff;
	netBuffer[ IP_ID_L_P ] = ip_identifier & 0xff;
	ip_identifier++;
	
	// set fragment flags	
	netBuffer[ IP_FLAGS_H_P ] = 0x00;
	netBuffer[ IP_FLAGS_L_P ] = 0x00;
	
	// set Time To Live
	netBuffer[ IP_TTL_P ] = 128;

	memcpy(netBuffer + IP_SRC_P,ip,4);
	memcpy(netBuffer + IP_DST_P,server,4);


	netBuffer[IP_PROTO_P] = IP_PROTO_UDP_V;

	netBuffer[IP_TOTLEN_H_P] = 0;
	netBuffer[IP_TOTLEN_L_P] = IP_HEADER_LEN + UDP_HEADER_LEN + size;
	
	es->ES_fill_ip_hdr_checksum(netBuffer);	
	
	es->ES_make_eth_ip_new(netBuffer,serverMac);
	//es->ES_make_ip(netBuffer);
	netBuffer[UDP_SRC_PORT_H_P] = localPort >> 8;
	netBuffer[UDP_SRC_PORT_L_P] = localPort && 0xff;
	netBuffer[UDP_DST_PORT_H_P] = remotePort >> 8;
	netBuffer[UDP_DST_PORT_L_P] = remotePort && 0xff;

	netBuffer[UDP_LEN_H_P] = 0;
	netBuffer[UDP_LEN_L_P] = UDP_HEADER_LEN + size;
	netBuffer[UDP_CHECKSUM_H_P] = 0;
	netBuffer[UDP_CHECKSUM_L_P] = 0;

	ck = es->ES_checksum(&netBuffer[IP_SRC_P],16 + size,1);

	netBuffer[UDP_CHECKSUM_H_P] = ck >> 8;
	netBuffer[UDP_CHECKSUM_L_P] = ck & 0xff;

	es->ES_enc28j60PacketSend(UDP_HEADER_LEN + IP_HEADER_LEN + ETH_HEADER_LEN + size,netBuffer);
}

uint8_t *NanoEnc28j60Udp::recvUdpData(uint8_t *size){
	if(memcmp(netBuffer + UDP_DST_PORT_H_P,&localPort,2) == 0 && memcmp(netBuffer + UDP_SRC_PORT_H_P,&remotePort,2) == 0 && netBuffer[UDP_LEN_H_P] == 0){
		(*size) = netBuffer[UDP_LEN_L_P] - UDP_HEADER_LEN;
		return netBuffer + UDP_DATA_P;
	}else{
		return NULL;
	}
}


uint8_t NanoEnc28j60Udp::recvUdpDataLength(){
	if(memcmp(netBuffer + UDP_DST_PORT_H_P,&localPort,2) == 0 && memcmp(netBuffer + UDP_SRC_PORT_H_P,&remotePort,2) == 0 && netBuffer[UDP_LEN_H_P] == 0){
		return netBuffer[UDP_LEN_L_P] - UDP_HEADER_LEN;
	}else{
		return 0;
	}
}

uint8_t NanoEnc28j60Udp::send(NanoSocket *to,uint8_t *data,uint8_t size){
	writeData(0,to->addr,NANO_ADDR_SIZE);
	uint8_t offset = NANO_ADDR_SIZE;

	writeData(offset++,&to->rport,1);
	writeData(offset++,&to->lport,2);
	writeData(offset++,&size,1);
	writeData(offset,addr,NANO_ADDR_SIZE);
	offset+= NANO_ADDR_SIZE;

	if(size){
		writeData(offset,data,size);
	}

	sendUdpPacket(offset + NANO_DATA_SIZE);
}

uint8_t *NanoEnc28j60Udp::recv(uint8_t size){
	memcpy(netBuffer + UDP_DATA_P + NANO_ADDR_SIZE,inBuffer,size);
	availableSize = 0;
	return inBuffer;
}
