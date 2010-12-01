#include "NanoEnc28j60Udp.h"
#include <HardwareSerial.h>

NanoEnc28j60Udp::NanoEnc28j60Udp() : NanoInterface(NULL,NULL) {
	identity = NULL;
	availableSize = 0;
	poll = false;
	handler = this;
}

NanoEnc28j60Udp::NanoEnc28j60Udp(uint8_t *identity,uint8_t *nanoAddr,uint8_t *nanoMaskAddr) : NanoInterface(nanoAddr,nanoMaskAddr){
	init(identity,nanoAddr,nanoMaskAddr);
}

void NanoEnc28j60Udp::init(uint8_t *id,uint8_t *nanoAddr,uint8_t *nanoMaskAddr){
	NanoInterface::init(nanoAddr,nanoMaskAddr);
	identity = id;
}

/*
 * See comments in NanoEnc28j60Udp.h
 */

uint8_t NanoEnc28j60Udp::available(){
	if(poll){
		if(!availableSize){
			parent->poll();
		}
		return availableSize;
	}else{
		return 0;
	}
}

void NanoEnc28j60Udp::serviceUdp(uint8_t *data,uint16_t size){
	if(size > 255){
		return;
	}

	/* Check to address */
	if(memcmp(data,addr,NANO_ADDR_SIZE) == 0){
		if(poll){
			/* Copy data and set availableSize */
			memcpy(pendingBuffer,data + NANO_ADDR_SIZE,NANO_PACKET_SIZE);
			availableSize = NANO_PACKET_SIZE;
		}else{
			/* Look no copy */
			net->handlePacket(this,data + NANO_ADDR_SIZE);
		}
	}
}

uint8_t *NanoEnc28j60Udp::recv(uint8_t size){
	if(size == availableSize){
		availableSize = 0;
		return pendingBuffer;
	}
	return NULL;
}

uint8_t NanoEnc28j60Udp::send(NanoSocket *to,uint8_t *data,uint8_t size){
	parent->write(this,0,to->addr,NANO_ADDR_SIZE);
	parent->write(this,NANO_ADDR_SIZE,&to->rport,1);
	parent->write(this,NANO_ADDR_SIZE + 1,&to->lport,1);
	parent->write(this,NANO_ADDR_SIZE + 2,&size,1);
	parent->write(this,NANO_ADDR_SIZE + 3,addr,NANO_ADDR_SIZE);
	parent->write(this,(NANO_ADDR_SIZE * 2) + 3,data,size);
	parent->send(this,(NANO_ADDR_SIZE * 2) + 3 + NANO_DATA_SIZE);
}

bool NanoEnc28j60Udp::connect(){
	parent->write(this,0,identity,NANO_ADDR_SIZE);
	parent->write(this,NANO_ADDR_SIZE,addr,NANO_ADDR_SIZE);
	parent->send(this,NANO_ADDR_SIZE * 2);
}

