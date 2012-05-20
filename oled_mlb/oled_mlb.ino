/* -*- mode: c -*- */

#include <avr/pgmspace.h>
#include "U8glib.h"

// U8G Library setup
U8GLIB_NHD27OLED_BW u8g(13, 11, 10, 9);       // SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9

// Function declarations
void structure(void);
void balls_rst(void);
void strs_rst(void);
void outs_rst(void);
void balls_set(char b);
void strs_set(char s);
void outs_set(char o);
void matchup_set(char *ateam, char *hteam);
void team_set(char *ateam, char *hteam);
void onbase_rst(void);
void onbase_set(char r);
void rhe_set(char *arhe, char *hrhe);
void inn_set(char *inn);
void checkASCII(char inp);

// Global variables and arrays

/* Kind of ridiculous, but the team names are all in program space.
 *   Here we need to declare each team name string as an array
 *   in the program space before finally declaring the entire teams
 *   array.
 */
const prog_char t1[] PROGMEM = "Angels";
const prog_char t2[] PROGMEM = "Dbacks";
const prog_char t3[] PROGMEM = "Braves";
const prog_char t4[] PROGMEM = "Orioles";
const prog_char t5[] PROGMEM = "Red Sox";
const prog_char t6[] PROGMEM = "White Sox";
const prog_char t7[] PROGMEM = "Cubs";
const prog_char t8[] PROGMEM = "Reds";
const prog_char t9[] PROGMEM = "Indians";
const prog_char t10[] PROGMEM = "Rockies";
const prog_char t11[] PROGMEM = "Tigers";
const prog_char t12[] PROGMEM = "Marlins";
const prog_char t13[] PROGMEM = "Astros";
const prog_char t14[] PROGMEM = "Royals";
const prog_char t15[] PROGMEM = "Dodgers";
const prog_char t16[] PROGMEM = "Brewers";
const prog_char t17[] PROGMEM = "Twins";
const prog_char t18[] PROGMEM = "Yankees";
const prog_char t19[] PROGMEM = "Mets";
const prog_char t20[] PROGMEM = "Athletics";
const prog_char t21[] PROGMEM = "Phillies";
const prog_char t22[] PROGMEM = "Pirates";
const prog_char t23[] PROGMEM = "Padres";
const prog_char t24[] PROGMEM = "Mariners";
const prog_char t25[] PROGMEM = "Giants";
const prog_char t26[] PROGMEM = "Cardinals";
const prog_char t27[] PROGMEM = "Rays";
const prog_char t28[] PROGMEM = "Rangers";
const prog_char t29[] PROGMEM = "Blue Jays";
const prog_char t30[] PROGMEM = "Nationals";

const char *teams[] PROGMEM = {t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, 
			       t11, t12, t13, t14, t15, t16, t17, t18, t19, t20,
			       t21, t22, t23, t24, t25, t26, t27, t28, t29, t30};

char a_team[10];
char h_team[10];

char data[] = {0};
char game_data[28] = {0};
char b_name[15] = {0};
char p_name[15] = {0};
char game_tmp[28] = {0};
char b_tmp[15] = {0};
char p_tmp[15] = {0};
char ser_data[57] = {0};
boolean valid = false;
int a_index,h_index;

/* Data format: 
 *  #,inning(3),ateam(2),hteam(2),arhe(8),hrhe(8),count(3),onbase(1),names(28),*
 *  "xxx/xx/xx/xxxxxxxx/xxxxxxxx/xxx/xx"
 *  Ex: #m7 1422 7 12  1 0  3  11230Gordon        Brach         *
 */

char test_data[] = "#f9 2514 0  6  1 6  9  20030Carpenter, M  Kershaw       *";

void draw(void) {
  // Function to redraw the entire screen

  // Set font
  u8g.setFont(u8g_font_fixed_v0);

  // Draw static elements of scoreboard
  structure();

  // Draw dynamic game info
  inn_set(&game_data[0]);
   
  team_set(&game_data[3],&game_data[5]);
  
  rhe_set(&game_data[7],&game_data[15]);
  
  balls_set(game_data[23]);
  strs_set(game_data[24]);
  outs_set(game_data[25]);
  onbase_set(game_data[26]);
  
  matchup_set(game_data[0],b_name,p_name);
}

void setup(void) {
  // flip screen, if required
  u8g.setRot180();
  Serial.begin(9600);
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
void matchup_set(char state, char* batter, char* pitcher) {
  u8g.setFont(u8g_font_5x8);

  if ((state == 't') | (state == 'm')) {
      u8g.drawStr(6,32,batter);  //top    (away)
      u8g.drawStr(6,51,pitcher); //bottom (home)
  }
  else if ((state == 'b') | (state == 'e')) {
      u8g.drawStr(6,32,pitcher); //top    (home)
      u8g.drawStr(6,51,batter);  //bottom (away)
  } 
  else {
      u8g.drawStr(6,32," ");
      u8g.drawStr(6,51," ");
  }
  
  u8g.setFont(u8g_font_fixed_v0);
}

/* Find and draw the team names
 * Input should be pointers to arrays of the names
 */
void team_set(char *ateam, char *hteam) {
  char at[3],ht[3];

  at = {ateam[0],ateam[1]};
  a_index = atoi(at);
  
  ht = {hteam[0],hteam[1]};
  h_index = atoi(ht);
    
  /* Have to copy the team name strings out of 
   * program space before drawing it on the screen.
   */
  strcpy_P(a_team, (char*)pgm_read_word(&teams[a_index]));
  strcpy_P(h_team, (char*)pgm_read_word(&teams[h_index]));

  u8g.drawStr(0,23,a_team);
  u8g.drawStr(0,42,h_team);
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
 * Takes two array pointers. These arrays should
 * always be 8 characters long. If R, H, or E
 * are single digits, replace the tens digit 
 * with a space. 
 */
void rhe_set(char* arhe, char* hrhe) {
  char a[9],h[9];
  
  for (int i = 0; i < 8; i++) {
    a[i] = arhe[i];
    h[i] = hrhe[i];
  }
  
  u8g.drawStr(81,23,&a[0]);
  u8g.drawStr(81,42,&h[0]);
}

void inn_set(char* inn_info) {
  char state = inn_info[0];
  char inn_num[3] = {inn_info[1],inn_info[2]};
  
  switch (state) {
  case 'w':
    u8g.drawStr(0,10,"Warmup");
    break;
  case 'p':
    u8g.drawStr(0,10,"Pre-Game");
    break;
  case 't':
    u8g.drawStr(0,10,"Top of ");
    u8g.drawStr(42,10,&inn_num[0]);
    break;
  case 'm':
    u8g.drawStr(0,10,"Mid of ");
    u8g.drawStr(42,10,&inn_num[0]);
    break;
  case 'b':
    u8g.drawStr(0,10,"Bot of ");
    u8g.drawStr(42,10,&inn_num[0]);
    break;
  case 'e':
    u8g.drawStr(0,10,"End of ");
    u8g.drawStr(42,10,&inn_num[0]);
    break;
  case 'f':
    u8g.drawStr(0,10,"Final");
    break;
  case 'o':
    u8g.drawStr(0,10,"Final");
    break;
  }
}

void checkASCII(char inp) {
  if ((inp >= ' ') && (inp <= '9')) {
    valid = true;
  }
  else if ((inp >= 'A') && (inp <= 'z')) {
    valid = true;
  }
  else if ((inp >= 'a') && (inp <= 'z')) {
    valid = true;
  }
  else if (inp == ' ') {
    valid = true;
  }
  else {
    valid = false;
  }
  
}

void loop(void) {
  // Reset ser_data
  ser_data[57] = {0};
  
  // Read in all serial data from Python script
  while (Serial.available()) {
    for (int i = 0; i < 57; i++) {
      ser_data[i] = Serial.read();
    }
  }
  
  /* Data should be keyed like so: #<data>*
   *  Check that incoming data is valid and
   *  separate as necessary.
   * NOTE: this may potentially be redundant
   *       and perhaps isn't necessary
  */
  if ((ser_data[0] == '#') && (ser_data[56] = '*')) {
    for (int i = 1; i < 56; i++) {
      checkASCII(ser_data[i]);
      if (!valid) {
	break;
      }
    }
    
    if (valid) {
      for (int i = 1, j = 0; j < 28; i++, j++) {
	game_data[j] = ser_data[i];
      }
      for (int i = 28, j = 0; j < 15; i++, j++) {
	b_name[j] = ser_data[i];
      }
      for (int i = 42, j = 0; j < 15; i++, j++) {
	p_name[j] = ser_data[i];
      }
       
      // Terminate arrays with NULL
      game_data[27] = '\0';
      b_name[14] = '\0';
      p_name[14] = '\0';
    }
  }
  
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );


  // Redraw the picture after some delay
  delay(1000);
}

