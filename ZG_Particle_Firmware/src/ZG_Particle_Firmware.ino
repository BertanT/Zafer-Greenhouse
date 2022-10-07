/*
 * Project ZG_Particle_Firmware
 * Description: Particle Photon Greenhouse Management and Monitoring System
 * Author: Bertan
 * Date: October 10 2020
 */

/*
  The firmware includes code for all functionality to be used in the greenhouse, that is:
  - Environmental monitoring (temperature, humidity, CO2)
  - Soil moisture monitoring for 5 pots
  - Watering control with a solenoid valve

  As multiple Particle devices will be used in the greenhouse and each will be responsible of carrying out different tasks,
  you should modify the code for each device by commenting out the functionality you don't need.

  If anyone from my school is reading this after several years, HI THERE! :) Please don't hesitate to contact me with any questions.
*/

// Include libraries for the environmental monitoring sensor
#include <DFRobot_ENS160.h>
#include "DFRobot_BME280.h"

// Define soil moisture sensor pins
#define soil_1 A0
#define soil_2 A1
#define soil_3 A2
#define soil_4 A3
#define soil_5 A4

// Define solenoid water valve pin (valve will be connected to an N-type MOSFET. See schematic for details.)
#define water_valve D6

/*
  Calibrate each soil moisture data and enter the corresponding values here
  Check out the wiki PDF for the soil moisture sensor for more info on how to calibrate
*/
#define MOISTURE_SENSOR_1_DRY 0
#define MOISTURE_SENSOR_1_WET 4095

#define MOISTURE_SENSOR_2_DRY 0
#define MOISTURE_SENSOR_2_WET 4095

#define MOISTURE_SENSOR_3_DRY 0
#define MOISTURE_SENSOR_3_WET 4095

#define MOISTURE_SENSOR_4_DRY 0
#define MOISTURE_SENSOR_4_WET 4095

#define MOISTURE_SENSOR_5_DRY 0
#define MOISTURE_SENSOR_5_WET 4095

// Arduino standard setup method
void setup() {
  // Set soil moisture sensor pin modes
  pinMode(soil_1, INPUT);
  pinMode(soil_2, INPUT);
  pinMode(soil_3, INPUT);
  pinMode(soil_4, INPUT);
  pinMode(soil_5, INPUT);

  // Set solenoid water valve pin mode
  pinMode(water_valve, OUTPUT);

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

}