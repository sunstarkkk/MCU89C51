#include "temp.h"
#include "delay.h"

#define uchar unsigned char
#define uint unsigned int
	

// 全局变量
uchar currentLevel = 0;        // 当前档位
float currentTemp = 0.0;       // 当前温度值
uchar newLevel;


/* DAC控制引脚 */
#define DAC_DATA P0
sbit DAC_WR = P3^6;
/* DS18B20引脚 */
sbit DS18B20_DQ = P2^7;  // 假设DS18B20数据线连接到P2.7


/* 档位对应输出电压值 */
uchar code VOLTAGE_LEVEL[7] = {
    0x00,   // 0档：0V
    0x24,   // 1档
    0x48,   // 2档
    0x6C,   // 3档
    0x90,   // 4档
    0xB4,   // 5档
    0xD8,   // 6档
};


/******************** DS18B20驱动程序 ********************/
// DS18B20初始化
void DS18B20_Init() {
    DS18B20_DQ = 0x01;   // 拉高总线
    DelayUS(8);       // 延时
    DS18B20_DQ = 0x00;   // 拉低总线(复位脉冲)
    DelayUS(80);      // 480-960μs复位脉冲
    DS18B20_DQ = 0x01;   // 释放总线(上升沿)
    DelayUS(14);      // 等待15-60μs
    // 等待DS18B20回应(低电平60-240μs)
    while(DS18B20_DQ);
    while(!DS18B20_DQ);
    DelayUS(20);
}

// 向DS18B20写入一个字节
void DS18B20_WriteByte(uchar dat) {
    uchar i;
    for(i = 0; i < 8; i++) {
        DS18B20_DQ = 0x00;   // 拉低总线(启动写时隙)
        DS18B20_DQ = dat & 0x01; // 写入数据位
        DelayUS(10);      // 保持时间>1μs
        DS18B20_DQ = 0x01;   // 释放总线
        dat >>= 1;        // 准备下一位
        DelayUS(5);       // 恢复时间>1μs
    }
}

// 从DS18B20读取一个字节
uchar DS18B20_ReadByte() {
    uchar i, dat = 0;
    for(i = 0; i < 8; i++) {
        DS18B20_DQ = 0x00;   // 拉低总线(启动读时隙)
        dat >>= 1;         // 准备接收位
        DS18B20_DQ = 0x01;   // 释放总线
        if(DS18B20_DQ)    // 读取数据位
            dat |= 0x80;
        DelayUS(8);       // 等待采样
    }
    return dat;
}

// 设置DAC输出电压
void SetDACVoltage(uchar level) {
    // 确保档位在有效范围内
    if(level > 5) level = 5;
    
    DAC_DATA = VOLTAGE_LEVEL[level];
    currentLevel = level; // 更新当前档位
}

// 读取温度并计算结果
void DS18B20_ReadTemp() {
    uchar TL, TH;
    int temp;
    
    DS18B20_Init();             // 初始化
    DS18B20_WriteByte(0xCC);    // 跳过ROM匹配
    DS18B20_WriteByte(0x44);    // 启动温度转换
    DelayMS(150);              // 等待转换完成 
	
    DS18B20_Init();             // 重新初始化
    DS18B20_WriteByte(0xCC);    // 跳过ROM匹配
    DS18B20_WriteByte(0xBE);    // 读取温度寄存器
    
    TL = DS18B20_ReadByte();    // 读取温度低字节
    TH = DS18B20_ReadByte();    // 读取温度高字节
    
    // 组合温度值
    temp = (TH << 8) | TL;
    // 转换为实际温度值(除以16)
    currentTemp = temp * 0.0625;
}

// 获取当前温度
float GetTemperature() {
    DS18B20_ReadTemp();
    return currentTemp;
}

/******************** 自动控制函数 ********************/
// 温度自动控制策略
void TempAutoControl() {
    float temp = GetTemperature();
    if(temp <= 27.0) {
		SetDACVoltage(0);
    } else {
        // 温度>=28℃，开启自动控制
        // 计算档位：每升高1℃升一档
        newLevel = (uchar)(temp - 27.0);
        
        // 确保档位在有效范围内
        if(newLevel > 5) newLevel = 5;
        SetDACVoltage(newLevel);
    }
}