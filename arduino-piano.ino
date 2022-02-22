#include <LiquidCrystal_I2C.h>
#include "pitches.h"

// Define pins
int shLdPin = 13;
int clockPin = 12;
int inputPin = 11;
int redPin = 10;
int greenPin = 9;
int bluePin = 6;
int speakerPin = 2;
int leftButtonPin = 7;
int rightButtonPin = 5;

// Tones in array
// Since we have 25 push button for keyboard,
// put 25 tones in one array,
// total 4 tones group (which seems listenable by Arduino speaker) 
// (too low or high frequency can't hear clearly, unlike PC speaker)
int tones[4][25] = {{NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2,
                  NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2, NOTE_C3, NOTE_CS3,
                  NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3,
                  NOTE_A3, NOTE_AS3, NOTE_B3, NOTE_C4},

                  {NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3,
                  NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3, NOTE_C4, NOTE_CS4,
                  NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4,
                  NOTE_A4, NOTE_AS4, NOTE_B4, NOTE_C5},

                  {NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4,
                  NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4, NOTE_C5, NOTE_CS5,
                  NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5,
                  NOTE_A5, NOTE_AS5, NOTE_B5, NOTE_C6},

                  {NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5,
                  NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5, NOTE_C6, NOTE_CS6,
                  NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6,
                  NOTE_A6, NOTE_AS6, NOTE_B6, NOTE_C7}};

// To store tones order 
String tone_order[] = {"C ", "C#", "D ", "D#", "E ", "F", "F#",
                      "G ", "G#", "A", "A#", "B", "C ", "C#",
                      "D", "D#", "E ", "F ", "F#", "G ", "G#",
                      "A ", "A#", "B ", "C "};

// For LED Fade
int pwmValue = 0;
bool pwmIncrease = true;
unsigned long fadeLedDelay = millis();

void fadeLed(){
  if((millis() - fadeLedDelay) > 20){
    if(pwmIncrease){
      if(pwmValue < 255){
        analogWrite(redPin, (pwmValue + 5));
        analogWrite(greenPin, (pwmValue + 5));
        analogWrite(bluePin, (pwmValue + 5));
        pwmValue = pwmValue + 5;
      }else
        pwmIncrease = false;
    }else{
      analogWrite(redPin, (pwmValue - 5));
      analogWrite(greenPin, (pwmValue - 5));
      analogWrite(bluePin, (pwmValue - 5));
      pwmValue = pwmValue - 5;
      if(pwmValue <= 0)
        pwmIncrease = true;
    }
    fadeLedDelay = millis();
  }
}

//0x27 = lcd address
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Let user to know if hardware is working correctly or not
void setup() {  
  // Setup LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  
  // Pin setup
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(leftButtonPin, INPUT);
  pinMode(rightButtonPin, INPUT);  
  pinMode(shLdPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(inputPin, INPUT);

  // Check LED
  lcd.setCursor(0, 1);
  lcd.print("Check LED (RGB) ");
  digitalWrite(redPin, HIGH);
  delay(1000);
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, HIGH);
  delay(1000);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, HIGH);
  delay(1000);
  digitalWrite(bluePin, LOW);
  
  // Check speaker
  lcd.setCursor(0, 1);
  lcd.print("Check speaker   ");
  tone(speakerPin, NOTE_C3, 1000);
  delay(1500);

  // Check register
  lcd.setCursor(0, 1);
  lcd.print("Check register  ");
  delay(1500);
  lcd.clear();
  digitalWrite(shLdPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(shLdPin, HIGH);  

  int row = 0;
  int column = 0;
  lcd.setCursor(0,0);
  
  for(int i = 0; i < 25; i++){
    lcd.setCursor(column++, row);
    lcd.print(digitalRead(inputPin));

    if(column == 16){
      row = 1;
      column = 0;
    } 
    
    digitalWrite(clockPin, LOW);
    digitalWrite(clockPin, HIGH);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
  }
  
  // Check two button
  lcd.setCursor(14, 1);
  lcd.print(digitalRead(leftButtonPin));
  lcd.setCursor(15, 1);
  lcd.print(digitalRead(rightButtonPin));
  delay(2500);

  // Clear LCD
  lcd.clear();

}

void piano(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Piano mode");

  // For displaying piano instruction
  unsigned long lastDisplayTime = millis();
  // For debouncing
  unsigned long lastPressTime = millis();

  // Print instruction
  lcd.setCursor(0, 1);
  lcd.print("Exit:Press both ");
  
  // To print instruction message
  int message = 0;

  // Octave
  int octave = 1;

  while(true){
    // LED dimming effect
    fadeLed();
    
    // Shows piano instruction
    if((millis() - lastDisplayTime) > 3000){
      lcd.setCursor(0, 1);
      if(message == 0){
        lcd.print("Left:Octave down");
        message = 1;
      }
      else if(message == 1){
        lcd.print("Right:Octave up ");
        message = 2;
      }
      else if(message == 2){
        lcd.print("Exit:Press both ");
        message = 0;
      }
      lastDisplayTime = millis();
    }

    // Get input for octave up and down
    if(digitalRead(leftButtonPin) == LOW){
      // if both button pressed,
      // finish loop and go back to main function
      if(digitalRead(rightButtonPin) == LOW){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Returning to    ");
        lcd.setCursor(0, 1);
        lcd.print("main...         ");
        delay(2000);
        lcd.clear();
        break;
      }
      // otherwise, update tone index
      if((millis() - lastPressTime) > 1000){
        lastPressTime = millis();
        if(octave != 0)
          octave--;
      }
    }
      
    if(digitalRead(rightButtonPin) == LOW)
      if((millis() - lastPressTime) > 1000){
        lastPressTime = millis();
        if(octave != 3)
          octave++;  
      }

    // Play tone
    int input = getInput();
    if(input != -1)
      tone(speakerPin, tones[octave][input], 50);
  }
} 

int getInput(){
  // Get register input
  digitalWrite(shLdPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(shLdPin, HIGH);  

  for(int i = 0; i < 25; i++){
    if(digitalRead(inputPin) == LOW)
      return(i);
      
    digitalWrite(clockPin, LOW);
    digitalWrite(clockPin, HIGH);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
  }
  // Return -1 if nothing pressed
  return(-1);
}

void pitchTest(){
  // Stop dimming LED
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);

  // Show instruction
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Perfect pitch   ");
  lcd.setCursor(0, 1);
  lcd.print("tester");
  delay(2000);
  
  // To generate better random value
  randomSeed(analogRead(0));
  
  // Store correct count
  int correct_count = 0;
  
  // Will test 20 tones
  for(int i = 0; i < 20; i++){
    lcd.clear();
    lcd.setCursor(0, 0);
    char buf[16];
    sprintf(buf, "Playing..  %02d/20", i + 1);
    lcd.print(buf);

    // Get random tone
    int x = random(4);
    int y = random(25);

    // Play tone for 2 seconds
    tone(speakerPin, tones[x][y]);
    delay(2000);
    noTone(speakerPin);

    // Wait answer for 3 seconds
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press the button");
    unsigned long waitInput = millis();
    int answer;
    while((millis() - waitInput) < 3000){
      lcd.setCursor(0, 1);
      if((millis() - waitInput) < 1000)
        lcd.print("Remaining: 3    ");
      else if((millis() - waitInput) < 2000)
        lcd.print("Remaining: 2    ");
      else
        lcd.print("Remaining: 1    ");
      
      answer = getInput();
      if(answer != -1)
        break;
    }

    // When button is pressed
    if(answer != -1)
      // Judge
      if(answer == y || answer + 12 == y || answer - 12 == y 
      || answer - 24 == y || answer + 24 == y){ 
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Correct!");
        digitalWrite(greenPin, HIGH);
        delay(2000);
        lcd.clear();
        digitalWrite(greenPin, LOW);
        correct_count++;
      }
      // Wrong answer
      else{
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Wrong! Correct  ");
        lcd.setCursor(0, 1);
        lcd.print("answer is:   " + tone_order[y] + " ");
        digitalWrite(redPin, HIGH);
        delay(2000);
        lcd.clear();
        digitalWrite(redPin, LOW);
      }   
    // Timeout (no answer provided)
    else{
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Timeout! ");
      lcd.setCursor(0, 1);
      lcd.print("answer is:   " + tone_order[y] + " ");
      digitalWrite(redPin, HIGH);
      delay(2000);
      lcd.clear();
      digitalWrite(redPin, LOW);
    }
  }

  // Shows result
  // if has perfect pitch
  if(correct_count >= 17){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Congratulation! ");
    lcd.setCursor(0, 1);
    lcd.print("U have abs pitch");
    digitalWrite(greenPin, HIGH);
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    char buf[16];
    sprintf(buf, "Correct:   %02d/20", correct_count);
    lcd.print(buf);
    delay(3000);
    digitalWrite(greenPin, LOW);
  
  // Do not have perfect pitch
  }else{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Test done, U do ");
    lcd.setCursor(0, 1);
    lcd.print("nt hve abs pitch");
    digitalWrite(redPin, HIGH);
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    char buf[16];
    sprintf(buf, "Correct:   %02d/20", correct_count);
    lcd.print(buf);
    delay(3000);
    digitalWrite(redPin, LOW);
  }

  // Return to main menu
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Returning to    ");
  lcd.setCursor(0, 1);
  lcd.print("main...         ");
  delay(2000);
  lcd.clear();
}

// To display main menu
unsigned long menuDisplayTime = millis();
bool menuDisplaySwitch = false;

void loop() {
  // Show menu message
  lcd.setCursor(0, 0);
  lcd.print("Welcome!        ");
  
  if((millis() - menuDisplayTime) > 3000){
    lcd.setCursor(0, 1);
    if(menuDisplaySwitch == false){
      lcd.print("Left: Piano     ");
      menuDisplaySwitch = true;
    }
    else{
      lcd.print("Right: Tone test");   
      menuDisplaySwitch = false;
    }
    menuDisplayTime = millis();
  }

  // Goto menu
  if(digitalRead(leftButtonPin) == LOW){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Go to piano     ");
    lcd.setCursor(0, 1);
    lcd.print("menu...         ");
    delay(2000);
    piano();
  }
  else if(digitalRead(rightButtonPin) == LOW){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Go to pitch test");
    lcd.setCursor(0, 1);
    lcd.print("menu...         ");
    delay(2000);
    pitchTest();  
  }

  // LED dimming effect
  fadeLed();
}
