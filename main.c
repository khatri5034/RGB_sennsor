/**************************************************************
* Class::  CSC-615-01 Spring 2026
* Name:: Haibin Cao, Eric Ahsue, Kiran Khatri, John Tsiglieris
* Student ID:: 923756077, 922711514, 925750019, 923593954
* GitHub-Name:: haibinc, Jasuv, khatri5034, John-Tsiglieris
* Project:: Assignment 5 – RGB Sensor
*
* File:: main.c
*
* Description:: Implement an RGB detection system by reading sensor data 
* and using I2C communication between sensor and raspberry PI.
*
**************************************************************/

#include <stdio.h>
#include "ColorLib.h"
#include "DEV_Config.h"
#include <signal.h>

void cleanup_handler(int sig) {
    printf("\nCaught Ctrl+C (SIGINT), exiting program.\n");
    DEV_ModuleExit();
    exit(0);
}

int main(void) {
	signal(SIGINT, cleanup_handler);

	if (DEV_ModuleInit() != 0) {
		printf("system hardware init failed\n");
		return 1;
	}

	if (ColorLib_Init() != 0) {
		printf("TCS34725 sensor not found\n");
		return 1;
	}

	while(1) {
		ColorResult result = ColorLib_GetMatch();
		printf("Hex: #%06X | Color: %-10s \n", 
		result.hexValue, result.name);

		DEV_Delay_ms(500);
	}

	DEV_ModuleExit();
	return 0;
}
