// 2025 01 27
// Simple sketch via Arduino IDE 
// with assistance from ChatGPT-4o
// 
// Script provides a baseline for an ESP32 Wroom 32E devkit connected to a Waveshare St7789 240x240 display
// This script displays the Date, Time, IP address via WiFI and the mac address of the ESP32
// 
// Wiring and pin connections are as shown below in the DEFINE section
// With the Adafruit libraruy for ST7789, no other additional hacks are required for display configuration
//

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>

// Define pins for the display
#define TFT_CS    22  // Chip Select
#define TFT_DC    16  // Data/Command
#define TFT_RST   4   // Reset
#define TFT_MOSI  23  // Data In (DIN)
#define TFT_SCLK  18  // Clock (CLK)

// WiFi credentials
const char* ssid = "xxxxx"; // Replace with your WiFi SSID
const char* password = "xxxxxxxxxxxxx"; // Replace with your WiFi Password

// NTP server settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0; // Adjust for your timezone (e.g., -18000 for EST)
const int   daylightOffset_sec = 3600; // Daylight saving time offset

// Initialize the ST7789 display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

String previousDate = "";
String previousTime = "";

void setup() {
  // Initialize the serial monitor
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize the display
  tft.init(240, 240); // ST7789 is a 240x240 pixel display
  tft.setRotation(0); // Set rotation (0 to 3 depending on orientation preference)

  // Fill the screen with black
  tft.fillScreen(ST77XX_BLACK);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 50);
  tft.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Connected to WiFi
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 30);
  tft.println("WiFi Connected");

  // Display static information (MAC and IP address)
  tft.setCursor(10, 90);
  tft.println("IP: " + WiFi.localIP().toString());
    tft.setCursor(10, 120);
  tft.println(WiFi.macAddress());

  // Initialize NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  delay(2000);
}

void loop() {
  // Get current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    tft.setCursor(10, 60);
    tft.println("Failed to get time");
    delay(1000);
    return;
  }

  // Get formatted date and time
  char dateStr[32];
  char timeStr[32];
  strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo);
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

  String currentDate = String(dateStr);
  String currentTime = String(timeStr);

  // Update only if date or time changes
  if (currentDate != previousDate) {
    tft.fillRect(10, 30, 220, 20, ST77XX_BLACK); // Clear only the date area
    tft.setCursor(10, 30);
    tft.setTextColor(ST77XX_WHITE);
    tft.println("Date: " + currentDate);
    previousDate = currentDate;
  }

  if (currentTime != previousTime) {
    tft.fillRect(10, 60, 220, 20, ST77XX_BLACK); // Clear only the time area
    tft.setCursor(10, 60);
    tft.setTextColor(ST77XX_WHITE);
    tft.println("Time: " + currentTime);
    previousTime = currentTime;
  }

  delay(1000); // Update every second
}
