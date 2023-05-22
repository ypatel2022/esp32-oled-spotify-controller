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
const char *song = "this is a test song name before it gets updated";
long long timestamp = 0;
long progress = 0;

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

      client.begin(SERVER_IP + String("/is-playing"));
      int httpCode = client.GET();

      if (httpCode > 0)
      {
        String payload = client.getString();
        // Serial.println("\nStatus Code: " + String(httpCode));
        // Serial.println(payload);

        if (payload == "true")
        {
          // Serial.println("Getting Currently Playing Song");

          client.begin(SERVER_IP + String("/currently-playing"));
          httpCode = client.GET();

          if (httpCode > 0)
          {
            payload = client.getString();
            // Serial.println("\nStatus Code: " + String(httpCode));
            // Serial.println(payload);

            StaticJsonDocument<0> filter;
            filter.set(true);

            StaticJsonDocument<192> doc;

            DeserializationError error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));

            if (error)
            {
              Serial.print("deserializeJson() failed: ");
              Serial.println(error.c_str());
              return;
            }

            album = doc["album"];         // "Luv Is Rage 2"
            artist = doc["artist"];       // "Lil Uzi Vert"
            song = doc["song"];           // "The Way Life Goes (feat. Oh Wonder)"
            timestamp = doc["timestamp"]; // 1684637890615
            progress = doc["progress"];   // 166610

            // display the text on the screen
            display.clearDisplay();
            display.setTextColor(WHITE);

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
            display.print(String(miliseconds_to_minutes(progress)));

            display.display();
          }
          else
          {
            Serial.println("Error on HTTP request");
          }
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

String miliseconds_to_minutes(long miliseconds)
{
  long seconds = miliseconds / 1000;
  long minutes = seconds / 60;
  seconds = seconds % 60;

  String secondsString = String(seconds);
  if (seconds < 10)
  {
    secondsString = "0" + secondsString;
  }

  return String(minutes) + ":" + secondsString;
}