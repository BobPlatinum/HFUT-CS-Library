////
//// Created by Chiu-xaH on 2025/6/19.
////
//
//#ifndef IRQ_OK_SHARE_H
//#define IRQ_OK_SHARE_H
//
//#endif //IRQ_OK_SHARE_H

#include "buzzer.h"

#define GPKCON0 (*((volatile unsigned long *)0x7F008800))
#define GPKDATA (*((volatile unsigned long *)0x7F008808))
#define GPNCON (*((volatile unsigned long *)0x7F008830))
#define GPNDAT (*((volatile unsigned long *)0x7F008834))
#define EINT0CON0 (*((volatile unsigned long *)0x7F008900))
#define EINT0MASK (*((volatile unsigned long *)0x7F008920))
#define EINT0PEND (*((volatile unsigned long *)0x7F008924))
#define PRIORITY (*((volatile unsigned long *)0x7F008280))
#define SERVICE (*((volatile unsigned long *)0x7F008284))
#define SERVICEPEND (*((volatile unsigned long *)0x7F008288))
#define VIC0IRQSTATUS (*((volatile unsigned long *)0x71200000))
#define VIC0FIQSTATUS (*((volatile unsigned long *)0x71200004))
#define VIC0RAWINTR (*((volatile unsigned long *)0x71200008))
#define VIC0INTSELECT (*((volatile unsigned long *)0x7120000c))
#define VIC0INTENABLE (*((volatile unsigned long *)0x71200010))
#define VIC0INTENCLEAR (*((volatile unsigned long *)0x71200014))
#define VIC0PROTECTION (*((volatile unsigned long *)0x71200020))
#define VIC0SWPRIORITYMASK (*((volatile unsigned long *)0x71200024))
#define VIC0PRIORITYDAISY (*((volatile unsigned long *)0x71200028))
#define VIC0ADDRESS (*((volatile unsigned long *)0x71200f00))

#define PWMTIMER_BASE (0x7F006000)
#define TCFG0 (*((volatile unsigned long *)(PWMTIMER_BASE + 0x00)))
#define TCFG1 (*((volatile unsigned long *)(PWMTIMER_BASE + 0x04)))
#define TCON (*((volatile unsigned long *)(PWMTIMER_BASE + 0x08)))
#define TCNTB0 (*((volatile unsigned long *)(PWMTIMER_BASE + 0x0C)))
#define TCMPB0 (*((volatile unsigned long *)(PWMTIMER_BASE + 0x10)))
#define TCNTO0 (*((volatile unsigned long *)(PWMTIMER_BASE + 0x14)))
#define TINT_CSTAT (*((volatile unsigned long *)(PWMTIMER_BASE + 0x44)))

// 任何按键按压检测
extern volatile int no_any_press_time;
extern volatile int any_press;

extern volatile int mode;

extern volatile int high_bit_count;
extern volatile int low_bit_count;
extern volatile int need_show;
// 未录取数据
int has_input();
// 延时
void delay_ms(volatile int time);
// 使LEDi亮
void light_led(int i);
// 使LEDi灭
void extinguish_led(int i);
// 闪烁LED一次
void l_and_e_led(int i);
// 初始化定时器
void timer_init(unsigned long utimer, unsigned long uprescaler, unsigned long udivider, unsigned long utcntb, unsigned long utcmpb);
// 定时器中断
void timer_interrupted(int half_second);
// 定时器中断0.5s
void timer_interrupted_500() ;
// 定时器中断1s
void timer_interrupted_1000() ;