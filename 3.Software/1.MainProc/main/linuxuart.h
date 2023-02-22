#ifndef __LINUX_UART_H_
#define __LINUX_UART_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <termios.h>
#include <string.h>
#include "data_global.h"

//一切皆文件
//　open   read   write  

int set_com_config(int fd, int baud_rate, int data_bits, char parity, int stop_bits);
int open_port(char *com_port);   // "/dev/ttyUSB0"
void USB_UART_Config(char* path, int baud_rate);

#endif

