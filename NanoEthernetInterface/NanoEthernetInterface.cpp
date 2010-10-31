#include "NanoEthernetInterface.h"

NanoEthernetInterface::NanoEthernetInterface(){
	cli = NULL;
}

NanoEthernetInterface::NanoEthernetInterface(Client *c,uint8_t *addr,uint8_t *msk,uint8_t *id) : NanoInterface(addr,msk){
	init(c,addr,msk,id);
}

void NanoEthernetInterface::init(Client *c,uint8_t *addr,uint8_t *msk,uint8_t *id){
	NanoInterface::init(addr,mask);
	cli = c;
	memcpy(curAddr,addr,NANO_ADDR_SIZE);
	identity = id;
}


uint8_t NanoEthernetInterface::available(){
	uint8_t available = cli->available();

	if(available <= NANO_ADDR_SIZE){
		return 0;
	}

	return (available - NANO_ADDR_SIZE);
}

uint8_t *NanoEthernetInterface::recv(uint8_t size){
	uint8_t i;

	for(i = 0;i < NANO_ADDR_SIZE;i++){
		curAddr[i] = cli->read();
	}

	for(i = 0;i < size;i++){
		inBuffer[i] = cli->read();
	}

	return inBuffer;
}

uint8_t NanoEthernetInterface::send(NanoSocket *to,uint8_t *data,uint8_t size){
	uint8_t i;

	outBuffer[0] = to->rport;
	outBuffer[1] = to->lport;
	outBuffer[2] = size;
	memcpy((outBuffer + 3),addr,NANO_ADDR_SIZE);

	if(size){
		memcpy((outBuffer + 3 + NANO_ADDR_SIZE),data,size);
	}
	
	cli->write(to->addr,NANO_ADDR_SIZE);

	cli->write(outBuffer,NANO_PACKET_SIZE);

	sock = to;

	return size;
}

bool NanoEthernetInterface::connect(){
	uint8_t i;
	if(cli->connect()){
		cli->write(identity,NANO_ADDR_SIZE);
		cli->write(addr,NANO_ADDR_SIZE);

		return true;
	}

	cli->stop();

	return false;
}
