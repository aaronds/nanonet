#include <NanoNrf24l01Interface.h>
#include <NanoSocketHandler.h>

NanoNrf24l01Interface::NanoNrf24l01Interface() : NanoInterface() {
	
}

NanoNrf24l01Interface::NanoNrf24l01Interface(Nrf24l *m,uint8_t *addr,uint8_t *msk) : NanoInterface(addr,msk){
	init(m,addr,msk);
}

void NanoNrf24l01Interface::init(Nrf24l *m,uint8_t *addr,uint8_t *msk){
	NanoInterface::init(addr,msk);
	mirf = m;
	mirf->init();
	mirf->setRADDR(addr);
	mirf->payload = NANO_PACKET_SIZE;
	mirf->config();
}

uint8_t NanoNrf24l01Interface::available(){

	/*
	 Functionality moved into poll
	 
	if(errorSocket != NULL){
		if(errorSocket->handler){
			errorSocket->handler->error(sock,NanoSocketHandler::SOCK_ERROR_NO_ACK);
		}
		errorSocket = NULL;
	}
	*/
	
	uint8_t status = mirf->getStatus();

	if(status & (1 << TX_DS)){
		if(sock){
			sock->state = NanoSocketHandler::SOCK_ACTION_ACK;  
			sock = NULL;
		}
		mirf->powerUpRx();
	}else if(status & (1 << MAX_RT)){
		if(sock){
			sock->state = NanoSocketHandler::SOCK_ERROR_NO_ACK;
			sock = NULL;
		}
		sock = NULL;
		mirf->powerUpRx();
	}

	if(status & (1 << RX_DR) || !mirf->rxFifoEmpty()){
		return mirf->payload;
	}else{
		return false;
	}
}

uint8_t *NanoNrf24l01Interface::recv(uint8_t size){
	if(size == mirf->payload){
		mirf->getData(inBuffer);
		return inBuffer;
	}
	return NULL;
}

uint8_t NanoNrf24l01Interface::send(NanoSocket *to,uint8_t *data,uint8_t size){
	mirf->setTADDR(to->addr);

	outBuffer[0] = to->rport;
	outBuffer[1] = to->lport;
	outBuffer[2] = size;
	memcpy((outBuffer + 3),addr,NANO_ADDR_SIZE);

	if(size){
		memcpy((outBuffer + 3 + NANO_ADDR_SIZE),data,size);
	}

	/* Wait till any current send has finished. */

	while(mirf->PTX){
		interupt();
	}

	sock = to;
	mirf->send(outBuffer);
	return size;
}

void NanoNrf24l01Interface::interupt(){
	uint8_t status = mirf->getStatus();
	
	if(status & (1 << TX_DS)){
		if(sock){
			sock->state = NanoSocketHandler::SOCK_ACTION_ACK;
			sock = NULL;
		}
		mirf->powerUpRx();
	}else if(status & (1 << MAX_RT)){
		if(sock){
			sock->state = NanoSocketHandler::SOCK_ACTION_ACK;
			sock = NULL;
		}
		mirf->powerUpRx();
	}
}

void NanoNrf24l01Interface::powerUp(){
	mirf->powerUpRx();
}

void NanoNrf24l01Interface::powerDown(){
	mirf->powerDown();
}

bool NanoNrf24l01Interface::isTransmitting(){
	return mirf->PTX > 0;
}
