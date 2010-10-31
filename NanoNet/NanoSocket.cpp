/**
 * NanoSocket.
 * 
 * A very very small socket.
 */

#include <NanoSocket.h>

/**
 * Set address, socket handler, local and remote ports.
 *
 * Note: For 'listening' sockets set address to NULL and remote port to 0. 
 */

NanoSocket::NanoSocket(uint8_t portl,uint8_t portr,uint8_t *saddr,NanoSocketHandler *h){
	init(portl,portr,saddr,h);
}

/**
 * Note: lport will be allocated when it is connected to a network.
 */

NanoSocket::NanoSocket(uint8_t portr,uint8_t *saddr,NanoSocketHandler *h){
	init(0,portr,saddr,h);
}

/**
 * Initalize socket.
 */

void NanoSocket::init(uint8_t portl,uint8_t portr,uint8_t *saddr,NanoSocketHandler *h){
	lport = portl;
	rport = portr;
	addr = saddr;
	intf = NULL;
	handler = h;
	next = NULL;
	timeout = 0;
	state = 0;
}

/**
 * Set everything to null.
 */

NanoSocket::NanoSocket(){
	rport = 0;
	addr = NULL;
	handler = NULL;
	intf = NULL;
	next = NULL;
	timeout = 0;
	state = 0;
}

/**
 * NB: There is currently no support for sending over NANO_DATA_SIZE.
 */

void NanoSocket::send(uint8_t *data,uint8_t len){
	state = 0;
	intf->send(this,data,len);
	if(!handler || handler->timeout){
		timeout = intf->net->timeout;
	}else{
		timeout = 0;
	}
}

/**
 * Applications should check the number of bytes avaiable before getting the data.
 * Warning: Reading more than NANO_DATA_SIZE bytes from one socker (per request) 
 * will result in genuinly bad things happening.
 */

uint8_t *NanoSocket::recv(uint8_t size){
	uint8_t *rv = data;
	data += size;
	dataLength =- size;
	return rv;
}

/**
 * Fetch the amount of data still to be receved from the socket.
 */

uint8_t NanoSocket::available(){
	return dataLength;
}

/**
 * Legacy.
 */

void NanoSocket::close(){
	dataLength = 0;
	data = NULL;
}

/**
 * Force the cleasing of the recvable data.
 */

void NanoSocket::flush(){
	dataLength = 0;
	data = NULL;
}


