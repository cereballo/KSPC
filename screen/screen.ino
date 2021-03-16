#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Adafruit_SPIFlash.h>
#include "Adafruit_ADT7410.h"

#include <krpc.h>
#include <krpc/services/krpc.h>
#include <krpc/services/space_center.h>

HardwareSerial * conn;

#define RED_LED       13
#define TFT_RESET     24
#define TFT_BACKLIGHT 25
#define LIGHT_SENSOR  A2
#define SD_CS         32       
#define SPKR_SHUTDOWN 50

#define TFT_D0        34 // Data bit 0 pin (MUST be on PORT byte boundary)
#define TFT_WR        26 // Write-strobe pin (CCL-inverted timer output)
#define TFT_DC        10 // Data/command pin
#define TFT_CS        11 // Chip-select pin
#define TFT_RST       24 // Reset pin
#define TFT_RD         9 // Read-strobe pin
#define TFT_BACKLIGHT 25

krpc_SpaceCenter_Flight_t flight;
krpc_SpaceCenter_Control_t control;
krpc_SpaceCenter_Vessel_t vessel;
krpc_SpaceCenter_Resources_t resources;
krpc_list_object_t resourceList;

Adafruit_ILI9341 tft = Adafruit_ILI9341(tft8bitbus, TFT_D0, TFT_WR, TFT_DC, TFT_CS, TFT_RST, TFT_RD);
Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS, PIN_QSPI_IO0, PIN_QSPI_IO1, PIN_QSPI_IO2, PIN_QSPI_IO3);
Adafruit_SPIFlash flash(&flashTransport);

Adafruit_GFX gfx = Adafruit_GFX();

void setup() {
  Serial.begin(115200);

  pinMode(RED_LED, OUTPUT);
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);
  pinMode(TFT_RESET, OUTPUT);

  tft.begin();

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextWrap(true);
  tft.setCursor(0, 0);

  tft.print("Connecting to KRPC server...");

  conn = &Serial;
  krpc_open(&conn, NULL);
  krpc_connect(conn, "Arduino Repeater");
  krpc_SpaceCenter_ActiveVessel(conn, &vessel);
  krpc_SpaceCenter_Vessel_Control(conn, &control, vessel);
  krpc_SpaceCenter_Vessel_Flight(conn, &flight, vessel, KRPC_NULL);
  krpc_SpaceCenter_Vessel_Resources(conn, &resources);

  tft.setTextColor(ILI9341_GREEN);
  tft.println("success");
}

void loop() {
  // Get current speed
  double speed;
  krpc_SpaceCenter_Flight_Speed(conn, &speed);

  // Display current speed
  tft.setTextColor(ILI9341_WHITE);
  tft.print("Speed: ");
  tft.setTextColor(ILI9341_GREEN);
  tft.println(speed + " m/s");

  // Get all resource names
  krpc_SpaceCenter_Resources_Names(conn, &resourceList);
  Serial.println(resourceList);

  // Get max fuel level
  // double maxFuelLevel;
  // krpc_SpaceCenter_Resources_Max(conn, &maxFuelLevel, "");

  // Get fuel level
  // double currentFuelLevel;
  // krpc_SpaceCenter_Resources_Amount(conn, &currentFuelLevel, "liquid_rocket_propellent");

  // Show fuel status as bar
  // const int maxBarWidth = 100;
  // int fuelBarWidth = (int) ((double) fuelLevel / (double) maxFuelLevel * (double) maxBarWidth);
  // gfx.fillRect(120, 100, fuelBarWidth, 30, ILI9341_RED);
}