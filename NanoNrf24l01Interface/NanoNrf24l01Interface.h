#ifndef __NANO_NRF24L01_INTERFACE
#define __NANO_NRF24L01_INTERFACE

#include <Mirf.h>
#include <NanoNet.h>

class NanoNrf24l01Interface : public NanoInterface {
	public:
		Nrf24l *mirf;
		NanoNrf24l01Interface();
		NanoNrf24l01Interface(Nrf24l *m,uint8_t *addr,uint8_t *msk);
		uint8_t available();
		uint8_t *recv(uint8_t size);
		uint8_t send(NanoSocket *to,uint8_t *data,uint8_t size);
		void init(Nrf24l *m,uint8_t *addr,uint8_t *msk);
		void interupt();
		void powerUp();
		void powerDown();
		bool isTransmitting();
		void zeroRegister(uint8_t reg,uint8_t len);
		void zeroR0();
};

#endif
