#include <reg51.h>
#include "temp.h"
#include "key.h"
#include "delay.h"

#define uchar unsigned char
#define uint unsigned int
	
// 定义IO口
#define KEY_PORT P3        // 按键输入端口
#define SEG_PORT P1        // 数码管段选端口
#define BIT_PORT P2        // 数码管位选端口

bit autoMode = 0;              // 自动模式标志
uchar keyValue;


// 全局变量
unsigned int timer_count = 0;  // 定时器计数
unsigned int countdown = 0;    // 倒计时时间(单位:10ms)
bit timing_flag = 0;           // 定时标志


// 数码管段码表(共阴)
code unsigned char seg_table[] = {
    0x3F, 0x06, 0x5B, 0x4F, // 0-3
    0x66, 0x6D, 0x7D, 0x07, // 4-7
    0x7F, 0x6F, 0x77, 0x7C, // 8-9,A-B
    0x39, 0x5E, 0x79, 0x71  // C-D-E-F
};


// 定时器0初始化(1ms)
void timer0_init(void) {
    TMOD |= 0x01;       // 定时器0工作在模式1(16位定时器)
    TH0 = 0xFC;         // 定时初值设置为1ms
    TL0 = 0x66;
    ET0 = 1;            // 使能定时器0中断
    TR0 = 1;            // 启动定时器0
}

// 定时器0中断服务函数
void timer0_isr(void) interrupt 1 {
    TH0 = 0xFC;         // 重新加载初值
    TL0 = 0x66;
    
    timer_count++;      // 定时器计数加1
    
    // 每10ms更新一次倒计时
    if(timer_count >= 10) {
        timer_count = 0;
        
        if(timing_flag && countdown > 0) {
            countdown--;
        }
    }
}

// 系统初始化
void system_init(void) {
    timer0_init();  // 初始化定时器0
    EA = 1;         // 开总中断
}


// 数码管显示倒计时
void display_countdown(void) {
    uint seconds;
    uchar digit[4];
    uchar i;
    
    seconds = countdown / 100;  // 转换为秒
    
    // 分解为各位数字
    digit[0] = seconds / 1000;
    digit[1] = (seconds % 1000) / 100;
    digit[2] = (seconds % 100) / 10;
    digit[3] = seconds % 10;
    
    // 动态扫描显示
    for(i = 0; i < 4; i++) {
        // 选通对应位数码管
        switch(i) {
            case 0: BIT_PORT = 0x01; break;
            case 1: BIT_PORT = 0x02; break;
            case 2: BIT_PORT = 0x04; break;
            case 3: BIT_PORT = 0x08; break;
        }
        
        // 输出段码
        SEG_PORT = ~seg_table[digit[i]];
        
        // 短暂延时
        DelayMS(2);
    }
}



// 主函数
void main() {
	
    // 初始化配置
    SetDACVoltage(0);      // 启动时风扇停止
    DS18B20_Init();        // DS18B20初始化
   
	
    // 初始化温度读取并等待稳定
    GetTemperature();
    DelayMS(1000);
	
    while(1) {
		
		if(autoMode == 1){
			TempAutoControl();
        }
		//DelayMS(1000);
        keyValue = KeyScan();  // 扫描按键
        if(keyValue == 7){
			autoMode = 1;
			TempAutoControl();
		}
		else if(keyValue == 6){
			system_init();
			while(1){
				keyValue = KeyScan();
				display_countdown();// 显示倒计时
				if(keyValue != 8){
					
					countdown = 1000*keyValue;
					timing_flag = 1;
					break;
				}
			}
		}
        else if(0 <= keyValue && keyValue <= 5) {
            autoMode = 0;            // 手动控制模式
            SetDACVoltage(keyValue);  // 设置档位
			
			if(timing_flag == 1){
				timing_flag = 0;
				EA = 0;
				countdown = 0;
				SEG_PORT = 0xff;
			}
        }
		
		
		if(timing_flag == 1){
			display_countdown();// 显示倒计时
		}
		
		if(timing_flag == 1 && countdown == 0){
			SetDACVoltage(0);
			timing_flag = 0;
			EA = 0;
			SEG_PORT = 0xff;
		}
		DelayMS(10); // 释放CPU时间
    }
}