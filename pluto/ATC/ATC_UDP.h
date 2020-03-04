/*
 * atc_socket.h
 *
 *  Created on: Jul 22, 2016
 *      Author: lort
 */

#ifndef ATC_SOCKET_H_
#define ATC_SOCKET_H_

#include "eloop.h"
#include "ATC_Config.h"

#define DBG_ATC_SOCKET      0
/****************************************************************
 * port: the udp listen port
 * input_cb : the callback funtion of received message
 * return: 0 is succeed, other's faild
 */
int atc_socket_init(uint16 port);
int atc_socket_deinit(void);
/********************************************************************
 * ip: data to be send target
 * port:data to be send target
 * pdata :the data to be send
 * len : the length of the data to be send
 * return: 0 is succeed, other's faild
 */
int     atc_socket_send(uint32 ip, uint16 port,uint8 *pdata, uint16 len);
uint8   atc_udp_send_command(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len);

#endif /* LSL_ATC_SOCKET_H_ */
