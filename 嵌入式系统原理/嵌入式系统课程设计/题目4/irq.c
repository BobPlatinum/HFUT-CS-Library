#include "share.h"

typedef void(isr)(void);
extern void asm_timer_irq();
extern void asm_k1_irq();

// K4长短按识别
volatile int key4_is_pressed = 0;
volatile int press_time = 0;
// 初始化中断
void irq_init(void) {
    /* 配置GPN0~4引脚为中断功能 */
    GPNCON &= ~(0xff);
    GPNCON |= 0xaa; // 10101010

    /* 设置中断触发方式 */
    EINT0CON0 &= ~(0xff);
    EINT0CON0 |= 0x33; // 77

    /* 禁止屏蔽中断 */
    EINT0MASK &= ~(0x0f);

    // Select INT_EINT0 mode as irq
    VIC0INTSELECT = 0;

    /* 在中断控制器里使能这些中断 */
    VIC0INTENABLE |= (0x3); /* bit0: eint0~3, bit1: eint4~11 */

    isr **isr_array_0 = (isr **)(0x71200100);
    isr_array_0[0] = (isr *)asm_k1_irq;

    /* 在中断控制器里使能timer0中断 */
    VIC0INTENABLE |= (1 << 23);

    // VIC0INTSELECT =0;
    isr **isr_array_1 = (isr **)(0x7120015C);
    isr_array_1[0] = (isr *)asm_timer_irq;

    /* 将GPK4-GPK7配置为输出口 */
    GPKCON0 = 0x11110000;

    /* 熄灭四个LED灯 */
    GPKDATA = 0xf0;
}
// 当前周几
int get_current_weekday() {
    return 4;
}
// 使LED全灭
void extinguish_all_led() {
    GPKDATA = 0xf0;
}
// 使LED全亮
void light_all_led() {
    GPKDATA &= ~0xf0;
}
// 跑马灯一次
void run_led_marquee() {
    int i;
    for (i = 0; i < 4; i++)
    {
        GPKDATA &= ~(1 << (i + 4)); // 点亮LED
        delay_ms(500);
        GPKDATA |= (1 << (i + 4)); // 熄灭LED
        delay_ms(500);
    }
}
// 闪烁所有LED一次
void l_and_e_all_led() {
    light_all_led();
    delay_ms(500);
    extinguish_all_led();
    delay_ms(500);
}
// 间隔鸣叫一次
void septal_buzzer() {
    buzzer_on();
    delay_ms(1000);
    buzzer_off();
    delay_ms(1000);
}
// 一长一短鸣叫一次
void long_short_buzzer() {
    buzzer_on();
    delay_ms(1000); // 长鸣 1 秒
    buzzer_off();
    delay_ms(200); // 间隔 200 毫秒

    buzzer_on();
    delay_ms(200); // 短鸣 0.2 秒
    buzzer_off();
}
// 同时鸣叫并且闪烁所有LED
void sync_buzzer_all_led() {
    light_all_led();
    long_short_buzzer();
    extinguish_all_led();
    delay_ms(500);
}
// 同时跑马灯并鸣叫
void sync_buzzer_run_led() {
    buzzer_on();
    run_led_marquee();
    buzzer_off();
    delay_ms(1000);
}
// 转换为十进制数
int toDec() {
    int hex = (high_bit_count << 4) | low_bit_count; // 合成 8 位十六进制数
    return hex;                  // 返回十进制值
}
// 取余
unsigned int mod(unsigned int a, unsigned int b) {
    while (a >= b)
        a -= b;
    return a;
}
// 取十进制个位
int get_ones_digit(int num) {
    return mod(num, 10);
}
// 取十进制十位
int get_tens_digit(int num) {
    int mod100 = mod(num, 100); // 得到最后两位
    int digit = 0;
    while (mod100 >= 10) {
        mod100 -= 10;
        digit++;
    }
    return digit;
}
// 复原
void init_state() {
    light_led(1);
    // 计数器复位
    no_any_press_time = 0;
    press_time = 0;
    need_show = 1;
    // 数字复位
    high_bit_count = 0;
    low_bit_count = 0;
    // 熄灯灭蜂鸣
    extinguish_all_led();
    buzzer_off();
}
// 重启定时器
void restart_timer() {
    TCON &= ~(0 << 0);
}
// 停止定时器
void stop_timer() {
    TCON &= ~(1 << 0);
}
// 恢复中断配置为初始状态
void restore_irq() {
    EINT0CON0 &= ~(0xff);
    EINT0CON0 |= 0x33; // 77
}
// 按下了任意一个按钮重置时间与状态
void press() {
    any_press = 1;
    no_any_press_time = 0;
}
// 按键中断处理程序
void do_irq_1(void) {
    any_press = 0;

    // 按下K1
    if (EINT0PEND & (1 << 0)) {
        delay_ms(20); // 短暂去抖
        press();
        //  同时按下K2
        if (EINT0PEND & (1 << 1)) {
            // 高位A~F的输入
            if(high_bit_count == 0) { // 第一次按
                high_bit_count += 10;
                l_and_e_led(2);
            } else if (high_bit_count >= 10 && high_bit_count < 16) {
                high_bit_count++;
                l_and_e_led(2);
            }
        }
        // 同时按下K3
        else if (EINT0PEND & (1 << 2)) {
            // 高位A~F的输入
            if(low_bit_count == 0) { // 第一次按
                low_bit_count += 10;
                l_and_e_led(3);
            } else if (low_bit_count >= 10 && low_bit_count < 16) {
                low_bit_count++;
                l_and_e_led(3);
            }
        }
    }
    // 按下 K2
    else if (EINT0PEND & (1 << 1)) {
        // 录入高位1~9
        delay_ms(20); // 短暂去抖
        press();
        if (high_bit_count < 10) {
            high_bit_count++;
            l_and_e_led(2);
        }
    }
    // 按下 K3
    else if (EINT0PEND & (1 << 2)) {
        // 录入低位1~9
        delay_ms(20); // 短暂去抖
        press();
        if (low_bit_count < 10) {
            low_bit_count++;
            l_and_e_led(3);
        }
    }
    // 按下K4
    else if ((EINT0PEND & (1 << 3))) {
        need_show = 0;
        press();
        EINT0CON0 |= 0x77; // 设置K4为边沿触发方式
        if ((GPNDAT & (1 << 3)) == 0) {
            // 下降沿（按下）
            key4_is_pressed = 1;
            press_time = 0;
            mode = 0;
            timer_interrupted_1000(); // 开始1ms周期的计时器
        } else {
            // 上升沿（松开）
            // 关掉定时器
            stop_timer();
            key4_is_pressed = 0;
            restart_timer();

            if (press_time >= 3000) { // 大于3秒是长按
                // DEBUG
                l_and_e_led(1);
                mode = 4;
            } else {
                restore_irq();
                int dec = toDec();
                int value = get_ones_digit(dec);
                if (value >= get_current_weekday()) {
                    mode = 2;
                } else {
                    mode = 3;
                }
            }
            timer_interrupted_1000();
        }
    }
    
    
    /* 清中断 */
    EINT0CON0 |= 0x33;
    EINT0PEND = 0x3f;
    VIC0ADDRESS = 0;
}
// 时钟中断处理程序
void do_irq_2(void) {
    // K4计时器 识别长短按
    if (key4_is_pressed) {
        light_led(3);
        light_led(4);
        delay_ms(500);
        extinguish_led(3);
        extinguish_led(4);
        delay_ms(500);

        press_time += 1000; // 计时器每1s触发一次
    }
    
    int i;
    // 四个LED按1s的间隔闪烁该16进制数的高位对应的次数
    if (mode == 1) {
        need_show = 0;
        i = high_bit_count;
        while (i != 0)
        {
            l_and_e_all_led();
            i--;
        }
        no_any_press_time = 0;
        mode = 0;
    }
    // 四个LED按1s的间隔持续闪烁 十位次
    else if (mode == 2) {
        int dec = toDec();
        i = get_tens_digit(dec);
        while (i != 0)
        {
            sync_buzzer_all_led();
            i--;
        }
        mode = 0;
    }
    // 蜂鸣器按0.5s的间隔鸣叫比较值（当前星期）对应的次数，同时四个LED按相同的频率从1开始按单向跑马灯显示，并循环显示比较值（当前星期）对应的次数后熄灭
    else if (mode == 3) {
        i = get_current_weekday();
        while (i != 0)
        {
            sync_buzzer_run_led();
            i--;
        }
        mode = 0;
    }
    // K4长按，复原初态
    else if (mode == 4) {
        init_state();
        mode = 0;
    }

    unsigned long uTmp;
    // 清timer0的中断状态寄存器
    uTmp = TINT_CSTAT;
    TINT_CSTAT = uTmp;
    VIC0ADDRESS = 0x0;
}
