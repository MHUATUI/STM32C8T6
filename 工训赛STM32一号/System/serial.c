#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "Delay.h"
#include "DuoJi.h"

// 定义状态机状态
typedef enum {
    WAIT_START,        // 等待'@'字符
    RECV_FIRST_PART,   // 接收逗号前的3位数字
    WAIT_COMMA,        // 等待逗号
    RECV_SECOND_PART   // 接收逗号后的3位数字
} RxState;

// 定义全局变量
volatile RxState rxState = WAIT_START;
volatile uint8_t dataIndex = 0;
volatile uint8_t numberArray[6];  // 存储最终6个数字的数组


uint8_t Serial_RxData;
uint8_t Serial_RxFlag;





void Serial_Init(void){
	//发送命令控制舵机和步进电机，和收到色块
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART2, ENABLE);

/******************************************************************************/
//从k210接受6个数字 ，再给主摄像头发送6个数字
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructures;
	GPIO_InitStructures.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructures.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructures.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructures);

	GPIO_InitStructures.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructures.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructures.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructures);
	
	USART_InitTypeDef USART_InitStructures;
	USART_InitStructures.USART_BaudRate = 115200;
	USART_InitStructures.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructures.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructures.USART_Parity = USART_Parity_No;
	USART_InitStructures.USART_StopBits = USART_StopBits_1;
	USART_InitStructures.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructures);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructures;
	NVIC_InitStructures.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructures.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructures.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructures.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructures);
	
	USART_Cmd(USART1, ENABLE);
}

void Serial_Send6Number(uint8_t* Array){
	
	USART_SendData(USART1, '@');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	
	if(Array[0] == 1) {
		USART_SendData(USART1, 'R');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}else if(Array[0] == 2) {
		USART_SendData(USART1, 'G');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	}else if(Array[0] == 3){
		USART_SendData(USART1, 'B');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	}

	if(Array[1] == 1) {
		USART_SendData(USART1, 'R');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}else if(Array[1] == 2) {
		USART_SendData(USART1, 'G');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	}else if(Array[1] == 3){
		USART_SendData(USART1, 'B');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	}
	
	if(Array[2] == 1) {
		USART_SendData(USART1, 'R');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}else if(Array[2] == 2) {
		USART_SendData(USART1, 'G');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	}else if(Array[2] == 3){
		USART_SendData(USART1, 'B');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	}
	
	USART_SendData(USART1, '+');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	
	if(Array[3] == 1) {
		USART_SendData(USART1, 'R');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}else if(Array[3] == 2) {
		USART_SendData(USART1, 'G');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	}else if(Array[3] == 3){
		USART_SendData(USART1, 'B');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	}

	if(Array[4] == 1) {
		USART_SendData(USART1, 'R');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}else if(Array[4] == 2) {
		USART_SendData(USART1, 'G');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	}else if(Array[4] == 3){
		USART_SendData(USART1, 'B');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	}

	if(Array[5] == 1) {
		USART_SendData(USART1, 'R');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}else if(Array[5] == 2) {
		USART_SendData(USART1, 'G');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	}else if(Array[5] == 3){
		USART_SendData(USART1, 'B');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	}	
}

void Serial_SendByte(uint8_t Byte){
	USART_SendData(USART2, Byte);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)//数据发送
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        Serial_SendByte(Array[i]);
    }
    Delay_ms(10);  
}

void UDmotor_Up(uint8_t s1, uint8_t s2)
{
	uint8_t cmd[] = {0x01, 0xFB, 0x00, 0x05, 0xDC, 0x00, 0x00, s1, s2, 0x00, 0x00, 0x6B};//数组的第五第六元素控制电机旋转的速度(1500)10=(05DC)16,函数传递参数s1，s2为电机旋转的圈数，(0E10)16=(3600)10 3600为一圈
    Serial_SendArray(cmd, 12); 
}

void UDmotor_Down(uint8_t s1, uint8_t s2)
{
	uint8_t cmd[] = {0x01, 0xFB, 0x01, 0x05, 0xDC, 0x00, 0x00, s1, s2, 0x00, 0x00, 0x6B};//与上一个函数相同，区别为数组的第三个参数0x01为负方向旋转，0x00为正方向旋转
    Serial_SendArray(cmd, 12);  
}

void UDmotor_select(int8_t s)//正数为正转，负数为反转
{
    switch (s)
    {
        case 1: UDmotor_Up(0x0E, 0x10); break;  //1圈
        case -1: UDmotor_Down(0x0E, 0x10);break;			
			
        case 2: UDmotor_Up(0x1C, 0x20); break;  //2圈		
        case -2: UDmotor_Down(0x1C, 0x20); break;
			
        case 3: UDmotor_Up(0x2A, 0x30); break;  //3圈
        case -3: UDmotor_Down(0x2A, 0x30); break;
			
				case 4:UDmotor_Up(0x38,0x40);break;			//4圈
				case -4:UDmotor_Down(0x38,0x40);break;
			
				case 5:UDmotor_Up(0x46,0x50);break;	//5圈
				case -5:UDmotor_Down(0x46,0x50);break;
			
        case 6: UDmotor_Up(0x54, 0xCD); break;  //6圈
        case -6: UDmotor_Down(0x54, 0xCD); break;
			
				case 7:UDmotor_Up(0X07,0X08);break;//旋转半圈
				case -7:UDmotor_Down(0x07,0x08);break;
				
			  case 8: UDmotor_Up(0x15, 0x18); break;  //1.5圈
        case -8: UDmotor_Down(0x15, 0x18);break;
					
				default: break;
    }
}

void Serial_Send(uint8_t ID, uint8_t Length, uint8_t Cmd, uint8_t Prm1, uint8_t Prm2, uint8_t Prm3, uint8_t Prm4, uint8_t Checksum) {
	USART_SendData(USART2,0x55);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2,0x55);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2,ID);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2,Length);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2,Cmd);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2,Prm1);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2,Prm2);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2,Prm3);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2,Prm4);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2,Checksum);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}





void Serial_SendString(char *String){
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y){
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length){
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int ch, FILE *f){
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...){
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

uint8_t Serial_GetRxFlag(void){
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void){
	return Serial_RxData;
}

void Serial_SendKkon(void) {
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'k');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'k');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, ' ');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'o');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'n');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, '\n');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
}

void Serial_SendKkoff(){
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'k');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'k');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, ' ');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'o');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'f');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'f');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	USART_SendData(USART1, '\n');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
}

void Serial_SendHhoff(void){
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'h');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'h');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, ' ');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'o');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'f');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'f');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
	USART_SendData(USART1, '\n');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
}
void Serial_SendHhon(void){
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'h');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'h');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, ' ');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'o');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, 'n');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, '\n');
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
}

static inline int my_isdigit(uint8_t c) {
    // 注意转换为无符号比较，避免符号扩展问题
    return (c >= '0' && c <= '9');
}

void USART1_IRQHandler(void) {
	 // 检查接收中断标志
	
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        // 读取接收到的字符
        uint8_t receivedChar = USART_ReceiveData(USART1);
        
        switch(rxState)
        {
        case WAIT_START:
            if (receivedChar == '@')
            {
                // 检测到起始字符，准备接收第一个数字部分
                rxState = RECV_FIRST_PART;
                dataIndex = 0;
            }
            break;
            
        case RECV_FIRST_PART:
            if (my_isdigit(receivedChar))  // 检查是否为数字字符
            {
                // 将ASCII字符转换为数字并存储
                numberArray[dataIndex++] = receivedChar - '0';
                
                // 已接收3个数字后切换状态
                if (dataIndex >= 3)
                {
                    rxState = WAIT_COMMA;
                }
            }
            else
            {
                // 非数字字符，重置状态机
                rxState = WAIT_START;
            }
            break;
            
        case WAIT_COMMA:
            if (receivedChar == '+')
            {
                // 检测到逗号，准备接收第二个数字部分
                rxState = RECV_SECOND_PART;
            }
            else
            {
                // 非逗号字符，重置状态机
                rxState = WAIT_START;
            }
            break;
            
        case RECV_SECOND_PART:
            if (my_isdigit(receivedChar))
            {
                // 存储后续数字（从数组索引3开始）
                numberArray[dataIndex++] = receivedChar - '0';
                
                // 已接收6个数字时完成接收
                if (dataIndex >= 6)
                {
                    // 这里可以添加数据处理或通知标志
                    // 重置状态机准备下一次接收
                    rxState = WAIT_START;
                    dataIndex = 0;
                }
            }
            else
            {
                // 非数字字符，重置状态机
                rxState = WAIT_START;
                dataIndex = 0;
            }
            break;
            
        default:
            rxState = WAIT_START;
            break;
        }
        
        // 清除接收中断标志
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
