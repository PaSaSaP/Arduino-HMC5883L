/*
HMC5883L.h - Header file for the HMC5883L Triple Axis Digital Compass Arduino Library.

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

#ifndef HMC5883L_h
#define HMC5883L_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define HMC5883L_ADDRESS              (0x0D)
#define HMC5883L_REG_CONFIG_A         (0x09)
#define HMC5883L_REG_CONFIG_B         (0x0A)
#define HMC5883L_REG_PERIOD           (0x0B)
#define HMC5883L_REG_OUT_X            (0x00)
#define HMC5883L_REG_OUT_Z            (0x04)
#define HMC5883L_REG_OUT_Y            (0x02)
#define HMC5883L_REG_STATUS           (0x06)
#define HMC5883L_REG_TEMPERATURE      (0x07)
#define HMC5883L_REG_IDENT_A          (0x0D)

typedef enum
{
    HMC5883L_SAMPLES_64      = 0b11,
    HMC5883L_SAMPLES_128     = 0b10,
    HMC5883L_SAMPLES_256     = 0b01,
    HMC5883L_SAMPLES_512     = 0b00
} hmc5883l_samples_t;

typedef enum
{
    HMC5883L_DATARATE_200HZ      = 0b11,
    HMC5883L_DATARATE_100HZ      = 0b10,
    HMC5883L_DATARATE_50HZ       = 0b01,
    HMC5883L_DATARATE_10HZ       = 0b00
} hmc5883l_dataRate_t;

typedef enum
{
    HMC5883L_RANGE_2GA       = 0b00,
    HMC5883L_RANGE_8GA       = 0b01,
    HMC5883L_RANGE_RESERVED1 = 0b10,
    HMC5883L_RANGE_RESERVED2 = 0b11
} hmc5883l_range_t;

typedef enum
{
    HMC5883L_STANDBY         = 0b00,
    HMC5883L_CONTINOUS       = 0b01,
    HMC5883L_RESERVED1       = 0b10,
    HMC5883L_RESERVED2       = 0b11,
} hmc5883l_mode_t;

#ifndef VECTOR_STRUCT_H
#define VECTOR_STRUCT_H
struct Vector
{
    float XAxis;
    float YAxis;
    float ZAxis;
};
#endif

class HMC5883L
{
    public:

	bool begin(void);

	Vector readRaw(void);
	Vector readNormalize(void);

	void  setOffset(int xo, int yo);

	void  setRange(hmc5883l_range_t range);
	hmc5883l_range_t getRange(void);

	void  setMeasurementMode(hmc5883l_mode_t mode);
	hmc5883l_mode_t getMeasurementMode(void);

	void  setDataRate(hmc5883l_dataRate_t dataRate);
	hmc5883l_dataRate_t getDataRate(void);

	void  setSamples(hmc5883l_samples_t samples);
	hmc5883l_samples_t getSamples(void);

    private:

	float mgPerDigit;
	Vector v;
	int xOffset, yOffset;

	void writeRegister8(uint8_t reg, uint8_t value);
	uint8_t readRegister8(uint8_t reg);
	uint8_t fastRegister8(uint8_t reg);
	int16_t readRegister16(uint8_t reg);
};

#endif
