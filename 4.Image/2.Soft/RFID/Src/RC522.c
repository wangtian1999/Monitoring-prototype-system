#include "RC522.h"

uint8_t buf[30];
uint8_t len;
uint8_t ID[4];
/**************************************************************
NAME:	Spi_Read
INPUT:	address
OUTPUT: none
RETURN: data
AUTHOR: david
DATE:
**************************************************************/
unsigned char Spi_Read(uint8_t addr)
{
    uint8_t uaddr = addr;
    uint8_t i;
    uint8_t result = 0;

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_RESET);    // 设置clk引脚为低电平
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,GPIO_PIN_RESET);    // 设置nss引脚为低电平

    for (i=0; i<8; i++)
    {
        if (uaddr&0x80)
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_SET);	//得到mosi
        else
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_SET);			//产生上升沿
        uaddr <<= 1;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_RESET);
    }
    for (i=0; i<8; i++)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET );		//产生下降沿
        result <<= 1;
        result |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);				//读取数据miso

        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET );
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,GPIO_PIN_SET);    // 设置引脚为高电平
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET ) ;
    delay(10);
    return result;
}
/**************************************************************
NAME:	Spi_Write
INPUT:	address, data
OUTPUT: none
RETURN: none
AUTHOR: david
DATE:
**************************************************************/
void Spi_Write(uint8_t addr, uint8_t data)
{
    uint8_t uaddr = addr;
    uint8_t i;
    uint8_t udata = data;

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_RESET);    // 设置clk引脚为低电平
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,GPIO_PIN_RESET);    // 设置nss引脚为低电平

    for (i=0; i<8; i++)
    {
        if (uaddr&0x80)
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_SET);	//得到mosi
        else
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_SET);			//产生上升沿
        uaddr <<= 1;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_RESET);
    }
    for (i=0; i<8; i++)
    {
        if (udata&0x80)
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_SET);	//得到mosi
        else
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_SET);			//产生上升沿
        udata <<= 1;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,GPIO_PIN_SET);    // 设置引脚为高电平
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET ) ;
    delay(10);
}
/**************************************************************
NAME:	void RC522_ReadReg()
INPUT:	addr
OUTPUT:	none
RETURN:	data
AUTHOR: david
DATE:
**************************************************************/
unsigned char RC522_ReadReg(uint8_t addr)
{
    uint8_t uaddr;
    uaddr = ((uaddr<<1)|0x80)&0xfe;
    return Spi_Read(uaddr);
}
/**************************************************************
NAME:	void RC522_WriteReg()
INPUT:	addr
OUTPUT:	none
RETURN:	data
AUTHOR: david
DATE:
**************************************************************/
void RC522_WriteReg(uint8_t addr, uint8_t data)
{
    uint8_t uaddr;
    uaddr = ((uaddr<<1)|0x80)&0x7e;
    Spi_Write(uaddr, data);
}
/**************************************************************
NAME:	RC522_SetBit
INPUT:	addr, mask
OUTPUT:	none
RETURN: none
AUTHOR: david
DATE:
**************************************************************/
void RC522_SetBit(uint8_t addr, uint8_t mask)
{
    uint8_t temp = RC522_ReadReg(addr);
    RC522_WriteReg(addr, temp|mask);
}
/**************************************************************
NAME:	RC522_ClearBit
INPUT:	addr, mask
OUTPUT: none
RETURN: none
AUTHOR: david
DATE:
**************************************************************/
void RC522_ClearBit(uint8_t addr, uint8_t mask)
{
    uint8_t temp = RC522_ReadReg(addr);
    RC522_WriteReg(addr, temp&(~mask));
}

/*******************************************************************************
* Function Name  	: RC522_Off()
* Description    	: 关闭天线
* Input          	: none
* Output         	: none
* Return         	: none
* date				:
* modify			:
*******************************************************************************/
void RC522_Off()
{
    RC522_ClearBit(TxControlReg,0x03);
}
/*******************************************************************************
* Function Name  	: RC522_On()
* Description    	: 打开天线
* Input          	: none
* Output         	: none
* Return         	: none
* date				:
* modify			:
*******************************************************************************/
void RC522_On()
{
    RC522_SetBit(TxControlReg,0x03);
}
/*******************************************************************************
* Function Name  	:
* Description    	: 复位
* Input          	: none
* Output         	: none
* Return         	: none
* date				:
* modify			:
*******************************************************************************/
void RC522_Reset()
{
    RC522_WriteReg(CommandReg, PCD_RESETPHASE);
    printf("pcdreset in\n");
    delay(10);
    RC522_WriteReg(ModeReg, 0x3d);
    delay(10);
    RC522_WriteReg(TReloadRegL, 32);//48

    RC522_WriteReg(TReloadRegH, 0);

    RC522_WriteReg(TModeReg, 0x8d);
    printf("pcdreset in1\n");
    RC522_WriteReg(TPrescalerReg, 0x3e);//分频参数3990，得到2k，定时24ms

    RC522_WriteReg(TxAskReg, 0x40);
}
/**************************************************************
NAME:	RC522_to_S50
INPUT:	cmd, inDta
OUTPUT: outData, inLen, outLen
RETURN: none
AUTHOR: david
DATE:
**************************************************************/
uint8_t RC522_to_S50(uint8_t cmd, uint8_t *inData, uint8_t inLen, uint8_t *outData, uint8_t *outLen)
{
    uint8_t n;
    uint8_t i;
    uint8_t status=0;
//    RC522_SetBit(ComIEnReg, 0xf3);			//是能相应中断传输
    RC522_ClearBit(ComIrqReg, 0x80);		//清除中断位
    RC522_WriteReg(CommandReg, PCD_IDLE);	//终止当前所有命令
    RC522_SetBit(FIFOLevelReg, 0x80);		//FIFO指针复位


    for (int i=0; i<inLen; i++)
        RC522_WriteReg(FIFODataReg, inData[i]);	//将数据写入FIFO

    RC522_WriteReg(CommandReg, cmd);
    if (cmd == PCD_TRANSCEIVE)
        RC522_SetBit(BitFramingReg, 0x80);		//启动传输
    printf("step1\n");
    i = 200;
    do
    {
        n = RC522_ReadReg(ComIrqReg);			//读取中断寄存器
        printf("n = 0x%x\n", n);
        i--;
    } while ((i!=0) && !(n&0x01) && !(n&0x30));
    printf("step2 n = %d\n", n);
    RC522_ClearBit(BitFramingReg, 0x80);		//结束发送

    if (n&0x01)
        status = 1;
    else
    {
        if (RC522_ReadReg(ErrorReg)&0x01)
            status = 2;
        else
        {
            *outLen = RC522_ReadReg(FIFOLevelReg)&0xef;
            printf("len = %d\n", *outLen);
            for (int i=0; i<*outLen; i++)
            {
                outData[i] = RC522_ReadReg(FIFODataReg);	//读出数据
                //	printf("0x%x,", outData[i]);
            }
        }
    }

    RC522_SetBit(ControlReg,0x80);
    return status;
}

/**************************************************************
NAME:	req，短格式
INPUT:	none
OUTPUT: none
RETURN: none
AUTHOR: david
DATE:
**************************************************************/
void request()
{
    unsigned int  l;
    uint8_t status;
    buf[0] = PICC_REQALL;
    ClearBitMask(Status2Reg,0x08);		//清除MF author标志
    RC522_WriteReg(BitFramingReg,0x07);//短格式发送
    status = RC522_to_S50(PCD_TRANSCEIVE, buf, 1, buf, &len);
    printf("status = %d\n", status);
    if (status == 0)
    {
        printf("Card Type : %x,%x\n", buf[0], buf[1]);
    }
}
/*******************************************************************************
* Function Name  	: anti
* Description    	: 防冲突,如果成功的话会返回ID号,校验值
* Input          	: none
* Output         	: 卡片id，校验
* Return         	: none
* date				:
* modify			:
*******************************************************************************/
void anti()
{
    uint8_t status;
    RC522_ClearBit(CollReg, 0x80);	//清空冲突标记
    ClearBitMask(Status2Reg,0x08);	//清除MF author标志
    RC522_WriteReg(BitFramingReg,0x00);//最后一个字节发送所有位
    buf[0] = PICC_ANTICOLL1;
    buf[1] = 0x20;
    status = RC522_to_S50(PCD_TRANSCEIVE, buf, 2, buf, &len);
    if (status == 0)
    {
        printf("Card ID : ");
        for (int i=0; i<len; i++)
        {
            ID[i] = buf[i];
            printf("0x%x,", ID[i]);
        }
        printf("\n");
    }
    RC522_SetBit(CollReg,0x80);//置位防碰撞位
}
/*******************************************************************************
* Function Name  	: select
* Description    	: 选择卡片
* Input          	: 卡片id
* Output         	: none
* Return         	: none
* date				:
* modify			:
*******************************************************************************/
void select()
{
    uint8_t status=0;	
    RC522_ClearBit(Status2Reg,0x08);	//清除MF author标志
    buf[0] = PICC_ANTICOLL1;			//选择命令
    buf[1] = 0x70;
    buf[6] = 0;
    for (int i=0; i<4; i++)				//id
    {
        buf[i+2] = ID[i];
        buf[6] ^= buf[i+2]; 			//校验
        printf("0x%x,", ID[i]);
    }
    CalulateCRC(buf, 7, &buf[7]);		//CRC16校验
    status = RC522_to_S50(PCD_TRANSCEIVE, buf, 9, buf, &len);
    if (status == 0)
    {
        printf("Select result : ");
        for (int i=0; i<len; i++)
            printf("0x%x,", buf[i]);
        printf("\n");
    }
}
/*******************************************************************************
* Function Name  	: AUTHOR
* Description    	: 认证卡片
* Input          	: 认证命令，地址，秘钥
* Output         	: none
* Return         	: none
* date				:
* modify			:
*******************************************************************************/
uint8_t KEY[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
void author(uint8_t mode, uint8_t addr, uint8_t *key)
{
    uint8_t status;
	unsigned int l;
    buf[0] = mode;
    buf[1] = addr;
    for (int i=0; i<6; i++)			//秘钥
        buf[i+2] = key[i];
    for (int i=0; i<4; i++)			//id
        buf[i+8] = ID[i];
    status = PcdComMF522(PCD_AUTHENT, buf, 12, buf, &l);
    if (status == 0)
    {
        printf("Author result : ");
//        for (int i=0; i<l/8; i++)
//            printf("0x%x,", buf[i]);
        printf("\n");
    }
}
/*******************************************************************************
* Function Name  	: read
* Description    	: 读取卡片
* Input          	: 卡片地址
* Output         	: 对应地址的内容
* Return         	: none
* date				:
* modify			:
*******************************************************************************/
void read(uint8_t addr)
{
    uint8_t status;
	unsigned int l;
    buf[0] = PICC_READ;
    buf[1] = addr;
    CalulateCRC(buf, 2, &buf[2]);
    status = PcdComMF522(PCD_TRANSCEIVE, buf, 4, buf, &l);
    if (status == 0)
    {
        printf("Read result : ");
        for (int i=0; i<l/8; i++)
            printf("0x%x,", buf[i]);
        printf("\n");
    }
}
/*******************************************************************************
* Function Name  	: read
* Description    	: 向卡片写入内容
* Input          	: 卡片地址，内容
* Output         	: none
* Return         	: none
* date				:
* modify			:
*******************************************************************************/
void write(uint8_t addr, unsigned char *buffer)
{
    uint8_t status;
	unsigned int l;
    buf[0] = PICC_WRITE;		//写入命令
    buf[1] = addr;				//地址
    CalulateCRC(buf, 2, &buf[2]);//crc校验
    status = PcdComMF522(PCD_TRANSCEIVE, buf, 4, buf, &l);
    if (status == 0)
    {
		for(int i=0; i<16; i++)
		{
			buf[i] = buffer[i];	//16byte数据
		}
		CalulateCRC(buf, 16, &buf[16]);	//crc
		status = PcdComMF522(PCD_TRANSCEIVE, buf, 18, buf, &l);//写入
        printf("write result : ");
		read(addr);				//读出已经写入的数据
    }
}

void RFID_Read()
{
	RC522_Reset();
	RC522_On();
	request();
	anti();
	select();
	author(PICC_AUTHENT1A, cmd_addr, KEY_A);

	read(cmd_addr);
}
void RFID_Write()
{
	RC522_Reset();
	RC522_On();
	request();
	anti();
	select();
	author(PICC_AUTHENT1A, cmd_addr, KEY_A);

	write(cmd_addr, cmd_dat);
}