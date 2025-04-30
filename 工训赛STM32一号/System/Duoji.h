#ifndef _DUOJI_H
#define _DUOJI_H

#include "stdbool.h"


void Duoji_Set(void);	//置于工作状态

//基础函数
void Duoji_JiaJu(bool state);									//true:打开夹具     false:关闭夹具		
void Duoji_liaoPan(uint8_t LiaoPanNum);				//0：一号位料盘	 		1：二号位料盘	2：三号位料盘
void Duoji_TaDiao(bool state);								//ture:归位					false：向后转

/*
//完全体函数
//0：一号位料盘	 	1：二号位料盘	2：三号位料盘      
//true:装货 false:卸货	 
//height1 外升高度
	height2 外降高度
//height3 内升高度
	height4 内降高度	
*/
void Duoji_ZhuangXie(bool state, uint8_t LiaoPanNum, int8_t height1, int8_t height2, int8_t height3, int8_t height4);	


#endif
