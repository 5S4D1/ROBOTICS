#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//    ==========================
//    ||   GND  ->  GND       ||
//    ||   VDD  ->  3.3V      ||
//    ||   SCK  ->  D22       ||
//    ||   SDA  ->  D21       ||
//    ==========================

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
  Serial.begin(9600);
  
  // initialize the OLED object
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer.
  display.clearDisplay();

  // Display Inverted Text
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.setCursor(0,28);
  display.setTextSize(2);
  display.print("GEARHEAD");
  display.write(3);
  display.display();
  delay(5000);
  display.clearDisplay();

  // Change bg color to black
  display.setTextColor(WHITE);

  // Scroll part of the screen
  display.setCursor(1,2);
  display.setTextSize(2);
  display.println("NOT NOW");
  display.println();
  display.println("STAY");
  display.println("AWAY...");
  display.display();
  display.startscrollleft(0x00, 0x01);
}

void loop() {
}