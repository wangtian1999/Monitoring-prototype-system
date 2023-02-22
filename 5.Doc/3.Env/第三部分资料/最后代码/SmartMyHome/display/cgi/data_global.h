#ifndef __DATA_GLOBAL_H__
#define __DATA_GLOBAL_H__

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <errno.h>
#include <strings.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <string.h>


 #define MONITOR_NUM 1
#define QUEUE_MSG_LEN 32

//定义一些设备
#define 	GPRS_DEV 		"/dev/ttyUSB1"
#define 	ZIGBEE_DEV 		"/dev/ttyUSB0"
#define 	BEEPER_DEV 		"/dev/beeper0"
#define 	LED_DEV 		"/dev/led"
#define		ADC_DEV  		"/dev/adc0"
#define		MPU6050_DEV 	"/dev/mpu60500"

unsigned char led_cmb[QUEUE_MSG_LEN];
unsigned char buzzer_cmb[QUEUE_MSG_LEN];
unsigned char fen_cmb[QUEUE_MSG_LEN];
unsigned char temp_cmb[QUEUE_MSG_LEN];
unsigned char gprs_cmb[QUEUE_MSG_LEN];


//全局的结构体声明
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;




//考虑内存对齐的问题
struct makeru_zigbee_info{
	uint8_t head[3];
	uint8_t type;
	float temperature;
	float humidity;
	float tempMIN;
	float tempMAX;
	float humidityMIN;
	float humidityMAX;
	uint32_t reserved[2];
};

struct makeru_a9_info{
	uint8_t head[3];
	uint8_t type;
	float adc;
	short gyrox;
	short gyroy;
	short gyroz;
	short aacx;
	short aacy;
	short aacz;
	uint32_t reserved[2];
};
struct makeru_env_data{
	struct makeru_a9_info a9_info;
	struct makeru_zigbee_info zigbee_info;
	uint32_t reserved[2];
};


//老家还是新家

struct env_info_client_addr
{
	struct makeru_env_data monitor_no[MONITOR_NUM];  
};
	




//全局的外部线程函数声明
extern void*pthread_client_request(void *arg);   //接受CGI等请求
extern void*pthread_refresh(void *arg);						//刷新共享内存数据线程
extern void*pthread_sqlite(void *arg);                      //数据库线程，保存数据
extern void* pthread_led(void *arg);                         //led灯线程控制
extern void* pthread_gprs(void *arg);						//蜂鸣器控制线程
extern void *  pthread_buzzer(void *arg);		//发送短信线程
extern void * pthread_transfer(void * arg);
extern void* pthread_fen(void *arg);                         //风扇线程控制


extern int send_msg_queue(long type, unsigned char text);


//全局消息队列的结构体声明
struct msg
{
	long type;
	long msgtype;
	unsigned char text[QUEUE_MSG_LEN];  //消息正文
										/* 01-------LED
										   02-------buzzer
										   03-------fen*/
};

#endif



