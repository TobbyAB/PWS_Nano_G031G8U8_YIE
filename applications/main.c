/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-16     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include "Device_BuzzLedRelay.h"
#include "my_rtc_init.h"

#define IWDG_DEVICE_NAME    "wdt"

static rt_device_t wdg_dev;

static uint8_t powerstop_flag = 0;

static uint8_t powerstop_test = 0;
static uint8_t powerstop_test_time = 0;

static uint8_t powerstop_test_zs = 0;

static uint8_t powerstop_err  = 0;
static uint8_t powerstop_err_value = 0;
static uint8_t powerstop_err_time = 0;

static uint8_t out_err;
static uint8_t duanlu_err;
static uint8_t test_err;

static uint8_t powerstop_SelfTest_time = 0;

static uint8_t test_wdt = 0;

static rt_uint32_t timeout = 8;    /* 溢出时间 */
static uint8_t wdt_number = 0;

void powerstop_flag_handle(){
    if(powerstop_flag == 0){
        if(rt_pin_read(WATER_LOS) == PIN_LOW){
            powerstop_flag = 1;
//            if(powerstop_err == 1)powerstop_err = 0;
            out_err = 0;
            if(rt_pin_read(WATER_SINGAL) == PIN_LOW && duanlu_err == 0){
                powerstop_test = 0;
                duanlu_err = 1;
                rt_pin_write(POWER_CTRL1, PIN_LOW);
                rt_pin_write(POWER_CTRL2, PIN_LOW);
                set_relay_off();
            }
        }else {
//            if(powerstop_err == 0)powerstop_err = 1;
            out_err = 1;
        }
        if(rt_pin_read(SW_CTRL) == PIN_LOW){
            if(powerstop_test == 0 && (rt_pin_read(HALL_SINGAL1) == PIN_HIGH || rt_pin_read(HALL_SINGAL2) == PIN_HIGH || duanlu_err == 1) && powerstop_test_zs == 0){
                powerstop_test = 1;
                powerstop_test_zs = 1;
            }
        }
    }else if(powerstop_flag == 1){
        if(rt_pin_read(WATER_LOS) == PIN_HIGH){
            powerstop_flag = 0;
        }
        if(rt_pin_read(WATER_SINGAL) == PIN_LOW && duanlu_err == 0){
            powerstop_test = 0;
            duanlu_err = 1;
            rt_pin_write(POWER_CTRL1, PIN_LOW);
            rt_pin_write(POWER_CTRL2, PIN_LOW);
            set_relay_off();
        }
        if(rt_pin_read(SW_CTRL) == PIN_LOW){
            if(rt_pin_read(WATER_SINGAL) == PIN_HIGH){
                if(powerstop_test == 0 && powerstop_test_zs == 0){
                    if(rt_pin_read(HALL_SINGAL1) == PIN_HIGH || rt_pin_read(HALL_SINGAL2) == PIN_HIGH || duanlu_err == 1)
                    powerstop_test = 1;
                    powerstop_test_zs = 1;
                }
            }
        }
    }
    if(rt_pin_read(SW_CTRL) == PIN_HIGH)powerstop_test_zs = 0;
}
void powerstop_error_handle(){
    if(out_err == 1 && powerstop_err_value == 0){
        powerstop_err_value = 1;
        powerstop_err_time = 0;
    }else if(duanlu_err == 1 && powerstop_err_value < 2){
        powerstop_err_value = 2;
        powerstop_err_time = 0;
    }else if(test_err == 1 && powerstop_err_value < 3){
        powerstop_err_value = 3;
        powerstop_err_time = 0;
    }
    if(out_err == 0 && duanlu_err == 0 && test_err == 0){
        if(powerstop_test == 0)set_led_GreenOn();
        buzz_off();
        powerstop_err_time = 0;
    }else if(test_err == 1){
        powerstop_err_time %= 100;
        if(powerstop_err_time < 24){
            if((powerstop_err_time % 4) >= 2){
                buzz_on();
                if(powerstop_test == 0)set_led_RedOn();
            }else {
                buzz_off();
                if(powerstop_test == 0)set_led_off();
            }
        }else {
            buzz_off();
            if(powerstop_test == 0)set_led_off();
        }
    }else if(duanlu_err == 1){
        powerstop_err_time %= 50;
        if(powerstop_err_time < 12){
            if((powerstop_err_time % 4) >= 2){
                buzz_on();
                if(powerstop_test == 0)set_led_RedOn();
            }else {
                buzz_off();
                if(powerstop_test == 0)set_led_off();
            }
        }else {
            buzz_off();
            if(powerstop_test == 0)set_led_off();
        }
    }else if(out_err == 1){
        powerstop_err_time %= 50;
        if(powerstop_err_time < 8){
            if((powerstop_err_time % 4) >= 2){
                buzz_on();
                if(powerstop_test == 0)set_led_RedOn();
            }else {
                buzz_off();
                if(powerstop_test == 0)set_led_off();
            }
        }else {
            buzz_off();
            if(powerstop_test == 0)set_led_off();
        }
    }
}

void powerstop_test_handle(){
    static uint8_t valve1,valve2;
    static uint8_t valve1_test_value,valve2_test_value;
    if(powerstop_test){ 
        switch(powerstop_test){
            case 1:
                if(rt_pin_read(HALL_SINGAL1) == PIN_HIGH)valve1 = 1;else valve1 = 0;
                if(rt_pin_read(HALL_SINGAL2) == PIN_HIGH)valve2 = 1;else valve2 = 0;
                valve1_test_value = 0;
                valve2_test_value = 0;
                powerstop_test_time = 0;
                if(duanlu_err == 1 && test_err == 0){
                    duanlu_err = 0;
                    powerstop_flag = 0;
                    powerstop_test = 0;
                    buzz_off();
                    set_led_GreenOn();
                    rt_pin_write(POWER_CTRL1, PIN_HIGH);        //开阀�?1
                    rt_pin_write(POWER_CTRL2, PIN_HIGH);        //开阀�?2
                    set_relay_on();
                }else {
                    set_led_RedOn();
                    if(duanlu_err == 0)powerstop_test = 2;
                    else powerstop_test = 3;
                }
                break;
            case 2:
                if(valve1){
                    rt_pin_write(POWER_CTRL1, PIN_LOW);
                    if(rt_pin_read(HALL_SINGAL1) == PIN_LOW)valve1_test_value++;
                }
                if(valve2){
                    rt_pin_write(POWER_CTRL2, PIN_LOW);
                    if(rt_pin_read(HALL_SINGAL2) == PIN_LOW)valve2_test_value++;
                }
                if((valve1 == 0 || valve1_test_value >= 10) && (valve2 == 0 || valve2_test_value >= 10)){
                    powerstop_test = 3;
                    valve1_test_value = 0;
                    valve2_test_value = 0;
                    powerstop_test_time = 0;
                }
                if(powerstop_test_time >= 90){
                    test_err = 1;
                    test_wdt = 1;
                    powerstop_test = 0;
                    rt_pin_write(POWER_CTRL1, PIN_HIGH);        //开阀�?1
                    rt_pin_write(POWER_CTRL2, PIN_HIGH);        //开阀�?2
                }
                break;
            case 3:
                rt_pin_write(POWER_CTRL1, PIN_HIGH);        //开阀�?1
                rt_pin_write(POWER_CTRL2, PIN_HIGH);        //开阀�?2
                if(valve1){
                    if(rt_pin_read(HALL_SINGAL1) == PIN_HIGH)valve1_test_value++;
                }
                if(valve2){
                    if(rt_pin_read(HALL_SINGAL2) == PIN_HIGH)valve2_test_value++;
                }
                if((valve1 == 0 || valve1_test_value >= 5) && (valve2 == 0 || valve2_test_value >= 5)){
                    test_err = 0;
                    powerstop_flag = 0;
                    powerstop_test = 0;
                }
                if(powerstop_test_time >= 90){
                    test_err = 1;
                    test_wdt = 1;
                    powerstop_test = 0;
                }
                break;
        }
    }
}

void powerstop_self_test(){
    time_t now;
    uint8_t date;
    if(powerstop_SelfTest_time == 0){
        now = time(RT_NULL);
        date = (uint8_t)localtime(&now)->tm_mday;
        if(date > 5){                                       //data大于几就�?几天
            rt_thread_mdelay(200);
            set_date(2022, 1, 1);
//            rt_thread_mdelay(200);
//            set_time(23, 59, 0);
            if(powerstop_flag == 0){
                if(powerstop_test == 0 && (rt_pin_read(HALL_SINGAL1) == PIN_HIGH || rt_pin_read(HALL_SINGAL2) == PIN_HIGH)){
                    powerstop_test = 1;
                }
            }else {
                if(rt_pin_read(WATER_SINGAL) == PIN_HIGH){
                    if(powerstop_test == 0 && (rt_pin_read(HALL_SINGAL1) == PIN_HIGH || rt_pin_read(HALL_SINGAL2) == PIN_HIGH)){
                        powerstop_test = 1;
                    }
                }
            }
        }
    }
}

void powerstop_thread(){

    powerstop_err_time++;
    powerstop_err_time %= 100;
    powerstop_test_time++;
    powerstop_test_time %= 120;
    powerstop_SelfTest_time++;
    powerstop_SelfTest_time%=5;

    powerstop_flag_handle();
    powerstop_error_handle();
    powerstop_test_handle();
    powerstop_self_test();              //RTC�?测函�?



    rt_thread_mdelay(100);
}
int main(void)
{
    rt_err_t ret = RT_EOK;
        

    rt_thread_mdelay(500);
    my_device_init();
    MX_RTC_Init();

    wdg_dev = rt_device_find(IWDG_DEVICE_NAME);
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL);
    while (1)
    { 
        powerstop_thread();
        rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
    }

    return RT_EOK;
}
