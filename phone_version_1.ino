#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string>
#include <list>
#include <vector>
#include <BluetoothSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BluetoothSerial serialBT;

String text;
int cursor_x = 0;
int cursor_y = 0;

String lowlist = "abcdefghijklmnopqrstuvwxyz ";
String uplist = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
String list = lowlist;
String output_text;
class app{
  public:
    app(){
      int index;
    }
    
};
void cursor(int x,int y,int interior_size,int exterior_size){
  display.drawRect(x, y, exterior_size, exterior_size, SSD1306_WHITE);
  display.drawRect((exterior_size-interior_size)/2 +x, (exterior_size-interior_size)/2+y, interior_size, interior_size, SSD1306_BLACK);
}

void screen1(String text,String text2){
  for (int i = 0; i < text.length(); i++) {
    char c = text[i];
    display.setCursor(((i%16)*8)+1, ((i/16)*8)+1);
    display.print(c);
  }
  display.setCursor(0, 56);
  display.print(text2);
  display.setCursor(112, 55);
  display.print("->");
}

void setup() {
  serialBT.begin("ESP32_phone");
  pinMode(4, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  // put your setup code here, to run once:
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setCursor(1, 1);
  display.setTextWrap(true); 
  output_text = "";
}

void loop() {
  int norm_pos = cursor_y*16+cursor_x;
  int right = digitalRead(4);
  int down = digitalRead(12);
  int up = digitalRead(13);
  int left = digitalRead(14);
  int button_one = digitalRead(15);
  int button_two = digitalRead(25);
  // put your main code here, to run repeatedly:
  display.setTextColor(SSD1306_WHITE);
  if(Serial.available()>0){
    text = Serial.readStringUntil('\n'); 
  }
  if (serialBT.available()) {
      text = serialBT.readStringUntil('\n');   // reads full message
      text.trim();  // remove newline or spaces
  }
  if (up == 0) {
    if (cursor_y > 0) cursor_y--;
  }
  if (down == 0) {
      if (cursor_y < 1) cursor_y++;   // only 2 rows
  }
  if (left == 0) {
      if (cursor_x > 0) cursor_x--;
  }
  if (right == 0) {
      if (cursor_x < 15) cursor_x++;
  }

// SELECT BUTTON
  if (button_one == 0) {
    int normalized_position = cursor_y * 16 + cursor_x;

    // Wait for release to debounce
    delay(10);
    while (digitalRead(15) == 0) delay(10);

    if (normalized_position == 27) {
      // Toggle shift once
      list = (list == lowlist) ? uplist : lowlist;
    }
    else if (normalized_position == 28) {
      // Backspace
      if (output_text.length() > 0) output_text.remove(output_text.length() - 1);
    }
    else if (normalized_position <= 26) {
      // Normal character
      output_text += list[normalized_position];
    }
  }
  if(button_two == 0){
    Serial.println(output_text);
    serialBT.println(output_text);
    output_text = "";
  }
  display.clearDisplay();
  cursor(cursor_x*8,cursor_y*8,6,8);
  if (list == lowlist){
  screen1("abcdefghijklmnopqrstuvwxyz_^<",output_text);
  }
  if (list == uplist){
    screen1("ABCDEFGHIJKLMNOPQRSTUVWXYZ_^<", output_text);
  }
  display.setCursor(0, 16);
  display.print(text);
  display.display();
  delay(100);
}
