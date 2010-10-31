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
 * Sleep.
 */

#include <avr/sleep.h>

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

/*
 * Night night.
 */

void toSleep(){
  sleep_enable();
  sleep_mode();
  sleep_disable();
}

/*
 * Start Timer
 */

void startTimer2(){
  TIMSK2 |= (1 << TOIE2);
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
}


/*
 * Count the number of times timer2 overflows.
 */ 
unsigned long timerOverflows = 0;
//unsigned long lastTimerOverflows = 0;

void setup(){
  Serial.begin(9600);
  //Mirf.csnPin = 7;
  //Mirf.cePin = 8;
  primary.init(&Mirf,addr,mask);
  net.init(&primary);
  net.connect(&pingSocket);
  
  /*
   * Choose how deep the processor sleeps.
   *
   * SLEEP_MODE_PWR_SAVE is my personal favorite because 
   * of its low power consumption and timer2 can be used to
   * wake it up. Very usefull for periodicly recording data.
   *
   * However I'm going to use SLEEP_MODE_IDLE to keep all the 
   * other timers going (i.e Timer0 for millis())
   */
   
  set_sleep_mode(SLEEP_MODE_IDLE);
  
  /*
   * Attache the interupt to wake us up when the mirf needs 
   * some attention.
   */
  
  attachInterrupt(0,wakeupFunction,LOW);
  
  /* Enable interupts */
  sei();
  
  /* Start timer 2. */
  startTimer2();
  
  /**
   * Set the number of timer2 timeouts to wait before generating
   * an error.
   */
  
  net.timeout = 100;
  
  Serial.println("NN Up");
}

void loop(){
  unsigned long time;

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
    timerOverflows++;
    net.processTimeout();
  }


  /**
   * Wait for the last response to be recived or an error.
   */
  if(pingWaiting == false && (timerOverflows % 100) == 0){
    /*
     * Record the time to be sent to the server.
     */
    
    timerOverflows++;
    
    time = millis();
    pingSocket.send((byte *)&time,sizeof(unsigned long));
    pingWaiting = true;
  }

  /*
   * If there is no data waiting we may as well sleep.
   */
  
  if(!timerWakeup && !mirfWakeup){
    toSleep();
  }
}
