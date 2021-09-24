/* Create By Academy
 *  https://www.youtube.com/channel/UC1THBy7ma-G4TPlAv0u6jqg
 *  https://www.facebook.com/Pi-Academy-109378171323222
 *  little.monkey.pi@gmail.com
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

Adafruit_8x16matrix matrix = Adafruit_8x16matrix();

TaskHandle_t TaskHandle_1;
TaskHandle_t TaskHandle_2;
TaskHandle_t TaskHandle_3;
TaskHandle_t TaskHandle_4;
TaskHandle_t TaskHandle_5;

#define LED1 17
#define LED2 2
#define LED3 15
#define LED4 12

#define button1 16
#define button2 14

#define fw_switch 32
#define bw_switch 33
#define throttle 34

#define relay1 18
#define relay2 19
#define buzzer_pin 13

#define LED_PIN 23 // neopixel 2x8LED
#define LED_COUNT 16
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

#define pwm1_pin 26
#define pwm2_pin 27

bool pressed = false;
int speed = 0;
int direction = 1;
bool siren = false;

// setting PWM properties
const int freq1 = 5000;
const int freq2 = 2000;
const int pwm1 = 0;
const int pwm2 = 1;
const int pwm3_buz = 2;
const int resolution = 8;

void Task1_control(void * parameter) { // control
  for (;;) {
   if(digitalRead(button1) == pressed){
     while(digitalRead(button1)==pressed){
       // Wait key release
     }
     if (speed != 3){
        speed++;
     }
     else{
        speed = 0;
     }
   }
   
   if(digitalRead(button2) == pressed){
     while(digitalRead(button2)==pressed){
       // Wait key release
     }
     vTaskDelay(10/portTICK_PERIOD_MS);
     siren = !siren;
     digitalWrite(relay1, siren ? HIGH : LOW);

     if(siren == true){
      xTaskCreatePinnedToCore(Task3_neopixel,"Task3",1000,NULL,1,&TaskHandle_3,1);
      xTaskCreatePinnedToCore(Task4_speaker,"Task4",1000,NULL,1,&TaskHandle_4,1);
      xTaskCreatePinnedToCore(Task5_blink,"Task5",1000,NULL,1,&TaskHandle_5,1);
     }
     else{
      vTaskDelete(TaskHandle_3);
      vTaskDelete(TaskHandle_4);
      vTaskDelete(TaskHandle_5);
     }
   }

   if(siren == false){
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
    //tone(0);
    ledcWrite(pwm3_buz, 0);
    vTaskDelay(10/portTICK_PERIOD_MS);
   }

   if(digitalRead(fw_switch) == 0){
     vTaskDelay(10/portTICK_PERIOD_MS);
     direction = 1; // Forward dirction
     Serial.println("Going Forward Direction");
   } 
   else if(digitalRead(bw_switch) == 0){
    vTaskDelay(10/portTICK_PERIOD_MS);
    direction = 2; // Backward dirction
    Serial.println("Going Backward Direction");
   }
   else {
    Serial.println("Neutral");
   }

   if (digitalRead(throttle) == 0){
    if((direction == 1)&&(digitalRead(fw_switch) == 0)){
      switch (speed) {
        case 1:
          ledcWrite(pwm1, 127);// 50%
          ledcWrite(pwm2, 0);
          Serial.println("Forward Mode 50%");
          vTaskDelay(10/portTICK_PERIOD_MS);
          break;
        case 2:
          ledcWrite(pwm1, 191);// 75%
          ledcWrite(pwm2, 0);
          Serial.println("Forward Mode 75%");
          vTaskDelay(10/portTICK_PERIOD_MS);
          break;
        case 3:
          ledcWrite(pwm1, 255);// 100%
          ledcWrite(pwm2, 0);
          Serial.println("Forward Mode 100%");
          vTaskDelay(10/portTICK_PERIOD_MS);
          break;
        default:
          ledcWrite(pwm1, 0);
          ledcWrite(pwm2, 0);
          Serial.println("Forward Mode 0%");
          vTaskDelay(10/portTICK_PERIOD_MS);
          break;
      }
    }
    else if ((direction == 2)&&(digitalRead(bw_switch) == 0)){
      ledcWrite(pwm1, 0);
      ledcWrite(pwm2, 127);
      Serial.println("Backward Mode 50%");
      vTaskDelay(10/portTICK_PERIOD_MS);
    }
   }
   else {
    ledcWrite(pwm1, 0);
    ledcWrite(pwm2, 0);
    vTaskDelay(10/portTICK_PERIOD_MS);
    Serial.println("Stop Mode 0%");
   }
  }
}

void Task2_8x16led_matrix(void * parameter) { // display 8x16 led matrix
  static const uint8_t PROGMEM
    speed0_11[] ={ B00111011,B00011010,B01011110,B00111100,B00011100,B11111100,B00101000,B11110000 },
    speed0_12[] ={ B01000100,B00101000,B10111010,B10111010,B01111111,B00010000,B00111000,B00111000 },
    speed0_21[] ={ B00111011,B00011010,B00011110,B11111100,B00011100,B11111100,B00101000,B11110000 },
    speed0_22[] ={ B01000100,B00101000,B10111000,B10111000,B01111000,B00010100,B00111010,B00111000 },
    speed0_31[] ={ B11111110,B10101011,B01010101,B11111111,B00111110,B00100010,B11100010,B00111110 },
    speed0_32[] ={ B00000000,B00000001,B00000001,B00000001,B00000000,B00000001,B00000001,B00000001 },
    speed0_41[] ={ B11111110,B10101011,B01010101,B11111111,B00111110,B00100010,B11100010,B00111110 },
    speed0_42[] ={ B00000000,B00000001,B00000001,B00000001,B00000000,B00000101,B00001101,B00000101 },
    speed0_51[] ={ B11111110,B10101011,B01010101,B11111111,B00111110,B00100010,B11100010,B00111110 },
    speed0_52[] ={ B00000000,B00000001,B00000001,B00000001,B00010000,B00110001,B00010001,B00000001 },
    speed0_61[] ={ B11111110,B10101011,B01010101,B11111111,B00111110,B00100010,B11100010,B00111110 },
    speed0_62[] ={ B00000000,B00000001,B00000001,B01000001,B11000000,B01000001,B00000001,B00000001 },
    speed0_71[] ={ B11111110,B10101011,B01010101,B11111111,B00111110,B00100010,B11100010,B00111110 },
    speed0_72[] ={ B00000000,B01000001,B10000001,B01000001,B11000000,B01000001,B10000001,B01000001 },
    speed0_81[] ={ B11111110,B10101011,B01010101,B11111111,B00111110,B00100010,B11100010,B00111110 },
    speed0_82[] ={ B00100000,B01000001,B10100001,B01100001,B11100000,B01100001,B10100001,B01000001 },
    speed0_91[] ={ B11111110,B10101011,B01010101,B11111111,B00111110,B00100010,B11100010,B00111110 },
    speed0_92[] ={ B00101000,B01010001,B10100001,B01111001,B11100000,B01111001,B10100001,B01001001 },
    speed0_101[] ={ B11111110,B10101011,B01010101,B11111111,B00111110,B00100010,B11100010,B00111110 },
    speed0_102[] ={ B00101000,B01010101,B10111001,B01110001,B11111100,B01110001,B10111001,B01010101 },
    speed1_11[] ={ B00000000,B00000001,B00011001,B00111101,B00110000,B00011000,B00001101,B00000111 },
    speed1_12[] ={ B11111110,B10101011,B01010101,B11111111,B01101010,B11101010,B10100010,B00011110 },
    speed1_21[] ={ B00000011,B00000110,B01100101,B11110111,B11000001,B01100011,B00110110,B00011100 },
    speed1_22[] ={ B11111000,B10101100,B01010100,B11111100,B10101000,B10101000,B10001000,B01111000 },
    speed1_31[] ={ B00001111,B00011010,B10010101,B11011111,B00000110,B10001110,B11011010,B01110001 },
    speed1_32[] ={ B11100000,B10110000,B01010001,B11110011,B10100011,B10100001,B00100000,B11100000 },
    speed1_41[] ={ B00111111,B01101101,B01010010,B01111111,B00011010,B00111010,B01101000,B11000111 },
    speed1_42[] ={ B10000000,B11000000,B11000110,B11001111,B10001100,B10000110,B10000011,B10000001 },
    speed1_51[] ={ B11111110,B10100111,B01001011,B11111111,B01101010,B11101010,B10100010,B00011110 },
    speed1_52[] ={ B00000000,B00000001,B00011001,B00111101,B00110000,B00011000,B00001101,B00000111 },
    speed1_61[] ={ B11111000,B01011100,B10101100,B11111100,B10101000,B10101000,B10001000,B01111000 },
    speed1_62[] ={ B00000011,B00000110,B01100101,B11110111,B11000001,B01100011,B00110110,B00011100 },
    speed1_71[] ={ B11100000,B01110000,B10110001,B11110011,B10100011,B10100001,B00100000,B11100000 },
    speed1_72[] ={ B00001111,B00011010,B10010101,B11011111,B00000110,B10001110,B11011010,B01110001 },
    speed2_11[] ={ B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B00000000 },
    speed2_12[] ={ B11111110,B01001000,B11111100,B11011111,B01111001,B00010000,B11101111,B00010000 },
    speed2_21[] ={ B00000011,B00000001,B00000011,B00000011,B00000001,B00000000,B00000111,B00000000 },
    speed2_22[] ={ B11111000,B00100000,B11110000,B01111100,B11100100,B01000000,B10111100,B01000000 },
    speed2_31[] ={ B00001111,B00000100,B00001111,B00001101,B00000111,B00000001,B00011110,B00000001 },
    speed2_32[] ={ B11100000,B10000000,B11000000,B11110000,B10010000,B00000000,B11110000,B00000000 },
    speed2_41[] ={ B00111111,B00010010,B00111111,B00110111,B00011110,B00000010,B01111011,B00000100 },
    speed2_42[] ={ B10000000,B00000000,B00000000,B11000000,B01000000,B00000000,B11000000,B00000000 },
    speed2_51[] ={ B11111110,B01001000,B11111100,B11011111,B01111001,B00010000,B11101111,B00010000 },
    speed2_52[] ={ B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B00000000 },
    speed2_61[] ={ B11111000,B00100000,B11110000,B01111100,B11100100,B01000000,B10111100,B01000000 },
    speed2_62[] ={ B00000011,B00000001,B00000011,B00000011,B00000001,B00000000,B00000111,B00000000 },
    speed2_71[] ={ B11100000,B01000000,B11000000,B11110000,B10010000,B00000000,B11110000,B00000000 },
    speed2_72[] ={ B00001111,B00000100,B00001111,B00001101,B00000111,B00000001,B00011110,B00000001 },
    speed2_81[] ={ B10000000,B00000000,B00000000,B11000000,B01000000,B00000000,B11000000,B00000000 },
    speed2_82[] ={ B00111111,B00010010,B00111111,B00110111,B00011110,B00000100,B01111011,B00000100 },
    speed3_11[] ={ B00000000,B00000000,B00000001,B00000001,B00000000,B00000000,B00000000,B00000000 },
    speed3_12[] ={ B00011100,B00111000,B11111110,B01010111,B11111111,B00111001,B00011100,B00001100 },
    speed3_21[] ={ B00000000,B00000000,B00000111,B00000101,B00000011,B00000000,B00000000,B00000000 },
    speed3_22[] ={ B01110000,B11100000,B11111000,B01011100,B11111100,B11100100,B01110000,B00110000 },
    speed3_31[] ={ B00000001,B00000011,B00011111,B00010101,B00001111,B00000011,B00000001,B00000000 },
    speed3_32[] ={ B11000000,B10000000,B11100000,B01110000,B11110000,B10010000,B11000000,B11000000 },
    speed3_41[] ={ B00000111,B00001110,B01111111,B01010101,B00111111,B00001110,B00000111,B00000011 },
    speed3_42[] ={ B00000000,B00000000,B10000000,B11000000,B11000000,B01000000,B00000000,B00000000 },
    speed3_51[] ={ B00011100,B00111000,B11111110,B01010111,B11111111,B00111001,B00011100,B00001100 },
    speed3_52[] ={ B00000000,B00000000,B00000001,B00000001,B00000000,B00000000,B00000000,B00000000 },
    speed3_61[] ={ B01110000,B11100000,B11111000,B01011100,B11111100,B11100100,B01110000,B00110000 },
    speed3_62[] ={ B00000000,B00000000,B00000111,B00000101,B00000011,B00000000,B00000000,B00000000 },
    speed3_71[] ={ B11000000,B10000000,B11100000,B01110000,B11110000,B10010000,B11000000,B11000000 },
    speed3_72[] ={ B00000001,B00000011,B00011111,B00010101,B00001111,B00000011,B00000001,B00000000 },
    speed3_81[] ={ B00000000,B10000000,B10000000,B11000000,B11000000,B01000000,B00000000,B00000000 },
    speed3_82[] ={ B00000111,B00001110,B01111111,B01010101,B00111111,B00001110,B00000111,B00000011 };
    
  for (;;) {
    switch (speed) {
      case 1:
          matrix.clear();
          matrix.drawBitmap(0, 0, speed1_11, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed1_12, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed1_21, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed1_22, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed1_31, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed1_32, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed1_41, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed1_42, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed1_51, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed1_52, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed1_61, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed1_62, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed1_71, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed1_72, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);
          break;
      case 2:
          matrix.clear();
          matrix.drawBitmap(0, 0, speed2_11, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed2_12, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(300/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed2_21, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed2_22, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(300/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed2_31, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed2_32, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(300/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed2_41, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed2_42, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(300/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed2_51, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed2_52, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(300/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed2_61, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed2_62, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(300/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed2_71, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed2_72, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(300/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed2_81, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed2_82, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(300/portTICK_PERIOD_MS);
          break;
      case 3:
          matrix.clear();
          matrix.drawBitmap(0, 0, speed3_11, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed3_12, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(100/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed3_21, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed3_22, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(100/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed3_31, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed3_32, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(100/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed3_41, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed3_42, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(100/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed3_51, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed3_52, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(100/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed3_61, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed3_62, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(100/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed3_71, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed3_72, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(100/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed3_81, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed3_82, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(100/portTICK_PERIOD_MS);
          break;
      default:
          matrix.clear();
          matrix.drawBitmap(0, 0, speed0_11, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed0_12, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed0_21, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed0_22, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed0_31, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed0_32, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed0_41, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed0_42, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed0_51, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed0_52, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed0_61, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed0_62, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed0_71, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed0_72, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed0_81, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed0_82, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed0_91, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed0_92, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);

          matrix.clear();
          matrix.drawBitmap(0, 0, speed0_101, 8, 8, LED_ON);
          matrix.drawBitmap(0, 8, speed0_102, 8, 8, LED_ON);
          matrix.writeDisplay();
          vTaskDelay(500/portTICK_PERIOD_MS);
          break;
    }
    matrix.setRotation(0);
  }
}

void Task3_neopixel(void * parameter) { // NeoPixel 16 LED Strip
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for (;;) {
    for(int a=0; a<30; a++) {  // Repeat 30 times...
      for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
        strip.clear();         //   Set all pixels in RAM to 0 (off)
        // 'c' counts up from 'b' to end of strip in increments of 3...
        for(int c=b; c<strip.numPixels(); c += 3) {
          // hue of pixel 'c' is offset by an amount to make one full
          // revolution of the color wheel (range 65536) along the length
          // of the strip (strip.numPixels() steps):
          int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
          uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
          strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        }
        strip.show();                // Update strip with new contents
        vTaskDelay(50/portTICK_PERIOD_MS);
        firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
      }
    }
  }
}

void Task4_speaker(void * parameter) { // NeoPixel 16 LED Strip
  int f;
  int d = 0;
  for (;;) {
      ledcWrite(pwm3_buz, 125);
      for (int i=1;i<=3;i++) {
        switch (i) {
          case 1:
            d = 7;
            break;
          case 2:
            d = 10;
            break;
          default:
            d = 0;
            break;
        }
        for(f=635;f<=912;f++) {
          ledcWriteTone(pwm3_buz, f);
          delay(d);
        }
        for(f=911;f>=634;f--) {
          ledcWriteTone(pwm3_buz, f);
          delay(d);
        }
        vTaskDelay(200/portTICK_PERIOD_MS);
      }
    ledcWrite(pwm3_buz, 0);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    
  }
}

void Task5_blink(void * parameter) { // Blink
  for (;;) {
    digitalWrite(LED1, HIGH);
    vTaskDelay(100/portTICK_PERIOD_MS);
    digitalWrite(LED2, HIGH);
    vTaskDelay(100/portTICK_PERIOD_MS);
    digitalWrite(LED3, HIGH);
    vTaskDelay(100/portTICK_PERIOD_MS);
    digitalWrite(LED4, HIGH);
    vTaskDelay(100/portTICK_PERIOD_MS);
    digitalWrite(LED1, LOW);
    vTaskDelay(100/portTICK_PERIOD_MS);
    digitalWrite(LED2, LOW);
    vTaskDelay(100/portTICK_PERIOD_MS);
    digitalWrite(LED3, LOW);
    vTaskDelay(100/portTICK_PERIOD_MS);
    digitalWrite(LED4, LOW);
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Pi Motorbike Started.");

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  pinMode(fw_switch, INPUT_PULLUP);
  pinMode(bw_switch, INPUT_PULLUP);
  pinMode(throttle, INPUT_PULLUP);
  
  pinMode(LED1, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);

  // config Motor PWM
  ledcSetup(pwm1, freq1, resolution);
  ledcSetup(pwm2, freq1, resolution);
  ledcSetup(pwm3_buz, freq2, resolution);
  ledcAttachPin(pwm1_pin, pwm1);
  ledcAttachPin(pwm2_pin, pwm2);
  ledcAttachPin(buzzer_pin, pwm3_buz);
  

  xTaskCreatePinnedToCore(Task1_control,"Task1",1000,NULL,3,&TaskHandle_1,0);
  xTaskCreatePinnedToCore(Task2_8x16led_matrix,"Task2",1000,NULL,2,&TaskHandle_2,1);

  matrix.begin(0x70);  // pass in the address

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  
  Serial.println();    
  Serial.println("Pi Motorbike Started.");
}
  
void loop()
{
  delay(1);
}
