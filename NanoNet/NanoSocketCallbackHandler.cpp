#include <NanoSocketCallbackHandler.h>
NanoSocketCallbackHandler::NanoSocketCallbackHandler(void (*cb) (NanoSocket *sock),void (*er) (NanoSocket *sock,uint8_t error)) : NanoSocketHandler() {
	callback = cb;
	errorCallback = er;
	openCallback = NULL;
	closeCallback = NULL;
	ackCallback = NULL;
}

NanoSocketCallbackHandler::NanoSocketCallbackHandler(void (*cb) (NanoSocket *sock)){
	callback = cb;
	errorCallback = NULL;
	openCallback = NULL;
	closeCallback = NULL;
	ackCallback = NULL;
}


void NanoSocketCallbackHandler::handle(NanoSocket *socket){
	callback(socket);
}

void NanoSocketCallbackHandler::error(NanoSocket *socket,uint8_t errorCode){
	if(errorCallback){
		errorCallback(socket,errorCode);
	}
}

void NanoSocketCallbackHandler::open(NanoSocket *socket){
	if(openCallback){
		openCallback(socket);
	}
}

void NanoSocketCallbackHandler::close(NanoSocket *socket){
	if(closeCallback){
		closeCallback(socket);
	}
}

void NanoSocketCallbackHandler::ack(NanoSocket *socket){
	if(ackCallback){
		ackCallback(socket);
	}
}

bool NanoSocketCallbackHandler::timeout(NanoSocket *socket){
	return socket->timeout == 65535;
}
