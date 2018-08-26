//https://www.waitingforfriday.com/wp-content/uploads/2017/01/C64_Keyboard_Schematics_PNG.png
/*  MATRIX / PIN
 *    1      1
 *    NC     2
 *    8      3
 *    NC     4
 *    7      5
 *    6      6
 *    5      7
 *    4      8
 *    3      9
 *    2      10
 *    1      11
 *    0      12
 *    H      13
 *    G      14
 *    F      15
 *    E      16
 *    D      17
 *    C      18
 *    B      19
 *    A      20
 */

/*
 * Arduino Leonardo 32U4 mappings
 *     Arduino Name     Real name
 *     Digital 0(RX)    PD2
 *             1(TX)    PD3
 *             2        PD1
 *             3        PD0
 *             4        PD4
 *             5        PC6
 *             6        PD7
 *             7        PE6
 *             8        PB4
 *             9        PB5
 *             10       PB6
 *             11       PB7
 *             12       PD6
 *             13(LED)  PC7
 *    Analog   0        PF7
 *             1        PF6
 *             2        PF5
 *             3        PF4
 *             4        PF1
 *             5        PF0
 * 
 */

//wiring diagram:
//commodore | arduino
// 5..8     | A0->A3
// 19       | 12
// 9..12    | 11..8
// 13..18   | 7..2
// 20       | 0

#include "Keyboard.h"

//Known issues:
//Arrow keyys don't really work
//some shift keys don't work either
//ie... shift+9 etc
//some bouncing issues with the keys as well
//otherwise, i would say we are getting close  to it being perfect
//perhaps we could change this so that it only sends the shift key when it realises it needs to, instead of 
//by default?

/*
 * Lookup table
 */

//COMMODORE specific keys
#define CKEY_RUN_STOP    KEY_TAB
#define CKEY_COMMODORE   KEY_LEFT_ALT
#define CKEY_POUNDS      '{'        
#define CKEY_UP_PI       '^'
#define CKEY_ARROW_LEFT_RIGHT  KEY_RIGHT_ARROW
#define CKEY_ARROW_UP_DOWN     KEY_DOWN_ARROW

#define CKEY_F1_F2             KEY_F1
#define CKEY_F3_F4             KEY_F3
#define CKEY_F5_F6             KEY_F5
#define CKEY_F7_F8             KEY_F7

const unsigned char keys_lut[64] = {
  '1',
  KEY_BACKSPACE,
  KEY_LEFT_CTRL,
  CKEY_RUN_STOP,
  ' ',
  CKEY_COMMODORE,
  'q',
  '2',

  '3',
  'w',
  'a',
  KEY_LEFT_SHIFT, //pos 11
  'z',
  's',
  'e',
  '4',

  '5',
  'r',
  'd',
  'x',
  'c',
  'f',
  't',
  '6',

  '7',
  'y',
  'g',
  'v',
  'b',
  'h',
  'u',
  '8',

  '9',
  'i',
  'j',
  'n',
  'm',
  'k',
  'o',
  '0',

  '+',
  'p',
  'l',
  ',',
  '.',
  ':',
  '@',
  '-',

  CKEY_POUNDS,
  '*',
  ';',
  '/',
  KEY_RIGHT_SHIFT, //pos 52
  '=',
  CKEY_UP_PI,
  KEY_HOME,

  KEY_DELETE,
  KEY_RETURN,
  CKEY_ARROW_LEFT_RIGHT,
  CKEY_ARROW_UP_DOWN,
  CKEY_F1_F2,
  CKEY_F3_F4,
  CKEY_F5_F6,
  CKEY_F7_F8
};

bool cur_keys_state[64] = {0};
bool last_keys_state[64] = {0};
bool wasShiftPressed = false;

bool isShiftPressed() {
  //pos 11 and 52 are the shift keys
  return cur_keys_state[11] | cur_keys_state[52];
}

int colCount;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600); // open the serial port at 9600 bps: 
  Keyboard.begin();
  
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(12, OUTPUT);
  
  pinMode(8, INPUT_PULLUP); 
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  
  pinMode(A0, INPUT_PULLUP); 
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);

  colCount = 0;
}

char createCols(int c) {
  return ~(1 << c);
}

void emitCols(char c) {
  for(int i=0; i<=7; i++) {
    if( (c >> i) & 0x01 ) {
      digitalWrite(i == 1 ? 12 : i, HIGH);
    } else {
      digitalWrite(i == 1 ? 12 : i, LOW);
    }
  }
}

char readRows() {
  return digitalRead(8)  << 0 |
         digitalRead(9)  << 1 |
         digitalRead(10) << 2 |
         digitalRead(11) << 3 |
         digitalRead(A3) << 4 |
         digitalRead(A2) << 5 |
         digitalRead(A1) << 6 |
         digitalRead(A0) << 7;
}

void incrUpdateKeyStates() {
  colCount++;
  if(colCount > 8) {
    colCount = 0;
  }

  char cols = createCols(colCount);
  emitCols(cols);
  char rows = readRows();

  //check each row for whether or not it is pressed
  for(int i=0; i<=7; i++) {
    //i represesnts the rowCount here;
    char addr = colCount * 8 + i;
    if( ~(rows >> i) & 0x01 ) { 
      cur_keys_state[addr] = true; 
    } else {
      cur_keys_state[addr] = false;
    }
  }
}

unsigned char getKeyCode(unsigned char addr) {
  unsigned char keyCode = keys_lut[addr];
  if(!wasShiftPressed) {
    return keyCode;
  }
  //still here? deal with complicated shifts
  switch(keyCode) {
    case '2':
      return '"';
    case '6':
      return '&';
    case '7':
      return '`';
    case '8':
      return '(';
    case '9':
      return ')';
    case '0':
      return '0';
    case ':':
      return '[';
    case ';':
      return ']';
    case '{':
      return '}';
  }
  //still here?
  return keyCode;//
}

void loop() {
  // put your main code here, to run repeatedly: 
  incrUpdateKeyStates();

  if(wasShiftPressed != isShiftPressed()) {
    wasShiftPressed = isShiftPressed();
    Keyboard.releaseAll();
    for(int i=0; i<64; i++) {
      last_keys_state[i] = false;
    }
  }

  for(int i=0; i < 64; i++) {
    if(cur_keys_state[i] != last_keys_state[i]) {
      if(cur_keys_state[i]) {
        
        Keyboard.press(getKeyCode(i));
      } else {
        Keyboard.release(getKeyCode(i));
      }
      last_keys_state[i] = cur_keys_state[i];
    }
  }

  //If serial is needed for debugging, the following commented out code may be helpful
    //    Serial.print("Found:");
    //    Serial.print(0x000000FF & rowCount, DEC);
    //    Serial.print(", ");
    //    Serial.print(0x000000FF & colCount, DEC);
    //    Serial.print(", ");
    //    char addr = colCount * 8 + rowCount;
    //    Serial.print(addr, DEC);
    //    Serial.print(", ");
    //    
    //    Serial.print(c);
    //    Serial.print("\r\n");
}
