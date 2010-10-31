#include <NanoNet.h>
#include <HardwareSerial.h>
/**
 * Initalize class variables.
 */

NanoNet::NanoNet(){
	primaryIf = NULL;
	sock = NULL;
	nextPort = 1;
}

/**
 * Call init with NanoInterface.
 */

NanoNet::NanoNet(NanoInterface *pri){
	init(pri);
}

/**
 * Initalize network with primary interface.
 */

void NanoNet::init(NanoInterface *pri){
	primaryIf = pri;
	sock = NULL;
	nextPort = 1;
	pri->net = this;
	pri->inBuffer = inBuffer;
	pri->outBuffer = outBuffer;
	timeout = 65535;
}

/**
 * Allocate a local port, chose the correct interface and add to the list of sockets in
 * this network.
 *
 * Note: It is important all listening sockets are bound before outbound sockets are connected.
 * 
 * Added test to lport the user could specify a lport.
 */

void NanoNet::connect(NanoSocket *socket){

	if(!socket->lport){
		socket->lport = allocatePort();
	}

	/*
	 * Optimisation for single interface.
	 */

	#ifndef NANO_SINGLE_IF
		socket->intf = resolveInterface(socket->addr);
	#else
		socket->intf = primaryIf;
	#endif
	
	if(sock){
		socket->next = sock;
		sock = socket;
	}else{
		sock = socket;
	}

	if(socket->handler){
		socket->handler->open(socket);
	}
}

/**
 * disconnect a socket.
 *
 * Call handler->close and remove the socket from the linked list.
 */

void NanoNet::disconnect(NanoSocket *sck){
	NanoSocket *tmp;

	if(sck->handler){
		sck->handler->close(sck);
	}

	if(sock == sck){
		sock = sck->next;
	}else{
		tmp = sock;

		while(tmp){
			if(tmp->next == sck){
				tmp->next = sck->next;
				return;
			}
			tmp = tmp->next;
		}
	}
}

/**
 * Starting from net->nextPort check all sockets for conflicts
 * if no conflict return net->nextPort and increment net->nextPort.
 * if conflict increment net->nextPort and check for conflicts.
 */

uint8_t NanoNet::allocatePort(){
	NanoSocket *sck;
	bool sockOk = false;

	while(!sockOk){
		sck = sock;
		sockOk = true;
		while(sck){
			if(sck->lport == nextPort){
				nextPort++;
				sockOk = false;
				break;
			}
			sck = sck->next;
		}

		if(sockOk){
			return nextPort++;
		}
	}
	return 0;
}

/**
 * Note: All listening sockets should be bound before any outbound sockets are 
 * connected.
 */

void NanoNet::bind(NanoSocket *socket){
	socket->rport = 0;
	socket->intf = NULL;
	if(sock){
		socket->next = sock;
		sock = socket;
	}else{
		sock = socket;
	}
}

/**
 * for each interface if inf->available() > NANO_PACKET_SIZE
 * compare packet address,rport and lport with connect sockets if a match is found:
 * 	call handler->handle(socket)
 * 	reset timeout.
 * else compare packet rport with listening sockets if a matches if found:
 * 	set listening socket rport and addr from packet.
 * 	call handler->open(socket)
 * 	call handler->handle(socket)
 * 	call handler->close(socket)
 * 	reset listening socket rport and addr to 0,NULL
 */

void NanoNet::poll(){
	NanoSocket *sck;
	uint8_t lport;
	uint8_t rport;
	uint8_t dataLength;
	uint8_t *data;
	uint8_t *raddr;
	bool handled = false;

	NanoInterface *intf = primaryIf;

	uint8_t *packet = NULL;

	/* Process ack / errors */ 

	sck = sock;

	while(sck){
		if(sck->state && sck->timeout && sck->handler){
			processSocketState(sck);
		}
		sck = sck->next;
	}


	while(intf){
		while(intf->available() >= NANO_PACKET_SIZE){

			packet = intf->recv(NANO_PACKET_SIZE);

			handlePacket(intf,packet);
		}

		intf = intf->next;
	}

	processTimeout();
}

/**
 * handlePacket
 */

void NanoNet::handlePacket(NanoInterface *intf,uint8_t *packet){
	uint8_t lport = packet[0];
	uint8_t rport = packet[1];
	uint8_t dataLength = packet[2];
	uint8_t *data = packet + 3 + NANO_ADDR_SIZE;
	NanoSocket *sck = sock;
	bool handled = false;

	uint8_t *raddr = packet + 3;

	/**
	 * Outbound sockets.
	 */

	while(sck){

		if(sck->state && sck->timeout && sck->handler){
			processSocketState(sck);
		}

		if(sck->rport > 0 && sck->addr != NULL && sck->lport == lport && memcmp(sck->addr,raddr,NANO_ADDR_SIZE) == 0 && sck->handler){
			sck->data = data;
			sck->dataLength = dataLength;
			sck->handler->handle(sck);
			sck->timeout = 0;
			handled = true;
			break;
		}
		sck = sck->next;
	}

	/**
	 * Listening sockets.
	 */

	if(!handled){
		sck = sock;

		while(sck){
			if(sck->rport == 0 && sck->lport == lport && sck->addr == NULL && sck->handler){
				if(sck->state && sck->timeout && sck->handler){
					processSocketState(sck);
				}

				sck->rport = rport;
				sck->addr = raddr;
				sck->intf = intf; 
				sck->handler->open(sck);

				sck->data = data;
				sck->dataLength = dataLength;
				sck->handler->handle(sck);

				sck->handler->close(sck);

				sck->rport = 0;
				sck->addr = NULL;
				sck->intf = NULL;
				handled = true;
				intf->interupt();
				break;
			}
			sck = sck->next;
		}
	}
}

/**
 * Find the last interface add the new interface on the end.
 * Assign in/out buffers.
 */

void NanoNet::addInterface(NanoInterface *intf){
	NanoInterface *tmp = primaryIf;

	while(tmp->next){
		tmp = tmp->next;
	}

	tmp->next = intf;

	intf->net = this;

	intf->inBuffer = inBuffer;
	intf->outBuffer = outBuffer;

	intf->next = NULL;
}

/**
 * Match an address to an interface using interface address and netmask.
 */

NanoInterface *NanoNet::resolveInterface(uint8_t *addr){
	NanoInterface *intf = primaryIf;
	uint8_t i;
	bool match;

	while(intf){
		match = true;
		for(i = 0;i < NANO_ADDR_SIZE;i++){
			if((intf->addr[i] & intf->mask[i]) != (intf->mask[i] & addr[i])){
				match = false;
				break;
			}
		}

		if(match){
			return intf;
		}else{
			intf = intf->next;
		}
	}

	return primaryIf;
}

/**
 * Call each interfaces interupt function used for returning half-duplex interfaces
 * to listening mode.
 */

void NanoNet::tickle(){
	NanoInterface *intf = primaryIf;

	while(intf){
		intf->available();
		intf = intf->next;
	}
}

/**
 * Cycle though each socket see if any is waiting for data.
 */

bool NanoNet::isWaiting(){

	NanoSocket *sck = sock;	

	while(sck){
		if(sck->timeout > 0)
			return true;
		else
			sck = sck->next;
	}

	return false;
}

void NanoNet::processSocketState(NanoSocket *sock){
	switch(sock->state){
		//Ack
		case NanoSocketHandler::SOCK_ACTION_ACK:
			sock->handler->ack(sock);
			break;

		//All other states are error.
		default:
			sock->handler->error(sock,sock->state);
			sock->timeout = 0;
			break;
	}
	sock->state = 0;
}

/*
 * In aid of interupts, cycle over sockets and fire off any state changes.
 */

void NanoNet::processStateChanges(){
	NanoSocket *sck = sock;

	while(sck){
		if(sck->state && sck->timeout && sck->handler){
			processSocketState(sck);
		}

		sck = sck->next;
	}
}



void NanoNet::processTimeout(){

	/*
	 * Cycle through sockets decreacing the timeout variable
	 * If any socket reaches zero call the error callback.
	 */
	
	NanoSocket *sck = sock;

	while(sck){

		if(sck->state && sck->timeout && sck->handler){
			processSocketState(sck);
		}

		if(sck->rport > 0 && sck->timeout > 0){
			sck->timeout--;

			if(sck->timeout == 0 && sck->handler){
				sck->handler->error(sck,NanoSocketHandler::SOCK_ERROR_TIMEOUT);
				sck->state = 0;
			}
		}

		sck = sck->next;
	}
}
