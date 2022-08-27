/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-14     GWVT       the first version
 */

#include "my_wdt.h"

static rt_device_t wdg_dev;         /* 看门狗设备句柄 */
static rt_uint32_t timeout = 6;

void my_wdt_init(){
    rt_err_t ret = RT_EOK;
    wdg_dev = rt_device_find(WDT_DEVICE_NAME);
    if (!wdg_dev){
        rt_kprintf("find %s failed!\n", WDT_DEVICE_NAME);
        return;
    }
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    if (ret != RT_EOK)
    {
        rt_kprintf("set %s timeout failed!\n", WDT_DEVICE_NAME);
        return;
    }
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL);
    if (ret != RT_EOK)
    {
        rt_kprintf("start %s failed!\n", WDT_DEVICE_NAME);
        return;
    }
}

void write_wdt(){
    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
}
