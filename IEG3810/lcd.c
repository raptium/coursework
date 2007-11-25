#include "lcd.h"

/* Delay program */
void delay(unsigned long value) {
    while (value > 0) value--;
}

/* write command word to GRAPHIC LCD*/
void glcd_wc(unsigned char lcd, unsigned char cTempdata) {
    TR0 = 0; //temperoray stop timer0 (dot matrix LED)
    if (lcd == 2) P0 = (LCDCont & 0x0F) | 0x80; //CONTROL WORD=1000xxxx, [CS2] [CS1] [write] [command]
    else 		P0 = (LCDCont & 0x0F) | 0x40; //CONTROL WORD=0100xxxx, [CS2] [CS1] [write] [command]
    P2 = (address & 0xF0) | 0x0E;	//latch LCDCont to U20
    P2 = address; //unselect U20
    P0 = cTempdata;
    P2 = (address & 0xF0) | 0x0B; 	//select (pulse trigger) Graphic LCD
    P2 = address;
    TR0 = 1; //resume dot matrix LED scanning
}

/* read status word of GRAPHIC LCD*/
unsigned char glcd_rs(unsigned char lcd) {
    unsigned char cTemp;
    TR0 = 0; //temperoray stop timer0 (dot matrix LED)
    if (lcd == 2) P0 = (LCDCont & 0x0F) | 0xA0; //CONTROL WORD=1000xxxx, [CS2] [CS1] [read] [command]
    else 		P0 = (LCDCont & 0x0F) | 0x60; //CONTROL WORD=0100xxxx, [CS2] [CS1] [read] [command]
    P2 = (address & 0xF0) | 0x0E;	//latch LCDCont to U20
    P2 = address; //unselect U20
    P0 = 0xff;
    P2 = (address & 0xF0) | 0x0B; 	//select (pulse trigger) Graphic LCD
    cTemp = P0;
    P2 = address;
    TR0 = 1; //resume dot matrix LED scanning
    return cTemp;
}

/* write data to GRAPHIC LCD */
void glcd_wd(unsigned char lcd, unsigned char cTempdata) {
    TR0 = 0; //temperoray stop timer0 (dot matrix LED)
    if (lcd == 3) 		P0 = (LCDCont & 0x0F) | 0xD0; //CONTROL WORD=1001xxxx, [CS2=1] [CS1=1] [write=0] [data=1]xxxx
    else if (lcd == 2)	P0 = (LCDCont & 0x0F) | 0x90; //CONTROL WORD=1001xxxx, [CS2=1] [CS1=0] [write=0] [data=1]xxxx
    else 				P0 = (LCDCont & 0x0F) | 0x50; //CONTROL WORD=0101xxxx, [CS2=0] [CS1=1] [write=0] [data=1]xxxx
    P2 = (address & 0xF0) | 0x0E;  // latch LCDCont to U20
    P2 = address; // unselect U20
    P0 = cTempdata;
    P2 = (address & 0xF0) | 0x0B; 	//select (pulse trigger) Graphic LCD
    P2 = address;
    TR0 = 1; //resume dot matrix LED scanning
    //delay(1);
}

/* select location of GRAPHIC LCD  */
void glcd_pos(unsigned char lcd, unsigned char xpos, unsigned char ypos) {
    TR0 = 0; //temperoray stop timer0 (dot matrix LED)
    if (lcd == 3) 		P0 = (LCDCont & 0x0F) | 0xC0; //CONTROL WORD=1000xxxx, [CS2=1] [CS1=1] [write=0] [command=0]
    else if (lcd == 2)	P0 = (LCDCont & 0x0F) | 0x80; //CONTROL WORD=1000xxxx, [CS2=1] [CS1=0] [write=0] [command=0]
    else 				P0 = (LCDCont & 0x0F) | 0x40; //CONTROL WORD=0100xxxx, [CS2=0] [CS1=1] [write=0] [command=0]
    P2 = (address & 0xF0) | 0x0E;  // latch LCDCont to U20
    P2 = address; // unselect U20
    P0 = (ypos & 0x3F) | 0x40; // form Y-position pattern, 0~63
    P2 = (address & 0xF0) | 0x0B; //select graphic LCD (latch y-position)
    P2 = address; //  unselect (pulse trigger)graphic LCD
    delay(1); //little delay for position change
    P0 = (xpos & 0x07) | 0xB8;  //form X-position pattern, 0~7
    P2 = (address & 0xF0) | 0x0B; //select graphic LCD (latch x-position)
    P2 = address; //  unselect (pulse trigger)graphic LCD
    TR0 = 1; //resume dot matrix LED scanning
}

/* Clear GRAPHIC LCD */
void glcd_fillup(unsigned char pattern) {
    unsigned char counter1, counter2;
    for (counter1 = 0; counter1 < 8; counter1++) { //x-pos
        glcd_pos(3, counter1, 0);
        for (counter2 = 0; counter2 < 64; counter2++) {
            glcd_wd(3, pattern); //y-pos auto increase 1
        }
    }
}

/* stage clear*/
void vos_cls(void) {
    unsigned char i, j;
    for (i = 1;i < 7;i++) {
        glcd_pos(3, i, 0);
        for (j = 0;j < 39;j++)
            glcd_wd(3, 0x00);
        glcd_pos(2,	i, 39);
        for (j = 39;j < 64;j++)
            glcd_wd(2, 0x00);
    }
}

