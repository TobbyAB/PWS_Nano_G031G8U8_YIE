/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-21     Gwvt       the first version
 */

#include "Device_BuzzLedRelay.h"

struct rt_device_pwm *pwm_dev;
TIM_HandleTypeDef htim16;
static rt_uint32_t period = 370370;

void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 63;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 370;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 184;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */
  HAL_TIM_MspPostInit(&htim16);

}

void pwm_sample(void){
    MX_TIM16_Init();
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if(pwm_dev == RT_NULL){
        rt_kprintf("1\n");
        return;
    }
    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, period/2);
}
void buzz_on(){
    period = 370370;
    rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
}
void buzz_off(){
    rt_pwm_disable(pwm_dev,PWM_DEV_CHANNEL);
}

void my_led_init(){
    rt_pin_mode(LED_CTRL1, PIN_MODE_OUTPUT);
    rt_pin_write(LED_CTRL1, PIN_HIGH);

    rt_pin_mode(LED_CTRL2, PIN_MODE_OUTPUT);
    rt_pin_write(LED_CTRL2, PIN_HIGH);
}

void set_led_off(){
    rt_pin_write(LED_CTRL1, LED_OFF);
    rt_pin_write(LED_CTRL2, LED_OFF);
}
void set_led_RedOn(){
    rt_pin_write(LED_CTRL1, LED_ON);
    rt_pin_write(LED_CTRL2, LED_OFF);
}
void set_led_GreenOn(){
    rt_pin_write(LED_CTRL1, LED_OFF);
    rt_pin_write(LED_CTRL2, LED_ON);
}

void set_relay_on(){
    rt_pin_write(RELAY_ON, PIN_HIGH);
    rt_thread_mdelay(200);
    rt_pin_write(RELAY_ON, PIN_LOW);
}
void set_relay_off(){
    rt_pin_write(RELAY_OFF, PIN_HIGH);
    rt_thread_mdelay(200);
    rt_pin_write(RELAY_OFF, PIN_LOW);
}

void my_device_init(){
    pwm_sample();

    my_led_init();

    rt_pin_mode(RELAY_ON, PIN_MODE_OUTPUT);     //继电器开脉冲
    rt_pin_mode(RELAY_OFF, PIN_MODE_OUTPUT);    //继电器关脉冲

    rt_pin_mode(POWER_CTRL1, PIN_MODE_OUTPUT);  //设备1关
    rt_pin_mode(POWER_CTRL2, PIN_MODE_OUTPUT);  //设备2关

    rt_pin_mode(SW_CTRL, PIN_MODE_INPUT);       //按键输入

    rt_pin_mode(HALL_SINGAL1, PIN_MODE_INPUT);  //设备1状态霍尔
    rt_pin_mode(HALL_SINGAL2, PIN_MODE_INPUT);  //设备2状态霍尔

    rt_pin_mode(WATER_LOS, PIN_MODE_INPUT);     //是否插入标志
    rt_pin_mode(WATER_SINGAL, PIN_MODE_INPUT);  //漏水标志

    rt_pin_write(POWER_CTRL1, PIN_HIGH);        //开阀门1
    rt_pin_write(POWER_CTRL2, PIN_HIGH);        //开阀门2

    if(rt_pin_read(WATER_SINGAL) == PIN_HIGH)set_relay_on();
}

