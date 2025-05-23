/*
HMC5883L.cpp - Class file for the HMC5883L Triple Axis Digital Compass Arduino Library.

Version: 1.1.0
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

#include "HMC5883L.h"

static void check(char const* str, int registry) {
    Wire.beginTransmission(HMC5883L_ADDRESS);
    Wire.write(registry);
    Wire.endTransmission();
    Wire.requestFrom(HMC5883L_ADDRESS,1);
    auto v = Wire.read();
    printf("%s=0x%X, %d\n", str, v, v);
}

bool HMC5883L::begin()
{
    // Wire.begin();

    check("hmc5883 reg ident", HMC5883L_REG_IDENT_A);

    if (fastRegister8(HMC5883L_REG_IDENT_A) != 0xFF)
    {
	return false;
    }

    writeRegister8(HMC5883L_REG_CONFIG_B, 0x01);
    writeRegister8(HMC5883L_REG_PERIOD, 0x01);

    setRange(HMC5883L_RANGE_2GA);
    setMeasurementMode(HMC5883L_CONTINOUS);
    setDataRate(HMC5883L_DATARATE_50HZ);
    setSamples(HMC5883L_SAMPLES_512);

    // mgPerDigit = 0.92f;
    mgPerDigit = 1.0f;
    setScale(1, 1, 1);

    return true;
}

Vector HMC5883L::readRaw(void)
{
    valid = true;
    v.XAxis = readRegister16AndCheckValidity(HMC5883L_REG_OUT_X);
    v.YAxis = readRegister16AndCheckValidity(HMC5883L_REG_OUT_Y);
    v.ZAxis = readRegister16AndCheckValidity(HMC5883L_REG_OUT_Z);
    
    return v;
}

Vector HMC5883L::readNormalize(void)
{
    Vector vec = readRaw();

    vec.XAxis -= xOffset;
    vec.YAxis -= yOffset;
    vec.ZAxis -= zOffset;

    vec.XAxis *= xScale;
    vec.YAxis *= yScale;
    vec.ZAxis *= zScale;

    return vec;
}

void HMC5883L::setOffset(int xo, int yo, int zo)
{
    xOffset = xo;
    yOffset = yo;
    zOffset = zo;
}

void HMC5883L::setScale(int xo, int yo, int zo)
{
    float average = (xo + yo + zo) / 3.0;
    xScale = average / xo;
    yScale = average / yo;
    zScale = average / zo;
}

void HMC5883L::resetOffsets() {
    setOffset(0, 0, 0);
    setScale(1, 1, 1);
}

void HMC5883L::setRange(hmc5883l_range_t range)
{
    switch(range)
    {
	case HMC5883L_RANGE_2GA:
	    // mgPerDigit = 0.073f;
	    break;

	case HMC5883L_RANGE_8GA:
	    // mgPerDigit = 0.92f;
	    break;

	default:
	    break;
    }

    uint8_t value = readRegister8(HMC5883L_REG_CONFIG_A);
    value &= 0b11001111;
    value |= range << 4;

    writeRegister8(HMC5883L_REG_CONFIG_A, value);
}

hmc5883l_range_t HMC5883L::getRange(void)
{
    return (hmc5883l_range_t)((readRegister8(HMC5883L_REG_CONFIG_A) >> 4) & 0b11);
}

void HMC5883L::setMeasurementMode(hmc5883l_mode_t mode)
{
    uint8_t value;

    value = readRegister8(HMC5883L_REG_CONFIG_A);
    value &= 0b11111100;
    value |= mode;

    writeRegister8(HMC5883L_REG_CONFIG_A, value);
}

hmc5883l_mode_t HMC5883L::getMeasurementMode(void)
{
    uint8_t value;

    value = readRegister8(HMC5883L_REG_CONFIG_A);
    value &= 0b00000011;

    return (hmc5883l_mode_t)value;
}

void HMC5883L::setDataRate(hmc5883l_dataRate_t dataRate)
{
    uint8_t value;

    value = readRegister8(HMC5883L_REG_CONFIG_A);
    value &= 0b11110011;
    value |= (dataRate << 2);

    writeRegister8(HMC5883L_REG_CONFIG_A, value);
}

hmc5883l_dataRate_t HMC5883L::getDataRate(void)
{
    uint8_t value;

    value = readRegister8(HMC5883L_REG_CONFIG_A);
    value &= 0b00001100;
    value >>= 2;

    return (hmc5883l_dataRate_t)value;
}

void HMC5883L::setSamples(hmc5883l_samples_t samples)
{
    uint8_t value;

    value = readRegister8(HMC5883L_REG_CONFIG_A);
    value &= 0b00111111;
    value |= (samples << 6);

    writeRegister8(HMC5883L_REG_CONFIG_A, value);
}

hmc5883l_samples_t HMC5883L::getSamples(void)
{
    uint8_t value;

    value = readRegister8(HMC5883L_REG_CONFIG_A);
    value &= 0b11000000;
    value >>= 6;

    return (hmc5883l_samples_t)value;
}

int16_t HMC5883L::readRegister16AndCheckValidity(uint8_t reg)
{
    int16_t reading = readRegister16(reg);
    using limits = std::numeric_limits<int16_t>;
    if (reading == limits::max() || reading == limits::min()) {
        valid = false;
    }
    return reading;
}

// Write byte to register
void HMC5883L::writeRegister8(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(HMC5883L_ADDRESS);
    #if ARDUINO >= 100
        Wire.write(reg);
        Wire.write(value);
    #else
        Wire.send(reg);
        Wire.send(value);
    #endif
    Wire.endTransmission();
}

// Read byte to register
uint8_t HMC5883L::fastRegister8(uint8_t reg)
{
    uint8_t value;
    Wire.beginTransmission(HMC5883L_ADDRESS);
    #if ARDUINO >= 100
        Wire.write(reg);
    #else
        Wire.send(reg);
    #endif
    Wire.endTransmission();

    Wire.requestFrom(HMC5883L_ADDRESS, 1);
    #if ARDUINO >= 100
        value = Wire.read();
    #else
        value = Wire.receive();
    #endif

    return value;
}

// Read byte from register
uint8_t HMC5883L::readRegister8(uint8_t reg)
{
    uint8_t value;
    Wire.beginTransmission(HMC5883L_ADDRESS);
    #if ARDUINO >= 100
        Wire.write(reg);
    #else
        Wire.send(reg);
    #endif
    Wire.endTransmission();

    Wire.beginTransmission(HMC5883L_ADDRESS);
    Wire.requestFrom(HMC5883L_ADDRESS, 1);
    #if ARDUINO >= 100
        value = Wire.read();
    #else
        value = Wire.receive();
    #endif

    return value;
}

// Read word from register
int16_t HMC5883L::readRegister16(uint8_t reg)
{
    int16_t value;
    Wire.beginTransmission(HMC5883L_ADDRESS);
    #if ARDUINO >= 100
        Wire.write(reg);
    #else
        Wire.send(reg);
    #endif
    Wire.endTransmission();

    // Wire.beginTransmission(HMC5883L_ADDRESS);
    int bytesRead = Wire.requestFrom(HMC5883L_ADDRESS, 2);
    if (bytesRead < 2) {
        if (bytesRead == 1) {
            Wire.read();
        }
        return 0x7FFF;
    }
    #if ARDUINO >= 100
        uint8_t vla = Wire.read();
        uint8_t vha = Wire.read();
    #else
        uint8_t vla = Wire.receive();
        uint8_t vha = Wire.receive();
    #endif

    value = int16_t(vha << 8 | vla);

    return value;
}
