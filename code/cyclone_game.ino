#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; // LCD pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Regestering LCD pins

const int red1 = 6, yellow1 = 7, green = 8, yellow2 = 9, red2 = 10; //LED pins
int leds[] = { // Array of LEDS

    red1, yellow1, green,
    yellow2, red2
  
};

const int mode_button = A0, play_button = A1; // Button pins

enum Modes { // Modes
  EASY,
  HARD,
  MASTER
};

enum Areas { // Different areas
  MENU,
  MODE_SELECT,
  INGAME,
  CREDITS
};

Modes mode = EASY; // Deffining the mode as Easy
Areas current_area = MENU; // The first area is the menu

int timer = 0, numofled = 0; // Variables related that are used for the cycling of the LEDs
bool high = false, on = false; 

int score, tries; // Variables for the players Score and how many tries they have left

int delay1 = 200, delay2 = 100, delay3 = 10, delay4 = 50, easy_mode_delay = 8, hard_mode_delay = 5, master_mode_delay = 4; //delay presets
int credits = 0;

void ledOn(int index) { // Turns off all LEDs except for one, indicated by the index variable

  for (int i = 0; i < 5; i++) {
    if (index - 1 != i) digitalWrite(leds[i], LOW);
    else digitalWrite(leds[i], HIGH); 
  } 
  
}

void allLedsOff() { // Turns all LEDs off
  for (int i = 0; i < 5; i++) {
    digitalWrite(leds[i], LOW);
  } 
}

void allLedsOn() { // Turns all LEDs on
  for (int i = 0; i < 5; i++) {
    digitalWrite(leds[i], HIGH);
  } 
}

void displayMessage(String message1, String message2, bool clear) { // Displays two row message
  
  if (clear) lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(message1);
  lcd.setCursor(0,1);
  lcd.print(message2);
  
}

void displayMessage(String message1, int message2, bool clear) { // Displays two row message
  
  if (clear) lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(message1);
  lcd.setCursor(0,1);
  lcd.print(message2);
  
}

bool pressed(int button_pin) {
  
  if (analogRead(button_pin) > 800) return true;
  return false;
  
}

void setup() {

  lcd.begin(16, 2); // Initiates LCD

  for (int i = 0; i < 5; i++) { // Sets all LED pins as output
    pinMode(leds[i], OUTPUT);  
  }

  pinMode(mode_button, INPUT); // Setting the mode button as an input
  pinMode(play_button, INPUT); // Setting the play button as an input
  
  allLedsOff(); // Turns all leds off
  
  Serial.begin(9600);

}

void loop() { // Loops infinately

  Serial.println(analogRead(play_button));
  Serial.println(analogRead(mode_button));

  if (current_area == MENU) { // While in the Menu
    timer++;
    if (timer == delay3 && !on) { // Led animation
        if (!high) numofled++;
        else numofled--;
        ledOn(numofled);
        timer = 0;
        if (numofled == 1) high = false;
        else if (numofled == 5) high = true; 
    } else if (timer == delay4 && on) { // Stays on led for 1 second
      on = false;
      timer = 0;
    }
    ledOn(numofled);
    displayMessage("Welcome!", "Press Play!", false);
    if (pressed(mode_button) && pressed(play_button)) { // If you hit both mode and play it sends you to the credits
      lcd.clear();
      allLedsOn();
      delay(500);
      allLedsOff();
      current_area = CREDITS;
      timer = 0;
    }else if (pressed(play_button)) { // If the second button is pressed, go to the Mode Selection Menu
      allLedsOn();
      delay(500);
      allLedsOff();
      displayMessage("Chose Your Mode!", "", true);
      delay(1000);
      displayMessage("Mode: Easy", "Press Play!", true);
      mode = EASY;
      current_area = MODE_SELECT;
      timer = 0;
      numofled = 1;
    } else if (pressed(mode_button)) { // Stops the LED
      on = true;
      timer = 0;
    }
    
  } else if (current_area == MODE_SELECT) { // If in the Mode Selection Menu 
    
    if (pressed(mode_button)) { // If the first button is pressed go to the next mode and displays that mode
      
      if (mode == EASY) {
        numofled = 0;
        high = false;
        timer = 0;
        displayMessage("Mode: Hard", "Press Play!", true);
        allLedsOn();
        mode = HARD;
        delay(500);
        allLedsOff();
         
      } else if (mode == HARD) {
        
        numofled = 0;
        high = false;
        timer = 0;
        displayMessage("Mode: Master", "Press Play!", true);
        allLedsOn();
        mode = MASTER;
        delay(500);
        allLedsOff();
         
      } else if (mode == MASTER) {
        numofled = 0;
        high = false;
        timer = 0;
        displayMessage("Mode: Easy", "Press Play!", true);
        allLedsOn();
        mode = EASY;
        delay(500);
        allLedsOff();
         
      }
    }
    
    if (pressed(play_button)) { // If the second button is pressed, start game and set the score to 0 and set the tries value to 5
      
      allLedsOn();
      delay(500);
      allLedsOff();
      displayMessage("Score: 0", "Tries: 5", true);
      tries = 5;
      score = 0;
      current_area = INGAME;
      
    }
  } else if (current_area == INGAME) { // If the player is playing the game
    int del = 0; // Sets delay time according to the mode
    timer++;
    switch (mode) {
      case EASY:
      del = easy_mode_delay;
      break;
      case HARD:
      del = hard_mode_delay;
      break;
      case MASTER:
      del = master_mode_delay;
      break;
    }
    if (tries != 0) { // If the player has more than 0 tries
      if (timer == del) { // Go to the next LED in the series, back and forth
        if (!high) numofled++;
        else numofled--;
        ledOn(numofled);
        timer = 0;
        if (numofled == 1) high = false;
        else if (numofled == 5) high = true; 
      }
    } else { // If the player's tries have ran out, display score until they press the play button
      displayMessage("Your Score was:", score, false);
      if (pressed(play_button) || pressed(mode_button)) {
        current_area = MENU;
        lcd.clear();
        allLedsOn();
        delay(500);
        allLedsOff();
        timer = 0;
      }
    }
    
    if ((pressed(play_button) || pressed(mode_button)) && tries != 0) { // If the button is pressed, add score depending on Mode and color, and descrease tries
      tries-=1; // Subtract one from the tries
      /* If the LED is green, add 10 if on Easy mode, add 25 on Hard mode, and add 50 if on Master mode
      If the LED is yellow, add one fifth of the green LED point value
      If the LED is red, add no points
      */
      if (mode == EASY) {
        if (numofled == 3) {
          
          int chance = random(4) + 1;
          if (chance != 4) {
            score += 10;
          } else {
            if (high) numofled--;
            else numofled++;
            ledOn(numofled);
            score+=2;
          }
          
        } else if (numofled == 2 || numofled == 4) {
          
          score += 2;
          
        }
      } else if (mode == HARD) {
        if (numofled == 3) {
          
          int chance = random(4) + 1;
          if (chance != 24) {
            score += 25;
          } else {
            if (high) numofled--;
            else numofled++;
            ledOn(numofled);
            score+=5;
          }
          
        } else if (numofled == 2 || numofled == 4) {
          
          score += 5;
          
        }
      } else {
        if (numofled == 3) {
          
          score += 50;
          
        } else if (numofled == 2 || numofled == 4) {
          
          score += 10;
          
        }
      }
      lcd.setCursor(7, 0); // Displays score
      lcd.print(score);
      lcd.setCursor(7, 1); // Displays tries
      lcd.print(tries);
      delay(500); // Wait 500 milliseconds
      if (tries == 0) {
        lcd.clear();
        timer = 0;
        allLedsOff();
      }
    }
    
  } else if (current_area == CREDITS) { //Displays credits
    
    timer++;
    if (timer == delay2) {
      credits++;
      lcd.clear();
      timer = 0;
    }
    if (credits == 0) displayMessage("Game By:", "Grant Cox", false);
    else if (credits == 1) displayMessage("Coded In:", "C/C++", false);
    else if (credits == 2) displayMessage("File Length:", "334 lines", false);
    else if (credits == 3) displayMessage("Micro Processor:", "Arduino Nano", false);
    else if (credits == 4) displayMessage("For more info", "Go to my website", false);
    else if (credits == 5) displayMessage("Thank You", "For Playing!", false);
    else {
      current_area = MENU;
      lcd.clear();
      credits = 0;
      timer = 0;
      numofled = 1;
      allLedsOff();
      on = false;
    }
    if (pressed(play_button) || pressed(mode_button)) { // if any button is pressed then go back to the menu
      current_area = MENU;
      lcd.clear();
      credits = 0;
      timer = 0;
      numofled = 1;
      allLedsOn();
      delay(500);
      allLedsOff();
      on = false;
    }
    
  }
  delay(10);
}
