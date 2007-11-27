#ifndef LCD_H
#define LCD_H 1



#include <AT89S8253.H>
//#include <stdio.h>

unsigned char data LCDCont = 0x00; //00100000, [CS1] [CS2] [write] [command]
unsigned char idata address;

void delay(unsigned long);
void glcd_wc(unsigned char, unsigned char); // write control word to GRAPHIC LCD [CS1/CS2][WORD]
void glcd_fillup(unsigned char); //GRAPHIC LCD fillup with a pattern
unsigned char glcd_rs(unsigned char lcd); // read status from GRAPHIC LCD
//void glcd_wd(unsigned char, unsigned char, unsigned char, unsigned char);
void glcd_wd(unsigned char, unsigned char); // [GLCD][DATA]
// write data word to GRAPHIC LCD, [CS1/CS2][X-pos][Y-pos][DATA].
void glcd_pos(unsigned char, unsigned char, unsigned char);  // [GLCD] [x-pos] [y-pos]
void vos_cls(void); // Clean the game main stage
#endif
