#include <Adafruit_Keypad.h>
#define KEYPAD_PID3845
#define R1    7
#define R2    6
#define R3    5
#define R4    4
#define C1    10
#define C2    9
#define C3    8
// leave this import after the above configuration
#include "keypad_config.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>

LiquidCrystal_I2C lcd(0x3F, 20, 4);

String deactivationCode = "";
int deactivationMethod = 0;
int wires[] = {0, 0, 0, 0, 0, 0, 0, 0};
int defuseTime = 0;
int bombState = 0;
int plantTime = 0;
int codeTries = 0;
unsigned long bombDuration = 60000;
unsigned long targetTime = 0;
bool plantWithCode = false;


#define SAFETY_SW 22
#define SAFETY_LED 24
#define GREEN_SW 26
#define GREEN_LED 28
#define RED_SW 30
#define RED_LED 32

#define WIRE0 47
#define WIRE1 45
#define WIRE2 43
#define WIRE3 41
#define WIRE4 39
#define WIRE5 37
#define WIRE6 35
#define WIRE7 33

#define LOWBEEP 12
#define HIGHBEEP 2

Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  // put your setup code here, to run once:
  pinMode(SAFETY_SW, INPUT_PULLUP);
  pinMode(GREEN_SW, INPUT_PULLUP);
  pinMode(RED_SW, INPUT_PULLUP);

  pinMode(SAFETY_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pinMode(WIRE0, OUTPUT);
  pinMode(WIRE1, OUTPUT);
  pinMode(WIRE2, OUTPUT);
  pinMode(WIRE3, OUTPUT);
  pinMode(WIRE4, OUTPUT);
  pinMode(WIRE5, OUTPUT);
  pinMode(WIRE6, OUTPUT);
  pinMode(WIRE7, OUTPUT);

  digitalWrite(WIRE0, LOW);
  digitalWrite(WIRE1, LOW);
  digitalWrite(WIRE2, LOW);
  digitalWrite(WIRE3, LOW);
  digitalWrite(WIRE4, LOW);
  digitalWrite(WIRE5, LOW);
  digitalWrite(WIRE6, LOW);
  digitalWrite(WIRE7, LOW);

  pinMode(LOWBEEP, OUTPUT);
  pinMode(HIGHBEEP, OUTPUT);

  digitalWrite(LOWBEEP, LOW);
  digitalWrite(HIGHBEEP, LOW);
  /*digitalWrite(RED_LED, HIGH);
  digitalWrite(SAFETY_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);*/

  lcd.init();                      // initialize the lcd
  lcd.backlight();
  //setCursor, col, row, starts from 0
  lcd.setCursor(0, 0);
  lcd.clear();
  customKeypad.begin();
}
/*if(digitalRead(SAFETY_SW)){
      digitalWrite(SAFETY_LED, HIGH);
    } else {
      digitalWrite(SAFETY_LED, LOW);
      bombState = 2;
    }*/
void loop() {
  // put your main code here, to run repeatedly:
  char keypressed;
  switch (bombState) {
    case 0: //initial setup
      lcd.clear();
      lcd.setCursor(0, 0);
      writeLCD(0, "Choose method:");
      writeLCD(1, "1)Wire 2)Code 3)Both");
      writeLCD(2, "4)Simple 5)Flag");
      keypressed = getKey();
      if (keypressed == '1') {
        deactivationMethod = 1;//Wire

        setWires();
        setPlantTime();
        setBombTime();
        lcd.clear();
        writeLCD(0, "OK, game on!");

        bombState = 1;
        delay(1000);
        lcd.clear();
      } else if (keypressed == '2') {
        deactivationMethod = 2;//Code

        setCode();
        if (!plantWithCode) {
          setPlantTime();
        }
        setBombTime();
        lcd.clear();
        writeLCD(0, "OK, game on!");

        bombState = 1;
        delay(1000);
        lcd.clear();
      } else if (keypressed == '3') {
        deactivationMethod = 3;//Both

        setCode();
        setWires();
        if (!plantWithCode) {
          setPlantTime();
        }
        setBombTime();
        lcd.clear();
        writeLCD(0, "OK, game on!");

        bombState = 1;
        delay(1000);
        lcd.clear();
      } else if (keypressed == '4') {
        deactivationMethod = 4; //simple: hold button  
        setPlantTime();
        setDefuseTime();
        setBombTime();
        
        lcd.clear();
        writeLCD(0, "OK, game on!");
        bombState = 1;
        delay(1000);
        lcd.clear();
      } else if (keypressed == '5') {
        lcd.clear();
        writeLCD(0, "not yet implemented");
        delay(1000);
      } else {
        lcd.clear();
        writeLCD(0, "Invalid option");
        delay(1000);
      }
      break;

    case 1: // pre-plant
      preplant();
      break;

    case 2: // planted
      lcd.clear();
      bombactive();
      break;

    case 3: // defused
      button_noises();
      break;

    case 4: //exploded
      break;

    default:
      writeLCD(1, "ERRORING TO DEFAULT");
      break;
  }
}

void button_noises() {
   bool SAFETYstate = !digitalRead(SAFETY_SW);
   bool ARMState = !digitalRead(RED_SW);
   bool DISARMState= !digitalRead(GREEN_SW);
   if (SAFETYstate || ARMState || DISARMState) {
    tone(LOWBEEP, 100);
   } else {
    noTone(LOWBEEP);
   }
}

char getKey() {
  bool nokey = true;
  char key;
  while (nokey) {
    customKeypad.tick();
    while (customKeypad.available()) {
      keypadEvent e = customKeypad.read();
      if (e.bit.EVENT == KEY_JUST_PRESSED) {
        tone(LOWBEEP, 1000, 50);
        key = (char)e.bit.KEY;
        nokey = false;
      }
    }
  }
  return key;
}

/**BOMB FUNCTIONS*/
void setDefuseTime() {
  bool okCode = false;
  String tempcode = "1";
  lcd.clear();
  writeLCD(0, "defuse time (sec):");
  writeLCD(2, "0 = feature disabled");
  writeLCD(3, "in code = per digit");
  writeLCD(1, tempcode);
  while (!okCode) {
    char temp = getKey();
    if (temp == '#' && tempcode.length() > 0) {
      okCode = true;
    } else if (temp == '*') {
      tempcode = tempcode.substring(0, tempcode.length() - 1);
      writeLCD(1, tempcode);
    } else {
      tempcode += temp;
      writeLCD(1, tempcode);
    }
  }
  defuseTime = tempcode.toInt();
  lcd.clear();
}
void setPlantMode() {
  bool okCode = false;
  while (!okCode) {
    writeLCD(0, "Set plant mode");
    writeLCD(1, "1)button 2)code");
    char temp = getKey();
    int res = ((String)temp).toInt();
    if (res < 1 || res > 2) {
      lcd.clear();
      writeLCD(0, "invalid argument");
      delay(500);
    } else {
      if (res == 1) {
        plantWithCode = false;
      } else {
        plantWithCode = true;
      }
      okCode = true;
    }
  }
}
void setCodeTries() {
  bool okCode = false;
  String tempcode = "3";
  lcd.clear();
  writeLCD(0, "Allowed code tries");
  writeLCD(1, tempcode);
  while (!okCode) {
    char temp = getKey();
    if (temp == '#' && tempcode.length() > 0) {
      okCode = true;
    } else if (temp == '*') {
      tempcode = tempcode.substring(0, tempcode.length() - 1);
      writeLCD(1, tempcode);
    } else {
      tempcode += temp;
      writeLCD(1, tempcode);
    }
  }
  codeTries = tempcode.toInt();
}
void setCode() {
  deactivationCode = "4528";
  boolean okCode = false;
  lcd.clear();
  writeLCD(0, "Set code:");
  writeLCD(1, deactivationCode);
  while (okCode == false) {
    char temp = getKey();
    if (temp == '#' && deactivationCode.length() > 2) {
      okCode = true;
    } else if (temp == '*') {
      deactivationCode = deactivationCode.substring(0, deactivationCode.length() - 1);
      writeLCD(1, deactivationCode);
    } else {
      deactivationCode += temp;
      writeLCD(1, deactivationCode);
    }
  }
  lcd.clear();
  setPlantMode();
  setDefuseTime();
  setCodeTries();
}
void setWires() {
  pinMode(WIRE0, INPUT_PULLUP);
  pinMode(WIRE1, INPUT_PULLUP);
  pinMode(WIRE2, INPUT_PULLUP);
  pinMode(WIRE3, INPUT_PULLUP);
  pinMode(WIRE4, INPUT_PULLUP);
  pinMode(WIRE5, INPUT_PULLUP);
  pinMode(WIRE6, INPUT_PULLUP);
  pinMode(WIRE7, INPUT_PULLUP);
  lcd.clear();
  for (int i = 0; i <= 7; i++) {
    writeLCD(0, "Set mode for wire " + (String)i);
    writeLCD(1, "0)off 1)half");
    writeLCD(2, "2)boom 3)defuse");
    char temp = getKey();
    int res = ((String)temp).toInt();
    wires[i] = res;
    if (res < 0 || res > 3) {
      lcd.clear();
      writeLCD(0, "invalid argument");
      i--;
      delay(500);
    }
  }
  lcd.clear();
}
void setPlantTime() {
  bool okCode = false;
  String tempcode = "5";
  lcd.clear();
  writeLCD(0, "Plant time (sec):");
  writeLCD(1, tempcode);
  while (!okCode) {
    char temp = getKey();
    if (temp == '#' && tempcode.length() > 0) {
      okCode = true;
    } else if (temp == '*') {
      tempcode = tempcode.substring(0, tempcode.length() - 1);
      writeLCD(1, tempcode);
    } else {
      tempcode += temp;
      writeLCD(1, tempcode);
    }
  }
  plantTime = tempcode.toInt();
}
void setBombTime() {
  bool okCode = false;
  String tempcode = "5";
  lcd.clear();
  writeLCD(0, "Bomb duration (min)");
  writeLCD(1, tempcode);
  while (!okCode) {
    char temp = getKey();
    if (temp == '#' && tempcode.length() > 0) {
      okCode = true;
    } else if (temp == '*') {
      tempcode = tempcode.substring(0, tempcode.length() - 1);
      writeLCD(1, tempcode);
    } else {
      tempcode += temp;
      writeLCD(1, tempcode);
    }
  }
  bombDuration = tempcode.toInt() * 60 * 1000;
}
void preplant() {
  bool planted = false;
  writeLCD(0, "-Waiting for plant--");
  if (plantWithCode) {
    writeLCD(3, "code:" + deactivationCode);
  } else {
    writeLCD(3, "red+SAFETY for " + (String)plantTime + "s");
  }
  unsigned long pressedAt = 0;
  bool buttonReleased = true;
  byte lastred = 0;
  byte lastSAFETY = 0;
  int lastseconds = -1;
  unsigned long lastbeep = 0;
  bool readytoplant = false;
  String plantcode = "";
  while (!planted) {
    if (!plantWithCode) {
      bool SAFETYstate = !digitalRead(SAFETY_SW);
      bool redstate = !digitalRead(RED_SW);
      if ((SAFETYstate != lastSAFETY) || (redstate != lastred)) {
        if (SAFETYstate && redstate) {
          buttonReleased = false;
          pressedAt = millis();
          writeLCD(2, "HOLD");
        } else {
          buttonReleased = true;
        }
      }
      lastred = SAFETYstate;
      lastSAFETY = redstate;
      long diff = millis() - pressedAt;
      int seconds = round(diff / 1000);
      int secondsleft = plantTime - seconds;
      if (secondsleft != lastseconds && pressedAt > 0 && diff <= plantTime * 1000) {
        tone(LOWBEEP, 1000, 50);
        lastseconds = secondsleft;
      }
      if (secondsleft >= 0  && pressedAt > 0) {
        writeLCD(1, (String)secondsleft);
      }
      if (diff > plantTime * 1000 && pressedAt > 0) {
        readytoplant = true;
        writeLCD(2, "RELEASE TO PLANT");
        tone(LOWBEEP, 1000);
      }
      if (buttonReleased) {
        if (diff > plantTime * 1000 && readytoplant) {
          noTone(LOWBEEP);
          lcd.clear();
          delay(100);
          writeLCD(0, " PLANTED");
          planted = true;
          bombState = 2;
          delay(1000);
        } else {
          writeLCD(2, "PRESS SAFETY+RED");
          if ((lastbeep+2000) < millis()) {
            tone(LOWBEEP, 2000, 20);
            lastbeep = millis();
          }
        }
        pressedAt = 0;
      }
    } else {
      customKeypad.tick();
      while (customKeypad.available()) {
        keypadEvent e = customKeypad.read();
        if (e.bit.EVENT == KEY_JUST_PRESSED) {
          tone(LOWBEEP, 1000, 50);
          char temp = (char)e.bit.KEY;
          if (temp == '#' && plantcode.length() > 2) {
            if (plantcode == deactivationCode) {
              writeLCD(0, " PLANTED");
              planted = true;
              bombState = 2;
              tone(HIGHBEEP, 2800, 150);
              delay(1000);
            } else {
              writeLCD(2, "WRONG CODE");
            }
            plantcode = "";
          } else if (temp == '*') {
            plantcode = plantcode.substring(0, plantcode.length() - 1);
            writeLCD(2, plantcode);
          } else {
            plantcode += temp;
            writeLCD(2, plantcode);
          }
        }
      }
      if ((lastbeep+2000) < millis()) {
        tone(LOWBEEP, 1000, 20);
        lastbeep = millis();
      }
    }
  }
}


bool armed = true;
void bombactive() {
  targetTime = millis();
  int lastseconds = -1;
  writeLCD(0, "BOMB ARMED");
  switch (deactivationMethod) {
    case 1:
      writeLCD(3, "WIRES");
      break;
    case 2:
      writeLCD(3, "CODE");
      break;
    case 3:
      writeLCD(3, "WIRES+CODE");
      break;
    case 4:
      writeLCD(3, "SIMPLE");
      break;
  }
  while (armed) {
    int hsecondsleft = (long)((targetTime + bombDuration) - millis()) / 500;
    int secondsleft = hsecondsleft / 2;
    if (hsecondsleft != lastseconds) {
      String ssleft = (String)(secondsleft % 60);
      if (ssleft.length() < 2) {
        ssleft = "0" + ssleft;
      }
      writeLCD(1, "    " + (String)((int)(secondsleft / 60)) + ":" + ssleft);
      tone(HIGHBEEP, 2800, 75);
      lastseconds = hsecondsleft;
    }
    if (secondsleft <= 0) {
      deactivationMethod = 0;
      armed = false;
      bombState = 4;
      writeLCD(0, "BOMB EXPLODED!");
      writeLCD(3, "RAN OUT OF TIME");
      explodesound();
    }
    switch (deactivationMethod) {
      case 1:
        checkwires();
        break;
      case 2:
        checkcode();
        break;
      case 3:
        checkwires();
        checkcode();
        break;
      case 4:
        checksimple();
        break;
    }
  }
}

String enteredcode = "";

unsigned long defusePressedAt = 0;
bool defuseButtonsReleased = true;
byte lastDefuseGREEN = 0;
byte lastDefuseSAFETY = 0;
bool doFreshPrint = false;
bool readymsg = false;
int lastDefuseSeconds = -1;
bool greenDetected = false;
void checksimple() {
  if (armed) { //no need to run if already blown up
      // start defuse decryptor
      bool SAFETYstate = !digitalRead(SAFETY_SW);
      bool GREENstate = !digitalRead(GREEN_SW);
      if (greenDetected) {
        GREENstate = true;
      }
      if ((SAFETYstate != lastDefuseSAFETY) || (GREENstate != lastDefuseGREEN)) {
        if (GREENstate && SAFETYstate) {
          greenDetected = true;
          defuseButtonsReleased = false;
          defusePressedAt = millis();
          enteredcode = "";
          writeLCD(2, enteredcode);
          writeLCD(3, "DECRYPTING...");
        } else {
          greenDetected = false;
          defuseButtonsReleased = true;
        }
      }
      
      lastDefuseGREEN = GREENstate;
      lastDefuseSAFETY = SAFETYstate;

      
      if (defuseButtonsReleased) {
        if (doFreshPrint) {
          doFreshPrint = false;
          noTone(LOWBEEP);
        }
        if (defusePressedAt > 0) {
          if (round((millis() - defusePressedAt) / 1000) > defuseTime) {
              writeLCD(0, "BOMB DEFUSED");
              writeLCD(3, "CORRECT! DEFUSED!");
              armed = false;
              bombState = 3;
          } else {
            writeLCD(0, "DEFUSE FAILED");
            writeLCD(3, "TRY AGAIN - HOLD LONGER");
          }
        }
      } else if (defuseTime > 0) {
        long diff = millis() - defusePressedAt;
        int seconds = round(diff / 1000);
        int secondsleft = (defuseTime) - seconds;
        if (secondsleft != lastDefuseSeconds && defusePressedAt > 0 && diff <= (defuseTime) * 1000) {
          tone(LOWBEEP, 1000, 50);
          lastDefuseSeconds = secondsleft;
        }
        if (secondsleft >= 0  && defusePressedAt > 0) {
          doFreshPrint = true;
          writeLCD(0, "DEFUSING");
          writeLCD(3, (String)secondsleft + " seconds left");
        }
        if (diff > (defuseTime) * 1000 && defusePressedAt > 0) {
          if (!readymsg) {
            (3, "DEFUSE COMPLETE");
            readymsg = true;
          }
          tone(LOWBEEP, 100);
        } else {
          readymsg = false;
        }
    }
  }
   // end defuse decryptor
}
void checkcode() {
  if (armed) { //no need to run if already blown up
      // start defuse decryptor
      bool SAFETYstate = !digitalRead(SAFETY_SW);
      bool GREENstate = !digitalRead(GREEN_SW);
      if (greenDetected) {
        GREENstate = true;
      }
      if ((SAFETYstate != lastDefuseSAFETY) || (GREENstate != lastDefuseGREEN)) {
        if (GREENstate && SAFETYstate && defuseTime > 0) {
          defuseButtonsReleased = false;
          greenDetected = true;
          defusePressedAt = millis();
          enteredcode = "";
          writeLCD(2, enteredcode);
          writeLCD(3, "DECRYPTING...");
        } else {
          greenDetected = false;
          defuseButtonsReleased = true;
        }
      }
      
      lastDefuseGREEN = GREENstate;
      lastDefuseSAFETY = SAFETYstate;

      
      if (defuseButtonsReleased) {
        if (doFreshPrint) {
          doFreshPrint = false;
          noTone(LOWBEEP);
          writeLCD(2, enteredcode);
        }
        customKeypad.tick();
        while (customKeypad.available()) {
          keypadEvent e = customKeypad.read();
          if (e.bit.EVENT == KEY_JUST_PRESSED) {
            tone(LOWBEEP, 1000, 50);
            char temp = (char)e.bit.KEY;
            if (temp == '#' && enteredcode.length() > 2) {
              if (enteredcode == deactivationCode) {
                writeLCD(0, "BOMB DEFUSED");
                writeLCD(3, "CORRECT! DEFUSED!");
                armed = false;
                bombState = 3;
              } else {
                codeTries--;
                if (codeTries > 0) {
                  writeLCD(3, "WRONG CODE "+ (String)codeTries + " left");
                } else {
                  armed = false;
                  bombState = 4;
                  writeLCD(0, "BOMB EXPLODED");
                  writeLCD(2, "EXPLODED***");
                  writeLCD(3, "TOO MANY INVALID");
                  explodesound();
                }
              }
              enteredcode = "";
              writeLCD(2, enteredcode);
            } else if (temp == '*') {
              enteredcode = enteredcode.substring(0, enteredcode.length() - 1);
              writeLCD(2, enteredcode);
            } else {
              enteredcode += temp;
              writeLCD(2, enteredcode);
            }
          }
        }
        // end keypad and code entry 
      } else if (defuseTime > 0) {
        long diff = millis() - defusePressedAt;
        int seconds = round(diff / 1000);
        int secondsleft = (defuseTime * deactivationCode.length()) - seconds;
        if (secondsleft != lastDefuseSeconds && defusePressedAt > 0 && diff <= (defuseTime * deactivationCode.length()) * 1000) {
          tone(LOWBEEP, 1000, 50);
          lastDefuseSeconds = secondsleft;
        }
        if (secondsleft >= 0  && defusePressedAt > 0) {
          String questionmarks = "";
          for(int i = 0; i<deactivationCode.length() - (int)(seconds/defuseTime);i++){
            questionmarks += "?";  
          }
          writeLCD(2, deactivationCode.substring(0, (int)(seconds/defuseTime)) + questionmarks);
          doFreshPrint = true;
          writeLCD(3, (String)secondsleft + " seconds left");
        }
        if (diff > (defuseTime * deactivationCode.length()) * 1000 && defusePressedAt > 0) {
          if (!readymsg) {
            (3, "DECODE COMPLETE");
            readymsg = true;
          }
          tone(LOWBEEP, 100);
        } else {
          readymsg = false;
        }
    }
  }
   // end defuse decryptor
}
void halftime() {
   long msleft = (long)((targetTime + bombDuration) - millis());
   targetTime -= (msleft / 2);
   tone(LOWBEEP, 1000, 50);
   delay(100);
   tone(LOWBEEP, 1000, 50);
   delay(100);
   tone(LOWBEEP, 1000, 50);
}
void explodesound() {
  for(int i = 0;i<10;i++){
    tone(HIGHBEEP, 3000, 150);
    delay(200);
    tone(HIGHBEEP, 2400, 150);
    delay(200);
    tone(HIGHBEEP, 3000, 150);
    delay(200);
    tone(HIGHBEEP, 2400, 150);
    delay(200);
  }
}
bool tripped[] = {false, false, false, false, false, false, false, false};
void checkwires() {
  if (armed) { //no need to run if already blown up
    for(int i = 0;i<8;i++){
      if(!tripped[i]){
        if(wirestate(i)){ //PULLUP RAISES TO HIGH WHEN CONNECTION IS LOST
          tripped[i] = true;
          switch(wires[i]) {
            case 0:
              break;
            case 1:
              writeLCD(3, "CAUTION, TIME HALVED");
              halftime();
              break;
            case 2:
              armed = false;
              bombState = 4;
              writeLCD(0, "WRONG WIRE! BOOM!");
              writeLCD(3, "Wrong wire-exploded");
              explodesound();
              break;
            case 3:
              armed = false;
              bombState = 3;
              writeLCD(0, "DEFUSE WIRE");
              writeLCD(3, "correct wire-defused");
              break;
          }
          /*
           * punishments for tripping wire come from wires[i] with following
           * 
           * writeLCD(1, "0)off 1)half");
           * writeLCD(2, "2)boom 3)defuse");
           */
        }
      }
    }
  }
}
bool wirestate(int index) {
  switch(index){
    case 0:
      return (int)digitalRead(WIRE0);
      break;
    case 1:
      return (int)digitalRead(WIRE1);
      break;
    case 2:
      return (int)digitalRead(WIRE2);
      break;
    case 3:
      return (int)digitalRead(WIRE3);
      break;
    case 4:
      return (int)digitalRead(WIRE4);
      break;
    case 5:
      return (int)digitalRead(WIRE5);
      break;
    case 6:
      return (int)digitalRead(WIRE6);
      break;
    case 7:
      return (int)digitalRead(WIRE7);
      break; 
  }
}


void writeLCD (int row, String message) {
  lcd.setCursor(0, row);
  for (int i = message.length(); i <= 19; i++) {
    message += ' ';
  }

  lcd.print(message);
}
