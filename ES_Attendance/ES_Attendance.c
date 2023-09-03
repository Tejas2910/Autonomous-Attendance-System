/*
 * sample.c
 *
 *  Created on: 19-Dec-2022
 *      Author: Aksh Garg
 */

//------------------------------------------
// TivaWare Header Files
//------------------------------------------
//#include "lcd.h"
#include "ST7735.h"
#include "myLib.h"

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
 * VCC -> 3.3V
 *
 */
#define redLED   0x00000002
#define blueLED  0x00000004
#define greenLED 0x00000008

#define CARD_LENGTH 10


void hardware_init(void);
void state1();
void state2(char *course_name);
void state3();
void state4(char *course_name, char *roll_no);
void state5(char *prof_name, char *course_name);
void state6();
void state7();

uint8_t status;
uint32_t readTeste;
unsigned char str[MAX_LEN];
unsigned char cardID[CARD_LENGTH];
unsigned char key[6];

unsigned char rollNo[MAX_LEN];
unsigned char profName[MAX_LEN];
unsigned char code[4];
char courseName[16];
char student[] = "128";
char prof[] = "131";
uint32_t Course[4];


//Library modified to work with CCS



int main(void) {

    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);  // 40MHz
    hardware_init();
    LcdInit(); // 128x160 pixels
    setRotation(1);        // to make screen horizontally.
    //char *prof_name = "Laxmeesha";
    //char *course_name = "EE756";
    //state5(prof_name, course_name);
    state1();
    delay_ms(5000);
//    while(1){
//        delay_ms(50000);
//    }

//    lcd_init();
//    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);

    Init();   // initialize MFRC522


    int te = ReadReg(TxControlReg);
    EEPROMRead(Course, 0x0, 16);
//    memcpy(courseName, Course, 16);
    int i;
    for(i=0; i<4; i++){
        char c1 = (Course[i] & 0xff000000) >> 24;
        if(c1==0) break;
        courseName[i*4 + 0] = c1;
        char c2 = (Course[i] & 0x00ff0000) >> 16;
        if(c2==0) break;
        courseName[i*4 + 1] = c2;
        char c3 = (Course[i] & 0x0000ff00) >> 8;
        if(c3==0) break;
        courseName[i*4 + 2] = c3;
        char c4 = (Course[i] & 0x000000ff);
        if(c4==0) break;
        courseName[i*4 + 3] = c4;
    }
    if(te==131){
//        lcd_string("Scan card to ");
//        lcd_cursor(2,1);
//        lcd_string("Mark Attendance");
        state2(courseName);
    }




    for(i=0; i<6; i++){
        key[i] = 0xFF;
    }

    while(1){
        status = Request(PICC_REQIDL, str);
        if(status == MI_OK){
            status = Anticoll(str);
            memcpy(cardID, str, 10);
            if(status == MI_OK){
                int size = SelectTag(cardID);
//                lcd_init();
                GPIOPinWrite(GPIO_PORTF_BASE, blueLED, blueLED);
                ////state6
//                lcd_string("Card Detected...");
//                lcd_cursor(2,1);
//                lcd_string("Please wait...");
                state6();
                GPIOPinWrite(GPIO_PORTF_BASE, blueLED, 0);


                status = Auth(0x61, 15, key, cardID);
                if(status==MI_OK){
                    int registered = 0;        // 0 for not registered, 1 for student, 2 for professor

                    status = ReadBlock(13, str);
                    if(status!=MI_OK){
                        GPIOPinWrite(GPIO_PORTF_BASE, redLED, redLED);
//                        lcd_init();
//                        lcd_string("Please try again");
//                        lcd_cursor(2,1);
//                        lcd_string("Itni kya jaldi h");
                        state7();
                    }
                    else{
                        memcpy(code, str, 4);
                        if(!strcmp((char *)code, student)){
                            registered = 1;
                        }
                        else if(!strcmp((char *)code, prof)){
                            registered = 2;
                        }
                    }


                    if(registered == 0){
                        GPIOPinWrite(GPIO_PORTF_BASE, redLED, redLED);
//                        lcd_init();
//                        lcd_cursor(1,1);
//                        lcd_string("Who are you?????");
//                        lcd_cursor(2,1);
//                        lcd_string("Contact admin");
                        state3();
                    }
                    else{
                        status = ReadBlock(14, str);
                        if(status!=MI_OK){
                            GPIOPinWrite(GPIO_PORTF_BASE, redLED, redLED);
//                            lcd_init();
//                            lcd_string("Please try again");
//                            lcd_cursor(2,1);
//                            lcd_string("Itni kya jaldi h");
                            state7();
                        }
                        else{
                            memcpy(rollNo, str, 16);
                            GPIOPinWrite(GPIO_PORTF_BASE, greenLED, greenLED);
//                            lcd_init();
                            if(registered==1){               // student
                                uint32_t Course[4];
                                EEPROMRead(Course, 0x0, 16);
//                                lcd_string((char *)rollNo);
//                                lcd_cursor(2,1);
//                                lcd_string("Attendance Done.");


                                state4(courseName, (char *)rollNo);

                                char *roll_no = (char *)rollNo;
                                for(i=0; i<16; i++){
                                    if(courseName[i]==0) break;
                                    UARTCharPut(UART1_BASE, courseName[i]);
                                }
                                UARTCharPut(UART1_BASE, ' ');
                                for(i=0; i<16; i++){
                                    if(roll_no[i]==0) break;
                                    UARTCharPut(UART1_BASE, roll_no[i]);
                                }
                                UARTCharPut(UART1_BASE, '\0');

                            }
                            else{                           // professor
//                                lcd_string("Course: ");
//                                lcd_cursor(1,9);
//                                lcd_string((char *)rollNo);
//                                lcd_cursor(2,1);
//                                lcd_string("now started!");
                                status = ReadBlock(12, str);
                                if(status!=MI_OK){
                                    GPIOPinWrite(GPIO_PORTF_BASE, redLED, redLED);
        //                            lcd_init();
        //                            lcd_string("Please try again");
        //                            lcd_cursor(2,1);
        //                            lcd_string("Itni kya jaldi h");
                                    state7();
                                }
                                else{
                                    memcpy(profName, str, 16);
                                }
                                //  rollNo - course name
                                state5((char *)profName, (char *)rollNo);
                                memcpy(courseName, (char *)rollNo, 16);
                                uint32_t course1 = 0;
                                course1 = course1 | (rollNo[0]) << 24;
                                course1 = course1 | (rollNo[1]) << 16;
                                course1 = course1 | (rollNo[2]) << 8;
                                course1 = course1 | (rollNo[3]);
                                uint32_t course2 = 0;
                                course2 = course2 | (rollNo[4]) << 24;
                                course2 = course2 | (rollNo[5]) << 16;
                                course2 = course2 | (rollNo[6]) << 8;
                                course2 = course2 | (rollNo[7]);
                                uint32_t course3 = 0;
                                uint32_t course4 = 0;
                                EEPROMProgram(&course1, 0x0, 4);
                                EEPROMProgram(&course2, 0x4, 4);
                                EEPROMProgram(&course3, 0x8, 4);
                                EEPROMProgram(&course4, 0x12, 4);
                            }
                        }
                    }

                    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                    Init();
                }
                else{
                    GPIOPinWrite(GPIO_PORTF_BASE, redLED, redLED);
//                    lcd_init();
//                    lcd_string("Please try again");
//                    lcd_cursor(2,1);
//                    lcd_string("Itni kya jaldi h");
                    state7();
                }
                SysCtlDelay(SysCtlClockGet());
                GPIOPinWrite(GPIO_PORTF_BASE, redLED, 0);
                GPIOPinWrite(GPIO_PORTF_BASE, greenLED, 0);
                te = ReadReg(TxControlReg);
                if(te==131){
//                    lcd_init();
//                    lcd_string("Scan card to ");
//                    lcd_cursor(2,1);
//                    lcd_string("Mark Attendance");
                    state2(courseName);
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

///asking for scan
void state2(char *course_name){
    fillScreen(ST7735_BLACK);
    delay_ms(2);
    fillRoundRect(10, 15, 140, 40, 8, ST7735_RED);
    delay_ms(2);
    drawString("Welcome to ", 14, 30, ST7735_BLACK, ST7735_RED, 1);
    delay_ms(2);
    drawString(course_name, 80, 25, ST7735_BLACK, ST7735_RED, 2);
    delay_ms(2);
    fillRoundRect(10, 70, 140, 40, 8, ST7735_WHITE);
    delay_ms(2);
    drawString("Scan Card", 25, 80, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
}


void state3(){
    fillScreen(ST7735_BLACK);
    delay_ms(2);
    fillRoundRect(25, 5, 110, 25, 8, ST7735_RED);
    delay_ms(2);
    drawString("Sorry :(", 30, 8, ST7735_BLACK, ST7735_RED, 2);
    delay_ms(2);
    fillRoundRect(10, 37, 140, 20, 8, ST7735_CYAN);
    delay_ms(2);
    drawString("You are not", 15, 39, ST7735_BLACK, ST7735_CYAN, 2);
    delay_ms(2);
    fillRoundRect(10, 60, 140, 20, 8, ST7735_CYAN);
    delay_ms(2);
    drawString("Registered", 20, 62, ST7735_BLACK, ST7735_CYAN, 2);
    delay_ms(2);
    fillRoundRect(1, 88, 158, 34, 1, ST7735_WHITE);
    delay_ms(2);
    drawString("Contact Admin", 3, 95, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
}

void state4(char *course_name, char *roll_no){
    fillScreen(ST7735_BLACK);
    delay_ms(2);
    fillRoundRect(25, 5, 110, 25, 8, ST7735_RED);
    delay_ms(2);
    drawString(course_name, 50, 9, ST7735_BLACK, ST7735_RED, 2);
    delay_ms(2);
    fillRoundRect(10, 38, 140, 34, 8, ST7735_WHITE);
    delay_ms(2);
    drawString(roll_no, 25, 48, ST7735_BLACK, ST7735_WHITE, 2);
    delay_ms(2);
    fillRoundRect(10, 81, 140, 20, 8, ST7735_CYAN);
    delay_ms(2);
    drawString("Attendance", 20, 83, ST7735_BLACK, ST7735_CYAN, 2);
    delay_ms(2);
    fillRoundRect(10, 104, 140, 20, 8, ST7735_CYAN);
    delay_ms(2);
    drawString("Marked", 44, 106, ST7735_BLACK, ST7735_CYAN, 2);
    delay_ms(2);
}

void state5(char *prof_name, char *course_name){
    fillScreen(ST7735_BLACK);
    delay_ms(2);
    fillRoundRect(10, 5, 140, 25, 8, ST7735_RED);
    delay_ms(2);
    drawString("Hello Prof", 20, 9, ST7735_BLACK, ST7735_RED, 2);
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

void state6(){
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

void state7(){
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
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // initializing EEPROM
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    EEPROMInit();
}

