/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-21     Gwvt       the first version
 */
#ifndef APPLICATIONS_DEVICE_BUZZLEDRELAY_H_
#define APPLICATIONS_DEVICE_BUZZLEDRELAY_H_

#include <rtthread.h>
#include <rtdbg.h>
#include "rtdevice.h"
#include <drv_common.h>

#define PWM_DEV_NAME        "pwm16"
#define PWM_DEV_CHANNEL     1

#define LED_CTRL1   0x0C        //红灯    low = on    high = off      PA12
#define LED_CTRL2   0x0B        //绿灯    low = on    high = off      PA11
#define LED_OFF     PIN_HIGH
#define LED_ON      PIN_LOW

#define RELAY_ON    0x26       //PC06
#define RELAY_OFF   0x02       //PA02

#define POWER_CTRL1 0x01        //阀门控制IO    输出high时为打开阀门1       输出low时为关闭阀门1
#define POWER_CTRL2 0x03        //阀门控制IO    输出high时为打开阀门2       输出low时为关闭阀门2

#define SW_CTRL     0x0f        //按键    松开为high     按下为low

#define HALL_SINGAL1    0x15    //阀门设备标志    阀门设备插入时为high    未插入时未low    下方阀门
#define HALL_SINGAL2    0x14    //阀门设备标志    阀门设备插入时为high    未插入时未low    上方阀门

#define WATER_LOS   0x16        //是否插入标志    插入为low  未插入为high
#define WATER_SINGAL    0x17    //漏水标志  漏水是为low     正常为high

void pwm_sample(void);
void buzz_on();
void buzz_off();

void my_led_init();
void set_led_off();
void set_led_RedOn();
void set_led_GreenOn();

void set_relay_on();
void set_relay_off();

void my_device_init();

#endif /* APPLICATIONS_DEVICE_BUZZLEDRELAY_H_ */
