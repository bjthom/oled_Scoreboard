/* -*- mode:c -*- */

#include <U8glib.h>
U8GLIB_NHD270LED_BW u8g(13, 11, 10, 9);

//Definitions
char data[19];
//char data[38];
char event[21];
//char RAt = 0;
//char RAo = 0;
//char HAt = 0;
//char HAo = 0;
char EA = 0;
//char RHt = 0;
//char RHo = 0;
//char HHt = 0;
//char HHo = 0;
char EH = 0;
char balls = 0;
char strs = 0;
char outs = 0;
char at = 0;
char ht = 0;
char innt = 0;
char inno = 0;
char tb = 0;
char* team = "team";
char* ateam;
char* hteam;

char RAs[3];
char RHs[3];
char HAs[3];
char HHs[3];
char inns[3];
int ras = 0;
int rhs = 0;
int has = 0;
int hhs = 0;
int inn = 0;

char c;
int flag = 0;

void setup(){
  introScreen();
  delay(1000);
  GLCD.ClearScreen();
  structure();
  balls_rst();
  str_rst();
  outs_rst();
  Serial.begin(9600);
}

void introScreen(){
  GLCD.SelectFont(System5x7);
  GLCD.GotoXY(20,2);
  GLCD.Puts("MLB Scoreboard");
  GLCD.DrawRoundRect(16,0,99,18, 5, BLACK);
}

void structure(){
//  GLCD.GotoXY(80,2);
//  GLCD.Puts("R  H  E");
  GLCD.DrawString("R  H  E",80,2);
//  GLCD.GotoXY(2,55);
//  GLCD.Puts("B: ");
  GLCD.DrawString("B: ",2,55);
//  GLCD.GotoXY(51,55);
//  GLCD.Puts("S: ");
  GLCD.DrawString("S: ",51,55);
//  GLCD.GotoXY(93,55);
//  GLCD.Puts("O: ");
  GLCD.DrawString("O: ",93,55);
  GLCD.DrawHLine(80,11,40,BLACK);
}

char* team_ref(char name) {
  switch (name) {
    case '0':
        team = "Angels";
      break;
    case '1':
        team = "Dbacks";
      break;
    case '2':
        team = "Braves";
      break;
    case '3':
        team = "Orioles";
      break;
    case '4':
        team = "Red Sox";
      break;
    case '5':
        team = "White Sox";
      break;
    case '6':
        team = "Cubs";
      break;
    case '7':
        team = "Reds";
      break;
    case '8':
        team = "Indians";
      break;
    case '9':
        team = "Rockies";
      break;
    case 'a':
        team = "Tigers";
      break;
    case 'b':
        team = "Marlins";
      break;
    case 'c':
        team = "Astros";
      break;
    case 'd':
        team = "Royals";
      break;
    case 'e':
        team = "Dodgers";
      break;
    case 'f':
        team = "Brewers";
      break;
    case 'g':
        team = "Twins";
      break;
    case 'h':
        team = "Yankees";
      break;
    case 'i':
        team = "Mets";
      break;
    case 'j':
        team = "Athletics";
      break;
    case 'k':
        team = "Phillies";
      break;
    case 'l':
        team = "Pirates";
      break;
    case 'm':
        team = "Padres";
      break;
    case 'n':
        team = "Mariners";
      break;
    case 'o':
        team = "Giants";
      break;
    case 'p':
        team = "Cardinals";
      break;
    case 'q':
        team = "Rays";
      break;
    case 'r':
        team = "Rangers";
      break;
    case 's':
        team = "Blue Jays";
      break;
    case 't':
        team = "Nationals";
      break;
  }
}

void balls_rst(){
  GLCD.FillRect(12,55,32,8,WHITE);
  GLCD.DrawCircle(15,58,3,BLACK); //Ball circles
  GLCD.DrawCircle(23,58,3,BLACK);
  GLCD.DrawCircle(31,58,3,BLACK);
  GLCD.DrawCircle(39,58,3,BLACK);
}

void str_rst(){
  GLCD.FillRect(61,55,24,8,WHITE);
  GLCD.DrawCircle(64,58,3,BLACK); //Strikes circles
  GLCD.DrawCircle(72,58,3,BLACK);
  GLCD.DrawCircle(80,58,3,BLACK); 
}
  
void outs_rst() {
  GLCD.FillRect(103,55,24,8,WHITE);
  GLCD.DrawCircle(106,58,3,BLACK); //Outs circles
  GLCD.DrawCircle(114,58,3,BLACK);
  GLCD.DrawCircle(122,58,3,BLACK);
}

void FillCirc(int x0,int y0, int rad){
  int f = 1 - rad;
  int ddF_x = 1;
  int ddF_y = -2* rad;
  int x = 0;
  int y = rad;
  GLCD.DrawLine(x0,y0-rad,x0,y0+rad,BLACK);
  while (x < y){
    if (f >= 0){
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    GLCD.DrawLine(x0+x, y0+x, x0+x, y0-y, BLACK);
    GLCD.DrawLine(x0-x, y0+y, x0-x, y0-y, BLACK);
    GLCD.DrawLine(x0+y, y0+x, y+x0, y0-x, BLACK);
    GLCD.DrawLine(x0-y, y0+x, x0-y, y0-x, BLACK);
  }
  GLCD.SetDot(x0+1,y0+2,BLACK);
}

void fill_rhe(int aruns, int ahits, char ea, int hruns, int hhits, char eh){
  GLCD.FillRect(74,12,46,30,WHITE);
  
  if (aruns >= 10) {        //Mark away team runs.
    GLCD.GotoXY(74,16);     //Double digits accounted for here.
    GLCD.PrintNumber(aruns);
  }
  else if (aruns < 10) {
    GLCD.GotoXY(80,16);
    GLCD.PrintNumber(aruns);
  }
  
  if (ahits >= 10) {        //Mark away team hits.
    GLCD.GotoXY(92,16);     //Double digits accounted for here.
    GLCD.PrintNumber(ahits);
  }
  else if (ahits < 10) {
    GLCD.GotoXY(98,16);
    GLCD.PrintNumber(ahits);
  }
  
  if (hruns >= 10) {        //Mark home team runs.
    GLCD.GotoXY(74,30);     //Double digits accounted for here.
    GLCD.PrintNumber(hruns);
  }
  else if (hruns < 10){
    GLCD.GotoXY(80,30);
    GLCD.PrintNumber(hruns);
  }
  
  if (hhits >= 10) {        //Mark home team hits.
    GLCD.GotoXY(92,30);     //Double digits accounted for here.
    GLCD.PrintNumber(hhits);
  }
  else if (hhits < 10) {
    GLCD.GotoXY(98,30);
    GLCD.PrintNumber(hhits);
  }
  
  GLCD.GotoXY(116,16);
  GLCD.PutChar(ea);
  GLCD.GotoXY(116,30);
  GLCD.PutChar(eh);
}

void team_set(char* team1, char* team2){
  GLCD.FillRect(2,16,60,24,WHITE);
  GLCD.DrawString(team1,2,16);
  GLCD.DrawString(team2,2,30);
}

void play_set(char* desc){
  GLCD.DrawString(desc,2,43);
}

void fill_balls(char b){
  switch (b){
    case '0':
      balls_rst();
      break;
    case '1':
      balls_rst();
      FillCirc(15,58,3);
      break;
    case '2':
      balls_rst();
      FillCirc(15,58,3);
      FillCirc(23,58,3);
      break;
    case '3':
      balls_rst();
      FillCirc(15,58,3);
      FillCirc(23,58,3);
      FillCirc(31,58,3);
      break;
    case '4':
      FillCirc(15,58,3);
      FillCirc(23,58,3);
      FillCirc(31,58,3);
      FillCirc(39,58,3);
      break;
  }
}

void fill_str(char s){
  switch (s){
    case '0':
      str_rst();
      break;
    case '1':
      str_rst();
      FillCirc(64,58,3);
      break;
    case '2':
      str_rst();
      FillCirc(64,58,3);
      FillCirc(72,58,3);
      break;
    case '3':
      FillCirc(64,58,3);
      FillCirc(72,58,3);
      FillCirc(80,58,3);
      break;
  }
}

void fill_outs(char o){
  switch (o){
    case '0':
      outs_rst();
      break;
    case '1':
      outs_rst();
      FillCirc(106,58,3);
      break;
    case '2':
      outs_rst();
      FillCirc(106,58,3);
      FillCirc(114,58,3);
      break;
    case '3':
      FillCirc(106,58,3);
      FillCirc(114,58,3);
      FillCirc(122,58,3);
      break;
  }
}

void set_inn(int inn, char topbot) {
  GLCD.FillRect(2,2,70,7,WHITE);
    switch (topbot){
      case 'w':
        GLCD.DrawString("Warmup",2,2);
        break;
      case 'p':
        GLCD.DrawString("Pre-Game",2,2);
        break;
      case 't':
        GLCD.DrawString("Top of ",2,2);
        GLCD.PrintNumber(inn);
        break;
      case 'm':
        GLCD.DrawString("Middle of ",2,2);
        GLCD.PrintNumber(inn);
        break;
      case 'b':
        GLCD.DrawString("Bottom of ",2,2);
        GLCD.PrintNumber(inn);
        break;
      case 'f': 
        GLCD.DrawString("Final",2,2);
        break;
      case '0':
       GLCD.DrawString(" ",2,2);
       break;
  }
}

void loop(){
  //Collect data from Python script here

  if (Serial.available() == 38){
      for (int i = 0; i < 18; i++) {
        data[i] = Serial.read();
      }
      for (int j = 0; j < 21; j++) {
        event[j] = Serial.read();
      }
      data[18] = '\0';
      event[20] = '\0';
      flag = 1;
    }
      
//  if (Serial.available() >= 1) {
//    c = Serial.read();
//  }
//  switch (c) {
//    case '!':
//      if (Serial.available() == 18){
//        for (int i = 0; i < 19; i++) {
//          data[i] = Serial.read();
//        }
//        data[18] = '\0';
//        flag = 1;
//      }
//      break;
//    case '@':
//      if (Serial.available() == 20){
//        for (int i = 0; i < 21; i++){
//          event[i] = Serial.read();
//        }
//        event[20] = '\0';
//        flag = 2;
//      }
//      break;
//  }
  if (flag == 1) {
    //Setup team names
    ateam = team_ref(data[0]);
    hteam = team_ref(data[1]);
    
    //Score away team's runs
    RAs[0] = data[2];
    RAs[1] = data[3];
    RAs[2] = '\0';
    ras = atoi(RAs);
    
    //Score home team's runs
    RHs[0] = data[7];
    RHs[1] = data[8];
    RHs[2] = '\0';
    rhs = atoi(RHs);
    
    //Score away team's hits
    HAs[0] = data[4];
    HAs[1] = data[5];
    HAs[2] = '\0';
    has = atoi(HAs);
    
    //Score home team's hits
    HHs[0] = data[9];
    HHs[1] = data[10];
    HHs[2] = '\0';
    hhs = atoi(HHs);
    
    //Score errors
    EA = data[6];
    EH = data[11];
    
    //Score count
    balls = data[12];
    strs = data[13];
    outs = data[14];
    
    //Score inning
    inns[0] = data[15];
    inns[1] = data[16];
    inns[2] = '\0';
    inn = atoi(inns);
  
    tb = data[17];

    team_set(ateam,hteam);
    fill_rhe(ras, has, EA, rhs, hhs, EH);
    fill_balls(balls);
    fill_str(strs);
    fill_outs(outs);
    set_inn(inn,tb);
    
//    flag = 0;
//  }
//  else if (flag = 2){
    play_set(event);
    
    flag = 0;
  }
//    Serial.println(event);
//    Serial.println(inn);
//    Serial.println(tb);
//    Serial.print(":");
//    Serial.println(at);
//    Serial.println(ht);
//    Serial.print(":");
//    Serial.println(ateam);
//    Serial.println(hteam);

//    Serial.print(ras);
//    Serial.print(",");
//    Serial.print(has);
//    Serial.print(",");
//    Serial.println(EA);
//    Serial.print(rhs);
//    Serial.print(",");
//    Serial.print(hhs);
//    Serial.print(",");
//    Serial.println(EH);
//    Serial.println(data);
//    Serial.println(RAs);
//    Serial.print(balls);
//    Serial.print(strs);
//    Serial.print(outs);
  delay(3000);
}
