#pragma once
#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <string>
#include <vector>
#include <cmath> 
#include <iostream>
#include "udpCom.h"
#include "inertialSensor.h"
#include <time.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.

/*motion range values
Tx: +- 0.13 m, +- 0.5 m/s, 6 m/s2 (0.6 G)
Ty: +- 0.10 m, +- 0.5 m/s, 6 m/s2 (0.6 G)
Tz: +- 0.07 m, +- 0.3 m/s, 5 m/s2 (0.5 G)

Rx: +- 15 deg, +- 35 deg/s, 600 deg/s2

Ry: +- 15 deg, +- 35 deg/s, 600 deg/s2

Rz: +- 15 deg, +- 40 deg/s, 600 deg/s2
*/


class edge6dCommandPacket
{
public: 
//	virtual	int set(float value, int index) = 0;
//	virtual	int get(float value, int index) = 0;
	char head;
	char packetVer;
	char commandType;
	char type;
	float values[6] = { 0,0,0,0,0,0 };
	//void sendEdge6dPacket();

	std::vector<char> inputBuffer;
	int setValues(float val, int index) {
		if (checkValRange(val, index))
			values[index] = val;
		else
			return -1;
		return 0;
	};
	/*int ensurePcktFreq(std::chrono::microseconds s)
	{
		auto start = std::chrono::high_resolution_clock::now();
		auto end=std::chrono::high_resolution_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>( end - start);

		printf("time elapsed:%d\n ", elapsed_time.count());
		while (elapsed_time <= s)
		{
			end = std::chrono::high_resolution_clock::now(); 
			elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

			printf("time elapsed:%d\n ", elapsed_time.count());

		}
		return 1;
	}*/
	friend std::ostream& operator<<(std::ostream& os, const edge6dCommandPacket& dt);
	friend udpCom& operator<<(udpCom& os, const edge6dCommandPacket& dt);

protected:	
	virtual bool checkValRange(float val, int index) = 0;
	std::vector<std::string > labels;
	std::vector<std::string > units;
	



};

class edge6dLogPacket
{
public:
	char head;
	char packetVer;
	char commandType;
	char type;
	char logPacketNUmb;
	std::vector<char> respPacketId; 
	std::vector<std::vector<float>> values;

	friend std::ostream& operator<<(std::ostream& os, const edge6dCommandPacket& dt);
	friend std::ostream& operator<<(std::ostream& os, const edge6dLogPacket& dt);
	friend udpCom& operator<<(udpCom& os, const edge6dCommandPacket& dt);
	friend udpCom& operator<<(udpCom& os, const edge6dLogPacket& dt);

protected:
	virtual bool checkValRange(float val, int index) = 0;
	std::vector<std::string > labels;
	std::vector<std::string > units;




};

std::ostream& operator<<(std::ostream& os, const edge6dCommandPacket& dt)
{
	os << "header:" << dt.head << "\tpacketVer:" << (int)dt.packetVer << "\tcommandType:" << dt.commandType << "\tType:" << (int)dt.type << std::endl;
	for (int i = 0; i < 6; i++)
		os << dt.labels[i] << ":" << (double)dt.values[i] << " " << dt.units[i] << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, const edge6dLogPacket& dt)
{
	os << "header:" << dt.head << "\tpacketVer:" << (int)dt.packetVer << "\tcommandType:" << dt.commandType << "\tNUmb of dataPckts:" << (int)dt.logPacketNUmb << "\tType:" << (int)dt.type<< std::endl;
	for (int i = 1; i < 6; i++)
		os << dt.labels[i] << ":" << (double)dt.values[0][i] << " " << dt.units[0][i] << std::endl;
	return os;
}


udpCom& operator<<(udpCom& udpPckt, const edge6dCommandPacket& dt)
{
	std::vector<char> outputBuffer;
	outputBuffer.push_back(dt.head);
	outputBuffer.push_back(dt.packetVer);
	outputBuffer.push_back(dt.commandType);
	outputBuffer.push_back(dt.type);
	for (int i = 0; i < 6; i++)
	{
		outputBuffer.insert(outputBuffer.end(),(char*)&dt.values[i], (char*)&dt.values[i]+4);
	}
	udpPckt.send(outputBuffer);
	return udpPckt;
};

udpCom& operator>>(udpCom& udpPckt, edge6dCommandPacket& dt)
{
	std::vector<char> inputBuffer(4 + 6 * sizeof(float));
	std::vector<char> tmp(6 * sizeof(float));

	udpPckt.rec(inputBuffer, 4 + 6 * sizeof(float));

	dt.head = inputBuffer[0];
	dt.packetVer = inputBuffer[1];
	dt.commandType = inputBuffer[2];
	dt.type = inputBuffer[3];
	int j = 0;
	for (int i = 4; i < 25; i += 4)
	{
		dt.values[j] = *reinterpret_cast<float*>(&inputBuffer[i]);
		j++;

	}
	return udpPckt;
};

udpCom& operator<<(udpCom& udpPckt, const std::string command)
{
	std::vector<char> v;
	std::copy(command.begin(), command.end(), std::back_inserter(v));
	udpPckt.send(v);
	return udpPckt;
};
udpCom& operator>>(udpCom& udpPckt, inertialSensor& data)
{
	std::vector<char> inputBuffer(6 * sizeof(float));

	udpPckt.rec(inputBuffer, 6 * sizeof(float));
	data.accX = *reinterpret_cast<float*>(&inputBuffer[0]);
	data.accY = *reinterpret_cast<float*>(&inputBuffer[4]);
	data.accZ = *reinterpret_cast<float*>(&inputBuffer[8]);
	data.gyrX = *reinterpret_cast<float*>(&inputBuffer[12]);
	data.gyrY = *reinterpret_cast<float*>(&inputBuffer[16]);
	data.gyrZ = *reinterpret_cast<float*>(&inputBuffer[20]);



	return udpPckt;
};

udpCom& operator<<(udpCom& udpPckt, const edge6dLogPacket& dt)
{
	std::vector<char> outputBuffer;
	outputBuffer.push_back(dt.head);
	outputBuffer.push_back(dt.packetVer);
	outputBuffer.push_back(dt.commandType);
	outputBuffer.push_back(dt.type);
	//outputBuffer.push_back(100);

	udpPckt.send(outputBuffer);
	return udpPckt;
};

udpCom& operator>>(udpCom& udpPckt, edge6dLogPacket& dt)
{
	std::vector<char> inputBuffer(5 + 6 * sizeof(float));
	std::vector<float> singlePcktData;
	if (udpPckt.rec(inputBuffer, (5 + 6 * sizeof(float))) != -1)
	{
		dt.head = inputBuffer[0];
		dt.packetVer = inputBuffer[1];
		dt.commandType = inputBuffer[2];
		dt.logPacketNUmb = inputBuffer[3];

		dt.respPacketId.push_back(inputBuffer[4]);

		int j = 0;
		for (int i = 5; i < 26; i += 4)
		{
			singlePcktData.push_back(*reinterpret_cast<float*>(&inputBuffer[i]));
			j++;
		}
		dt.values.push_back(singlePcktData);
	}
	
	return udpPckt;
};
// pacchetto dati per settaggio Servo Power offset
class eP_servoPowerOffset :public edge6dCommandPacket
{
public:
	enum servoIndex :int { servo_0 = 0, servo_1 = 1, servo_2 = 2, servo_3 = 3, servo_4 = 4, servo_5 = 5 };

	eP_servoPowerOffset()
	{ head = 'G'; packetVer = 0; commandType = 'M'; type = 0;
	labels = { "Power offset 0", "Power offset 1", "Power offset 2", "Power offset 3", "Power offset 4", "Power offset 5" };
	units = { " ", " ", " ", " ", " ", " " };
	};


	//int set(float value, int index) { return setValues(value, index); };

private:
	const float rangePP = 1.0f;
	bool checkValRange(float val, int index)
	{
		if (fabs(val) > rangePP)
			return false;
		else
			return true;
	}
};

class eP_servoAngleOffset :public edge6dCommandPacket
{
public:
	enum servoIndex :int { servo_0 = 0, servo_1 = 1, servo_2 = 2, servo_3 = 3, servo_4 = 4, servo_5 = 5 };

	eP_servoAngleOffset()
	{
		head = 'G'; packetVer = 0; commandType = 'M'; type = 1;
		labels = { "Angle offset 0", "Angle offset 1", "Angle offset 2", "Angle offset 3", "Angle offset 4", "Angle offset 5" };
		units = { "rad", "rad", "rad", "rad", "rad", "rad" };
	};


	//int set(float value, int index) { return setValues(value, index); };

private:
	const float rangePP = 2 * M_PI;
	bool checkValRange(float val, int index)
	{
		if (fabs(val) > rangePP)
			return false;
		else
			return true;
	}
};
class eP_servoLengthOffset :public edge6dCommandPacket
{
public:
	enum servoIndex :int { servo_0 = 0, servo_1 = 1, servo_2 = 2, servo_3 = 3, servo_4 = 4, servo_5 = 5 };

	eP_servoLengthOffset()
	{
		head = 'G'; packetVer = 0; commandType = 'M'; type = 2;
		labels = { "Length offset 0", "Length offset 1", "Length offset 2", "Length offset 3", "Length offset 4", "Length offset 5" };
		units = { "m", "m", "m", "m", "m", "m" };
	};


	//int set(float value, int index) { return setValues(value, index); };

private:
	const float rangePP = 0.2;
	bool checkValRange(float val, int index)
	{
		if (fabs(val) > rangePP)
			return false;
		else
			return true;
	}
};

class eP_plarformPosition :public edge6dCommandPacket
{
public:
	enum posIndex :int { Tx= 0, Ty= 1, Tz= 2, Rx= 3, Ry= 4, Rz= 5 };

	eP_plarformPosition()
	{
		head = 'G'; packetVer = 0; commandType = 'M'; type = 3;
		labels = { "Tx", "Ty", "Tz", "Rx", "Ry", "Rz" };
		units = { "m", "m", "m", "rad", "rad", "rad" };
	};

private:	
	bool checkValRange(float val, int index)
	{
		float rangePP = 0;
		switch (index)
		{
		case Tx:
			rangePP = 0.13;
			break;
		case Ty:
			rangePP = 0.10;
			break;
		case Tz:
			rangePP = 0.07;
			break;
		case Rx:
			rangePP = 15 * M_PI / 180;
			break;
		case Ry:
			rangePP = 15 * M_PI / 180;
			break;
		case Rz:
			rangePP = 15 * M_PI / 180;
			break;
		}

		if (fabs(val) > rangePP)
			return false;
		else
			return true;
	}
};

class eP_userInclgravityAndCentrifugal:public edge6dCommandPacket
{
public:
	enum userIndex :int { Tx_dot2 = 0, Ty_dot2 = 1, Tz_dot2 = 2, Rx_dot = 3, Ry_dot = 4, Rz_dot = 5 };

	eP_userInclgravityAndCentrifugal()
	{
		head = 'G'; packetVer = 0; commandType = 'M'; type = 2;
		labels = { "Tx_dot2", "Ty_dot2", "Tz_dot2", "Rx_dot", "Ry_dot", "Rz_dot" };
		units = { "m/s2", "m/s2", "m/s2", "rad/s", "rad/s", "rad/s" };
	};


private:

	bool checkValRange(float val, int index)
	{
		float rangePP = 0;
		switch (index)
		{
		case Tx_dot2:
			rangePP = 6;
			break;
		case Ty_dot2:
			rangePP = 6;
			break;
		case Tz_dot2:
			rangePP = 5;
			break;
		case Rx_dot:
			rangePP = 35 * M_PI / 180;
			break;
		case Ry_dot:
			rangePP = 35 * M_PI / 180;
			break;
		case Rz_dot:
			rangePP = 40 * M_PI / 180;
			break;
		}

		if (fabs(val) > rangePP)
			return false;
		else
			return true;
	}
};

class ep_logPowerOffset :public edge6dLogPacket
{
public:
	enum servoIndex :int { servo_0 = 0, servo_1 = 1, servo_2 = 2, servo_3 = 3, servo_4 = 4, servo_5 = 5 };

	ep_logPowerOffset()
	{
		head = 'G'; packetVer = 0; commandType = 'L'; type = 0;
		labels = { "type","Power offset 0", "Power offset 1", "Power offset 2", "Power offset 3", "Power offset 4", "Power offset 5" };
		units = { " ", " ", " ", " ", " ", " " };
	};
private:
	bool checkValRange(float val, int index)
	{
		return true;
	}
};

class ep_Resetlog:public edge6dLogPacket
{
public:
	enum servoIndex :int { servo_0 = 0, servo_1 = 1, servo_2 = 2, servo_3 = 3, servo_4 = 4, servo_5 = 5 };

	ep_Resetlog()
	{
		head = 'G'; packetVer = 0; commandType = 'L'; type = 255;
		labels = { "type","Power offset 0", "Power offset 1", "Power offset 2", "Power offset 3", "Power offset 4", "Power offset 5" };
		units = { " ", " ", " ", " ", " ", " " };
	};
private:
	bool checkValRange(float val, int index)
	{
		return true;
	}
};
std::vector<eP_userInclgravityAndCentrifugal> readUserPacktFromFile(std::string fileName);
std::vector<eP_plarformPosition> readPosPacktFromFile(std::string fileName);

float inputFromSineAccelaration(float accRMS, float limit, float frequency, float time, float dt, char linearOrAngular);
