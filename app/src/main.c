/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <misc/printk.h>
#include <shell/shell.h>
#include <shell/shell_uart.h>
#include <version.h>
#include <logging/log.h>
#include <stdlib.h>
#include <device.h>
#include <gpio.h>
#include <pwm.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <pinmux.h>
#include <sensor.h>
#include <stdio.h>
#include "../boards/x86/galileo/board.h"
#include "../boards/x86/galileo/pinmux_galileo.h"
#include "../drivers/gpio/gpio_dw_registers.h"
#include "../drivers/gpio/gpio_dw.h"

LOG_MODULE_REGISTER(app);

#define EDGE_FALLING    (GPIO_INT_EDGE | GPIO_INT_ACTIVE_LOW)
#define EDGE_RISING	(GPIO_INT_EDGE | GPIO_INT_ACTIVE_HIGH)
static struct device *pinmux , *gpiob; 
static struct gpio_callback gpio_cb;

static int cmd_version(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(shell, "Zephyr version %s", KERNEL_VERSION_STRING);

	return 0;
}

static int cmd_measure_0(const struct shell *shell, size_t argc, char **argv)
{
		pinmux=device_get_binding(CONFIG_PINMUX_NAME);
		struct galileo_data *dev = pinmux->driver_data;
	
    	gpiob=dev->gpio_dw; 	//retrieving gpio_dw driver struct from pinmux driver
				// Alternatively, gpiob = device_get_binding(PORT);
	if (!gpiob) {
		printk("error\n");
		return;
	}
	int ret;

	ret = pinmux_pin_set(pinmux,1,PINMUX_FUNC_A); 	//IO1 or zephyr GPIO4 as output
	if(ret < 0)
		printk("error setting pin for IO1\n");

	ret = pinmux_pin_set(pinmux,2,PINMUX_FUNC_B); 	//IO2 or zephyr GPIO5 as input
	if(ret < 0)
		printk("error setting pin for IO2\n");
	
	//Configure interrupt
	ret = gpio_pin_configure(gpiob, 5, GPIO_DIR_IN | GPIO_INT | EDGE_RISING);
	if(ret < 0) {
		printk("error setting pin for IO2\n");
		return ret;
	}

	ret = gpio_add_callback(gpiob, &gpio_cb);
	if(ret < 0) {
		printk("error in gpio_add_callback %d\n", ret);
		return ret;
	}
	ret = gpio_pin_enable_callback(gpiob, 5);
	if(ret < 0) {
		printk("error in gpio_pin_enable_callback %d\n", ret);
		return ret;
	}
	printk("Init GPIO done\n");

	//Selecting the device
	int device=atoi(argv[1]);
	//if(device==0)


	return 0;
	
}



SHELL_STATIC_SUBCMD_SET_CREATE(sub_measure,
	SHELL_CMD(0, NULL, "Select HCSR", cmd_measure_0),
	SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_REGISTER(measure, &sub_measure, "HCSR commands", NULL);

SHELL_CMD_ARG_REGISTER(version, NULL, "Show kernel version", cmd_version, 1, 0);

void main(void)
{

}
