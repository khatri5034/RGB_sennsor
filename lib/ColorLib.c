/**************************************************************
* Class::  CSC-615-01 Spring 2026
* Name:: Haibin Cao, Eric Ahsue, Kiran Khatri, John Tsiglieris
* Student ID:: 923756077, 922711514, 925750019, 923593954
* GitHub-Name:: haibinc, Jasuv, khatri5034, John-Tsiglieris
* Project:: Assignment 5 – RGB Sensor
*
* File:: ColorLib.c
*
* Description:: Implement an RGB detection system by reading sensor data 
* and using I2C communication between sensor and raspberry PI.
*
**************************************************************/

#include "ColorLib.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

// write to sensor
static void sensor_write(uint8_t reg, uint8_t data) { 
	DEV_I2C_WriteByte(reg | TCS_CMD, data); 
}

// read 16 bits from sensor over I2C
static uint16_t sensor_read(uint8_t reg) { 
	return DEV_I2C_ReadWord(reg | TCS_CMD | 0x20); 
}

int ColorLib_Init(void) {
	// sensor register check
	DEV_Set_I2CAddress(TCS_ADDR);
	uint8_t id = DEV_I2C_ReadByte(TCS_REG_ID | TCS_CMD);
	if (id != 0x44 && id != 0x4D) return 1;

	// sensor init
	sensor_write(TCS_REG_ATIME, INTEGRATIONTIME_154MS);
	sensor_write(TCS_REG_CONTROL, GAIN_1X);
	sensor_write(TCS_REG_ENABLE, TCS_PON);
	DEV_Delay_ms(3);
	sensor_write(TCS_REG_ENABLE, TCS_PON | TCS_AEN);

	return 0;
}

ColorResult ColorLib_GetMatch(void) {
    ColorResult bestMatch;

    // Read the raw RGBC (Red, Green, Blue, Clear) channel values from the sensor
    // The clear channel represents overall light intensity with no color filter
    uint16_t C = sensor_read(TCS_REG_CDATA);
    uint16_t R = sensor_read(TCS_REG_RDATA);
    uint16_t G = sensor_read(TCS_REG_GDATA);
    uint16_t B = sensor_read(TCS_REG_BDATA);

    // Normalize each channel against the clear channel to remove the effect
    // of ambient light brightness. This gives us the color ratios rather than
    // absolute values, so the same color looks the same under dim or bright light.
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    if (C > 0) {
        r = (float)R / C;
        g = (float)G / C;
        b = (float)B / C;
    }
	
    // Find the brightest channel so we can scale everything relative to it.
    // For example if we get (R: 5.0, G: 1.0 , B: 1.0), then red would be scaled
    // down to 5.0/5.0 = 1.0 and both green and blue would be 1.0/5.0
    float maxCh = r;

    if (g > maxCh) {
        maxCh = g;
    }

    if (b > maxCh) {
        maxCh = b;
    }

    if (maxCh > 0) {
        r = r / maxCh;
        g = g / maxCh;
        b = b / maxCh;
    }

    float gamma = 2.2f;
    r = powf(r, gamma);
    g = powf(g, gamma);
    b = powf(b, gamma);

    // Scale the normalized 0.0-1.0 float values up to the standard 0-255 range
    int currR = (int)(r * 255);
    int currG = (int)(g * 255);
    int currB = (int)(b * 255);
    printf("R:%d G:%d B:%d\n", currR, currG, currB);

    // Compare the sensor reading against every color in our target list.
    // minDist is going to calculate how close the sensor rgb values are
    // compared to the value it matched with
    float minDist = 1e9f;
    int bestIdx = 0;

    for (int i = 0; i < COLOR_COUNT; i++) {
        // Get the average of each color's defined range
        float centerR = (PROJECT_COLORS[i].rMin + PROJECT_COLORS[i].rMax) / 2.0f;
        float centerG = (PROJECT_COLORS[i].gMin + PROJECT_COLORS[i].gMax) / 2.0f;
        float centerB = (PROJECT_COLORS[i].bMin + PROJECT_COLORS[i].bMax) / 2.0f;

        // Computed the distance in 3D RGB space between the sensor reading
        //  and the actual color
        float dist = sqrtf(
            powf(currR - centerR, 2) +
            powf(currG - centerG, 2) +
            powf(currB - centerB, 2)
        );

        if (dist < minDist) {
            minDist = dist;
            bestIdx = i;
        }
    }

    // Store the name and hex value of the best matching color
    strcpy(bestMatch.name, PROJECT_COLORS[bestIdx].name);
    bestMatch.hexValue = (currR << 16) | (currG << 8) | currB;

    #define MAX_RGB_DIST 441.7f 
    bestMatch.confidence = (1.0f - (minDist / MAX_RGB_DIST)) * 100.0f;

    printf("Best: %s  dist=%.1f  confidence=%.1f%%\n",
           bestMatch.name, minDist, bestMatch.confidence);

    return bestMatch;
}