 /*  Creator: Kelvin O. Estrada Soto
  *  Date: 09/08/2020 (mm/dd/yy)
  *  Collaborators: Alexander Monell Rodriguez, Luis Rivera, Jazar Abuellouf
  *  This script provides the functionality of a dummy brief case bomb 
  *  that used for Airsoft game modes. 
 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
/*LCD object*/
LiquidCrystal_I2C lcd(0x27, 16, 2);
/*Time*/
unsigned long currentTime = 0;
unsigned long currentMillis;
int period = 1000;
int minutes = 2;  //Time for airsoft game
int seconds = 0;
/*Pins*/
const int redPin = 26;
const int greenLed = 11;    
const int yellowLed = 12;
const int redLed = 13;
const int greenPin = 24;
const int buzz = 22;
const byte rows = 4; //four rows
const byte cols = 4; //three columns
/*Codes*/
String actCode = "1234#";
String deactCode = "7890#";
String code = "";
/*Flags*/
boolean redPressed = false;
boolean greenPressed = false;
boolean bombPlanted = false;
boolean defused = false;
byte rowPins[rows] = {53, 51, 49, 47}; //connect to the row pinouts of the keypad
byte colPins[cols] = {45, 43, 41, 39}; //connect to the column pinouts of the keypad
char time[16]; //16 characters, LCD maximum chars
String notPressed = "Presione verde";
/*Keypad mappings*/
char keys[rows][cols] = {
  {'1','4','7','*'},
  {'2','5','8','0'},
  {'3','6','9','#'},
  {'A','B','C','D'}
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);
int numTones = 62;
int tones[] = {587, 784, 880, 988, 988, 988, 988, 988, 988, 988, 932, 988, 784, 784, 784, 784, 784, 784, 784, 880, 988, 1047, 1047, 
1319, 1319, 1319, 1319, 1175, 1047, 988, 988, 988, 784, 880, 988, 1047, 1047, 1319, 1319, 1319, 1319, 1175, 1047, 988, 988, 784, 784, 
784, 784, 880, 988, 988, 988, 1047, 880, 880, 880, 988, 784, 784, 784, 784};

void setup() {
  /*Initialize LCD*/
  lcd.begin();
  lcd.clear();
  lcd.backlight();  
  delay(1000);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(buzz, OUTPUT);
  digitalWrite(greenLed, OUTPUT);

  lcd.setCursor(2,0);
  String text = "Desactivado";
  lcd.print(text);
  lcd.setCursor(1, 1);
  text = "Presione rojo";
  lcd.print(text);
  Serial.begin(9600);
}

void loop() {
  if(redPressed){
    handler();
  }
  if(!redPressed && !greenPressed){
  //Poll until red button is pressed  
    if(digitalRead(redPin) > 0){
      redPressed = true;  
      digitalWrite(greenLed, LOW);
      digitalWrite(yellowLed, HIGH);
      lcd.clear();
      delay(500);
      lcd.setCursor(1,0);
      lcd.print("Ingrese Codigo");
      lcd.setCursor(2,1);
      lcd.print("Y Presione #"); 
    }
  }
  if(desactivated(defused)){
    gameover();  
  }
  if(bombPlanted){
    countDown();
    if(!greenPressed){
      if(digitalRead(greenPin) > 0){
        greenPressed = true;
        code = "";
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, HIGH);
        lcd.clear();
        delay(500);
        //lcd.setCursor(1,1);
        //lcd.print("Ingrese Codigo");
      }
      else{
        greenPressed = false; 
      }    
    } 
  } 
}

/*Handles keypad input keys.*/
void handler(){
  char input = keypad.getKey();
  if(input != NO_KEY){
    code.concat(input);
    Serial.println(code);
    if(!bombPlanted && redPressed && input == '#' && code.equals(actCode)){
      digitalWrite(redLed, HIGH);
      digitalWrite(yellowLed, LOW);
      isValid();
      bombPlanted = true;
      code = "";
    }
    else if(!bombPlanted && input == '#' && !code.equals(actCode)){
      notValid();
      code = "";
    }
    else if(bombPlanted && greenPressed && input == '#' && code.equals(deactCode)){
      digitalWrite(greenLed, HIGH);
      digitalWrite(yellowLed, LOW);
      isValid();
      defused = true;
      code = "";  
    }
    else if(bombPlanted && greenPressed && input == '#' && !code.equals(deactCode)){
      notValid();
      code = "";  
    }
  }  
}

/*Methods to count down timer.*/
void countDown(){
  lcd.setCursor(0, 0);
  sprintf(time, "%0.2d:%0.2d", minutes, seconds);
  lcd.print("Detonando: " + String(time));
  lcd.setCursor(0, 1);
  lcd.print(notPressed);
  if(greenPressed){
      notPressed = "Ingrese Codigo";
  }
  currentMillis = millis();
  if((unsigned long)currentMillis - currentTime >= period){
     if(minutes > 0 && seconds <= 0){
       seconds = 59;
       minutes--;
     }
    if(minutes <= 0 && seconds <= 0){
      lcd.clear();
      delay(500);
      lcd.setCursor(3,0);
      lcd.print("Game over!!!");
      tone(buzz, 1000);
      delay(20000);
    }
    else{
      seconds--;
      currentTime = currentMillis;
      tone(buzz, 2000, 250);
    }
  }
}

/*Print to screen if entered code is valid.*/
void isValid(){
  lcd.clear();
  delay(150);
  lcd.print("Codigo Valido");
  delay(150);
  lcd.clear();
  delay(100); 
}

/*Checks if bomb has been defused.*/
boolean desactivated(boolean action){
  if(action){
    lcd.clear();
    delay(250);
    lcd.setCursor(2,0);
    lcd.print("Desactivado");
    bombPlanted = false;
    return true;
  }
  return false;  
}

/*Prints to LCD when entered code is invalid.*/
void notValid(){
  lcd.clear();
  lcd.print("Codigo Invalido");
  tone(buzz, 2200, 100);
  delay(120);
  tone(buzz, 2200, 100);
  delay(120);
  tone(buzz, 2200, 100);
  delay(120);  
  lcd.clear();
  //delay(100);
  lcd.setCursor(1,0);
  lcd.print("Ingrese Codigo");
  lcd.setCursor(2,1);
  lcd.print("Y Presione #");
}

/*Plays buzzer tones when bomb is defused.*/
void gameover(){
  for(int i = 0; i < numTones; i++){
    tone(buzz, tones[i]);
    delay(280);  
  }
  noTone(buzz);
}
