#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "secrets.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1 // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long currentMillis = 0;
unsigned long previousDisplayMillis = 0;

const char *album = "";
const char *artist = "";
const char *song = "";
const char *progress = "";
const char *duration = "";
const char *nextSong = "";
const char *nextArtist = "";

void setup()
{
  Serial.begin(9600);

  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  // initialize the OLED object
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Clear the buffer.
  display.clearDisplay();
  display.setTextWrap(false);

  // Display Text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 28);
  display.println("Connecting...");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop()
{
  currentMillis = millis();

  if (currentMillis - previousDisplayMillis >= 3000)
  {
    previousDisplayMillis += 3000;

    // check if we're connected to the server (and the internet
    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient client;

      client.begin(SERVER_IP + String("/current-playback-state"));
      int httpCode = client.GET();

      if (httpCode > 0)
      {
        String payload = client.getString();

        if (payload != "error")
        {
          StaticJsonDocument<0> filter;
          filter.set(true);

          StaticJsonDocument<384> doc;

          DeserializationError error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));

          if (error)
          {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
          }

          album = doc["album"];
          artist = doc["artists"];
          song = doc["song"];
          progress = doc["progress"];
          duration = doc["duration"];
          nextSong = doc["nextSong"];
          nextArtist = doc["nextArtist"];

          // display the text on the screen
          display.clearDisplay();

          // display.setCursor(x, 0);
          display.setCursor(0, 0);
          display.setTextSize(1);
          display.print(String(song));
          // display.startscrollleft(0x00, 0x00);

          display.setCursor(0, 10);
          display.print(String(artist));

          display.setCursor(0, 20);
          display.print(String(album));

          display.setCursor(0, 30);
          display.print(String(progress) + String(" / ") + String(duration));

          // display next song
          display.setCursor(0, 40);
          display.print(String(">>> ") + String(nextSong));

          display.setCursor(0, 50);
          display.print(String("    ") + String(nextArtist));

          display.display();
        }

        client.end();
      }
      else
      {
        Serial.println("Error on HTTP request");
      }
    }
    else
    {
      Serial.println("Not connected to WiFi");
    }
  }
}