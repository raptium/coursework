#include "lcd.h"
#include <math.h>

idata unsigned char stage[6][13]={{0}};
xdata unsigned int hiScore[3];
xdata unsigned char first;
code unsigned char boy[] = {0x1e, 0x1e, 0x0c, 0x1e, 0x2d, 0x0c, 0x12, 0x21};
char boyX=0; // boy x offset 0~42
unsigned char empty[6] = {0};
char s; // stage bottom y
char life=20; // initial lives
code unsigned char d_01[] = {0x72,0x56,0x52,0x52,0x77};
code unsigned char d_23[] = {0x77,0x11,0x77,0x41,0x77};
code unsigned char d_45[] = {0x57,0x54,0x77,0x11,0x17};
code unsigned char d_67[] = {0x77,0x41,0x72,0x52,0x72};
code unsigned char d_89[] = {0x77,0x55,0x77,0x51,0x77};

unsigned char rnd(unsigned char n){
  static float seed=10.0;
  seed=(sinf(seed)*n);
  if(seed<0)
    seed*=-1;
  return (unsigned char)seed;
}

unsigned char key_rd(void){
  unsigned char cTemp;
  TR0 = 0; //temperoray stop timer0 (dot matrix LED)
  P0 = 0xff;
  P2 = (address & 0xF0) | 0x03;
  cTemp = P0;
  P2 = address;
  TR0 = 1; //resume dot matrix LED scanning
  return cTemp;
}

void print2d(unsigned char n, unsigned char lcd){
  unsigned char ch,m;
  unsigned char i;
  
  if(!n)
	return;

  for(i=0;i<5;i++){
    ch=0;
    m=n/10;
labelSw:
    switch(m){
      case 0:
        ch|=d_01[i]>>4;
        break;
      case 1:
        ch|=(d_01[i]&0x0f);
        break;
      case 2:
        ch|=d_23[i]>>4;
        break;
      case 3:
        ch|=(d_23[i]&0x0f);
        break;
      case 4:
        ch|=d_45[i]>>4;
        break;
      case 5:
        ch|=(d_45[i]&0x0f);
        break;
      case 6:
        ch|=d_67[i]>>4;
        break;
      case 7:
        ch|=(d_67[i]&0x0f);
        break;
      case 8:
        ch|=d_89[i]>>4;
        break;
      case 9:
        ch|=(d_89[i]&0x0f);
    }
    if((ch&0xf0)==0){
      ch<<=4;
      m=n%10;
      goto labelSw;
    }
	if(n<10)
	  ch&=0x0f;
    glcd_wd(lcd,ch);
  }

}

void printHiScore(void){
  unsigned int s;
  unsigned int i,x;

  EECON|=0x08; // enable EEPROM
  for(i=0;i<3;i++){
    x=0;
	s=hiScore[i];
	delay(500);
    while(s){
	  glcd_pos(1,5,i*7);
	  print2d(i+1,1);
      glcd_pos(1,2+x,i*7);
      print2d(s%100,1);
      x++;
      s/=100;
    }
  }
  EECON&=~0x08; // disable EEPROM

}

void updateHiScore(unsigned int s){
  unsigned int a,b,c;

  EECON|=0x08; // enable EEPROM
  a=hiScore[0];
  b=hiScore[1];
  c=hiScore[2];
  EECON&=~0x08; // disable EEPROM


	if(s>a){
		c=b;
		b=a;
		a=s;
	}
	else if(s>b){
		c=b;
		b=s;
		}
	else if(s>c)
		c=s;


  EECON|=0x08; // EEMEN=1 
  EECON|=0x10; // EEMWE=1; 
  hiScore[0]=a;
  while((EECON&0x02)==0){
	_asm
	nop
	_endasm;
  } // wait until not busy
  hiScore[1]=b;
  while((EECON&0x02)==0){
	_asm
	nop
	_endasm;
  } // wait until not busy
  hiScore[2]=c;
  while((EECON&0x02)==0){
	_asm
	nop
	_endasm;
  } // wait until not busy
  EECON&=~0x10; // EEMWE=0; 
  EECON&=~0x08; // EEMEN=0 
}





void bootScreen(void){
  unsigned char i;

  glcd_pos(2, 0, 7);
  glcd_wd(2, 0x30);
  glcd_wd(2, 0xd0);
  glcd_wd(2, 0x10);
  glcd_wd(2, 0x20);
  glcd_wd(2, 0x40);
  glcd_wd(2, 0x20);
  glcd_wd(2, 0x10);
  glcd_wd(2, 0x08);
  glcd_wd(2, 0x78);
  glcd_wd(2, 0x80);
  glcd_wd(2, 0x80);
  glcd_pos(2, 0, 30);
  glcd_wd(2, 0xc0);
  glcd_wd(2, 0xc0);
  glcd_wd(2, 0x80);
  glcd_wd(2, 0x80);
  glcd_pos(2, 0, 63);
  glcd_wd(2, 0xff);
  glcd_pos(1, 0, 55);
  glcd_wd(1, 0xad);
  glcd_pos(2, 1, 4);
  glcd_wd(2, 0x02);
  glcd_wd(2, 0x03);
  glcd_wd(2, 0x05);
  glcd_wd(2, 0x05);
  glcd_wd(2, 0x04);
  glcd_wd(2, 0x7c);
  glcd_wd(2, 0x30);
  glcd_wd(2, 0x0c);
  glcd_wd(2, 0x08);
  glcd_wd(2, 0x08);
  glcd_wd(2, 0x10);
  glcd_wd(2, 0x14);
  glcd_wd(2, 0x1a);
  glcd_wd(2, 0x11);
  glcd_wd(2, 0x01);
  glcd_pos(2, 1, 30);
  glcd_wd(2, 0xf1);
  glcd_wd(2, 0x73);
  glcd_wd(2, 0x73);
  glcd_wd(2, 0x77);
  glcd_wd(2, 0x3f);
  glcd_wd(2, 0x3f);
  glcd_wd(2, 0x3e);
  glcd_wd(2, 0x1e);
  glcd_wd(2, 0x1c);
  glcd_wd(2, 0x3c);
  glcd_wd(2, 0xf8);
  glcd_wd(2, 0xf0);
  glcd_pos(2, 1, 63);
  glcd_wd(2, 0xff);
  glcd_pos(1, 1, 38);
  glcd_wd(1, 0x7c);
  glcd_wd(1, 0x40);
  glcd_wd(1, 0x7c);
  glcd_wd(1, 0x04);
  glcd_wd(1, 0x7c);
  glcd_pos(1, 1, 55);
  glcd_wd(1, 0xb5);
  glcd_pos(2, 2, 11);
  glcd_wd(2, 0x70);
  glcd_wd(2, 0xf0);
  glcd_wd(2, 0xe0);
  glcd_wd(2, 0xc0);
  glcd_wd(2, 0xc0);
  glcd_wd(2, 0xe0);
  glcd_wd(2, 0xf0);
  glcd_wd(2, 0x78);
  glcd_pos(2, 2, 30);
  glcd_wd(2, 0xf8);
  glcd_wd(2, 0xfc);
  glcd_wd(2, 0xce);
  glcd_wd(2, 0x8e);
  glcd_wd(2, 0x8e);
  glcd_wd(2, 0xce);
  glcd_wd(2, 0xfc);
  glcd_wd(2, 0xf8);
  glcd_pos(2, 2, 63);
  glcd_wd(2, 0xff);
  glcd_pos(1, 2, 14);
  glcd_wd(1, 0xdf);
  glcd_wd(1, 0x44);
  glcd_wd(1, 0xc4);
  glcd_wd(1, 0x84);
  glcd_wd(1, 0x84);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0xd0);
  glcd_wd(1, 0x10);
  glcd_wd(1, 0xd0);
  glcd_wd(1, 0x10);
  glcd_wd(1, 0xdf);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0xde);
  glcd_wd(1, 0x11);
  glcd_wd(1, 0xd1);
  glcd_wd(1, 0x11);
  glcd_wd(1, 0xde);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0xdf);
  glcd_wd(1, 0x50);
  glcd_wd(1, 0xdf);
  glcd_wd(1, 0x90);
  glcd_wd(1, 0x9f);
  glcd_pos(1, 2, 55);
  glcd_wd(1, 0xd6);
  glcd_pos(2, 3, 7);
  for(i=0;i<3;i++)
    glcd_wd(2, 0x0e);
  glcd_wd(2, 0xce);
  glcd_wd(2, 0xce);
  glcd_wd(2, 0x0f);
  glcd_wd(2, 0x0f);
  glcd_wd(2, 0x0f);
  glcd_wd(2, 0xcf);
  glcd_wd(2, 0xce);
  glcd_wd(2, 0xce);
  glcd_pos(2, 3, 27);
  glcd_wd(2, 0xe0);
  glcd_wd(2, 0xf0);
  glcd_wd(2, 0xf0);
  glcd_wd(2, 0xf0);
  glcd_wd(2, 0xc1);
  glcd_wd(2, 0xe3);
  glcd_wd(2, 0x73);
  glcd_wd(2, 0x73);
  glcd_wd(2, 0x73);
  glcd_wd(2, 0xe1);
  glcd_wd(2, 0xc0);
  glcd_pos(2, 3, 63);
  glcd_wd(2, 0xff);
  glcd_pos(1, 3, 14);
  glcd_wd(1, 0xf7);
  glcd_wd(1, 0x14);
  glcd_wd(1, 0xf7);
  glcd_wd(1, 0x14);
  glcd_wd(1, 0x14);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x17);
  glcd_wd(1, 0x14);
  glcd_wd(1, 0x17);
  glcd_wd(1, 0xa4);
  glcd_wd(1, 0x47);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0xf7);
  glcd_wd(1, 0x04);
  glcd_wd(1, 0xf7);
  glcd_wd(1, 0x04);
  glcd_wd(1, 0xf7);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0xf7);
  glcd_wd(1, 0x14);
  glcd_wd(1, 0x17);
  glcd_wd(1, 0x14);
  glcd_wd(1, 0xf4);
  glcd_pos(1, 3, 55);
  glcd_wd(1, 0x5a);
  glcd_pos(2, 4, 11);
  glcd_wd(2, 0x07);
  glcd_wd(2, 0x0f);
  glcd_wd(2, 0x9e);
  glcd_wd(2, 0x9c);
  glcd_wd(2, 0x9c);
  glcd_wd(2, 0x9e);
  glcd_wd(2, 0x0f);
  glcd_wd(2, 0x07);
  glcd_pos(2, 4, 27);
  glcd_wd(2, 0x1f);
  glcd_wd(2, 0x1f);
  glcd_wd(2, 0x1c);
  glcd_wd(2, 0x1c);
  glcd_wd(2, 0x1f);
  glcd_wd(2, 0x1f);
  glcd_wd(2, 0x1c);
  glcd_wd(2, 0x1c);
  glcd_wd(2, 0x1c);
  glcd_wd(2, 0x1f);
  glcd_wd(2, 0x1f);
  glcd_pos(2, 4, 63);
  glcd_wd(2, 0xff);
  glcd_pos(1, 4, 14);
  glcd_wd(1, 0x7d);
  for(i=0;i<3;i++)
    glcd_wd(1, 0x11);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x7d);
  glcd_wd(1, 0x41);
  glcd_wd(1, 0x7d);
  glcd_wd(1, 0x40);
  glcd_wd(1, 0x7c);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x7d);
  glcd_wd(1, 0x45);
  glcd_wd(1, 0x7d);
  glcd_wd(1, 0x41);
  glcd_wd(1, 0x41);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x7d);
  glcd_wd(1, 0x41);
  glcd_wd(1, 0x41);
  glcd_wd(1, 0x41);
  glcd_wd(1, 0x7d);
  glcd_pos(1, 4, 55);
  glcd_wd(1, 0x6b);
  glcd_pos(2, 5, 11);
  glcd_wd(2, 0x3e);
  glcd_wd(2, 0x7f);
  glcd_wd(2, 0xf3);
  glcd_wd(2, 0xe3);
  glcd_wd(2, 0xe3);
  glcd_wd(2, 0xf3);
  glcd_wd(2, 0x7f);
  glcd_wd(2, 0x3e);
  glcd_pos(2, 5, 39);
  glcd_wd(2, 0xe0);
  glcd_wd(2, 0x40);
  for(i=0;i<4;i++)
    glcd_wd(2, 0x20);
  glcd_wd(2, 0x40);
  glcd_wd(2, 0x80);
  glcd_pos(2, 5, 60);
  glcd_wd(2, 0x40);
  glcd_wd(2, 0xc0);
  glcd_wd(2, 0x80);
  glcd_wd(2, 0xff);
  glcd_pos(1, 5, 14);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x10);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x01);
  glcd_wd(1, 0x5f);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  for(i=0;i<3;i++)
    glcd_wd(1, 0x10);
  glcd_wd(1, 0x5f);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x10);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x01);
  glcd_wd(1, 0x5f);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x10);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x01);
  glcd_wd(1, 0x5f);
  glcd_pos(1, 5, 55);
  glcd_wd(1, 0xad);
  glcd_pos(2, 6, 8);
  glcd_wd(2, 0xf8);
  glcd_wd(2, 0xfc);
  glcd_wd(2, 0x3c);
  glcd_wd(2, 0x3c);
  glcd_wd(2, 0x3c);
  glcd_wd(2, 0xf8);
  glcd_wd(2, 0xf0);
  glcd_wd(2, 0x78);
  glcd_wd(2, 0x38);
  glcd_wd(2, 0x3c);
  glcd_wd(2, 0x1e);
  glcd_pos(2, 6, 37);
  glcd_wd(2, 0x7f);
  glcd_wd(2, 0x7f);
  glcd_wd(2, 0xff);
  glcd_wd(2, 0x22);
  glcd_wd(2, 0x36);
  glcd_wd(2, 0x2a);
  glcd_wd(2, 0x22);
  glcd_wd(2, 0x3e);
  glcd_wd(2, 0x08);
  glcd_wd(2, 0x08);
  glcd_wd(2, 0xfd);
  glcd_wd(2, 0x9f);
  for(i=0;i<3;i++)
    glcd_wd(2, 0x9c);
  glcd_wd(2, 0x7c);
  glcd_wd(2, 0x1c);
  glcd_wd(2, 0x36);
  glcd_wd(2, 0x22);
  glcd_wd(2, 0x22);
  glcd_wd(2, 0x22);
  glcd_wd(2, 0x63);
  glcd_wd(2, 0x63);
  glcd_wd(2, 0xe3);
  glcd_wd(2, 0xe3);
  glcd_wd(2, 0xff);
  glcd_pos(1, 6, 14);
  for(i=0;i<4;i++)
    glcd_wd(1, 0x01);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x01);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x10);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x01);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x01);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x00);
  glcd_wd(1, 0x11);
  glcd_wd(1, 0x11);
  glcd_wd(1, 0x1f);
  glcd_wd(1, 0x01);
  glcd_wd(1, 0x01);
  glcd_pos(1, 6, 55);
  glcd_wd(1, 0xb5);
  glcd_pos(2, 7, 8);
  for(i=0;i<10;i++)
    glcd_wd(2, 0x07);
  glcd_pos(2, 7, 39);
  glcd_wd(2, 0x03);
  glcd_pos(2, 7, 60);
  glcd_wd(2, 0x01);
  glcd_wd(2, 0x01);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0xff);
  glcd_pos(1, 7, 14);
  glcd_wd(1, 0x04);
  glcd_wd(1, 0x3e);
  glcd_wd(1, 0x3f);
  glcd_wd(1, 0x3e);
  glcd_wd(1, 0x04);
  glcd_pos(1, 7, 55);
  glcd_wd(1, 0xd6);
}

void gameScreen(void){
  unsigned char i;

  glcd_pos(2, 0, 0);
  glcd_wd(2, 0xf0);
  glcd_wd(2, 0xfc);
  glcd_wd(2, 0xfe);
  glcd_wd(2, 0xfe);
  for(i=0;i<24;i++)
    glcd_wd(2, 0xff);
  glcd_wd(2, 0xc1);
  glcd_wd(2, 0xf5);
  glcd_wd(2, 0xf5);
  glcd_wd(2, 0xc5);
  glcd_wd(2, 0xd1);
  glcd_wd(2, 0xff);
  glcd_wd(2, 0xc1);
  glcd_wd(2, 0xed);
  glcd_wd(2, 0xed);
  glcd_wd(2, 0xed);
  glcd_wd(2, 0xc1);
  glcd_wd(2, 0xff);
  glcd_wd(2, 0xc1);
  glcd_wd(2, 0xed);
  glcd_wd(2, 0xed);
  glcd_wd(2, 0xed);
  glcd_wd(2, 0xe1);
  glcd_wd(2, 0xff);
  glcd_wd(2, 0xfd);
  glcd_wd(2, 0xfd);
  glcd_wd(2, 0xc1);
  glcd_wd(2, 0xfd);
  glcd_wd(2, 0xfd);
  glcd_wd(2, 0xff);
  glcd_wd(2, 0xdd);
  glcd_wd(2, 0xc1);
  glcd_wd(2, 0xdd);
  glcd_wd(2, 0xff);
  glcd_wd(2, 0xc1);
  glcd_wd(2, 0xdf);
  glcd_wd(2, 0xdf);
  glcd_wd(2, 0xdf);
  glcd_wd(2, 0xc1);
  glcd_wd(2, 0xff);
  glcd_wd(2, 0xc1);
  glcd_pos(1, 0, 0);
  glcd_wd(1, 0xfb);
  glcd_wd(1, 0xf7);
  glcd_wd(1, 0xfb);
  glcd_wd(1, 0xc1);
  for(i=0;i<31;i++)
    glcd_wd(1, 0xff);
  glcd_wd(1, 0xfe);
  glcd_wd(1, 0xfe);
  glcd_wd(1, 0xfc);
  glcd_wd(1, 0xf0);
  glcd_pos(1, 1, 39);
  glcd_wd(1, 0xff);
  glcd_pos(1, 2, 39);
  glcd_wd(1, 0xff);
  glcd_pos(1, 3, 39);
  glcd_wd(1, 0xff);
  glcd_pos(1, 4, 39);
  glcd_wd(1, 0xff);
  glcd_pos(1, 5, 39);
  glcd_wd(1, 0xff);
  glcd_pos(1, 6, 39);
  glcd_wd(1, 0xff);
  glcd_pos(2, 7, 0);
  glcd_wd(2, 0x0f);
  glcd_wd(2, 0x3f);
  glcd_wd(2, 0x7f);
  for(i=0;i<36;i++)
    glcd_wd(2, 0xff);
  for(i=0;i<3;i++)
    glcd_wd(2, 0xab);
  glcd_wd(2, 0x83);
  glcd_wd(2, 0xff);
  for(i=0;i<3;i++)
    glcd_wd(2, 0xbb);
  glcd_wd(2, 0x83);
  glcd_wd(2, 0xff);
  glcd_wd(2, 0xbb);
  glcd_wd(2, 0x83);
  glcd_wd(2, 0xbb);
  for(i=0;i<7;i++)
    glcd_wd(2, 0xff);
  glcd_wd(2, 0xff);
  glcd_pos(1, 7, 0);
  for(i=0;i<35;i++)
    glcd_wd(1, 0xff);
  glcd_wd(1, 0x7f);
  glcd_wd(1, 0x7f);
  glcd_wd(1, 0x3f);
  glcd_wd(1, 0x0f);
}

void gameOver(void){
  unsigned char i;

  glcd_pos(2, 1, 31);
  glcd_wd(2, 0x80);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0x80);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0x80);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0x80);
  glcd_wd(2, 0x80);
  glcd_wd(2, 0x80);
  glcd_pos(2, 2, 31);
  glcd_wd(2, 0x3f);
  glcd_wd(2, 0x30);
  glcd_wd(2, 0x3f);
  glcd_wd(2, 0x30);
  glcd_wd(2, 0x30);
  glcd_wd(2, 0x3f);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0x3f);
  glcd_wd(2, 0x31);
  glcd_wd(2, 0x3f);
  glcd_wd(2, 0x33);
  glcd_wd(2, 0x33);
  glcd_wd(2, 0x33);
  glcd_pos(2, 3, 31);
  glcd_wd(2, 0xff);
  for(i=0;i<4;i++)
    glcd_wd(2, 0xdb);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0x7f);
  glcd_wd(2, 0x60);
  glcd_wd(2, 0x7f);
  glcd_wd(2, 0x60);
  glcd_wd(2, 0x60);
  glcd_wd(2, 0x7f);
  glcd_pos(2, 4, 31);
  glcd_wd(2, 0xfe);
  glcd_wd(2, 0xc6);
  glcd_wd(2, 0xfe);
  glcd_wd(2, 0xc6);
  glcd_wd(2, 0xc6);
  glcd_wd(2, 0xc6);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0x00);
  for(i=0;i<3;i++)
    glcd_wd(2, 0x86);
  glcd_wd(2, 0xfc);
  glcd_wd(2, 0x30);
  glcd_pos(2, 5, 31);
  glcd_wd(2, 0xfc);
  glcd_wd(2, 0x80);
  glcd_wd(2, 0xfc);
  glcd_wd(2, 0x8c);
  glcd_wd(2, 0x8c);
  glcd_wd(2, 0xfc);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0xfd);
  glcd_wd(2, 0x8d);
  glcd_wd(2, 0x8d);
  glcd_wd(2, 0x8d);
  glcd_wd(2, 0x8c);
  glcd_wd(2, 0xfc);
  glcd_pos(2, 6, 31);
  for(i=0;i<5;i++)
    glcd_wd(2, 0x01);
  glcd_wd(2, 0x00);
  glcd_wd(2, 0x00);
  for(i=0;i<4;i++)
    glcd_wd(2, 0x01);
  glcd_wd(2, 0x01);
}

void printBoy(void){
  unsigned char i,ch;
  glcd_pos(1,(boyX+3)/8+1,32);
  for(i=0;i<8;i++){
    ch=boy[i];
    ch<<=((boyX+3)%8);
	/*
    if(stage[(boyX+3)/8][(s+96)%104%8]&(1<<i))
      ch|=0x18;
	  */
    glcd_wd(1,ch);
  }
  glcd_wd(1,0xff);
  if((boyX+3)%8>2){
    glcd_pos(1,(boyX+3)/8+2,31);
    for(i=0;i<8;i++){
      ch=boy[i];
      ch>>=(8-(boyX+3)%8);
	  /*
      if(stage[(boyX+3)/8+1][(s+96)%104%8]&(1<<i))
        ch|=0x18;
		*/
      glcd_wd(1,ch);
    }
	glcd_wd(1,0xff);
  }
}

void insertStage(unsigned char pattern){
  unsigned char i;
  s--;
  if(s<0)
	s+=103;
  for(i=0;i<6;i++){
    stage[i][s/8]&=(0xff-1<<(s%8));
	if(pattern&(1<<i))
		stage[i][s/8]|=(1<<(s%8));
  }
}

void printStage(void){
  unsigned char i,y;
  unsigned char bb; 
  for(i=0;i<6;i++){
	glcd_pos(2,i+1,0);
	for(y=0;y<64;y++){
		bb=stage[i][(s+y)%103/8];
		if(bb&(1<<((s+y)%103%8)))
			glcd_wd(2,0x18);
		else
			glcd_wd(2,0x00);
	}
	glcd_pos(1,i+1,0);
	for(y=64;y<96;y++){
		bb=stage[i][(s+y)%103/8];
		if(bb&(1<<((s+y)%103%8)))
			glcd_wd(1,0x18);
		else
			glcd_wd(1,0x00);
	}
    for(y=96;y<103;y++){
		bb=stage[i][(s+y)%103/8];
		if(boyX/8==i){
			if(bb&(1<<((s+y)%103%8))){
				glcd_wd(1,0x18|(boy[y-96]<<(boyX%8)));
				life--;
			}
			else
				glcd_wd(1,(boy[y-96]<<(boyX%8)));
		}else if(boyX/8+1==i && boyX%8>2){
			if(bb&(1<<((s+y)%103%8))){
				glcd_wd(1,0x18|(boy[y-96]>>(8-boyX%8)));
				life--;
			}
			else
				glcd_wd(1,(boy[y-96]>>(8-boyX%8)));
		}else {if(bb&(1<<((s+y)%103%8)))
				glcd_wd(1,0x18);
			else
				glcd_wd(1,0x00);
		}
	}
  }
}

void newGame(void){
  unsigned char bb[6]={0},bb2[6]={0};
  unsigned char pattern;
  unsigned char i,j;
  unsigned char ch;
  unsigned int score=0;
  unsigned char speed=2;

  //initialize
  life=20;
  glcd_fillup(0x00);
  gameScreen();
  for(i=0;i<6;i++)
    for(j=0;j<13;j++)
      stage[i][j]=0;
  boyX=21; //put rockboy at the center of screen

  for(i=0;i<100;i++){
	for(j=0;j<speed;j++){
	    pattern=0;
		for(i=0;i<6;i++){
		  if(bb[i]>0){
			pattern|=(1<<i);
			bb[i]--;
		  }
		  else {
			if(bb2[i])
				bb2[i]--;
			if(bb2[i]==0 && rnd(100)<10){ // difficult variable
				bb[i]=3; // rock height
				bb2[i]=5;
			}
		  }
		}
    insertStage(pattern);
	}
    printStage();

    // check keypad input
    for(j=0;j<6;j++){
      if(~key_rd()&(1<<j)){
        if(j==3)
          boyX-=2;
        else if(j==2)
          boyX+=2;
      }
	  if(boyX>42)
		boyX=42;
	  if(boyX<0)
		boyX=0;
    }
    // check keypad input END 

    //printBoy();
    // check rock and boy
	ch=stage[(boyX+3)/8][(s+96)%104/8];
	if(ch>(1<<((s+96)%8)))
	  life--;
	/*
      if((boyX+3)%8>2){
        if(stage[(boyX+3)/8+1][(s+96)%103/8];
            life--;
			
      }*/
	
	glcd_pos(1,1,120);
	print2d(life,1);

    if(life<=0){
      break;
    }
    score++;
	glcd_pos(1,3,120);
	print2d(score%100,1);
	glcd_pos(1,4,120);
	print2d(score/100,1);
  }

  // out of loop
  // boy die
  vos_cls(); //stage clear;
  updateHiScore(score);
  gameOver();
  printHiScore();
  while(1){
	if(~key_rd()&0x20)
		break;
	delay(500);
  }
}

void main(void) {
	unsigned char tmp;

  //==== INFO ===//
  // P1_0 : infra-red LED
  // GLCD = 1 for right LCD pixels
  // GLCD = 2 for left LCD pixels
  // GLCD = 3 for both LCD pixels

  //=== initialize =====================
  address = 0xF6;

  // test program for GRAPHIC LCD ==================================
  glcd_wc(1, 0x3F); // GRAPHIC LCD 1 on
  glcd_wc(2, 0x3F); // GRAPHIC LCD 2 on
  

  bootScreen();
  delay(200);
  EECON|=0x08;
  tmp=first;
  EECON&=~(0x08);
  
  glcd_pos(1,1,0);
  print2d(tmp,1);
  
  if(tmp!=0x11){
	EECON|=0x08;
	EECON|=0x10;
	delay(500);
	hiScore[0]=0x11;
	while((EECON&0x02)==0){
		_asm
		nop
		_endasm;
	} // wait until not busy
	delay(500);
	hiScore[1]=0x11;
	while((EECON&0x02)==0){
		_asm
		nop
		_endasm;
	} // wait until not busy
	delay(500);
	hiScore[2]=0x11;
	while((EECON&0x02)==0){
		_asm
		nop
		_endasm;
	} // wait until not busy
	delay(500);
	first=0x11;
	while((EECON&0x02)==0){
		_asm
		nop
		_endasm;
	} // wait until not busy
	EECON&=~0x10;
    EECON&=~0x08;
  }


 

  while(1){
	delay(200);
    glcd_fillup(0x00);
    bootScreen();
	while(key_rd()==0x3f){
	  delay(2000);
    }
    if(~key_rd()&0x01)
      newGame();
    if(~key_rd()&0x02)
      delay(100);
    if(~key_rd()&0x04)
      delay(100);
    if(~key_rd()&0x08){
	  glcd_fillup(0x00);
      gameScreen();
      printHiScore();
	  while(key_rd()&0x20)
		delay(100);
    }
  }

  /* end of "test GRPAHIC LCD" */
  //vos_cls();
}




