#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

void Serial_Init(void);
void Serial_Send(uint8_t ID, uint8_t Length, uint8_t Cmd, uint8_t Prm1, uint8_t Prm2, uint8_t Prm3, uint8_t Prm4, uint8_t Checksum);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

void Serial_Send6Number(uint8_t* Array);
void UDmotor_Up(uint8_t s1, uint8_t s2);
void UDmotor_Down(uint8_t s1, uint8_t s2);
void UDmotor_select(int8_t s);
void Serial_SendKkon(void);
void Serial_SendKkoff(void);
void Serial_SendHhon(void);
void Serial_SendHhoff(void);

uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);

#endif
