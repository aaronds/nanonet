#include <avr/sleep.h>
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
   
  //Serial.println("Handeling Ping");
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

/**
 * Called when the mirf interupt toggles.
 */

volatile boolean mirfWakeup = false;

void wakeupFunction(){
  mirfWakeup = true;
}

/**
 * Called when timer 2 overflows
 */

volatile boolean timerWakeup = false;

ISR (TIMER2_OVF_vect){
    timerWakeup = true;
}

void toSleep(){
  sleep_enable();
  sleep_mode();
  sleep_disable();
}

void startTimer2(){
  TIMSK2 |= (1 << TOIE2);
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
}

void setup(){
  Serial.begin(9600);
  
  //Mirf.csnPin = 8;
  //Mirf.cePin = 7;
  
  /*
   * Initalise the inerface.
   */
  
  primary.init(&Mirf,addr,mask);
  
  /*
   * Initialize the network.
   */
  net.init(&primary);
  
  /*
   * 'bind' a listening socket to a port.
   */
   
  net.bind(&pingSocket);
  
  /*
   * Choose how deep the processor sleeps.
   *
   * SLEEP_MODE_PWR_SAVE is my personal favorite because 
   * of its low power consumption and timer2 can be used to
   * wake it up. Very usefull for periodicly recording data.
   */
   
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  
  /*
   * Attache the interupt to wake us up when the mirf needs 
   * some attention.
   */
  
  attachInterrupt(0,wakeupFunction,LOW);
  
  /* Enable interupts */
  sei();
  
  /* Start timer 2. */
  startTimer2();
  
  /*
   * Serial is a bit unstable when used with sleeping expect
   * the odd random character when the processor wakes up.
   */
  Serial.println("NN Up");
}

void loop(){
  /* 
   * Does the mirf need attention.
   */
   
  if(mirfWakeup){
    mirfWakeup = false;
    if(primary.available() == NANO_PACKET_SIZE){
      net.handlePacket(&primary,primary.recv(NANO_PACKET_SIZE));
    }
    net.processStateChanges();
  }

  /*
   * Process socket timeouts
   */
  
  if(timerWakeup){
    timerWakeup = false;
    net.processTimeout();
  }  
  
  /*
   * If there is no data waiting we may as well sleep.
   */
  
  if(!timerWakeup && !mirfWakeup){
    toSleep();
  }
}
