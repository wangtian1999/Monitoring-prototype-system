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
#define 	BEEPER_DEV 		"/dev/buzzer0"
#define     ZIGBEE_DEV      "/dev/ttyUSB0"
#define		ADC_DEV  		"/dev/adc0"
#define		MPU6050_DEV 	"/dev/mpu6050"

unsigned char tube_cmb[QUEUE_MSG_LEN];
unsigned char buzzer_cmb[QUEUE_MSG_LEN];
unsigned char fan_cmb[QUEUE_MSG_LEN];
unsigned char temp_cmb[QUEUE_MSG_LEN];
unsigned char cloud_cmb[QUEUE_MSG_LEN];


//全局的结构体声明
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;




//考虑内存对齐的问题
struct makeru_zigbee_info{
	uint8_t head[3];
	uint8_t type;
	char temperature[4];
	char humidity[4];
	//float tempMIN;
	//float tempMAX;
	//float humidityMIN;
	//float humidityMAX;
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
extern void  * pthread_client_request(void *arg);   			//显示端web接收线程
extern void  * pthread_refresh(void *arg);						//刷新共享内存数据线程
extern void  * pthread_sqlite(void *arg);                      //数据库线程，保存数据
extern void  * pthread_tube(void *arg);                       //数码管线程控制
extern void  * pthread_cloud(void *arg);						//云线程
extern void  * pthread_buzzer(void *arg);					//蜂鸣器报警线程
extern void  * pthread_process(void * arg);                  //环境数据获取线程
extern void  * pthread_fan(void *arg);                        //风扇线程控制


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



