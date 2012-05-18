/* -*- mode: c -*- */

// U8G Library include and setup
#include "U8glib.h"
U8GLIB_NHD27OLED_BW u8g(13, 11, 10, 9);       // SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9

// Function declarations
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

// Global variables and arrays
char *teams[30] = {"Angels","Dbacks"   ,"Braves"  ,"Orioles","Red Sox"  ,"White Sox", \
                   "Cubs"  ,"Reds"     ,"Indians" ,"Rockies","Tigers"   ,"Marlins",   \
                   "Astros","Royals"   ,"Dodgers" ,"Brewers","Twins"    ,"Yankees",   \
                   "Mets"  ,"Athletics","Phillies","Pirates","Padres"   ,"Mariners",  \
                   "Giants","Cardinals","Rays"    ,"Rangers","Blue Jays","Nationals"  };

char data[] = {0};
char game_data[28] = {0};
//char game_data[55] = {0};
char b_name[15] = {0};
char p_name[15] = {0};
char game_tmp[28] = {0};
char b_tmp[15] = {0};
char p_tmp[15] = {0};
char ser_data[57] = {0};


/* Data format: 
 *  #,inning(3),ateam(2),hteam(2),arhe(8),hrhe(8),count(3),onbase(1),names(28),*
 *  "xxx/xx/xx/xxxxxxxx/xxxxxxxx/xxx/xx"
 *  Ex: #m7 1422 7 12  1 0  3  11230Gordon        Brach         *
 */

//char test_data[] = "f0  214 4  8  1 3 10  0202011202";
//char test_data[] = "f0  214 4  8  1 3 10  01202";
char test_data[] = "b8  8 4 7 11  0 3 10  02213";

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
  
  //matchup_set(game_data[0],&game_data[27]); 
  matchup_set(game_data[0],b_name,p_name);
  
}

void setup(void) {
  // flip screen, if required
  // u8g.setRot180();
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
//void matchup_set(char state, char* names) {
void matchup_set(char state, char* batter, char* pitcher) {
  u8g.setFont(u8g_font_5x8);

  /*char batter[15] = {0};
  char pitcher[15] = {0};
  
  for (int i = 0; i < 14; i++) {
    batter[i] = names[i];
    pitcher[i] = names[i+14];
  }
  batter[14] = '\0';
  pitcher[14] = '\0';*/

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

/* Draw the team names
 * Input should be pointers to arrays of the names
 */
void team_set(char* ateam, char* hteam) {
  int a_index,h_index;
  char at[3],ht[3];

  at = {ateam[0],ateam[1]};
  a_index = atoi(at);
  
  ht = {hteam[0],hteam[1]};
  h_index = atoi(ht);

  u8g.drawStr(0,23,teams[a_index]);
  u8g.drawStr(0,42,teams[h_index]);
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
    for (int i = 1, j = 0; j < 28; i++, j++) {
      game_data[j] = ser_data[i];
    }
    for (int i = 28, j = 0; j < 15; i++, j++) {
      b_name[j] = ser_data[i];
    }
    for (int i = 42, j = 0; j < 15; i++, j++) {
      p_name[j] = ser_data[i];
    }
    
    /*for (int i = 0; i < 54; i++) {
      game_data[i] = ser_data[i+1];
    }*/
    
    // Terminate arrays with NULL
    game_data[27] = '\0';
    //game_data[54] = '\0';
    b_name[14] = '\0';
    p_name[14] = '\0';
    
  }
  
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );


  // Redraw the picture after some delay
  delay(1000);
}

