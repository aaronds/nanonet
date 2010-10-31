/**
 * SPI Interface library.
 */
#include <Spi.h>

/**
 * nRF24L01 interface.
 */
#include <Mirf.h>
#include <nRF24L01.h>

/**
 * Nano Net
 */
#include <NanoNet.h>
#include <NanoInterface.h>
#include <NanoSocket.h>
#include <NanoSocketHandler.h>
#include <NanoSocketCallbackHandler.h>

/**
 * NanoNet Interface
 * The wrappers arround the nRF24L01 driver.
 */
#include <NanoNrf24l01Interface.h>

/**
 * The addresses.
 */
byte addr[] = {1,0,0,0,1};

/**
 * The interface mask.
 * Think like IP.
 */
byte mask[] = {0,0,0,0,0};

/**
 * Prototypes for the callback.
 */

void pingHandleData(NanoSocket *sock);

/**
 * The interface, a wrapper around the nRF24L01.
 */ 
//NanoNrf24l01Interface primary(&Mirf,addr,mask);
NanoNrf24l01Interface primary;
/**
 * The network.
 */
//NanoNet net(&primary);
NanoNet net;


/**
 * The callback handler.
 */
 
NanoSocketCallbackHandler pingHandler(&pingHandleData);

/**
 * The socket for sending and reciving the pings.
 */
 
NanoSocket pingSocket(1,0,NULL,&pingHandler);

/**
 * Socket handler.
 */

void pingHandleData(NanoSocket *sock){
  byte dataSize = sock->available();
  byte *data;
  /**
   * Check the data recived is the right size.
   */
   
  Serial.println("Handeling Ping");
  if(dataSize == sizeof(unsigned long)){
    /**
     * Recive the data from the client.
     */
    data = sock->recv(sizeof(unsigned long));
    
    /**
     * Return the data.
     */
    sock->send(data,sizeof(unsigned long));
  }
}

void setup(){
  Serial.begin(9600);
  
  
  Mirf.csnPin = 8;
  Mirf.cePin = 7;
  
  primary.init(&Mirf,addr,mask);
  net.init(&primary);
  
  net.bind(&pingSocket);
  Serial.println("NN Up");
}

void loop(){
  net.poll();
}
