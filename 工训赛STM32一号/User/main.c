#include "stm32f10x.h"   
#include "Serial.h"
#include "Duoji.h"
#include "Delay.h"
#include "lcd.h"
#include "GUI.h"
#include "Serial.h"

extern char Serial_RxSeKuaiPacket;
extern volatile uint8_t numberArray[];
uint8_t data_state = 0;
char Serial_RxSeKuai;

bool RxState;
uint8_t RxSeKuai;
bool* RxStatePtr = &RxState;
uint8_t* RxSeKuaiPtr = &RxSeKuai;
uint8_t SendDuoJiFlag = 0; 

bool flag1 = false;

bool flag2 = false;
bool flag21 = false;
bool flag22 = false;
bool flag23 = false;
bool flag24 = false;
bool flag25 = false;
bool flag26 = false;

bool flag3 = false;
bool flag4 = false;
bool flag5 = false;
bool flag6 = false;

uint8_t while1Count = 0;
uint8_t while2Count = 0;
uint8_t while3Count = 0;
uint8_t while4Count = 0;

int main(void) {
	//初始化
		SystemInit();
		LCD_Init();
		Serial_Init();	
		LCD_Clear(BLACK);
		Delay_ms(100);
		Duoji_liaoPan(2);
		Duoji_TaDiao(true);
		Duoji_JiaJu(true);
	

		//初始化完成
		//在屏幕没有显示6个数字之前一直在该循环里
		while(LCD_ShowNum0x0(numberArray[0],numberArray[1],numberArray[2],numberArray[3],numberArray[4],numberArray[5]) == 0){}
		Duoji_TaDiao(false);
		Delay_ms(20);	
	
		Serial_Send6Number(numberArray); 				//有警告不用管我已经实行过了没问题的,收到6个数字后向主摄像头或zet6发送
		Delay_s(2);

		
/*
			在flag1循环中执行第一次从圆盘中抓取三个货物			
*/
		Serial_SendKkon();
		while(!flag1){
			if(SendDuoJiFlag == 1) {
				Duoji_ZhuangXie(*RxStatePtr,*RxSeKuaiPtr,3,3,8,8);
				while1Count++;
				SendDuoJiFlag = 0;
			}
			if(while1Count == 3){flag1 = true; Serial_SendKkoff();}
		}
		
//给上位机发送开启色环识别的指令
	Delay_s(10);
//	Serial_SendHhon();
		
/*
			在flag2循环中只循环一遍
			在圆环卸货三次并且装货三次
			先卸货三次 后装货三次
*/
//		while(!flag2){
//			
//			UDmotor_select(-5);
//			while(!flag21){
//				if(SendDuoJiFlag == 1) {
//					UDmotor_select(5);
//					Delay_ms(1000);
//					Duoji_ZhuangXie(false,*RxSeKuaiPtr,5,5,8,8);
//					SendDuoJiFlag = 0;
//					flag21 = true;
//				}
//			}
//			Delay_ms(1500);
//			UDmotor_select(-5);
//			while(!flag22){
//				if(SendDuoJiFlag == 1) {
//					UDmotor_select(5);
//					Delay_ms(1000);
//					Duoji_ZhuangXie(false,*RxSeKuaiPtr,5,5,8,8);
//					SendDuoJiFlag = 0;
//					flag22 = true;
//				}
//			}
//			Delay_ms(1500);
//			UDmotor_select(-5);
//			while(!flag23){
//				if(SendDuoJiFlag == 1) {
//					UDmotor_select(5);
//					Delay_ms(1000);
//					Duoji_ZhuangXie(false,*RxSeKuaiPtr,5,5,8,8);
//					SendDuoJiFlag = 0;
//					flag23 = true;
//				}
//			}
//			Delay_ms(1500);
//			UDmotor_select(-5);
//			while(!flag24){
//				if(SendDuoJiFlag == 1) {
//					Duoji_ZhuangXie(true,*RxSeKuaiPtr,5,0,8,8);
//					SendDuoJiFlag = 0;
//					flag24 = true;
//				}
//			}
//			Delay_ms(1500);
//			UDmotor_select(-5);
//			while(!flag25){
//				if(SendDuoJiFlag == 1) {
//					Duoji_ZhuangXie(true,*RxSeKuaiPtr,5,0,8,8);
//					SendDuoJiFlag = 0;
//					flag25 = true;
//				}
//			}
//			Delay_ms(1500);
//			UDmotor_select(-5);
//			while(!flag26){
//				if(SendDuoJiFlag == 1) {
//					Duoji_ZhuangXie(true,*RxSeKuaiPtr,5,0,8,8);
//					SendDuoJiFlag = 0;
//					flag26 = true;
//				}
//			}
//			
//			while2Count++;
//			if(while2Count == 1){flag2 = true;}
//		}

////给上位机发送关闭色环识别的指令
//Serial_SendHhoff();		
		
		while(!flag3){
			
		
		}
		while(!flag4){
			
		
		}
		while(!flag5){
			
		
		}

}



/*
						USART2收到来自openmv的色块数据格式为 @x
*/
void USART2_IRQHandler(void){
	
	static int8_t count = 0;
	
	static bool ZhuangXie = true;
	
	char received_char = USART_ReceiveData(USART2);  	// 读取接收到的字符

	switch (data_state){
			case 0:
					if (received_char == '@'){
							data_state = 1; 
					}
					break;
			case 1:
					if (received_char != ' '){
							Serial_RxSeKuai = received_char;
					if(Serial_RxSeKuai == 'R'){
						count++;
						if(count > 3){
							ZhuangXie = !ZhuangXie;
							count = 0;
						}
						RxState = ZhuangXie;
						RxSeKuai = 1;
						SendDuoJiFlag = 1;
					}else if(Serial_RxSeKuai == 'G') {
						count++;
						if(count > 3){
							ZhuangXie = !ZhuangXie;
							count = 0;
						}
						RxState = ZhuangXie;
						RxSeKuai = 2;
						SendDuoJiFlag = 1;
					}else if(Serial_RxSeKuai == 'B') {
						count++;
						if(count > 3){
							ZhuangXie = !ZhuangXie;
							count = 0;
						}
						RxState = ZhuangXie;
						RxSeKuai = 3;
						SendDuoJiFlag = 1;
					}
					}
					data_state = 0;  
					break;
			case 2:
					default:
					data_state = 0;
					break;
	}

   USART_ClearITPendingBit(USART2, USART_IT_RXNE);  // 清除中断标志
}


