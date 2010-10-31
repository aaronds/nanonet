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
byte addr[] = {1,0,0,0,2};
byte server[] = {1,0,0,0,1};

/**
 * The interface mask.
 * Think like IP.
 */
byte mask[] = {0,0,0,0,0};

/**
 * Prototypes for the callback.
 */

void pingHandleData(NanoSocket *sock);
void pingHandleError(NanoSocket *sock,byte reason);

/**
 * Socket handler.
 * Calls c functions for handle and error.
 */

NanoSocketCallbackHandler pingHandler(&pingHandleData,&pingHandleError); 

/**
 * Waiting for ping response.
 */

boolean pingWaiting = false;

/**
 * The interface, a wrapper around the nRF24L01.
 */ 
 
NanoNrf24l01Interface primary;

/**
 * The network.
 */
 
NanoNet net;


/**
 * The socket for sending and reciving the pings.
 */

NanoSocket pingSocket(1,server,&pingHandler);



/**
 * Handle a response to the ping.
 *
 * The data in the response packet will be the time
 *   the packet was sent.
 */

void pingHandleData(NanoSocket *socket){
  byte *data = NULL;
  unsigned long time = 0;
  
  /**
   * Check the size of the data returned.
   * 
   * Rarely the on chip will recive random data.
   */
   
  if(socket->available() == sizeof(unsigned long)){
    /**
     * For clarity memcpy is used instead of dereferencing pointers etc.
     */
     
    /**
     * Get the data.
     */
    data = socket->recv(sizeof(unsigned long));
    
    /**
     * Copy the data from an array of meaningless bytes (data) into a sensible number (time)
     */
    memcpy((void *)&time,data,sizeof(unsigned long));
    
    /**
     * Compute the number of milliseconds since we sent the original request.
     */
     
    time = millis() - (time);
    Serial.print("Ping: ");
    Serial.println(time);
  }
  
  pingWaiting = false;
}

/**
 * Handle an error.
 */
 
void pingHandleError(NanoSocket *socket,byte reason){
  
  if(reason == NanoSocketHandler::SOCK_ERROR_TIMEOUT){
    /**
     * The socket has timed out wating for a response.
     */
    Serial.println("Ping: Timeout");
  }else{
    /**
     * Some interfaces (nRF24L01) support auto acknolagment
     * this is triggered when that acknolagemnt is not recived.
     */
    Serial.println("Ping: No Ack");
  }
  
  pingWaiting = false;
}


/*
 * Store the time the last ping was sent.
 */ 
unsigned long lastPingTime = 0;

void setup(){
  Serial.begin(9600);
  //Mirf.channel = 84;
  //Mirf.csnPin = 7;
  //Mirf.cePin = 8;
  primary.init(&Mirf,addr,mask);
  net.init(&primary);
  net.connect(&pingSocket);
  Serial.println("NN Up");
}

void loop(){
  unsigned long time;
  /**
   * Wait for the last response to be recived or an error.
   */
  if(pingWaiting == false && (millis() - lastPingTime) >= 1000){
    /*
     * Record the time to be sent to the server.
     */
    
    lastPingTime = time = millis();
    pingSocket.send((byte *)&time,sizeof(unsigned long));
    pingWaiting = true;
  }
  
  net.poll();
}
