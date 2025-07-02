#include "key.h"
#include "delay.h"


#define uchar unsigned char
#define uint unsigned int


/* 按键引脚定义 */
sbit KEY_A = P3^0;
sbit KEY_B = P3^1;
sbit KEY_C = P3^2;
sbit KEY_D = P3^3;
sbit KEY_E = P3^4;
sbit KEY_F = P3^5;
sbit KEY_G = P3^6;
sbit KEY_H = P3^7;



// 按键扫描函数
uchar KeyScan() {
    if(KEY_A == 0) {        
        DelayMS(100);
        if(KEY_A == 0) {
            while(!KEY_A);
            return 7;
        }
    }
    if(KEY_B == 0) {         
        DelayMS(100); 
        if(KEY_B == 0) {
            while(!KEY_B);   
            return 0;
        }
    }
    if(KEY_C == 0) {       
        DelayMS(100);  
        if(KEY_C == 0) {
            while(!KEY_C);   
            return 1;
        }
    }
    if(KEY_D == 0) {       
        DelayMS(100);
        if(KEY_D == 0) {
            while(!KEY_D);  
            return 2;
        }
    }
    if(KEY_E == 0) {        
        DelayMS(100); 
        if(KEY_E == 0) {
            while(!KEY_E);  
            return 3;
        }
    }
    if(KEY_F == 0) {         
        DelayMS(100); 
        if(KEY_F == 0) {
            while(!KEY_F);  
            return 4;
        }
    }
    if(KEY_G == 0) {        
        DelayMS(100); 
        if(KEY_G == 0) {
            while(!KEY_G); 
            return 5;
        }
    }
    if(KEY_H == 0) {         
        DelayMS(100);
        if(KEY_H == 0) {
            while(!KEY_H);
            return 6;
        }
    }
    return 8;  // 无按键按下
}