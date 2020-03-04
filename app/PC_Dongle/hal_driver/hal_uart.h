/*
 * hal_uart.h
 *
 *  Created on: Feb 26, 2019
 *      Author: lort
 */

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

#include "eloop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/uart.h"

uint8   hal_uart_init(uart_port_t uart);
uint8   hal_uart_write(uart_port_t uart, uint8 *pdata, uint16 len);
uint8   hal_uart_read(uart_port_t uart, uint8 *pdata, uint8 len);

#endif /* APP_GWZB_ESP32_V2_0_HAL_DRIVER_HAL_FACTORY_H_ */
