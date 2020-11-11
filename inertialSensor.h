#pragma once
#include "udpCom.h"
class inertialSensor
{
public:
	float accX = 0;
	float accY = 0;
	float accZ = 0;
	float gyrX = 0;
	float gyrY = 0;
	float gyrZ = 0;
	inertialSensor(void) {};
	~inertialSensor(void){};
	friend udpCom& operator<<(udpCom& udpPckt, std::string command);
	friend udpCom& operator>>(udpCom& udpPckt, inertialSensor& data);
};

