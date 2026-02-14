#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TM1637Display.h>
#include <EEPROM.h>

// PIN DEFINITION
#define BTN_UP     4
#define BTN_DOWN   5
#define BTN_LEFT   6
#define BTN_RIGHT  7
#define BTN_SELECT 2 
#define BUZZER     3 

// PIN 7-SEGMENT (TM1637)
#define CLK 11
#define DIO 12

Adafruit_SSD1306 oled(128, 64, &Wire, -1);
TM1637Display seg7(CLK, DIO);

enum State { MENU, MANUAL, PRESET, GAME, RUNNING, GAMEOVER };
State currentState = MENU;

int mnt = 0, dtk = 0, menuIdx = 0;
long totalDetik = 0;
unsigned long lastMillis = 0;
unsigned long lastDebounce = 0; 
int snakeX[30], snakeY[30], snakeLen, dirX, dirY, foodX, foodY, score, highScore;
const char* menuItems[] = {"MANUAL TIMER", "PRESET TIMER", "SNAKE GAME"};

// --- PROTOTYPE FUNCTIONS ---
void handleMenu();
void handleManual();
void handlePreset();
void handleCountdown();
void handleSnake();
void handleGameOver();
void playStartSound();

void setup() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH); // Inisialisasi awal Low Trigger (HIGH = MATI)

  if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { for(;;); }
  seg7.setBrightness(0x0f);
  
  highScore = EEPROM.read(0);
  if (highScore == 255) highScore = 0;

  oled.clearDisplay();
  oled.setTextSize(2); oled.setTextColor(WHITE);
  oled.setCursor(10, 20); oled.println("SUMBUL OS");
  oled.display();
  
  playStartSound(); //
  delay(1000);
}

void loop() {
  switch (currentState) {
    case MENU:    handleMenu(); break;
    case MANUAL:  handleManual(); break;
    case PRESET:  handlePreset(); break;
    case RUNNING: handleCountdown(); break;
    case GAME:    handleSnake(); break;
    case GAMEOVER: handleGameOver(); break;
  }
}

// --- LOGIKA MENU UTAMA ---
void handleMenu() {
  if (millis() - lastDebounce > 250) {
    if (digitalRead(BTN_UP) == LOW) { menuIdx--; tone(BUZZER, 800, 50); if(menuIdx < 0) menuIdx = 2; lastDebounce = millis(); }
    if (digitalRead(BTN_DOWN) == LOW) { menuIdx++; tone(BUZZER, 800, 50); if(menuIdx > 2) menuIdx = 0; lastDebounce = millis(); }
    if (digitalRead(BTN_SELECT) == LOW) {
      tone(BUZZER, 1200, 100); lastDebounce = millis();
      if (menuIdx == 0) currentState = MANUAL;
      else if (menuIdx == 1) currentState = PRESET;
      else if (menuIdx == 2) { resetSnake(); currentState = GAME; }
    }
  }
  if (millis() - lastDebounce > 100) digitalWrite(BUZZER, HIGH); // Anti-dengung

  oled.clearDisplay();
  oled.setTextSize(1); oled.setCursor(25, 0); oled.println("- MAIN MENU -");
  oled.drawLine(0, 10, 128, 10, WHITE);
  for(int i=0; i<3; i++) {
    oled.setCursor(10, 25 + (i*12));
    if(menuIdx == i) oled.print("> "); else oled.print("  ");
    oled.println(menuItems[i]);
  }
  oled.display();
  seg7.showNumberDec(highScore);
}

// --- LOGIKA TIMER MANUAL ---
void handleManual() {
  if (millis() - lastDebounce > 150) {
    if(digitalRead(BTN_UP) == LOW) { mnt++; lastDebounce=millis(); }
    if(digitalRead(BTN_DOWN) == LOW) { if(mnt > 0) mnt--; lastDebounce=millis(); }
    if(digitalRead(BTN_RIGHT) == LOW) { dtk += 5; if(dtk > 59) dtk = 0; lastDebounce = millis(); }
    if(digitalRead(BTN_LEFT) == LOW) { dtk -= 5; if(dtk < 0) dtk = 55; lastDebounce = millis(); }
    if(digitalRead(BTN_SELECT) == LOW) { 
      totalDetik = (mnt * 60L) + dtk; 
      if(totalDetik > 0) { currentState = RUNNING; lastMillis = millis(); }
      lastDebounce = millis(); 
    }
  }
  if (millis() - lastDebounce > 100) digitalWrite(BUZZER, HIGH);

  oled.clearDisplay();
  oled.setTextSize(1); oled.setCursor(15, 0); oled.println("SET TIMER (MIN):");
  oled.setTextSize(4); oled.setCursor(40, 25);
  if(mnt < 10) oled.print("0"); oled.print(mnt); // Menit saja di OLED
  oled.display();

  seg7.showNumberDecEx(mnt * 100 + dtk, 0x40, true); // Menit & Detik di 7-Segment
}

// --- LOGIKA PRESET TIMER ---
void handlePreset() {
  oled.clearDisplay();
  oled.setTextSize(1); oled.setCursor(0, 0); oled.println("CHOOSE PRESET:");
  oled.setCursor(0, 15);
  oled.println("UP    : 1 Min");
  oled.println("DOWN  : 5 Min");
  oled.println("RIGHT : 10 Min");
  oled.println("LEFT  : Back");
  oled.display();

  if (millis() - lastDebounce > 250) {
    if(digitalRead(BTN_UP) == LOW)    { mnt=1; dtk=0; totalDetik=60; currentState=RUNNING; lastDebounce=millis(); }
    if(digitalRead(BTN_DOWN) == LOW)  { mnt=5; dtk=0; totalDetik=300; currentState=RUNNING; lastDebounce=millis(); }
    if(digitalRead(BTN_RIGHT) == LOW) { mnt=10; dtk=0; totalDetik=600; currentState=RUNNING; lastDebounce=millis(); }
    if(digitalRead(BTN_LEFT) == LOW)  { currentState = MENU; lastDebounce=millis(); }
  }
  if (millis() - lastDebounce > 100) digitalWrite(BUZZER, HIGH);
}

// --- LOGIKA COUNTDOWN ---
void handleCountdown() {
  if (millis() - lastMillis >= 1000) {
    lastMillis = millis();
    if (totalDetik > 0) { totalDetik--; mnt = totalDetik / 60; dtk = totalDetik % 60; }
    else { 
      for(int i=0; i<3; i++){ tone(BUZZER, 1000, 500); delay(600); digitalWrite(BUZZER, HIGH); }
      currentState = MENU; 
    }
  }
  seg7.showNumberDecEx(mnt * 100 + dtk, 0x40, true);
  oled.clearDisplay();
  oled.setTextSize(1); oled.setCursor(35, 0); oled.print("COUNTDOWN");
  oled.setTextSize(4); oled.setCursor(40, 25);
  if(mnt < 10) oled.print("0"); oled.print(mnt);
  oled.display();
  if(digitalRead(BTN_SELECT) == LOW) { currentState = MENU; delay(300); }
}

// --- LOGIKA SNAKE GAME ---
void resetSnake() {
  snakeLen = 3; score = 0; dirX = 1; dirY = 0;
  for(int i=0; i<snakeLen; i++) { snakeX[i] = 5-i; snakeY[i] = 5; }
  foodX = random(1, 30); foodY = random(1, 14);
}

void handleSnake() {
  if (millis() - lastDebounce > 100) {
    if(digitalRead(BTN_UP) == LOW && dirY != 1)    { dirX=0; dirY=-1; lastDebounce=millis(); }
    if(digitalRead(BTN_DOWN) == LOW && dirY != -1)  { dirX=0; dirY=1; lastDebounce=millis(); }
    if(digitalRead(BTN_LEFT) == LOW && dirX != 1)   { dirX=-1; dirY=0; lastDebounce=millis(); }
    if(digitalRead(BTN_RIGHT) == LOW && dirX != -1) { dirX=1; dirY=0; lastDebounce=millis(); }
  }
  if (millis() - lastDebounce > 80) digitalWrite(BUZZER, HIGH);

  static unsigned long lM = 0;
  if(millis() - lM > 150) {
    lM = millis();
    for(int i=snakeLen-1; i>0; i--) { snakeX[i] = snakeX[i-1]; snakeY[i] = snakeY[i-1]; }
    snakeX[0] += dirX; snakeY[0] += dirY;
    if(snakeX[0] == foodX && snakeY[0] == foodY) {
      snakeLen++; score++; foodX = random(1, 30); foodY = random(1, 14);
      tone(BUZZER, 2000, 30);
    }
    if(snakeX[0] < 0 || snakeX[0] > 31 || snakeY[0] < 0 || snakeY[0] > 15) {
      if(score > highScore) { highScore = score; EEPROM.write(0, highScore); }
      currentState = GAMEOVER;
    }
  }
  seg7.showNumberDec(score);
  oled.clearDisplay();
  for(int i=0; i<snakeLen; i++) oled.fillRect(snakeX[i]*4, snakeY[i]*4, 3, 3, WHITE);
  oled.fillRect(foodX*4, foodY*4, 3, 3, WHITE);
  oled.display();
}

void handleGameOver() {
  oled.clearDisplay();
  oled.setTextSize(2); oled.setCursor(10, 25); oled.print("GAME OVER");
  oled.display();
  tone(BUZZER, 200, 1000);
  delay(1000);
  digitalWrite(BUZZER, HIGH);
  currentState = MENU;
}

void playStartSound() {
  int melody[] = {659, 587, 370, 415};
  for (int i = 0; i < 4; i++) {
    tone(BUZZER, melody[i], 150);
    delay(200);
  }
  noTone(BUZZER);
  digitalWrite(BUZZER, HIGH);
}