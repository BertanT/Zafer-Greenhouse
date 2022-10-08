/*
 * Project ZG_Particle_Firmware
 * Description: Particle Photon Greenhouse Management and Monitoring System
 * Author: Mehmet Bertan Tarakçıoğlu
 * Date: October 10 2020

  The firmware includes code for all functionality to be used in the greenhouse, that is:
  - Environmental monitoring (temperature, humidity, CO2)
  - Soil moisture monitoring for 5 pots
  - Watering control with a solenoid valve

  As multiple Particle devices will be used in the greenhouse and each will be responsible of carrying out different tasks,
  you should modify the code for each device by commenting out the functionality you don't need.

  If anyone from my school is reading this after several years, HI THERE! :) Please don't hesitate to contact me with any questions.

  Copyright (c) 2022 M. Bertan Tarakçıoğlu
*/

// Include libraries for the environmental monitoring sensor
#include <DFRobot_ENS160.h>
#include "DFRobot_BME280.h"

// Time between measurements in seconds
#define MEASUREMENT_INTERVAL 300

// Define the sea level pressure in the current location for BME280 calibration
#define SEA_LEVEL_PRESSURE 1013.25

/*
  Calibrate each soil moisture data and enter the corresponding values here
  Check out the wiki PDF for the soil moisture sensor for more info on how to calibrate
*/
#define SOIL_1_DRY 0
#define SOIL_1_WET 4095
#define SOIL_2_DRY 0
#define SOIL_2_WET 4095
#define SOIL_3_DRY 0
#define SOIL_3_WET 4095
#define SOIL_4_DRY 0
#define SOIL_4_WET 4095
#define SOIL_5_DRY 0
#define SOIL_5_WET 4095

// Define soil moisture sensor pins
#define soil_1 A0
#define soil_2 A1
#define soil_3 A2
#define soil_4 A3
#define soil_5 A4

// Define solenoid water valve pin (valve will be connected to an N-type MOSFET. See schematic for details.)
#define water_valve D6

// Define environmental monitoring sensor
DFRobot_BME280_IIC bme280(&Wire, 0x76);
DFRobot_ENS160_I2C ens160(&Wire, 0x53);

// Define soft delay function!
// The normal delay prevents Particle.process() from being called automatically, which is necessary for maintaining cloud connection.
inline void softDelay(uint32_t duration) {
    for (uint32_t ms = millis(); millis() - ms < duration; Particle.process());
}

// Define variables for the soil moisture sensors
double soil_1_rh;
double soil_2_rh;
double soil_3_rh;
double soil_4_rh;
double soil_5_rh;

// Define variables for the environmental monitoring sensor
double temperature;
double air_humidity;
double barometric_pressure;
double eCO2;
double TVOC;
double AQI;

// Function to read relative humidity from soil moisture sensor
int read_soil_rh(int soil_pin, int soil_dry, int soil_wet) {
    int soil_value = analogRead(soil_pin);
    int soil_rh = map(soil_value, soil_dry, soil_wet, 0, 100);
    return soil_rh;
}

// Function to turn the watering system on and off
int setWatering(String is_on) {
  if (is_on == "true") {
    digitalWrite(water_valve, HIGH);
    Particle.publish("wateringStateChanged", "true");
    Serial.println("Watering system turned on!");
  } else if (is_on == "false") {
    digitalWrite(water_valve, LOW);
    Particle.publish("wateringStateChanged", "true");
    Serial.println("Watering system turned off!");
  }else {
    Serial.println("Invalid command for function setWatering!");
    return -1;
  }
  return 0;
}

// Arduino standard setup method
void setup() {
  // Start serial communication
  Serial.begin(115200);
  Serial.println("Welcome to the Zafer Smart Greenhouse Project! Courtesy of IB Class of 23 :)");

  // Set soil moisture sensor pin modes
  pinMode(soil_1, INPUT);
  pinMode(soil_2, INPUT);
  pinMode(soil_3, INPUT);
  pinMode(soil_4, INPUT);
  pinMode(soil_5, INPUT);

  // Set solenoid water valve pin mode
  pinMode(water_valve, OUTPUT);

  // Start and configure BME280 sensor
  if (!bme280.begin()) {
    Serial.println("BME280 initialization failed! Will restart in 30 seconds");
    delay(30000);
    System.reset();
  } else {
    Serial.println("BME280 initialization successful!");
  }

  // Start and configure ENS160 sensor
  if (!ens160.begin()) {
    Serial.println("ENS160 initialization failed! Will restart in 30 seconds");
    delay(30000);
    System.reset();
  } else {
    Serial.println("ENS160 initialization successful!");
  }
  ens160.setPWRMode(ENS160_STANDARD_MODE);

  // Register watering control function to the cloud
  Particle.function("setWatering", setWatering);
}

// Unified function to update all sensor data
int updateReadings() {
  // Update soil moisture sensor readings
  soil_1_rh = read_soil_rh(soil_1, SOIL_1_DRY, SOIL_1_WET);
  soil_2_rh = read_soil_rh(soil_2, SOIL_2_DRY, SOIL_2_WET);
  soil_3_rh = read_soil_rh(soil_3, SOIL_3_DRY, SOIL_3_WET);
  soil_4_rh = read_soil_rh(soil_4, SOIL_4_DRY, SOIL_4_WET);
  soil_5_rh = read_soil_rh(soil_5, SOIL_5_DRY, SOIL_5_WET);

  // Update environmental monitoring sensor readings
  temperature = bme280.getTemperature();
  air_humidity = bme280.getHumidity();
  barometric_pressure = bme280.getPressure();
  eCO2 = ens160.getECO2();
  TVOC = ens160.getTVOC();
  AQI = ens160.getAQI();
  return 0;
}

// Take new measurement and publish to cloud
int cloudUpdate() {
  // Take new measurements
  updateReadings();
  // Publish to cloud
  Particle.publish("soil_1_rh", String::format("%.1f", soil_1_rh));
  Particle.publish("soil_2_rh", String::format("%.1f", soil_2_rh));
  Particle.publish("soil_3_rh", String::format("%.1f", soil_3_rh));
  Particle.publish("soil_4_rh", String::format("%.1f", soil_4_rh));
  Particle.publish("soil_5_rh", String::format("%.1f", soil_5_rh));
  Particle.publish("temperature", String::format("%.1f", temperature));
  Particle.publish("humidity", String::format("%.1f", air_humidity));
  Particle.publish("barometric_pressure", String::format("%.1f", barometric_pressure));
  Particle.publish("eCO2", String::format("%.1f", eCO2));
  Particle.publish("TVOC", String::format("%.1f", TVOC));
  Particle.publish("AQI", String::format("%.1f", AQI));
  return 0;
}

void loop() {
  // Periodically calibrate the ENS160 sensor according to the current conditions
  ens160.setTempAndHum(bme280.getTemperature(), bme280.getHumidity());
  cloudUpdate();
  softDelay(MEASUREMENT_INTERVAL*1000);
}