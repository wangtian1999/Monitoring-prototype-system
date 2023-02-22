#include "RC522.h"
#include "logo.h"

#define MAXRLEN 18

extern uint8_t ID[4];
extern uint8_t idcs;	//id校验字节
extern uint8_t card_type[2];	//卡类型
extern uint8_t cmd_data[16];	//数据

void BoardInit(void)
{
    GPIOA->BSRR = GPIO_PIN_0;
    Lcd_Init();
    Lcd_Clear(GREEN);
    showimage_farsight(gImage_logo);
    Gui_DrawFont_GBK16(10,40,BLUE,GREEN,(uint8_t *)"RFID 13.56MHz");
    Gui_DrawFont_GBK16(10,80,RED,GREEN,(uint8_t *)"PW:");
    TIP_WRITE_SUCCESS();
}

void delay(unsigned int i)
{
    while (i--);
}

/***********************************************************************/
unsigned char ReadRawRC(unsigned char addr)
{
    unsigned char i, ucaddr;
    unsigned char ucResult  =0;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_RESET);    // 设置引脚为低电平
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,GPIO_PIN_RESET);    // 设置引脚为低电平
    ucaddr = ((addr<<1)&0x7e)| 0x80;   //bit7读写，bit6-1地址，bit0必须为0
    for (i=8;i>0;i--)
    {
        if (ucaddr & 0x80)
        {
            //SDA=1;
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_SET);
        }
        else
        {
            //	SDA=0;
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_RESET);
        }
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET) ;
        ucaddr <<= 1;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET ) ;
    }
    for (i=8;i>0;i--)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET ) ;
        ucResult <<= 1;
        ucResult |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) ;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET ) ;
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,GPIO_PIN_SET);    // 设置引脚为高电平
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET ) ;
    delay(10);
    return ucResult;
}



/******************************************************************/
void WriteRawRC(unsigned char addr, unsigned char val)
{
    unsigned char i, ucaddr;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET ) ;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET ) ;
    ucaddr = ((addr<<1)&0x7e);	 //write higher bit 0 and lower bit 0
    for (i=8;i>0;i--)
    {
        if (ucaddr & 0x80)
        {
            //SDA=1;
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_SET);
        }
        else
        {
            //	SDA=0;
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_RESET);
        }
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET ) ;
        ucaddr <<= 1;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET ) ;
    }
    for (i=8;i>0;i--)
    {
        if (val & 0x80)
        {
            //SDA=1;
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_SET);
        }
        else
        {
            //SDA=0;
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15,GPIO_PIN_RESET);
        }
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET ) ;
        val <<= 1;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET ) ;
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET ) ;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET ) ;
    delay(10);
}



/*******************************************************************************
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
*******************************************************************************/
void SetBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);         //读Reg寄存器的状态
    WriteRawRC(reg,tmp | mask);  //向Reg寄存器写入 MASK|tmp位
}
/*******************************************************************************
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
*******************************************************************************/
void ClearBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);          //读Reg寄存器的状态
    WriteRawRC(reg, tmp & ~mask);  //向Reg寄存器写入tmp&~mask位
}
/***********************************************************************/
void PcdAntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}

/***************************************************************************/
void PcdAntennaOn(void)
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i&0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

/*******************************************************************/
char PcdReset(void)
{
//  GPIOSetValue( PORT0, RC522_RESET, 1);	 // reset rc522;
// __nop_();
//  GPIOSetValue( PORT0, RC522_RESET, 0);
//  __nop_();
//  GPIOSetValue( PORT0, RC522_RESET, 1);
//  __nop_();
    WriteRawRC(CommandReg, PCD_RESETPHASE);
    //printf("pcdreset in\n");
    delay(10);
    WriteRawRC(ModeReg, 0x3d);
    delay(10);
    WriteRawRC(TReloadRegL, 30);//48

    WriteRawRC(TReloadRegH, 0);

    WriteRawRC(TModeReg, 0x8d);
    //printf("pcdreset in1\n");
    WriteRawRC(TPrescalerReg, 0x3e);//分频参数3990，得到2k，定时24ms

    WriteRawRC(TxAskReg, 0x40);

    return MI_OK;
}
/*******************************************************************************
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
*******************************************************************************/
char PcdComMF522(unsigned char Command,unsigned char *pInData,unsigned char InLenByte,unsigned char *pOutData,unsigned int  *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    unsigned char m;
    switch (Command)
    {
    case PCD_AUTHENT:    // 0x0E   验证密钥
        irqEn   = 0x12;   // 0001 0010
        waitFor = 0x10;   // 0001 0000
        break;
    case PCD_TRANSCEIVE://0x0C   发送并接收数据
        irqEn   = 0x77;   // 0111 0111
        waitFor = 0x30;   // 0011 0000
        break;
    default:
        break;
    }

    //初始化
    WriteRawRC(ComIEnReg,irqEn|0x80);  //使能相应中断,错误、定时器、发送、接收。默认没有中断
    ClearBitMask(ComIrqReg,0x80);      //清零所有中断标志
    WriteRawRC(CommandReg,PCD_IDLE);   //命令寄存器复零，取消当前的所有命令    P59
    SetBitMask(FIFOLevelReg,0x80);     //复位FIFO读写指针,   P18


    //写入协议数据
    for (i=0; i<InLenByte; i++)
    {
        WriteRawRC(FIFODataReg, pInData[i]);    //把要发送给卡的数据写入522的FIFO数据寄存器
    }
    //启动传输
    WriteRawRC(CommandReg, Command);//522执行发送命令   //P13
    //收发命令需要，BitFramingReg的bit7为1
    if (Command == PCD_TRANSCEIVE)     //是否为发送数据的命令
    {
        SetBitMask(BitFramingReg,0x80);     //执行发送数据命令 仅与发送数据命令有效结合    P19，P59
    }



    //等待数据发出，并等待应答
    i = 500;//根据时钟频率调整，操作M1卡最大等待时间25ms
    do
    {
        n = ReadRawRC(ComIrqReg);// 读中断标志寄存器    P14，TIMER中断，接收，命令结束
        i--;
    } while ((i!=0) && !(n&0x01) && !(n&waitFor));//退出的IRQ条件:1. i=0 2.定时器减到零中断3.接收器检测到有效的数据中断OR命令本身终止中断
    //printf("i=%d\n",i);
    ClearBitMask(BitFramingReg,0x80);//结束发送数据命令


    //正常5ms可以处理卡片，不会定时器中断
    if (n!=0x01)                       //射频区无卡
    {
        m = ReadRawRC(ErrorReg);//p15
        //printf("error:%d\n",m);
        if (!(m&0x1B))//读错误标志寄存器&0001 1011不是一下错误：数据FIFO已满、碰撞错误、奇偶校验错误、SOF错误
        {
            status = MI_OK;             //暂时无错误
             if (n & irqEn & 0x01)       //是否为定时器中断
             {   status = MI_NOTAGERR;   }//错误
            if (Command == PCD_TRANSCEIVE)
            {
                n = ReadRawRC(FIFOLevelReg);   //共接收了几个字节
                lastBits = ReadRawRC(ControlReg) & 0x07;//接收的最后一个字节的有效位
                if (lastBits)
                {
                    *pOutLenBit = (n-1)*8 + lastBits;      //N*8+LASTBITS位
                }
                else
                {
                    *pOutLenBit = n*8;      //N*8位
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAXRLEN)
                {
                    n = MAXRLEN;      //最多可接收18个字节
                }
                for (i=0; i<n; i++)
                {
                    pOutData[i] = ReadRawRC(FIFODataReg);       //取出接收到的数据放到数组POUTDATA【】--ucComMF522Buf[MAXRLEN]
                }

            }
        }
        else
        {
            status = MI_ERR;      //错误标志
        }

    }

    SetBitMask(ControlReg,0x80);           //停止定时器
    WriteRawRC(CommandReg,PCD_IDLE);     //复位COMMANDREG
    return status;
}

//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);//禁止CRC
    WriteRawRC(CommandReg,PCD_IDLE);//复位522CommandReg寄存器
    SetBitMask(FIFOLevelReg,0x80);// 复位FIFO的读写指针
    for (i=0; i<len; i++)
    {
        WriteRawRC(FIFODataReg, *(pIndata+i));      //把*pIndata缓冲区的值写如FIFODataReg
    }
    WriteRawRC(CommandReg, PCD_CALCCRC);//执行CRC校验
    i = 0xFF;//等待255us
    do
    {
        n = ReadRawRC(DivIrqReg);//读中断请求标志寄存器
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);//CRC校验的低8位
    pOutData[1] = ReadRawRC(CRCResultRegM);//CRC校验的高8位
}



/**************************************************这里是分割线********************************************************/



/*******************************************************************************
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
*******************************************************************************/
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ClearBitMask(Status2Reg,0x08);    //清零Status2Reg的MFAuthent Command执行成功标志位
    WriteRawRC(BitFramingReg,0x07);   //清零Transceive命令开始位
    SetBitMask(TxControlReg,0x03);    //开启天线
    ucComMF522Buf[0] = req_code;      //取522要执行的命令
    //printf("%x\n",ucComMF522Buf[0]);
    //printf("%x\n",ucComMF522Buf[1]);
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);//向PICC发送寻天线区内全部卡命令，并接收PICC返回的数据

    if ((status == MI_OK))
   if ((status == MI_OK) && (unLen == 0x10))//没有错误并接接收为2个字节
    {
        *pTagType     = ucComMF522Buf[0];//取接收缓冲区的第一个字节
        *(pTagType+1) = ucComMF522Buf[1];//取接收缓冲区的第二个字节
    }
    else
    {
        status = MI_ERR;      //错误
    }
	#if PRINT
	printf("card type is:\n");
    printf("%x\n",ucComMF522Buf[0]);
    printf("%x\n",ucComMF522Buf[1]);
	#endif
    
    return status;
}
/*******************************************************************************/
/*******************************************************************************
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
//协	议：SEL  NVB(0X20)  
*******************************************************************************/
char PcdAnticoll(unsigned char *id)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];


    ClearBitMask(Status2Reg,0x08);//清除标志位
    WriteRawRC(BitFramingReg,0x00);//000 指示最后一个字节的所有位将被发送。
    ClearBitMask(CollReg,0x80);//发生碰撞所有接收位将被清除
    ucComMF522Buf[0] = PICC_ANTICOLL1;//0x93 防冲撞 发到卡里的命令
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);//获得卡的序列号，ucComMF522Buf[]
    if (status == MI_OK)
    {
        for (i=0; i<4; i++)
        {
			id[i] = ucComMF522Buf[i];		//返回卡的序列号
            snr_check ^= ucComMF522Buf[i];	//计算校验码
        }
        if (snr_check != ucComMF522Buf[i])
        {
            status = MI_ERR;       //有错误
        }
    }

    SetBitMask(CollReg,0x80);//置位防碰撞位
    return status;

}

/*******************************************************************************
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
//协	议：SEL  	NVB(0X70)  	ID(4个byte)  	IDCHECK  	CRC16
*******************************************************************************/
char PcdSelect(unsigned char *id)
{
    char status = 0XFF;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 	//MAXRLEN = 18

    ucComMF522Buf[0] = PICC_ANTICOLL1;//防冲撞命令
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
        ucComMF522Buf[i+2] = *(id+i);//填充卡的序列号
        ucComMF522Buf[6]  ^= *(id+i);//计算校验码
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);//获得CRC校验结果的16位值
    //放入ucComMF522Buf【0，1】
    ClearBitMask(Status2Reg,0x08);//清零MFAuthent Command执行成功标志位

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);//把CRC值和卡号发的卡里

    if ((status == MI_OK) && (unLen == 0x18))//返回24个字节&状态为无错误
    {
        status = MI_OK;
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}
/*******************************************************************************
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
//发	送：authorA/authorB  	addr  	keyA/B(6bytes)  	ID(4bytes)
*******************************************************************************/
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;//PICC验证A密钥指令
    ucComMF522Buf[1] = addr;//块地址
    for (i=0; i<6; i++)
    {
        ucComMF522Buf[i+2] = *(pKey+i);      //向缓冲区填充密码
    }
    for (i=0; i<4; i++)
    {
        ucComMF522Buf[i+8] = *(pSnr+i);      //向缓冲区填充与密码对应的卡的序列号，有效4个字节
    }
//   memcpy(&ucComMF522Buf[2], pKey, 6);
//   memcpy(&ucComMF522Buf[8], pSnr, 4);

    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);//验证密码和卡号
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))//密码验证是否成功
    {
        status = MI_ERR;
    }

    return status;
}
/*******************************************************************************
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
//发	送：cmd_read  	addr  	CRC16
//返回16B,   如果读block3，  返回的keyA是0
*******************************************************************************/
char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
    {
        for (i=0; i<16; i++)
        {
            *(pData+i) = ucComMF522Buf[i];
        }
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}


void RFID_Read()
{
    uint8_t i;
    unsigned char status = 0;
    PcdReset();		    //初始化射频芯片
    PcdAntennaOff();	//关闭天线
    PcdAntennaOn();		//打开天线
    while (1)
    {
        //寻卡
        //printf("req\n");
        status = PcdRequest(PICC_REQALL, card_type);

        if (status != MI_OK)
        {
            break;
        }
       // printf("anti\n");
        //防冲撞处理
        status = PcdAnticoll(ID);//执行成功得到卡的序列号
        if (status != MI_OK)
        {
            break;
        }
        //选择卡片
        //printf("select\n");
        status = PcdSelect(ID);
        if (status != MI_OK)
        {
            break;
        }
        //验证卡片密码
       // printf("ahthor\n");
				for(int i = 0;i<6;i++)
				{
					KEY_A[i] = 0xff;
				}
				cmd_addr = 0;
        status = PcdAuthState(PICC_AUTHENT1A, cmd_addr, KEY_A, ID);
        if (status != MI_OK)
        {
            break;
        }
        //读地址的数据
        memset(cmd_dat,0,sizeof(cmd_dat));
        status = PcdRead(cmd_addr, cmd_dat);
//        if (status != MI_OK)
//        {
//            break;
//        }
        //printf("Card_Data:\n");
        for (i=0;i<2;i++) {
            printf("%02x ",cmd_dat[i]);
        }printf("\n");
        TIP_SUCCESS();
       
        break;
    }
}

























