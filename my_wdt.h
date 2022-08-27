/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-14     GWVT       the first version
 */
#ifndef APPLICATIONS_MY_WDT_H_
#define APPLICATIONS_MY_WDT_H_

#include <rtthread.h>
#include <rtdbg.h>
#include "rtdevice.h"

#define WDT_DEVICE_NAME    "wdt"    /* 看门狗设备名称 */

void my_wdt_init();
void write_wdt();

#endif /* APPLICATIONS_MY_WDT_H_ */
