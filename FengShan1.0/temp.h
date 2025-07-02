#ifndef __TEMP_H_
#define __TEMP_H_

#include <reg51.h>
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif


// DS18B20初始化
void DS18B20_Init();

// 向DS18B20写入一个字节
void DS18B20_WriteByte(uchar dat);

// 从DS18B20读取一个字节
uchar DS18B20_ReadByte();

// 设置DAC输出电压
void SetDACVoltage(uchar level);

// 读取温度并计算结果
void DS18B20_ReadTemp();

// 获取当前温度
float GetTemperature();

// 温度自动控制策略
void TempAutoControl();