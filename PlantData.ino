// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//LCD Address
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2  //Порт датчика/ов температуры
#define TEMPERATURE_PRECISION 9

const int air = 623;    //Данные датчика влажности в воздухе
const int water = 317;  //Данные датчика влажности в воде
#define HUMIDITY_SENSOR A0  //Порт датчика влажности

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Assign address manually. The addresses below will need to be changed
// to valid device addresses on your bus. Device address can be retrieved
// by using either oneWire.search(deviceAddress) or individually via
// sensors.getAddress(deviceAddress, index)
DeviceAddress insideThermometer = { 0x28, 0xFF, 0xEA, 0x1D, 0x36, 0x16, 0x4, 0xA2 };
DeviceAddress outsideThermometer   = { 0x28, 0xFF, 0xC3, 0x2D, 0x36, 0x16, 0x4, 0xE1 };

void setup(void)
{
  // start serial port
  Serial.begin(9600);

  // Start up the library
  sensors.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print("Found ");
  lcd.setCursor(0, 0);
  lcd.print(sensors.getDeviceCount());
  lcd.print(" temp sensors.");
  delay(2000);
  lcd.clear();

  // locate devices on the bus
  //Serial.print("Found ");
  //Serial.print(sensors.getDeviceCount(), DEC);
  //Serial.println(" temp sensors.");

  // report parasite power requirements
  //Serial.print("Parasite power is: ");
  //if (sensors.isParasitePowerMode()) Serial.println("ON");
  //else Serial.println("OFF");

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Unable to find address for Device 1");

  // show the addresses we found on the bus
  //Serial.print("Device 0 Address: ");
  //printAddress(insideThermometer);
  //Serial.println();

  //Serial.print("Device 1 Address: ");
  //printAddress(outsideThermometer);
  //Serial.println();

  // set the resolution to 9 bit per device
  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);

  //Serial.print("Device 0 Resolution: ");
  //Serial.print(sensors.getResolution(insideThermometer), DEC);
  //Serial.println();

  //Serial.print("Device 1 Resolution: ");
  //Serial.print(sensors.getResolution(outsideThermometer), DEC);
  //Serial.println();
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  lcd.setCursor(0, 0);
  lcd.print("Temp C: ");
  lcd.print(tempC);
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

void printHum()
{
  uint16_t sensorVal = analogRead(HUMIDITY_SENSOR);
  int percentageHumididy = map(sensorVal, water, air, 100, 0); 
  double flHumidity = (sensorVal-water)/(air-water) * 100.0f;
  //air = 623 = 0%
  //water = 317 = 100%
  //Serial.print("Sensor data: ");
  //Serial.println(sensorVal);
  Serial.print("Soil Humidity: ");
  Serial.print(percentageHumididy);
  Serial.println(" %");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(percentageHumididy);
  lcd.print(" %");
  //Serial.print("Float Humidity: ");
  //Serial.print(flHumidity);
  //Serial.println(" %");
}

/*
   Main function, calls the temperatures in a loop.
*/
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures();
  // print the device information
  printData(insideThermometer);
  printData(outsideThermometer);
  printHum();
  delay(10000);
}
