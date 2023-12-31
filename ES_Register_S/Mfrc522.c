/*
 * Mfrc522.c
 *
 *  Created on: 12-Feb-2023
 *      Author: Aksh garg
 */
#include "myLib.h"
#include "Mfrc522.h"



/*
 * Function：WriteReg
 * Description：write a byte data into one register of MR RC522
 * Input parameter：addr--register address；val--the value that need to write in
 * Return：Null
 */

uint8_t SPI_transfer(uint8_t data){
       uint32_t rxtxData;

        rxtxData = data;

        SSIDataPut(SSI2_BASE, (uint8_t) rxtxData);

        while(SSIBusy(SSI2_BASE)){}

        SSIDataGet(SSI2_BASE, &rxtxData);

        return (uint8_t) rxtxData;
}

void SPI_transferW(uint8_t data){
       uint32_t rxtxData;

        rxtxData = data;

        SSIDataPut(SSI2_BASE, (uint8_t) rxtxData);

        while(SSIBusy(SSI2_BASE)){}

        SSIDataGet(SSI2_BASE, &rxtxData);
}



void WriteReg(uint8_t addr, uint8_t val)
{
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0);
    //digitalWrite(_chipSelectPin, 0);

    SPI_transferW((addr<<1)&0x7E);
    SPI_transferW(val);


    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_PIN_5);
    //digitalWrite(_chipSelectPin, 1);
}


/*
 * Function：ReadReg
 * Description：read a byte data into one register of MR RC522
 * Input parameter：addr--register address
 * Return：return the read value
 */
uint8_t ReadReg(uint8_t addr)
{
    uint8_t val;
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0);
    //digitalWrite(_chipSelectPin, 0);

    SPI_transfer(((addr<<1)&0x7E) | 0x80);
    val = SPI_transfer(0x00);

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_PIN_5);
    //digitalWrite(_chipSelectPin, 1);

    return val;
}


/*
 * Function：SetBitMask
 * Description：set RC522 register bit
 * Input parameter：reg--register address;mask--value
 * Return：null
 */
void SetBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    tmp = ReadReg(reg);
    WriteReg(reg, tmp | mask);  // set bit mask
}

/*
 * Function：ClearBitMask
 * Description：clear RC522 register bit
 * Input parameter：reg--register address;mask--value
 * Return：null
 */
void ClearBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    tmp = ReadReg(reg);
    WriteReg(reg, tmp & (~mask));  // clear bit mask
}


/*
 * Function：AntennaOn
 * Description：Turn on antenna, every time turn on or shut down antenna need at least 1ms delay
 * Input parameter：null
 * Return：null
 */
void AntennaOn(void)
{
    uint8_t temp;

    temp = ReadReg(TxControlReg);
    if (!(temp & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


/*
 * Function：AntennaOff
 * Description：Turn off antenna, every time turn on or shut down antenna need at least 1ms delay
 * Input parameter：null
 * Return：null
 */
void AntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}


/*
 * Function：Reset
 * Description： reset RC522
 * Input parameter：null
 * Return：null
 */
void Reset(void)
{
    WriteReg(CommandReg, PCD_RESETPHASE);
}


/*
 * Function：Init
 * Description：initilize RC522
 * Input parameter：null
 * Return：null
 */
void Init(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
    //digitalWrite(_NRSTPD,1);

    Reset();

    //Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    WriteReg(TModeReg, 0x8D);       //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    WriteReg(TPrescalerReg, 0x3E);  //TModeReg[3..0] + TPrescalerReg
    WriteReg(TReloadRegL, 30);
    WriteReg(TReloadRegH, 0);

    WriteReg(TxAutoReg, 0x40);      //100%ASK
    WriteReg(ModeReg, 0x3D);

    //ClearBitMask(Status2Reg, 0x08);       //MFCrypto1On=0
    //WriteReg(RxSelReg, 0x86);     //RxWait = RxSelReg[5..0]
    //WriteReg(RFCfgReg, 0x7F);         //RxGain = 48dB
    uint8_t temp = ReadReg(TxControlReg);
    temp = ReadReg(TModeReg);
    temp = ReadReg(TReloadRegL);
    AntennaOn();
    temp = ReadReg(TxControlReg);  // to check if antenna on or not
}

/*
 * Function：Request
 * Description：Searching card, read card type
 * Input parameter：reqMode--search methods，
 *           TagType--return card types
 *              0x4400 = Mifare_UltraLight
 *              0x0400 = Mifare_One(S50)
 *              0x0200 = Mifare_One(S70)
 *              0x0800 = Mifare_Pro(X)
 *              0x4403 = Mifare_DESFire
 * return：return MI_OK if successed
 */
unsigned char Request(unsigned char reqMode, unsigned char *TagType)
{
    unsigned char status;
    unsigned int backBits[1];

    WriteReg(BitFramingReg, 0x07);      //TxLastBists = BitFramingReg[2..0] ???

    TagType[0] = reqMode;
    status = ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, backBits);

    if ((status != MI_OK) || (backBits[0] != 0x10))
    {
        status = MI_ERR;
    }

    return status;
}


/*
 * Function：ToCard
 * Description：communicate between RC522 and ISO14443
 * Input parameter：command--MF522 command bits
 *           sendData--send data to card via rc522
 *           sendLen--send data length
 *           backData--the return data from card
 *           backLen--the length of return data
 * return：return MI_OK if successed
 */
unsigned char ToCard(unsigned char command, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned int *backLen)
{
    unsigned char status = MI_ERR;
    unsigned char irqEn = 0x00;
    unsigned char waitIRq = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;

    switch (command)
    {
        case PCD_AUTHENT:
        {
            irqEn = 0x12;
            waitIRq = 0x10;
            break;
        }
        case PCD_TRANSCEIVE:
        {
            irqEn = 0x77;
            waitIRq = 0x30;
            break;
        }
        default:
            break;
    }

    WriteReg(CommIEnReg, irqEn|0x80);
    ClearBitMask(CommIrqReg, 0x80);
    SetBitMask(FIFOLevelReg, 0x80);

    WriteReg(CommandReg, PCD_IDLE);

    for (i=0; i<sendLen; i++)
    {
        WriteReg(FIFODataReg, sendData[i]);
    }

    WriteReg(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg, 0x80);        //StartSend=1,transmission of data starts
    }

    i = 10000;
    do
    {
        n = ReadReg(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    ClearBitMask(BitFramingReg, 0x80);          //StartSend=0

    if (i != 0)
    {
        if(!(ReadReg(ErrorReg) & 0x1B)) //BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;           //??
            }

            if (command == PCD_TRANSCEIVE)
            {
                n = ReadReg(FIFOLevelReg);
                lastBits = ReadReg(ControlReg) & 0x07;
                if (lastBits)
                {
                    *backLen = (n-1)*8 + lastBits;
                }
                else
                {
                    *backLen = n*8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAX_LEN)
                {
                    n = MAX_LEN;
                }

                for (i=0; i<n; i++)
                {
                    backData[i] = ReadReg(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }
    }

//SetBitMask(ControlReg,0x80);           //timer stops
//WriteReg(CommandReg, PCD_IDLE);

    return status;
}


/*
 * Function：MFRC522_Anticoll
 * Description：Prevent conflict, read the card serial number
 * Input parameter：serNum--return the 4 bytes card serial number, the 5th byte is recheck byte
 * return：return MI_OK if successed
 */
unsigned char Anticoll(unsigned char *serNum)
{
    unsigned char status;
    unsigned char i;
    unsigned char serNumCheck=0;
    unsigned int unLen;


//ClearBitMask(Status2Reg, 0x08);       //TempSensclear
//ClearBitMask(CollReg,0x80);           //ValuesAfterColl
    WriteReg(BitFramingReg, 0x00);      //TxLastBists = BitFramingReg[2..0]
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
    {
        for (i=0; i<4; i++)
        {
            serNumCheck ^= serNum[i];
        }
        if (serNumCheck != serNum[i])
        {
            status = MI_ERR;
        }
    }

    //SetBitMask(CollReg, 0x80);        //ValuesAfterColl=1

    return status;
}


/*
 * Function：CalulateCRC
 * Description：Use MF522 to calculate CRC
 * Input parameter：pIndata--the CRC data need to be read，len--data length，pOutData-- the caculated result of CRC
 * return：Null
 */
void CalulateCRC(unsigned char *pIndata, unsigned char len, unsigned char *pOutData)
{
    unsigned char i, n;

    ClearBitMask(DivIrqReg, 0x04);          //CRCIrq = 0
    SetBitMask(FIFOLevelReg, 0x80);
    //WriteReg(CommandReg, PCD_IDLE);

    for (i=0; i<len; i++)
    {
        WriteReg(FIFODataReg, *(pIndata+i));
    }
    WriteReg(CommandReg, PCD_CALCCRC);

    i = 0xFF;
    do
    {
        n = ReadReg(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));            //CRCIrq = 1

    pOutData[0] = ReadReg(CRCResultRegL);
    pOutData[1] = ReadReg(CRCResultRegM);
}


/*
 * Function：SelectTag
 * Description：Select card, read card storage volume
 * Input parameter：serNum--Send card serial number
 * return：return the card storage volume
 */
unsigned char SelectTag(unsigned char *serNum)
{
    unsigned char i;
    unsigned char status;
    unsigned char size;
    unsigned int recvBits;
    unsigned char buffer[9];

    //ClearBitMask(Status2Reg, 0x08);           //MFCrypto1On=0

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
        buffer[i+2] = *(serNum+i);
    }
    CalulateCRC(buffer, 7, &buffer[7]);     //??
    status = ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

    if ((status == MI_OK) && (recvBits == 0x18))
    {
        size = buffer[0];
    }
    else
    {
        size = 0;
    }

    return size;
}


/*
 * Function：Auth
 * Description：verify card password
 * Input parameters：authMode--password verify mode
                 0x60 = verify A password key
                 0x61 = verify B password key
             BlockAddr--Block address
             Sectorkey--Block password
             serNum--Card serial number ，4 bytes
 * return：return MI_OK if successed
 */
unsigned char Auth(unsigned char authMode, unsigned char BlockAddr, unsigned char *Sectorkey, unsigned char *serNum)
{
    unsigned char status;
    unsigned int recvBits[1];
    unsigned char i;
    unsigned char buff[12];

    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {
        buff[i+2] = *(Sectorkey+i);
    }
    for (i=0; i<4; i++)
    {
        buff[i+8] = *(serNum+i);
    }
    status = ToCard(PCD_AUTHENT, buff, 12, buff, recvBits);

    if ((status != MI_OK) || (!(ReadReg(Status2Reg) & 0x08)))
    {
        status = MI_ERR;
    }

    return status;
}


/*
 * Function：ReadBlock
 * Description：Read data
 * Input parameters：blockAddr--block address;recvData--the block data which are read
 * return：return MI_OK if successed
 */
unsigned char ReadBlock(unsigned char blockAddr, unsigned char *recvData)
{
    unsigned char status;
    unsigned int unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    CalulateCRC(recvData,2, &recvData[2]);
    status = ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90))
    {
        status = MI_ERR;
    }

    return status;
}


/*
 * Function：WriteBlock
 * Description：write block data
 * Input parameters：blockAddr--block address;writeData--Write 16 bytes data into block
 * return：return MI_OK if successed
 */
unsigned char WriteBlock(unsigned char blockAddr, unsigned char *writeData)
{
    unsigned char status;
    unsigned int recvBits[1];
    unsigned char i;
    unsigned char buff[18];

    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalulateCRC(buff, 2, &buff[2]);
    status = ToCard(PCD_TRANSCEIVE, buff, 4, buff, recvBits);

    if ((status != MI_OK) || (recvBits[0] != 4) || ((buff[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        for (i=0; i<16; i++)
        {
            buff[i] = *(writeData+i);
        }
        CalulateCRC(buff, 16, &buff[16]);
        status = ToCard(PCD_TRANSCEIVE, buff, 18, buff, recvBits);

        if ((status != MI_OK) || (recvBits[0] != 4) || ((buff[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }

    return status;
}


/*
 * Function：Halt
 * Description：Command the cards into sleep mode
 * Input parameters：null
 * return：null
 */
void Halt(void)
{
    unsigned int unLen;
    unsigned char buff[4];

    buff[0] = PICC_HALT;
    buff[1] = 0;
    CalulateCRC(buff, 2, &buff[2]);

    ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}
