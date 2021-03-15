#include <krpc.h>
#include <krpc/services/krpc.h>
#include <krpc/services/space_center.h>

HardwareSerial * conn;
unsigned long currentMillis;
bool reset = false;

const int gearInPin = 22;
const int gearOutPin = 23;

const int lightsInPin = 24;
const int lightsOutPin = 25;

const int brakesInPin = 26;
const int brakesOutPin = 27;

const int abortInPin = 28;
const int abortOutPin = 29;
bool abortActive = false;
int abortLightState = LOW;
unsigned long previousAbortLightMillis = 0;
const long abortLightInterval = 1000;

const int activateNextStageInPin = 30;
bool stagingActive = false;

float throttleVal;
const float maxAnalogVal = 1023;
const int throttleInputPin = A0;

krpc_SpaceCenter_Flight_t flight;
krpc_SpaceCenter_Control_t control;
krpc_SpaceCenter_Vessel_t vessel;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(gearInPin, INPUT_PULLUP);
  pinMode(gearOutPin, OUTPUT);
  pinMode(lightsInPin, INPUT_PULLUP);
  pinMode(lightsOutPin, OUTPUT);
  pinMode(brakesInPin, INPUT_PULLUP);
  pinMode(brakesOutPin, OUTPUT);
  pinMode(abortInPin, INPUT_PULLUP);
  pinMode(abortOutPin, OUTPUT);
  pinMode(activateNextStageInPin, INPUT_PULLUP);

  conn = &Serial;
  krpc_open(&conn, NULL);
  krpc_connect(conn, "Arduino Repeater");
  krpc_SpaceCenter_ActiveVessel(conn, &vessel);
  krpc_SpaceCenter_Vessel_Control(conn, &control, vessel);
  krpc_SpaceCenter_Vessel_Flight(conn, &flight, vessel, KRPC_NULL);

  // Indicate succesful connection by lighting the on-board LED
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  currentMillis = millis();

  // Check if game has been reset
//  if ((krpc_SpaceCenterVessel_Situation(conn) == KRPC_SPACECENTER_VESSELSITUATION_PRELAUNCH) & (reset == false)) {
//    reset = true;
//  }
  
  // Landing gear
  if (digitalRead(gearInPin) == HIGH) {
    krpc_SpaceCenter_Control_set_Gear(conn, control, true);
    digitalWrite(gearOutPin, HIGH);
  } else {
    krpc_SpaceCenter_Control_set_Gear(conn, control, false);
    digitalWrite(gearOutPin, LOW);
  }

  // Lights
  if (digitalRead(lightsInPin) == HIGH) {
    krpc_SpaceCenter_Control_set_Lights(conn, control, true);
    digitalWrite(lightsOutPin, HIGH);
  } else {
    krpc_SpaceCenter_Control_set_Lights(conn, control, false);
    digitalWrite(lightsOutPin, LOW);
  }

  // Brakes
  if (digitalRead(brakesInPin) == HIGH) {
    krpc_SpaceCenter_Control_set_Brakes(conn, control, true);
    digitalWrite(brakesOutPin, HIGH);
  } else {
    krpc_SpaceCenter_Control_set_Brakes(conn, control, false);
    digitalWrite(brakesOutPin, LOW);
  }

  // Abort
  if (digitalRead(abortInPin) == HIGH) {
//    krpc_SpaceCenter_Control_set_Abort(conn, control, true);
    abortActive = true;
  }

  // Abort light control
  if ((currentMillis - previousAbortLightMillis >= abortLightInterval) & (abortActive)) {
    previousAbortLightMillis = currentMillis;
    
    if (abortLightState == LOW) {
      abortLightState = HIGH;
    } else {
      abortLightState = LOW;
    }
    digitalWrite(abortOutPin, abortLightState);
  }

  // Activate next stage
  if ((digitalRead(activateNextStageInPin) == HIGH) & (stagingActive == false)) {
    krpc_list_object_t stageResults;
    krpc_SpaceCenter_Control_ActivateNextStage(conn, &stageResults, control);
    stagingActive = true;
  } else {
    stagingActive = digitalRead(activateNextStageInPin);
  }

//   Read throttle input
  throttleVal = (float) 1 - (maxAnalogVal - (float) analogRead(throttleInputPin)) / maxAnalogVal;
  krpc_SpaceCenter_Control_set_Throttle(conn, control, throttleVal);
}
