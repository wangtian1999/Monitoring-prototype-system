
#include "data_global.h"
#include "common.h"
#include "mpu6050.h"
#include <strings.h>
#include "linuxuart.h"
#include "sqlite3.h"
#include <time.h>

#define DATABASE "my.db"

//接收ZigBee的数据和采集的A9平台的传感器数据
int adc_fd;
int mpu_fd;
int usb_fd;

extern pthread_cond_t cond_transfer;
extern pthread_mutex_t mutex_transfer;
extern struct env_info_client_addr  sm_all_env_info;
extern int card;

int file_env_info_a9_zigbee_debug(struct env_info_client_addr *rt_status,int home_id);
int file_env_info_a9_zigbee_stm32(struct env_info_client_addr *rt_status,int home_id);

int printf_sensor_info_debug(struct env_info_client_addr  *sm_all_env_info,int home_id);

void *pthread_transfer(void *arg)
{

	//打开设备文件
	int home_id = 1;
	char oldbuf[11];
	adc_fd = open(ADC_DEV,O_RDWR);
	mpu_fd = open(MPU6050_DEV,O_RDWR);
	usb_fd = open_port("/dev/ttyUSB1");
	if((adc_fd == -1) || (mpu_fd == -1) || (usb_fd == -1)){
		printf("open adc or mpu device failed.\n");
	}
	set_com_config(usb_fd,115200,8,'N',1);

	sleep(10);
	read(usb_fd,&oldbuf,sizeof(oldbuf));
	usleep(500000);
	printf("oldbuf ===== %s\n",oldbuf);
	while(1){
		pthread_mutex_lock(&mutex_transfer);
		pthread_cond_wait(&cond_transfer,&mutex_transfer);
//		printf("pthread_analysis and tranfer.\n");
#if 1
		file_env_info_a9_zigbee_stm32(&sm_all_env_info,home_id);
#else	
		file_env_info_a9_zigbee_debug(&sm_all_env_info,home_id);
#endif
		pthread_mutex_unlock(&mutex_transfer);
		sleep(1);
	}
	close(adc_fd);
	close(mpu_fd);
	close(usb_fd);
}




int file_env_info_a9_zigbee_debug(struct env_info_client_addr *rt_status,int home_id)
{
	static int temp = 0;
	int  env_info_size = sizeof(struct env_info_client_addr);
	//	printf("env_info_size = %d.\n",env_info_size);
	
	rt_status->monitor_no[home_id].zigbee_info.temperature = 10.0;
	rt_status->monitor_no[home_id].zigbee_info.tempMIN = 2.0;
	rt_status->monitor_no[home_id].zigbee_info.tempMAX = 20.0;
	rt_status->monitor_no[home_id].zigbee_info.humidity  = 20.0;
	rt_status->monitor_no[home_id].zigbee_info.humidityMIN  = 10.0;
	rt_status->monitor_no[home_id].zigbee_info.humidityMAX  = 30.0;
	rt_status->monitor_no[home_id].zigbee_info.reserved[0]  = 0.01;
	rt_status->monitor_no[home_id].zigbee_info.reserved[1]  = -0.01;

	temp ++;
	rt_status->monitor_no[home_id].a9_info.adc  = temp;
	rt_status->monitor_no[home_id].a9_info.gyrox  = -14.0;
	rt_status->monitor_no[home_id].a9_info.gyroy  = 20.0;
	rt_status->monitor_no[home_id].a9_info.gyroz  = 40.0;
	rt_status->monitor_no[home_id].a9_info.aacx  = 642.0;
	rt_status->monitor_no[home_id].a9_info.aacy  = -34.0;
	rt_status->monitor_no[home_id].a9_info.aacz  = 5002.0;
	rt_status->monitor_no[home_id].a9_info.reserved[0]  = 0.01;
	rt_status->monitor_no[home_id].a9_info.reserved[1]  = -0.01;
		
//	printf_sensor_info_debug(rt_status,home_id);

	//添加stm32部分的数据、arduino数据，
	
	return 0;
}


#if 0
int	get_sensor_data_from_a9(struct makeru_a9_info* a9_sensor_data)
{
	int adc_sensor_data;
	struct mpu6050_data data;

	/*get adc sensor data*/
	read(adc_fd,&adc_sensor_data,4); 
	printf("adc value :%0.2fV.\n",(1.8*adc_sensor_data)/4096);  

	/* get mpu6050 sensor data*/
	ioctl(mpu_fd,MPU6050_GYRO,&data);
	printf("gyro data: x = %05d, y = %05d, z = %05d\n", data.gyro.x,data.gyro.y,data.gyro.z);
	ioctl(mpu_fd,MPU6050_ACCEL,&data);
	printf("accel data: x = %05d, y = %05d, z = %05d\n", data.accel.x,data.accel.y,data.accel.z);

	/*预填充，有点浪费空间，大家可以优化一下*/
	a9_sensor_data->adc = (1.8 * adc_sensor_data)/4096 * 100; //定义未int32,应该是float,放大100倍，保护小数位
	a9_sensor_data->gyrox = data.gyro.x;
	a9_sensor_data->gyroy = data.gyro.y;
	a9_sensor_data->gyroz = data.gyro.z;
	a9_sensor_data->aacx  = data.accel.x;
	a9_sensor_data->aacy  = data.accel.y;
	a9_sensor_data->aacz  = data.accel.z;

	return 0;
}
#endif 


int file_env_info_a9_zigbee_stm32(struct env_info_client_addr *rt_status,int home_id)
{
	//set time
	char buftime[128] = {};
	time_t t;
	struct tm *tp;
	char sql[128] = {};
	
	int  env_info_size = sizeof(struct env_info_client_addr);
//	printf("env_info_size = %d.\n",env_info_size);

	char buf[8];
	write(usb_fd,"22",2);
	usleep(500000);
	read(usb_fd,&buf,sizeof(buf));
	usleep(500000);
	printf("buf ===== %s\n",buf);

	int a,b,c,d,e,f;
	a = buf[0]-48;
	b = buf[1]-48;
	c = a*10 + b;
	d = buf[4]-48;
	e = buf[5]-48;
	f = d*10 + e;

	rt_status->monitor_no[home_id].zigbee_info.head[0]  = 'm';
	rt_status->monitor_no[home_id].zigbee_info.head[1]  = 's';
	rt_status->monitor_no[home_id].zigbee_info.head[2]  = 'm';
	rt_status->monitor_no[home_id].zigbee_info.head[3]  = 'z';
	rt_status->monitor_no[home_id].zigbee_info.temperature = c;
	rt_status->monitor_no[home_id].zigbee_info.tempMIN = 2.0;
	rt_status->monitor_no[home_id].zigbee_info.tempMAX = 20.0;
	rt_status->monitor_no[home_id].zigbee_info.humidity  = f;
	rt_status->monitor_no[home_id].zigbee_info.humidityMIN  = 10.0;
	rt_status->monitor_no[home_id].zigbee_info.humidityMAX  = 30.0;
	rt_status->monitor_no[home_id].zigbee_info.reserved[0]  = 0.01;
	rt_status->monitor_no[home_id].zigbee_info.reserved[1]  = -0.01;

	
	//获取数据     
	int adc_sensor_data;
	struct mpu6050_data data;
	/*get adc sensor data*/
	read(adc_fd,&adc_sensor_data,4); /*遗留问题解决：忘记加载驱动了，同志们要注意了......*/
//	printf("adc value :%0.2fV.\n",(1.8*adc_sensor_data)/4096);  
	rt_status->monitor_no[home_id].a9_info.adc    = (float)((1.8*adc_sensor_data)/4096);
	
	/* get mpu6050 sensor data*/
	ioctl(mpu_fd,MPU6050_GYRO,&data);
//	printf("gyro data: x = %d, y = %d, z = %d\n", data.gyro.x,data.gyro.y,data.gyro.z);
	ioctl(mpu_fd,MPU6050_ACCEL,&data);
//	printf("accel data: x = %d, y = %d, z = %d\n", data.accel.x,data.accel.y,data.accel.z);
	
	rt_status->monitor_no[home_id].a9_info.head[0]  = 'm';
	rt_status->monitor_no[home_id].a9_info.head[1]  = 's';
	rt_status->monitor_no[home_id].a9_info.head[2]  = 'm';
	rt_status->monitor_no[home_id].a9_info.head[3]  = 'a';

	rt_status->monitor_no[home_id].a9_info.gyrox  =  (short)data.gyro.x;
	rt_status->monitor_no[home_id].a9_info.gyroy  =  (short)data.gyro.y;
	rt_status->monitor_no[home_id].a9_info.gyroz  =  (short)data.gyro.z;

	rt_status->monitor_no[home_id].a9_info.aacx   =  (short)data.accel.x;
	rt_status->monitor_no[home_id].a9_info.aacy   =  (short)data.accel.y;
	rt_status->monitor_no[home_id].a9_info.aacz   =  (short)data.accel.z;
	rt_status->monitor_no[home_id].a9_info.reserved[0]  = card;
	rt_status->monitor_no[home_id].a9_info.reserved[1]  = -0.01;
		printf("card = %d\n",card);

	//打开数据库
	sqlite3 *db;
	char *errmsg;
	if(sqlite3_open(DATABASE, &db) != SQLITE_OK)
	{
		//printf("%s\n", sqlite3_errmsg(db));
		return -1;
	}
	else
	{
		//printf("opsne DATABASE success!\n");
	}

	if(sqlite3_exec(db, "create table env2 (temp Integer, humi Integer, time char);", NULL, NULL, &errmsg) != SQLITE_OK)
	{
		//printf("%s\n", errmsg);
	}
	else
	{
		//printf("Creat table or Open success\n");
	}


	time(&t);
	tp =  localtime(&t);
	sprintf(buftime,"%d-%02d-%02d %02d:%02d:%02d\n",
				tp->tm_year+1900, tp->tm_mon+1,tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);

	//printf("%s",buftime);

	sprintf(sql, "insert into env2 values(%d, %d, '%s')", c, f, buftime);
	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		//printf("%s\n", errmsg);
	}
	else
	{
		printf("Insert done.\n");
	}

	//printf_sensor_info_debug(rt_status,home_id);
	
	//添加stm32部分的数据、arduino数据，

	return 0;
}


int printf_sensor_info_debug(struct env_info_client_addr  *sm_all_env_info,int home_id)
{
	printf("a9_info.adc  : %f.\n",sm_all_env_info->monitor_no[home_id].a9_info.adc  );
	printf("a9_info.gyrox: %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.gyrox);
	printf("a9_info.gyroy: %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.gyroy);
	printf("a9_info.gyroz: %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.gyroz);
	printf("a9_info.aacx : %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.aacx );
	printf("a9_info.aacy : %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.aacy );
	printf("a9_info.aacz : %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.aacz );
	printf("a9_info.reserved[0]: %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.reserved[0] );
	printf("a9_info.reserved[1]: %d.\n",sm_all_env_info->monitor_no[home_id].a9_info.reserved[1] );

	return 0;
}

