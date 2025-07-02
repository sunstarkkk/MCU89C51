
#include "delay.h"

// 延时函数
void DelayMS(uint ms) {
    uint i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 114; j++);
}

// 微秒级延时
void DelayUS(uint us) {
    while(us--);
}