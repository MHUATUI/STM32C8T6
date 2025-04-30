#include "stm32f10x.h"
#include "serial.h"
#include "stdbool.h"
#include "Delay.h"

/*
									塔吊ID02	夹具ID01	料盘ID03
									value:0-1000 	jiaodu:0-240
									R“1”为红色，G“2”为绿色，B“3”为蓝色
*/

#define TADIAO_RESET	Serial_Send(0x02,0x07,0x01,0x5B,0x00,0xF4,0x01,0xA5)  	//(将塔吊转盘置为初始位置 22度 时间500ms 设备ID02)
#define TADIAO_SET  	Serial_Send(0x02,0x07,0x01,0x49,0x03,0xF4,0x01,0xB4) 	//(将塔吊转向 202度 时间500ms 设备ID02)

#define JIAJU_OPEN  	Serial_Send(0x01,0x07,0x01,0xC8,0x00,0xF4,0x01,0x39)	
#define JIAJU_CLOSE  	Serial_Send(0x01,0x07,0x01,0x8A,0x02,0xF4,0x01,0x75)	

#define LIAOPAN_1		Serial_Send(0x03,0x07,0x01,0x00,0x00,0xF4,0x01,0xFF)	//1号位置 0°
#define LIAOPAN_2		Serial_Send(0x03,0x07,0x01,0x77,0x01,0xF4,0x01,0x87)	//2号位置 90°
#define LIAOPAN_3		Serial_Send(0x03,0x07,0x01,0xEE,0x02,0xF4,0x01,0x0F)	//3号位置 180°



void Duoji_JiaJu(bool state) {
	if(state == true){
		JIAJU_OPEN;
	} else if(state == false) {
		JIAJU_CLOSE;
	}
}

void Duoji_liaoPan(uint8_t LiaoPanNum) {
	if(LiaoPanNum == 1) {
		LIAOPAN_1;
	} else if(LiaoPanNum == 2) {
		LIAOPAN_2;
	} else if(LiaoPanNum == 3) {
		LIAOPAN_3;
	}
}

void Duoji_TaDiao(bool state) {
	if(state == true){
		TADIAO_RESET;
	} else if(state == false) {
		TADIAO_SET;
	}
}

/*
							（上电前需手动给调整到待机状态）从待机状态转为工作状态
							初始状态为塔吊朝向里面，夹具置于最高处并且张开，料盘处于2号位
							工作状态为塔吊朝向外面，夹具置于最高处并且张开，料盘处于2号位
*/
void Duoji_Set(void){
	TADIAO_RESET;
}

//夹具初始位置为最高位置
//height1 外升高度
//height2 外降高度
//height3 内升高度
//height4 内降高度	
void Duoji_ZhuangXie(bool state, uint8_t LiaoPanNum, int8_t height1, int8_t height2,int8_t height3, int8_t height4) {
	if(state == true) {
		UDmotor_select(-height2);								//下降
		Delay_ms(1500);	
		
		Duoji_JiaJu(false);								//抓取
		Duoji_liaoPan(LiaoPanNum);						//转动料盘到等待区
		Delay_ms(1500);										//等待夹具和料盘就位
		
		UDmotor_select(height1);	     						//上升
		Delay_ms(1500);	
		
		Duoji_TaDiao(true);								//转动塔吊
		Delay_s(1);										//等待塔吊就位
		
		UDmotor_select(-height4);								//下降
		Delay_s(1);
		
		Duoji_JiaJu(true);								//夹具释放
		Delay_ms(200);										//等待夹具释放完毕
		
		UDmotor_select(height3);								//上升
		Delay_s(1);
		
		Duoji_TaDiao(false);								//塔吊复位
	} else if(state == false) {
		Duoji_TaDiao(true);							//转动塔吊
		Duoji_liaoPan(LiaoPanNum);						//转动料盘到等待区
		Delay_ms(500);

		UDmotor_select(-height4);								//下降
		Delay_s(1);		
		
										//等待塔吊和料盘就位
		Duoji_JiaJu(false);								//抓取料件
		Delay_s(1);										//等待夹具抓取完毕

		UDmotor_select(height3);	     						//上升
		Delay_s(1);	
		
		Duoji_TaDiao(false);								//塔吊复位
		Delay_s(1);										//等待塔吊就位

		UDmotor_select(-height2);								//下降
		Delay_s(1);
		
		Duoji_JiaJu(true);								//夹具释放
		Delay_s(1);
		
		UDmotor_select(height1);	     						//上升
		Delay_s(1);	
	}
}
