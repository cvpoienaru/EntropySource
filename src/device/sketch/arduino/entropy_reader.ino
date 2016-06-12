/**
 * Copyright (c) 2016, Codrin-Victor Poienaru <cvpoienaru@gmail.com>.
 * All rights reserved.
 *
 * The design for this sketch is based on usage examples of SparkFun BME280
 * and TSL2561 sensor libraries.
 * See acknowledgement file for more info.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * This software is provided by the copyright holders and contributors "as is"
 * and any express or implied warranties, including, but not limited to, the
 * implied warranties of merchantability and fitness for a particular purpose
 * are disclaimed. In no event shall the copyright holder or contributors be
 * liable for any direct, indirect, incidental, special, exemplary, or
 * consequential damages (including, but not limited to, procurement of
 * substitute goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether in
 * contract, strict liability, or tort (including negligence or otherwise)
 * arising in any way out of the use of this software, even if advised of the
 * possibility of such damage.
 */

#include <stdint.h>

#include <SparkFunBME280.h>
#include <SparkFunTSL2561.h>
#include <Wire.h>

// General sketch parameters.
#define DEBUG 0
#define BAUD_RATE 9600
#define START_TRANSFER_CODE '^'
#define STOP_TRANSFER_CODE '$'
#define MAX_VALUE 256

// Adjustment values for sensor readings.
#define TEMPERATURE_ADJUSTMENT 1000000
#define PRESSURE_ADJUSTMENT 10000
#define HUMIDITY_ADJUSTMENT 1000000
#define ALTITUDE_ADJUSTMENT 10000
#define LIGHT_ADJUSTMENT 10000

// General BME280 parameters.
#define BME280_INTERFACE I2C_MODE
#define BME280_ADDRESS 0x77
#define BME280_STANDBY_TIME 0
#define BME280_DELAY_TIME 10

// BME280 running modes.
#define BME280_SLEEP_MODE 0
#define BME280_FORCE_MODE_1 1
#define BME280_FORCE_MODE_2 2
#define BME280_NORMAL_MODE 3

// BME280 filter levels.
#define BME280_FILTER_LEVEL_1 0
#define BME280_FILTER_LEVEL_2 (1 << 1)
#define BME280_FILTER_LEVEL_3 (1 << 2)
#define BME280_FILTER_LEVEL_4 (1 << 3)
#define BME280_FILTER_LEVEL_5 (1 << 4)

// BME280 oversampling levels.
#define BME280_OVERSAMPLING_LEVEL_1 (1 << 0)
#define BME280_OVERSAMPLING_LEVEL_2 (1 << 1)
#define BME280_OVERSAMPLING_LEVEL_3 (1 << 2)
#define BME280_OVERSAMPLING_LEVEL_4 (1 << 3)
#define BME280_OVERSAMPLING_LEVEL_5 (1 << 4)

// TSL2561 gain modes.
#define TSL2561_GAIN_LOW 0
#define TSL2561_GAIN_HIGH 1

// TSL2561 integration time levels.
#define TSL2561_INTEGRATION_TIME_LEVEL_1 0
#define TSL2561_INTEGRATION_TIME_LEVEL_2 1
#define TSL2561_INTEGRATION_TIME_LEVEL_3 2
#define TSL2561_INTEGRATION_TIME_LEVEL_4 3

// General sketch declarations.
boolean is_running = false;
double sum = 0.0;

// BME280 sensor declarations.
BME280 bme280_sensor;
double temperature = 0.0;
double pressure = 0.0;
double humidity = 0.0;
double altitude = 0.0;

// TSL2561 sensor declarations.
SFE_TSL2561 tsl2561_sensor;
boolean tsl2561_gain = TSL2561_GAIN_LOW;
unsigned char tsl2561_time = TSL2561_INTEGRATION_TIME_LEVEL_2;
unsigned int tsl2561_delay = 0.0;
unsigned int data_0 = 0;
unsigned int data_1 = 0;
boolean unsaturated = false;
double light = 0.0;

void init_serial()
{
    // Initialize the serial port and wait for it to open.
    Serial.begin(BAUD_RATE);

    // Wait for the serial port to connect.
    // Needed for native USB port only.
    while (!Serial);
}

int init_bme280()
{
    // Set the communication interface to I2C.
    // Set the I2C address to the default value (0x77).
    bme280_sensor.settings.commInterface = BME280_INTERFACE;
    bme280_sensor.settings.I2CAddress = BME280_ADDRESS;

    // Set the running mode to NORMAL.
    bme280_sensor.settings.runMode = BME280_NORMAL_MODE;

    // Set the stand-by time.
    bme280_sensor.settings.tStandby = BME280_STANDBY_TIME;

    // Set the filter to be OFF.
    bme280_sensor.settings.filter = BME280_FILTER_LEVEL_1;

    // Set the oversample level for temperature, pressure and humidity.
    bme280_sensor.settings.tempOverSample = BME280_OVERSAMPLING_LEVEL_1;
    bme280_sensor.settings.pressOverSample = BME280_OVERSAMPLING_LEVEL_1;
    bme280_sensor.settings.humidOverSample = BME280_OVERSAMPLING_LEVEL_1;

    // BME280 requires 2ms to start up.
    // Delay by 10ms to be sure the sensor has enough time.
    delay(BME280_DELAY_TIME);

    // Call sensor.begin() to load the settings.
    return bme280_sensor.begin();
}

void read_bme280_data()
{
    // Read the values for temperature, pressure and humidity.
    // Compute the altitude value in meters based on pressure.
    temperature = bme280_sensor.readTempC();
    pressure = bme280_sensor.readFloatPressure();
    humidity = bme280_sensor.readFloatHumidity();
    altitude = bme280_sensor.readFloatAltitudeMeters();
}

void adjust_bme280_data()
{
    // Adjust all the readings according to their defined adjustment.
    temperature *= TEMPERATURE_ADJUSTMENT;
    pressure *= PRESSURE_ADJUSTMENT;
    humidity *= HUMIDITY_ADJUSTMENT;
    altitude *= ALTITUDE_ADJUSTMENT;
}

void init_tsl2561()
{
    // Call sensor.begin() to load the default settings.
    tsl2561_sensor.begin();

    // Set delay parameter according to the requested integration time.
    tsl2561_sensor.setTiming(tsl2561_gain, tsl2561_time, tsl2561_delay);

    // Start the sensor.
    // After this call, the sensor will gather light during the integration
    // time.
    tsl2561_sensor.setPowerUp();
}

void read_tsl2561_data()
{
    // After this delay passes, we can retrieve data from the sensor in
    // the form of two values: one for visible light and one for infrared
    // light, both coresponding to the two light sensors embedded in the
    // device. The two values are needed for the final lux calculation.
    delay(tsl2561_delay);

    if(!tsl2561_sensor.getData(data_0, data_1)) {
        data_0 = 0;
        data_1 = 0;
        light = 0.0;
        return;
    }

    // Get the light value in luxs.
    // The function returns 1 if the calculation was successfull
    // or 0 if one or both of the sensors were saturated (too much
    // light). To prevent the saturation, we could reduce the integration
    // time and/or the value of the gain.
    unsaturated = tsl2561_sensor.getLux(
        tsl2561_gain,
        tsl2561_delay,
        data_0,
        data_1,
        light);

    // Mark the saturation of one or both of the sensors as an error.
    if(!unsaturated) {
        data_0 = 0;
        data_1 = 0;
        light = 0.0;
    }
}

void adjust_tsl2561_data()
{
    // Adjust light reading according to its specific adjustment.
    light *= LIGHT_ADJUSTMENT;
}

void setup()
{
    // Initialize the serial communication.
    init_serial();

    // Initialize the BME280 and TSL2561 sensors.
    init_bme280();
    init_tsl2561();
}

void loop()
{
    // Wait for a serial character to be available.
    if(Serial.available() > 0) {
        // Read a character on the serial interface.
        int received_byte = Serial.read();

        switch(received_byte) {
            case START_TRANSFER_CODE:
                // We received the start transfer signal.
                is_running = true;
                break;

            case STOP_TRANSFER_CODE:
                // We received the stop transfer signal.
                is_running = false;
                break;
        }
    }

    if(is_running) {
        // Read data from BME280 and TSL2561 sensors.
        read_bme280_data();
        read_tsl2561_data();

        // Compute the sum of all sensor readings.
        sum = temperature + pressure + humidity + altitude + light;

        if(DEBUG) {
            // Send separate sensor readings for debugging purposes.
            Serial.print("Temperature: ");
            Serial.println((double)temperature, 8);

            Serial.print("Pressure: ");
            Serial.println((double)pressure, 8);

            Serial.print("Humidity: ");
            Serial.println((double)humidity, 8);

            Serial.print("Altitude: ");
            Serial.println((double)altitude, 8);

            Serial.print("Light: ");
            Serial.println((double)light, 8);

            Serial.print("Sum: ");
            Serial.println((double)sum, 8);

            Serial.println("----------------------------------------");
        } else {
            // Adjust all sensor readings.
            adjust_bme280_data();
            adjust_tsl2561_data();

            // Compute the entropy byte to be sent.
            char entropy_byte =
                ((long)sum
                    ^ (long)temperature
                    ^ (long)pressure
                    ^ (long)humidity
                    ^ (long)altitude
                    ^ (long)light)
                % MAX_VALUE;

            // Send the previously computed entropy byte.
            Serial.println((char)entropy_byte);
        }
    }
}
