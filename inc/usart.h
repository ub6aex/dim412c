#ifndef _USART_H
#define _USART_H

#include <stdbool.h>

// maximum number of channels supported by hardware
#ifdef CH_NUM
    #define DMX_CHANNELS_NUM CH_NUM
#else
    #define DMX_CHANNELS_NUM 4
#endif

uint8_t* USART1_getDmxBuffer(void);
void USART1_init(void);
void USART1_sendByte(unsigned char ucSend_Data);
void USART1_sendString(char *pucString);
void USART1_sendUInt(uint32_t number);
void USART1_incDmxAddress(void);
void USART1_decDmxAddress(void);
void USART1_inc10DmxAddress(void);
void USART1_dec10DmxAddress(void);
void USART1_setDebugMode(bool debugMode);
void USART1_updateDmxAddressOffset(uint8_t offset);

#endif
