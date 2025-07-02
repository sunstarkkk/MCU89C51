#ifndef __DELAY_H_
#define __DELAY_H_

#include <reg51.h>
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif


// 延时函数
void DelayMS(uint ms);
// 微秒级延时
void DelayUS(uint us);