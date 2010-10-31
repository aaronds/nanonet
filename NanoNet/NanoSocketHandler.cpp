#include <NanoSocketHandler.h>

NanoSocketHandler::NanoSocketHandler(){
	timeout = true;
}

/**
 * Stub, for gcc vtables.
 */

void NanoSocketHandler::open(NanoSocket *socket){
}

/**
 * Stub.
 */

void NanoSocketHandler::ack(NanoSocket *socket){
}

/**
 * Stub.
 */

void NanoSocketHandler::handle(NanoSocket *socket){
}

/**
 * Stub.
 */

void NanoSocketHandler::error(NanoSocket *socket,uint8_t reason){
}

/**
 * Legacy.
 */

/**
 * Stub.
 */

void NanoSocketHandler::close(NanoSocket *socket){
}
