/**
 * Stubs for Nano Interface.
 */

#include <NanoInterface.h>

/**
 * Set everything to NULL.
 */

NanoInterface::NanoInterface(){
	addr = NULL;
	net = NULL;
	mask = NULL;
	next = NULL;
}

/**
 * Call init.
 */

NanoInterface::NanoInterface(uint8_t *iaddr,uint8_t *imask){
	init(iaddr,imask);
}

/**
 * Set interface address and mask.
 *
 */

void NanoInterface::init(uint8_t *iaddr,uint8_t *imask){
	addr = iaddr;
	mask = imask;
	net = NULL;
	next = NULL;
}

/**
 * Number of bytes waiting.
 *
 * Stub for the benifit of gcc, return zero.
 */

uint8_t NanoInterface::available(){
	return 0;
}

/**
 * Get data.
 *
 * Stub, return NULL
 */

uint8_t *NanoInterface::recv(uint8_t size){
	return NULL;
}

/**
 * Construct packet and send bytes.
 *
 * Stub, return size.
 */

uint8_t NanoInterface::send(NanoSocket *to,uint8_t *data,uint8_t size){
	return size;
}

/**
 * Return to listening if possible.
 *
 * Stub.
 */

void NanoInterface::interupt(){
}

/**
 * Dispatch error / ack.
 */
/*
void NanoInterface::processOutState(){
	if(outState && outSocket && outSocket->handler){
		switch(outState){
			//Ack
			case NanoSocketHandler::SOCK_ACTION_ACK:
				outSocket->handler->ack(outSocket);
				break;

			//All other states are error.
			default:
				outSocket->handler->error(outSocket,outState);
				outSocket->timeout = 0;
				break;
		}

		outState = 0;
		outSocket = NULL;
	}
}
*/
