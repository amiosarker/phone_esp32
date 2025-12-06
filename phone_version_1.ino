#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string>
#include <list>
#include <vector>
#include <BluetoothSerial.h>
#include <array>
#include <iostream>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BluetoothSerial serialBT;
int up = 13;
int down = 18;
int left = 4;
int right = 14;
int one = 15;
int two = 25;
class app{
  public:
    int cursor_x = 0;
    int cursor_y = 0;
    int index = 0;
    String text;
    app(){
      int index = 0;
    }
    void cursor(int x,int y,int interior_size,int exterior_size){
      display.drawRect(x, y, exterior_size, exterior_size, SSD1306_WHITE);
      display.drawRect((exterior_size-interior_size)/2 +x, (exterior_size-interior_size)/2+y, interior_size, interior_size, SSD1306_BLACK);
    }
    virtual void up(bool up){
      if (up == true){
        Serial.print("up");
      }
    }
    virtual void down(bool down){
      if (down == true){
        Serial.print("down");
      }
    }
    virtual void left(bool left){
      if (left == true){
        Serial.print("left");
      }
    }
    virtual void right(bool right){
      if (right == true){
        Serial.print("right");
      }
    }
    virtual void one(bool one){
      if (one == true){
        Serial.print("one");
      }
    }
    virtual void two(bool two){
      if (two == true){
        Serial.print("two");
      }
    }
    virtual void updateApp(){}
};
class text_app:public app{
  private:
    String lowlist = "abcdefghijklmnopqrstuvwxyz ";
    String uplist = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    String list = lowlist;

  public:
    String output_text;
    int index;
    text_app() {
      index = 0;          // sets class variable
      output_text = "";   // initialize text
      cursor_x = 0;
      cursor_y = 0;
    }
    int norm_pos() {
      return cursor_y * 16 + cursor_x;
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
    void up(bool up){
      if (up == 0) {
        if (cursor_y > 0) cursor_y--;
      }
    }
    void down(bool down){
      if (down == 0) {
          if (cursor_y < 1) cursor_y++;   // only 2 rows
      }
    }
    void left(bool left){
      if (left == 0) {
          if (cursor_x > 0) cursor_x--;
      }
    }
    void right(bool right){
      if (right == 0) {
          if (cursor_x < 15) cursor_x++;
      }
    }
    void one(bool button_one){
      if (button_one == 0) {
          // Wait for release to debounce
          delay(10);
          while (digitalRead(15) == 0) delay(10);

          if (norm_pos() == 27) {
            // Toggle shift once
            list = (list == lowlist) ? uplist : lowlist;
          }
          else if (norm_pos() == 28) {
            // Backspace
            if (output_text.length() > 0) output_text.remove(output_text.length() - 1);
          }
          else if (norm_pos() <= 26) {
            // Normal character
            output_text += list[norm_pos()];
          }
        }
    }
    void two(bool button_two){
      if (button_two == 0){
        Serial.println(output_text);
        serialBT.println(output_text);
        output_text = "";
      }  
    }
    void updateApp(){
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.clearDisplay();       // clear every frame

      // draw cursor
      cursor(cursor_x*8, cursor_y*8, 6, 8);

      // draw keyboard row
      if (list == lowlist){
          screen1("abcdefghijklmnopqrstuvwxyz_^<", output_text);
      } else {
          screen1("ABCDEFGHIJKLMNOPQRSTUVWXYZ_^<", output_text);
      }

      display.display();  // actually show the frame
    }

};
text_app textapp;

const int ammount_of_apps = 1;
std::array<app*, 1> applist = {&textapp} ;
void setup() {
  textapp.index = 0;
  serialBT.begin("ESP32_phone");
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(left, INPUT_PULLUP);
  pinMode(right, INPUT_PULLUP);
  pinMode(one, INPUT_PULLUP);
  pinMode(two, INPUT_PULLUP);
  // put your setup code here, to run once:
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setCursor(1, 1);
  display.setTextWrap(true); 
  textapp.output_text = "";
}

void loop() {
  // Read buttons
  int up_val    = digitalRead(up);
  int down_val  = digitalRead(down);
  int left_val  = digitalRead(left);
  int right_val = digitalRead(right);
  int one_val   = digitalRead(one);
  int two_val   = digitalRead(two);

  // Update app
  applist[0]->up(up_val);
  applist[0]->down(down_val);
  applist[0]->left(left_val);
  applist[0]->right(right_val);
  applist[0]->one(one_val);
  applist[0]->two(two_val);

  applist[0]->updateApp();  // draws and displays everything

  delay(50);
}
