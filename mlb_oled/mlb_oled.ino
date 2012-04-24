/* -*- mode: c -*- */

#include "U8glib.h"
U8GLIB_NHD27OLED_BW u8g(13, 11, 10, 9);       // SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9

void structure(void);
void balls_rst(void);
void strs_rst(void);
void outs_rst(void);
void balls_set(char b);
void strs_set(char s);
void outs_set(char o);
void matchup_set(char* ateam, char* hteam);
void team_set(char* ateam, char* hteam);
void onbase_rst(void);
void onbase_set(char r);
void rhe_set(char* arhe, char* hrhe);
void inn_set(char* inn);

char num[] = "01234567";
char *numptr = num;
char *num4ptr = num;
char *num3ptr = num;

void draw(void) {
  // graphic commands to redraw the complete screen
  u8g.setFont(u8g_font_fixed_v0);
  
  structure();

  inn_set("p0");
  //u8g.drawStr( 0, 10, "Top 10");
  //u8g.drawStr( 81, 23, " 1  2  3");
  //u8g.drawStr( 81, 42, "10 11 12");

  team_set("Athletics","Angels");
  matchup_set("Saltalamacchia","Santana");
  rhe_set(" 1 10  4","10  4  0");
  balls_set(*num4ptr);
  strs_set(*num3ptr);
  outs_set(*num3ptr);
  onbase_set(*numptr);
  
//  u8g.drawFillCirc( 99, 51, 3, U8G_CIRC_ALL);
//  u8g.drawEmpCirc( 111, 51, 3, U8G_CIRC_ALL);
  
}

void setup(void) {
  // flip screen, if required
  // u8g.setRot180();
}

/* Draw static structure elements of scoreboard */
void structure(void) {
  u8g.drawStr(81,10," R  H  E");// RHE line
  u8g.drawHLine(87,12,42);      // Separator

  u8g.drawStr(0,64,"B:  ");     // Balls
  u8g.drawStr(51,64,"S:  ");    // Strikes
  u8g.drawStr(93,64,"O:  ");    // Outs

  /* Draw blank circles and boxes for:
     balls, strikes, outs, and runners */
  onbase_rst();
  balls_rst();
  strs_rst();
  outs_rst();
}

/* Empty circles for balls
 * x-dim = xoffset+radius+spacing
 * y-dim should be constant at 60 along the bottom
 */
void balls_rst(void) {
  for (int i = 0; i < 4; i++) {
    u8g.drawEmpCirc(10+3+i*9,60,3,U8G_CIRC_ALL);
  }
}

/* Empty circles for strikes */
void strs_rst(void) {
  for (int i = 0; i < 3; i++) {
    u8g.drawEmpCirc(61+3+i*9,60,3,U8G_CIRC_ALL);
  }
}

/* Empty circles for outs */
void outs_rst(void) {
  for (int i = 0; i < 3; i++) {
    u8g.drawEmpCirc( 103+3+i*9, 60, 3, U8G_CIRC_ALL);
  }
}

/* Fill in balls circles according to received data */
void balls_set(char b) {
  int balls = b - '0';

  if (balls != 0) {
    for (int i = 0; i < balls; i++) {
      u8g.drawFillCirc(10+3+i*9,60,3,U8G_CIRC_ALL);
    }
  }
  else balls_rst();
}

/* Fill in strikes circles according to received data */
void strs_set(char s) {
  int strikes = s - '0';

  if (strikes != 0) {
    for (int i = 0; i < strikes; i++) {
      u8g.drawFillCirc(61+3+i*9,60,3,U8G_CIRC_ALL);
    }
  }
  else strs_rst();
}

/* Fill in outs circles according to received data */
void outs_set(char o) {
  int outs = o - '0';

  if (outs != 0) {
    for (int i = 0; i < outs; i++) {
      u8g.drawFillCirc(103+3+i*9,60,3,U8G_CIRC_ALL);
    }
  }
  else outs_rst();
}

/* Draw the pitcher and batter names
 * Input should be pointers to arrays of the names
 */
void matchup_set(char* ateam, char* hteam) {
  u8g.setFont(u8g_font_5x8);

  u8g.drawStr(6,32,ateam);
  u8g.drawStr(6,51,hteam);

  u8g.setFont(u8g_font_fixed_v0);
}

/* Draw the team names
 * Input should be pointers to arrays of the names
 */
void team_set(char* ateam, char* hteam) {
  u8g.drawStr(0,23,ateam);
  u8g.drawStr(0,42,hteam);
}

/* Clear the base squares */
void onbase_rst(void) {
  u8g.drawFrame( 64, 6, 4, 4);  // 3rd
  u8g.drawFrame( 70, 1, 4, 4);  // 2nd
  u8g.drawFrame( 76, 6, 4, 4);  // 1st
}


/* Draw baserunners
 * Takes a 3 bit number and fills in accordingly
 * 0b third second first
 */
void onbase_set(char r) {
  int runners = r - '0';
  onbase_rst();

  if (runners & 0b001) {
    u8g.drawFrame(77,7,2,2);
  }

  if (runners & 0b010) {
    u8g.drawFrame(71,2,2,2);
  }

  if (runners & 0b100) {
    u8g.drawFrame(65,7,2,2);
  }
}

/* Draw the RHE lines for home and away teams
 * Takes two array pointers. Arrays should
 * always be 8 characters long. If R, H, or E
 * are single digits, replace the tens digit 
 * with a space. 
 */
void rhe_set(char* arhe, char* hrhe) {
  u8g.drawStr(81,23,arhe);
  u8g.drawStr(81,42,hrhe);
}

void inn_set(char* inn_info) {
  char state = inn_info[0];

  switch (state) {
  case 'w':
    u8g.drawStr(0,10,"Warmup");
    break;
  case 'p':
    u8g.drawStr(0,10,"Pre-Game");
    break;
  case 't':
    u8g.drawStr(0,10,"Top of");
    u8g.drawStr(42,10,&inn_info[1]);
    break;
  case 'm':
    u8g.drawStr(0,10,"Mid of 1");
    u8g.drawStr(42,10,&inn_info[1]);
    break;
  case 'b':
    u8g.drawStr(0,10,"Bot of ");
    u8g.drawStr(42,10,&inn_info[1]);
    break;
  case 'f':
    u8g.drawStr(0,10,"Final");
    break;
  }
}

void loop(void) {
  
  if (*numptr == '7') {
    numptr = num;
  }
  else numptr++;
  if (*num4ptr == '4') {
    num4ptr = &num[0];
  }
  else num4ptr++;
  if (*num3ptr == '3') {
    num3ptr = &num[0];
  }
  else num3ptr++;
  
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  /*numptr++;
  num4ptr++;
  num3ptr++;*/
  
  // rebuild the picture after some delay
  delay(1000);
}
