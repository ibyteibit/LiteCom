#include "transmit_char_codes.h"
#include <PS2Keyboard.h>
#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

const int receiverPin = 4;
const int receiveSpeed = 20;

const int transmitPin = 7;
const String transmit_message;
const long transmitSpeed = receiveSpeed/2;

const int rotate_ccw_pin = 5;
const int rotate_cw_pin = 6;
const int motor_signal = 10;  

bool receive_flag, transmit_flag, rotate_flag, keyboard_flag = 0;

Servo myservo;

const int DataPin = 11;
const int IRQpin =  2;

float pos;

PS2Keyboard keyboard;

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  Serial.begin(9600);

  transmit_message = "Type Here!";

  pos = 90;

  pinMode(transmitPin, OUTPUT);
  pinMode(receiverPin, INPUT);

  pinMode(rotate_ccw_pin, INPUT);
  myservo.attach(motor_signal);
  myservo.write(pos);
  pinMode(rotate_cw_pin, INPUT);

  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  //lcd.setCursor(0,0);
  //lcd.print("Hello!");
  lcd.setCursor(0,0);
  lcd.print("--------------------");
  lcd.setCursor(0,1);
  lcd.print(transmit_message);

  keyboard.begin(DataPin, IRQpin);
}

void loop() {
  setFlags();

  if (receive_flag) {
    receiveMessage();
    receive_flag = 0;
  }
  else if (transmit_flag) {
    transmitMessage(transmit_message);
    transmit_flag = 0;
  }
  else if (keyboard_flag) {
    checkKeyboard();
    keyboard_flag = 0;
  }
  else if (rotate_flag) {
    rotate();
    rotate_flag = 0;
  }

}

void setFlags() {
  receive_flag = !digitalRead(receiverPin);
  rotate_flag = (digitalRead(rotate_ccw_pin) || digitalRead(rotate_cw_pin));
  keyboard_flag = keyboard.available();
}

//-----------------------------------------------KEYBOARD----------------------------------------------------

void checkKeyboard() {
  char c = keyboard.read();
  Serial.println(c);
  if (c == PS2_ENTER) {
    transmit_flag = 1;
  }
  else if (c == PS2_BACKSPACE) {       // delete last char in transmit message
    if (transmit_message.length() > 0) {
      transmit_message = transmit_message.substring(0, transmit_message.length() - 1);
      lcd.setCursor(0,1);
      lcd.print("                    ");
      lcd.setCursor(0,1);
      lcd.print(transmit_message);
      for (int i = 0; i < 20 - transmit_message.length(); i++)
        lcd.print(" ");

    }
  }
  else if (transmit_message.length() < 20 && (c >= ' ' && c <= '~')) {    // add char
    transmit_message += c;
    lcd.setCursor(0,1);
    lcd.print(transmit_message);
  }
}

//-----------------------------------------------ROTATE----------------------------------------------------


void rotate() {
  
  bool ccw, cw = 1;
  while (ccw || cw) {
    ccw = digitalRead(rotate_ccw_pin); 
    cw = digitalRead(rotate_cw_pin);
    if(ccw){
    Serial.println("rotate_ccw_pin input");
    pos = pos+1;
    if (pos > 180) pos = 180;
    myservo.write(pos);
    Serial.println("done");           
    }
    else if(cw){
      pos = pos -1; 
      if (pos < 0) pos = 0;
      Serial.println("rotate_cw_pin input");
      myservo.write(pos);              
    }
    delay(20);

  
  }
}

//-----------------------------------------------TRANSMIT----------------------------------------------------


void transmitMessage(String transmit_message) {
  Serial.println("started transmit");

  String bitString = encode(transmit_message);
  
  sendBits(bitString);

  Serial.println("sent bits!");
  lcd.setCursor(0,1);
  lcd.print("Sent!               ");
  delay(2000);
  lcd.setCursor(0,1);
  lcd.print("                    ");
  lcd.setCursor(0,1);
  lcd.print(transmit_message);
}

String encode(String message) {
  char bits[message.length()*16 + 16 + 16 + 1] = "";     // (8 bits per char + 8 bits per spacer)*total chars + 16 start bits + 16 end bits + null terminator
  
  strcat(bits, spacer);
  strcat(bits, spacer);

  for (int i = 0; i < message.length(); i++) {
    strcat(bits, charToBitString(message[i]).c_str());
    strcat(bits, spacer);
  }

  strcat(bits, end);
  strcat(bits, end);

  Serial.print("bitString: ");
  Serial.println(bits);
  return bits;
}

String charToBitString(char c) {
  char bitstr[9] = "00000000";
  for (int i = 0; i < 8; i++) {
    bitstr[7 - i] = ((c >> i) % 2) + '0';
  }
  Serial.print("char: ");
  Serial.print(c);
  Serial.print(" is equal to: ");
  Serial.println(bitstr);
  return bitstr;
}

void sendBits(String bits) {
  for (int i = 0; i < bits.length(); i++) {
      if (bits[i] == '0') {
        tone(transmitPin, 38000, transmitSpeed);   // change to pulse IR for faster?
        //pulseIR(transmitSpeed*1000);
      }
      delay(transmitSpeed*2);
  }

}

//-----------------------------------------------RECEIVE----------------------------------------------------


void receiveMessage() {
  Serial.println("Aligning...");

  bool alignSuccess = align();

  if (alignSuccess) {
    Serial.println("Align success!");
    Serial.println("Receiving Message...");

    String message = getMessage();

    Serial.println(message);
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(message);

  }
  else
    Serial.println("Align fail");

}

void sync() {
  int checkval = 1;
  while (checkval != 0) {
    checkval = digitalRead(receiverPin);
  }
}

String getMessage() {
  int ends = 0;
  int bit;
  int len = 0;
  char character[9] = "00000000";
  char message[37] = "";
  char letter[2] = "A";
  int lcdpos = 0;

  //delay(10);  // WHY??????

  while (ends < 2) {

    for (int i = 0; i < 8; i++) {
      delay(receiveSpeed);
      //delayMicroseconds(50); // necessary?

      bit = digitalRead(receiverPin);
      character[i] = bit + '0'; 

      if (i == 6 && (strcmp(character, "11000010") == 0 || strcmp(character, "11000011") == 0)) {
        sync();
        //Serial.print("sync");
        character[7] = '0';
        break;
      }

    }

    Serial.print(character);

    if (!strcmp(character, spacer)) {
      Serial.print("[SPACER]");
    }

    if (strcmp(character, spacer) != 0 && strcmp(character, end) != 0) {
      letter[0] = (char)strtol(character, NULL, 2);
      strcat(message, letter);
      len++;
    }

    if (!strcmp(character, end))
      ends++; 
    else
      ends = 0;

    if (len >= 20)
      break;
  
  }
  Serial.println("[END]");

  char chr[2] = "A";
  char out[len] = "";

  for (int i = 0; i < len; i++) {
    if (message[i] >= ' ' && message[i] <= '~') {
      chr[0] = message[i];
      strcat(out, chr);
    }
  }
  return out;

}

bool align() {
  int state = 0;
  int bit;
  int ones = 0;

  while (1) {
    delay(receiveSpeed);
    
    bit = digitalRead(receiverPin);
    if (bit == 1)
      ones++;
    else
      ones = 0;

    Serial.print(bit);

    switch (state) {
      case 0:
        if (bit == 1)
          state = 1;
        else
          state = 0;
        break;
      case 1:
        if (bit == 1)
          state = 2;
        else
          state = 0;
        break;
      case 2:
        if (bit == 0)
          state = 3;
        else
          state = 2;
        break;
      case 3:
        if (bit == 0)
          state = 4;
        else
          state = 1;
        break;
      case 4:
        if (bit == 0)
          state = 5;
        else
          state = 1;
        break;
      case 5:
        if (bit == 0)
          state = 6;
        else
          state = 1;
        break;
      case 6:
        if (bit == 1)
          state = 7;
        else
          state = 0;
        break;
      case 7:
        if (bit == 0)
          state = 8;
        else
          state = 2;
        break;
    }

    if (ones >= 10)
      return false;
    if (state == 8)
      return true;

  }
}

String parseMessage(String message) {
  char chr[2] = "A";
  char out[message.length()] = "";

  for (int i = 0; i < message.length(); i++) {
    if (message[i] >= ' ' && message[i] <= '~') {
      chr[0] = message[i];
      strcat(out, chr);
    }
  }
  return out;
}
