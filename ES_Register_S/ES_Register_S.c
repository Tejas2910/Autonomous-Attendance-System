/*
 * sample.c
 *
 *  Created on: 19-Dec-2022
 *      Author: Aksh Garg
 */

//------------------------------------------
// TivaWare Header Files
//------------------------------------------
#include "myLib.h"
#include "ST7735.h"
/*PIN Connections:
 * Used SSI0 (Module 0)
 *
 *
 * SDA / CS / FSS ------------ PA3
 * SCK  / CLK     ------------ PA2
 * MOSI / TX      ------------ PA5
 * MISO /  RX     ------------ PA4
 *
 * RST            ------------ PF4 *
 *
 */





/*PIN Connections:
 * Used SSI2 (Module 2)
 *
 *
 * SDA / CS / FSS ------------ PB5
 * SCK  / CLK     ------------ PB4
 * MOSI / TX      ------------ PB7
 * MISO /  RX     ------------ PB6
 *
 * RST            ------------ PF4 *
 *
 */

#define redLED   0x00000002
#define blueLED  0x00000004
#define greenLED 0x00000008

#define CARD_LENGTH 10


void hardware_init(void);
void state1();
void state2();
void state3(char *prof_name, char *course_name);
void state4(char *roll_no);
void state5();
void state6();
void state7();

uint8_t status;
uint32_t readTeste;
unsigned char str[MAX_LEN];
unsigned char cardID[CARD_LENGTH];
unsigned char key[6];
unsigned char first_name[MAX_LEN];
unsigned char rollNo[MAX_LEN];



//Library modified to work with CCS



int main(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);  // 40MHz
    hardware_init();
//    lcd_init();
//    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);
    LcdInit(); // 128x160 pixels
    setRotation(1);        // to make screen horizontally.
    //char *prof_name = "Laxmeesha";
   //char *roll_no = "EE345";
   //state4(roll_no);
   state1();
   delay_ms(5000);
//    while(1){
//        delay_ms(50000);
//    }

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);

    Init();   // initialize MFRC522

    int te = ReadReg(TxControlReg);
    if(te==131){
//        lcd_string("Scan your card");
//        lcd_cursor(2,1);
//        lcd_string("to Register");
        state2();
    }

    int i;
    for(i=0; i<6; i++){
        key[i] = 0xFF;
    }

    while(1){
        status = Request(PICC_REQIDL, str);
        if(status == MI_OK){

            status = Anticoll(str);
            memcpy(cardID, str, 10);
            if(status == MI_OK){
//                lcd_init();
                GPIOPinWrite(GPIO_PORTF_BASE, blueLED, blueLED);
//                lcd_string("Card Detected...");
//                lcd_cursor(2,1);
//                lcd_string("Please wait...");
                state5();
                GPIOPinWrite(GPIO_PORTF_BASE, blueLED, 0);





                unsigned char response;
                UARTCharPut(UART0_BASE, 13);
                UARTCharPut(UART0_BASE, '\n');
                UARTCharPut(UART0_BASE, 'P');
                UARTCharPut(UART0_BASE, 'l');
                UARTCharPut(UART0_BASE, 'e');
                UARTCharPut(UART0_BASE, 'a');
                UARTCharPut(UART0_BASE, 's');
                UARTCharPut(UART0_BASE, 'e');
                UARTCharPut(UART0_BASE, ' ');
                UARTCharPut(UART0_BASE, 'p');
                UARTCharPut(UART0_BASE, 'r');
                UARTCharPut(UART0_BASE, 'e');
                UARTCharPut(UART0_BASE, 's');
                UARTCharPut(UART0_BASE, 's');
                UARTCharPut(UART0_BASE, ' ');
                UARTCharPut(UART0_BASE, 'S');
                UARTCharPut(UART0_BASE, ' ');
                UARTCharPut(UART0_BASE, 'f');
                UARTCharPut(UART0_BASE, 'o');
                UARTCharPut(UART0_BASE, 'r');
                UARTCharPut(UART0_BASE, ' ');
                UARTCharPut(UART0_BASE, 's');
                UARTCharPut(UART0_BASE, 't');
                UARTCharPut(UART0_BASE, 'u');
                UARTCharPut(UART0_BASE, 'd');
                UARTCharPut(UART0_BASE, 'e');
                UARTCharPut(UART0_BASE, 'n');
                UARTCharPut(UART0_BASE, 't');
                UARTCharPut(UART0_BASE, ' ');
                UARTCharPut(UART0_BASE, 'a');
                UARTCharPut(UART0_BASE, 'n');
                UARTCharPut(UART0_BASE, 'd');
                UARTCharPut(UART0_BASE, ' ');
                UARTCharPut(UART0_BASE, 'P');
                UARTCharPut(UART0_BASE, ' ');
                UARTCharPut(UART0_BASE, 'f');
                UARTCharPut(UART0_BASE, 'o');
                UARTCharPut(UART0_BASE, 'r');
                UARTCharPut(UART0_BASE, ' ');
                UARTCharPut(UART0_BASE, 'P');
                UARTCharPut(UART0_BASE, 'r');
                UARTCharPut(UART0_BASE, 'o');
                UARTCharPut(UART0_BASE, 'f');
                UARTCharPut(UART0_BASE, 'e');
                UARTCharPut(UART0_BASE, 's');
                UARTCharPut(UART0_BASE, 's');
                UARTCharPut(UART0_BASE, 'o');
                UARTCharPut(UART0_BASE, 'r');
                UARTCharPut(UART0_BASE, ':');
                UARTCharPut(UART0_BASE, ' ');
                int professor = 0;
                while(1){
                   response = UARTCharGet(UART0_BASE);
                   if(response=='P' | response=='p'){
                       professor=1;
                       break;
                   }
                   if(response=='S' | response=='s'){
                      break;
                   }
                   UARTCharPut(UART0_BASE, 13);
                   UARTCharPut(UART0_BASE, '\n');
                   UARTCharPut(UART0_BASE, 'T');
                   UARTCharPut(UART0_BASE, 'y');
                   UARTCharPut(UART0_BASE, 'p');
                   UARTCharPut(UART0_BASE, 'e');
                   UARTCharPut(UART0_BASE, ' ');
                   UARTCharPut(UART0_BASE, 'S');
                   UARTCharPut(UART0_BASE, ' ');
                   UARTCharPut(UART0_BASE, 'o');
                   UARTCharPut(UART0_BASE, 'r');
                   UARTCharPut(UART0_BASE, ' ');
                   UARTCharPut(UART0_BASE, 'P');
                   UARTCharPut(UART0_BASE, ':');
                   UARTCharPut(UART0_BASE, ' ');
                }
                UARTCharPut(UART0_BASE, 13);
                UARTCharPut(UART0_BASE, '\n');
                unsigned char roll[MAX_LEN];
                unsigned char first[MAX_LEN];
                unsigned char code[4];

                if(!professor){
                    int counts = 0;
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'R');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'l');
                    UARTCharPut(UART0_BASE, 'l');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, '.');
                    UARTCharPut(UART0_BASE, ' ');
                    while(1){
                       unsigned char inp = UARTCharGet(UART0_BASE);
                       if(inp==13 | inp=='\n' | inp=='\t') break;
                       if(counts < MAX_LEN){
                           roll[counts++] = inp;
                       }
                    }
                    for(;counts<MAX_LEN;counts++){
                        roll[counts] = 0;
                    }
                    UARTCharPut(UART0_BASE, '\n');

                    counts = 0;
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'F');
                    UARTCharPut(UART0_BASE, 'i');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'm');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    while(1){
                       unsigned char inp = UARTCharGet(UART0_BASE);
                       if(inp==13 | inp=='\n' | inp=='\t') break;
                       if(counts < MAX_LEN){
                           first[counts++] = inp;
                       }
                    }
                    for(;counts<MAX_LEN;counts++){
                        first[counts] = 0;
                    }
                    UARTCharPut(UART0_BASE, '\n');


                    code[0]='1';
                    code[1]='2';
                    code[2]='8';
                    code[3]='\0';

                }
                else{
                    int counts = 0;
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'F');
                    UARTCharPut(UART0_BASE, 'i');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'N');
                    UARTCharPut(UART0_BASE, 'a');
                    UARTCharPut(UART0_BASE, 'm');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    while(1){
                       unsigned char inp = UARTCharGet(UART0_BASE);
                       if(inp==13 | inp=='\n' | inp=='\t') break;
                       if(counts < MAX_LEN){
                           first[counts++] = inp;
                       }
                    }
                    for(;counts<MAX_LEN;counts++){
                        first[counts] = 0;
                    }
                    UARTCharPut(UART0_BASE, '\n');


                    counts = 0;
                    UARTCharPut(UART0_BASE, 'E');
                    UARTCharPut(UART0_BASE, 'n');
                    UARTCharPut(UART0_BASE, 't');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'u');
                    UARTCharPut(UART0_BASE, 'r');
                    UARTCharPut(UART0_BASE, 's');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, ' ');
                    UARTCharPut(UART0_BASE, 'C');
                    UARTCharPut(UART0_BASE, 'o');
                    UARTCharPut(UART0_BASE, 'd');
                    UARTCharPut(UART0_BASE, 'e');
                    UARTCharPut(UART0_BASE, ':');
                    UARTCharPut(UART0_BASE, ' ');
                    while(1){
                       unsigned char inp = UARTCharGet(UART0_BASE);
                       if(inp==13 | inp=='\n' | inp=='\t') break;
                       if(counts < MAX_LEN){
                           roll[counts++] = inp;
                       }
                    }
                    for(;counts<MAX_LEN;counts++){
                        roll[counts] = 0;
                    }
                    UARTCharPut(UART0_BASE, '\n');

                    code[0]='1';
                    code[1]='3';
                    code[2]='1';
                    code[3]='\0';
                }





                int size = SelectTag(cardID);
                status = Auth(0x61, 15, key, cardID);
                if(status==MI_OK){
                    status = WriteBlock(12, first);
                    if(status!=MI_OK){
                        GPIOPinWrite(GPIO_PORTF_BASE, redLED, redLED);
//                        lcd_init();
//                        lcd_string("Please try again");
//                        lcd_cursor(2,1);
//                        lcd_string("Itni kya jaldi h");
                        state6();
                    }

                    status = WriteBlock(13, code);
                    if(status!=MI_OK){
                        GPIOPinWrite(GPIO_PORTF_BASE, redLED, redLED);
//                        lcd_init();
//                        lcd_string("Please try again");
//                        lcd_cursor(2,1);
//                        lcd_string("Itni kya jaldi h");
                        state6();
                    }

                    status = WriteBlock(14, roll);
                    if(status!=MI_OK){
                        GPIOPinWrite(GPIO_PORTF_BASE, redLED, redLED);
//                        lcd_init();
//                        lcd_string("Please try again");
//                        lcd_cursor(2,1);
//                        lcd_string("Itni kya jaldi h");
                        state6();
                    }
                    status = ReadBlock(14, str);
                    if(status!=MI_OK){
                        GPIOPinWrite(GPIO_PORTF_BASE, redLED, redLED);
//                        lcd_init();
//                        lcd_string("Please try again");
//                        lcd_cursor(2,1);
//                        lcd_string("Itni kya jaldi h");
                        state6();
                    }
                    else{
                        memcpy(rollNo, str, 16);
                        GPIOPinWrite(GPIO_PORTF_BASE, greenLED, greenLED);
//                        lcd_init();
//                        lcd_string((char *)rollNo);
//                        lcd_cursor(2,1);
//                        lcd_string("RegistrationDone");
                        state4((char *)rollNo);
                    }

                    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                    Init();
                }
                else{
                    GPIOPinWrite(GPIO_PORTF_BASE, redLED, redLED);
//                    lcd_init();
//                    lcd_string("Authentication");
//                    lcd_cursor(2,1);
//                    lcd_string("Failed!");
                    state7();

                }
                SysCtlDelay(SysCtlClockGet()); //Delay 3 sec
                GPIOPinWrite(GPIO_PORTF_BASE, redLED, 0);
                GPIOPinWrite(GPIO_PORTF_BASE, greenLED, 0);
                te = ReadReg(TxControlReg);
                if(te==131){
//                    lcd_init();
//                    lcd_string("Scan your card");
//                    lcd_cursor(2,1);
//                    lcd_string("to Register");
                    state2();
                }
            }
        }


    }
}

// team details
void state1(){

    fillScreen(ST7735_BLACK);
    delay_ms(2);
    fillRoundRect(25, 10, 110, 30, 8, ST7735_RED);
    delay_ms(2);
    drawString("Group 14", 35, 17, ST7735_BLACK, ST7735_RED, 2);
    delay_ms(2);
    fillRoundRect(20, 50, 120, 20, 4, ST7735_WHITE);
    delay_ms(2);
    drawString("Tejas", 50, 52, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
    fillRoundRect(20, 75, 120, 20, 4, ST7735_WHITE);
    delay_ms(2);
    drawString("Aksh", 56, 77, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
    fillRoundRect(20, 100, 120, 20, 4, ST7735_WHITE);
    delay_ms(2);
    drawString("Pavan", 50, 102, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
}

void state2(){
    fillScreen(ST7735_BLACK);
    delay_ms(2);
    fillRoundRect(20, 10, 120, 30, 8, ST7735_WHITE);
    delay_ms(2);
    drawString("Scan Your", 26, 17, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
    fillRoundRect(20, 50, 120, 30, 8, ST7735_WHITE);
    delay_ms(2);
    drawString("Card to", 38, 57, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
    fillRoundRect(20, 90, 120, 30, 8, ST7735_WHITE);
    delay_ms(2);
    drawString("Register", 32, 97, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
}

void state3(char *prof_name, char *course_name){
    fillScreen(ST7735_BLACK);
    delay_ms(2);
    fillRoundRect(10, 5, 140, 25, 8, ST7735_RED);
    delay_ms(2);
    drawString("Prof Name", 26, 9, ST7735_BLACK, ST7735_RED, 2);
    delay_ms(2);
    fillRoundRect(10, 38, 140, 34, 8, ST7735_WHITE);
    delay_ms(2);
    drawString(prof_name, 26, 47, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
    fillRoundRect(10, 81, 140, 20, 8, ST7735_CYAN);
    delay_ms(2);
    drawString("Course Code", 14, 83, ST7735_BLACK, ST7735_CYAN, 2);
    delay_ms(2);
    fillRoundRect(10, 104, 140, 20, 8, ST7735_CYAN);
    delay_ms(2);
    drawString(course_name, 50, 106, ST7735_BLACK, ST7735_CYAN, 2);
    delay_ms(2);
}

void state4(char *roll_no){
    fillScreen(ST7735_BLACK);
    delay_ms(2);
    fillRoundRect(20, 5, 120, 25, 8, ST7735_RED);
    delay_ms(2);
    drawString("Thank You", 26, 9, ST7735_BLACK, ST7735_RED, 2);
    delay_ms(2);
    fillRoundRect(10, 38, 140, 34, 8, ST7735_WHITE);
    delay_ms(2);
    drawString(roll_no, 26, 47, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
    fillRoundRect(4, 81, 152, 20, 4, ST7735_CYAN);
    delay_ms(2);
    drawString("Registered", 20, 83, ST7735_BLACK, ST7735_CYAN, 2);
    delay_ms(2);
    fillRoundRect(4, 104, 152, 20, 4, ST7735_CYAN);
    delay_ms(2);
    drawString("Successfully", 8, 106, ST7735_BLACK, ST7735_CYAN, 2);
    delay_ms(2);
}

void state5(){
    fillScreen(ST7735_BLACK);
    delay_ms(2);
    fillRoundRect(25, 15, 110, 25, 8, ST7735_WHITE);
    delay_ms(2);
    drawString("Card is", 38, 19, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
    fillRoundRect(25, 50, 110, 25, 8, ST7735_WHITE);
    delay_ms(2);
    drawString("Detected", 32, 54, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
    fillRoundRect(10, 95, 140, 25, 8, ST7735_CYAN);
    delay_ms(2);
    drawString("Please Wait", 14, 99, ST7735_BLACK, ST7735_CYAN, 2);
    delay_ms(2);
}

void state6(){
    fillScreen(ST7735_BLACK);
    delay_ms(2);
    fillRoundRect(20, 8, 120, 25, 8, ST7735_WHITE);
    delay_ms(2);
    drawString("Please", 44, 12, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
    fillRoundRect(20, 38, 120, 25, 8, ST7735_WHITE);
    delay_ms(2);
    drawString("Try Again", 26, 42, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
    fillRoundRect(20, 73, 120, 25, 8, ST7735_CYAN);
    delay_ms(2);
    drawString("Itni Kya", 32, 77, ST7735_BLACK, ST7735_CYAN, 2);
    delay_ms(2);
    fillRoundRect(20, 103, 120, 25, 8, ST7735_CYAN);
    delay_ms(2);
    drawString("Jaldi He", 32, 107, ST7735_BLACK, ST7735_CYAN, 2);
    delay_ms(2);
}


void state7(){
    fillScreen(ST7735_BLACK);
    delay_ms(2);
    fillRoundRect(20, 10, 120, 30, 8, ST7735_RED);
    delay_ms(2);
    drawString("Authenti-", 26, 17, ST7735_BLACK, ST7735_RED, 2);
    delay_ms(2);
    fillRoundRect(20, 50, 120, 30, 8, ST7735_RED);
    delay_ms(2);
    drawString("fication", 32, 57, ST7735_BLACK, ST7735_RED, 2);
    delay_ms(2);
    fillRoundRect(20, 90, 120, 30, 8, ST7735_RED);
    delay_ms(2);
    drawString("Failed", 44, 97, ST7735_BLACK, ST7735_RED, 2);
    delay_ms(2);
}

void hardware_init(void){
        // SPI Initialize
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);  //SDA
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);  // SPI
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // RESET
    GPIOPinConfigure(GPIO_PB4_SSI2CLK);
//    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PB7_SSI2TX);
    GPIOPinConfigure(GPIO_PB6_SSI2RX);
    GPIOPinTypeSSI(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5); //chipSelectPin
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //NRSTPD and LED Ports
    SSIConfigSetExpClk(SSI2_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 9600, 8);
    SSIEnable(SSI2_BASE);




    // to initialize UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // initializing EEPROM
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    EEPROMInit();
}

