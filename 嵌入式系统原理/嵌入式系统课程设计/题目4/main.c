#include "share.h"


int main()
{
	buzzer_init();


	while (1) {
 		if (mode == 0 && has_input() == 1 && need_show == 1) {
            delay_ms(20);
            no_any_press_time += 100;
            l_and_e_led(1);
            // 按键按每次调用press()，用于清零no_any_press_time重新计数
            // 如果5s内没有按下任何按键
            if (no_any_press_time >= 500) {
                // DEBUG
                mode = 1;
                timer_interrupted_1000();
            }
        } else {
             if(need_show == 1) {
                 l_and_e_led(4);
             }
            no_any_press_time = 0;
        }

	}
	
	return 0;
}
