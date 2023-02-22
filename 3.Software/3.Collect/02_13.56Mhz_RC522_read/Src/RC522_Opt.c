#include "RC522.h"
#include "logo.h"

#define MAXRLEN 18

extern uint8_t ID[4];
extern uint8_t idcs;	//idУ���ֽ�
extern uint8_t card_type[2];	//������
extern uint8_t cmd_data[16];	//����

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
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_RESET);    // ��������Ϊ�͵�ƽ
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,GPIO_PIN_RESET);    // ��������Ϊ�͵�ƽ
    ucaddr = ((addr<<1)&0x7e)| 0x80;   //bit7��д��bit6-1��ַ��bit0����Ϊ0
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
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12,GPIO_PIN_SET);    // ��������Ϊ�ߵ�ƽ
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
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
*******************************************************************************/
void SetBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);         //��Reg�Ĵ�����״̬
    WriteRawRC(reg,tmp | mask);  //��Reg�Ĵ���д�� MASK|tmpλ
}
/*******************************************************************************
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
*******************************************************************************/
void ClearBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);          //��Reg�Ĵ�����״̬
    WriteRawRC(reg, tmp & ~mask);  //��Reg�Ĵ���д��tmp&~maskλ
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
    WriteRawRC(TPrescalerReg, 0x3e);//��Ƶ����3990���õ�2k����ʱ24ms

    WriteRawRC(TxAskReg, 0x40);

    return MI_OK;
}
/*******************************************************************************
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pInData[IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOutData[OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
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
    case PCD_AUTHENT:    // 0x0E   ��֤��Կ
        irqEn   = 0x12;   // 0001 0010
        waitFor = 0x10;   // 0001 0000
        break;
    case PCD_TRANSCEIVE://0x0C   ���Ͳ���������
        irqEn   = 0x77;   // 0111 0111
        waitFor = 0x30;   // 0011 0000
        break;
    default:
        break;
    }

    //��ʼ��
    WriteRawRC(ComIEnReg,irqEn|0x80);  //ʹ����Ӧ�ж�,���󡢶�ʱ�������͡����ա�Ĭ��û���ж�
    ClearBitMask(ComIrqReg,0x80);      //���������жϱ�־
    WriteRawRC(CommandReg,PCD_IDLE);   //����Ĵ������㣬ȡ����ǰ����������    P59
    SetBitMask(FIFOLevelReg,0x80);     //��λFIFO��дָ��,   P18


    //д��Э������
    for (i=0; i<InLenByte; i++)
    {
        WriteRawRC(FIFODataReg, pInData[i]);    //��Ҫ���͸���������д��522��FIFO���ݼĴ���
    }
    //��������
    WriteRawRC(CommandReg, Command);//522ִ�з�������   //P13
    //�շ�������Ҫ��BitFramingReg��bit7Ϊ1
    if (Command == PCD_TRANSCEIVE)     //�Ƿ�Ϊ�������ݵ�����
    {
        SetBitMask(BitFramingReg,0x80);     //ִ�з����������� ���뷢������������Ч���    P19��P59
    }



    //�ȴ����ݷ��������ȴ�Ӧ��
    i = 500;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    do
    {
        n = ReadRawRC(ComIrqReg);// ���жϱ�־�Ĵ���    P14��TIMER�жϣ����գ��������
        i--;
    } while ((i!=0) && !(n&0x01) && !(n&waitFor));//�˳���IRQ����:1. i=0 2.��ʱ���������ж�3.��������⵽��Ч�������ж�OR�������ֹ�ж�
    //printf("i=%d\n",i);
    ClearBitMask(BitFramingReg,0x80);//����������������


    //����5ms���Դ���Ƭ�����ᶨʱ���ж�
    if (n!=0x01)                       //��Ƶ���޿�
    {
        m = ReadRawRC(ErrorReg);//p15
        //printf("error:%d\n",m);
        if (!(m&0x1B))//�������־�Ĵ���&0001 1011����һ�´�������FIFO��������ײ������żУ�����SOF����
        {
            status = MI_OK;             //��ʱ�޴���
             if (n & irqEn & 0x01)       //�Ƿ�Ϊ��ʱ���ж�
             {   status = MI_NOTAGERR;   }//����
            if (Command == PCD_TRANSCEIVE)
            {
                n = ReadRawRC(FIFOLevelReg);   //�������˼����ֽ�
                lastBits = ReadRawRC(ControlReg) & 0x07;//���յ����һ���ֽڵ���Чλ
                if (lastBits)
                {
                    *pOutLenBit = (n-1)*8 + lastBits;      //N*8+LASTBITSλ
                }
                else
                {
                    *pOutLenBit = n*8;      //N*8λ
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAXRLEN)
                {
                    n = MAXRLEN;      //���ɽ���18���ֽ�
                }
                for (i=0; i<n; i++)
                {
                    pOutData[i] = ReadRawRC(FIFODataReg);       //ȡ�����յ������ݷŵ�����POUTDATA����--ucComMF522Buf[MAXRLEN]
                }

            }
        }
        else
        {
            status = MI_ERR;      //�����־
        }

    }

    SetBitMask(ControlReg,0x80);           //ֹͣ��ʱ��
    WriteRawRC(CommandReg,PCD_IDLE);     //��λCOMMANDREG
    return status;
}

//��MF522����CRC16����
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);//��ֹCRC
    WriteRawRC(CommandReg,PCD_IDLE);//��λ522CommandReg�Ĵ���
    SetBitMask(FIFOLevelReg,0x80);// ��λFIFO�Ķ�дָ��
    for (i=0; i<len; i++)
    {
        WriteRawRC(FIFODataReg, *(pIndata+i));      //��*pIndata��������ֵд��FIFODataReg
    }
    WriteRawRC(CommandReg, PCD_CALCCRC);//ִ��CRCУ��
    i = 0xFF;//�ȴ�255us
    do
    {
        n = ReadRawRC(DivIrqReg);//���ж������־�Ĵ���
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);//CRCУ��ĵ�8λ
    pOutData[1] = ReadRawRC(CRCResultRegM);//CRCУ��ĸ�8λ
}



/**************************************************�����Ƿָ���********************************************************/



/*******************************************************************************
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
*******************************************************************************/
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ClearBitMask(Status2Reg,0x08);    //����Status2Reg��MFAuthent Commandִ�гɹ���־λ
    WriteRawRC(BitFramingReg,0x07);   //����Transceive���ʼλ
    SetBitMask(TxControlReg,0x03);    //��������
    ucComMF522Buf[0] = req_code;      //ȡ522Ҫִ�е�����
    //printf("%x\n",ucComMF522Buf[0]);
    //printf("%x\n",ucComMF522Buf[1]);
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);//��PICC����Ѱ��������ȫ�������������PICC���ص�����

    if ((status == MI_OK))
   if ((status == MI_OK) && (unLen == 0x10))//û�д��󲢽ӽ���Ϊ2���ֽ�
    {
        *pTagType     = ucComMF522Buf[0];//ȡ���ջ������ĵ�һ���ֽ�
        *(pTagType+1) = ucComMF522Buf[1];//ȡ���ջ������ĵڶ����ֽ�
    }
    else
    {
        status = MI_ERR;      //����
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
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
//Э	�飺SEL  NVB(0X20)  
*******************************************************************************/
char PcdAnticoll(unsigned char *id)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];


    ClearBitMask(Status2Reg,0x08);//�����־λ
    WriteRawRC(BitFramingReg,0x00);//000 ָʾ���һ���ֽڵ�����λ�������͡�
    ClearBitMask(CollReg,0x80);//������ײ���н���λ�������
    ucComMF522Buf[0] = PICC_ANTICOLL1;//0x93 ����ײ �������������
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);//��ÿ������кţ�ucComMF522Buf[]
    if (status == MI_OK)
    {
        for (i=0; i<4; i++)
        {
			id[i] = ucComMF522Buf[i];		//���ؿ������к�
            snr_check ^= ucComMF522Buf[i];	//����У����
        }
        if (snr_check != ucComMF522Buf[i])
        {
            status = MI_ERR;       //�д���
        }
    }

    SetBitMask(CollReg,0x80);//��λ����ײλ
    return status;

}

/*******************************************************************************
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
//Э	�飺SEL  	NVB(0X70)  	ID(4��byte)  	IDCHECK  	CRC16
*******************************************************************************/
char PcdSelect(unsigned char *id)
{
    char status = 0XFF;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 	//MAXRLEN = 18

    ucComMF522Buf[0] = PICC_ANTICOLL1;//����ײ����
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
        ucComMF522Buf[i+2] = *(id+i);//��俨�����к�
        ucComMF522Buf[6]  ^= *(id+i);//����У����
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);//���CRCУ������16λֵ
    //����ucComMF522Buf��0��1��
    ClearBitMask(Status2Reg,0x08);//����MFAuthent Commandִ�гɹ���־λ

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);//��CRCֵ�Ϳ��ŷ��Ŀ���

    if ((status == MI_OK) && (unLen == 0x18))//����24���ֽ�&״̬Ϊ�޴���
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
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
//��	�ͣ�authorA/authorB  	addr  	keyA/B(6bytes)  	ID(4bytes)
*******************************************************************************/
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;//PICC��֤A��Կָ��
    ucComMF522Buf[1] = addr;//���ַ
    for (i=0; i<6; i++)
    {
        ucComMF522Buf[i+2] = *(pKey+i);      //�򻺳����������
    }
    for (i=0; i<4; i++)
    {
        ucComMF522Buf[i+8] = *(pSnr+i);      //�򻺳�������������Ӧ�Ŀ������кţ���Ч4���ֽ�
    }
//   memcpy(&ucComMF522Buf[2], pKey, 6);
//   memcpy(&ucComMF522Buf[8], pSnr, 4);

    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);//��֤����Ϳ���
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))//������֤�Ƿ�ɹ�
    {
        status = MI_ERR;
    }

    return status;
}
/*******************************************************************************
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          pData[OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
//��	�ͣ�cmd_read  	addr  	CRC16
//����16B,   �����block3��  ���ص�keyA��0
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
    PcdReset();		    //��ʼ����ƵоƬ
    PcdAntennaOff();	//�ر�����
    PcdAntennaOn();		//������
    while (1)
    {
        //Ѱ��
        //printf("req\n");
        status = PcdRequest(PICC_REQALL, card_type);

        if (status != MI_OK)
        {
            break;
        }
       // printf("anti\n");
        //����ײ����
        status = PcdAnticoll(ID);//ִ�гɹ��õ��������к�
        if (status != MI_OK)
        {
            break;
        }
        //ѡ��Ƭ
        //printf("select\n");
        status = PcdSelect(ID);
        if (status != MI_OK)
        {
            break;
        }
        //��֤��Ƭ����
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
        //����ַ������
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

























