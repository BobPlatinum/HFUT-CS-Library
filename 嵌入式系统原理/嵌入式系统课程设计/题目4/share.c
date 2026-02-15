#include "share.h"

volatile int no_any_press_time = 0;
volatile int any_press = 0;
volatile int mode = 0;
volatile int high_bit_count = 0;
volatile int low_bit_count = 0;
volatile int need_show = 1;
// 未录取数据
int has_input() {
    if(high_bit_count != 0 && low_bit_count != 0) {
        return 1;
    } else {
        return 0;
    }
}
// 延时
void delay_ms(volatile int time) {
    volatile int i, j;
    for (i = 0; i < time; i++) {
        for (j = 0; j < 0x7ff; j++) {
        }
    }
}
// 使LEDi亮
void light_led(int i) {
    GPKDATA &= ~(1 << (i + 3)); // 使LEDi（GPKi+3）亮
}
// 使LEDi灭
void extinguish_led(int i) {
    GPKDATA |= (1 << (i + 3)); // 使LEDi（GPKi+3）亮
}
// 闪烁LED一次
void l_and_e_led(int i) {
    light_led(i);
    delay_ms(500);
    extinguish_led(i);
    delay_ms(500);
}
// 初始化定时器
void timer_init(unsigned long utimer, unsigned long uprescaler, unsigned long udivider, unsigned long utcntb, unsigned long utcmpb) {
    unsigned long temp0;

    // 定时器的输入时钟 = PCLK / ( {prescaler value + 1} ) / {divider value} = PCLK/(65+1)/16=62500hz

    // 设置预分频系数为66
    temp0 = TCFG0;
    temp0 = (temp0 & (~(0xff00ff))) | (uprescaler << 0);
    TCFG0 = temp0;

    // 16分频
    temp0 = TCFG1;
    temp0 = (temp0 & (~(0xf << 4 * utimer)) & (~(1 << 20))) | (udivider << 4 * utimer);
    TCFG1 = temp0;

    // 1s = 62500hz
    TCNTB0 = utcntb;
    TCMPB0 = utcmpb;

    // 手动更新
    TCON |= 1 << 1;

    // 清手动更新位
    TCON &= ~(1 << 1);

    // 自动加载和启动timer0
    TCON |= (1 << 0) | (1 << 3);

    // 使能timer0中断
    temp0 = TINT_CSTAT;
    temp0 = (temp0 & (~(1 << utimer))) | (1 << utimer);
    TINT_CSTAT = temp0;
}
// 定时器中断
void timer_interrupted(int half_second) {
    timer_init(0, 65, 4, 62500 / 2 * half_second, 0);
}
// 定时器中断0.5s
void timer_interrupted_500() {
    timer_interrupted(1);
}
// 定时器中断1s
void timer_interrupted_1000() {
    timer_interrupted(2);
}
