// Edge6dInterface.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//
#include "Edge6dInterface.h"


int main()
{
    float t, dt;
    std::chrono::nanoseconds elapsed_time;
    wchar_t gForceIp[INET6_ADDRSTRLEN] = L"255.255.255.255"; //motion platfrom server address @broadcast communication
    wchar_t localIp[INET6_ADDRSTRLEN] = L"169.254.201.42"; //local host platfrom server address

    eP_servoPowerOffset Po;
    ep_logPowerOffset lPo;
    ep_Resetlog lRst;

    udpCom gforce6dCom(50001, gForceIp, 1);

    if (gforce6dCom.connectToServer() == 0)
        std::cout << "connected to motion platform @255.255.255.255\n";
    else
        std::cout << "connection error\n";


    int k = 0;

    t = 0;
    dt = 0;
    float sine;
    const float  TXLIMIT = 0.13;
    const float TYLIMIT = 0.10;
    const float TZLIMIT = 0.07;
    const float RLIMIT = 15;

    eP_plarformPosition Pp;
    eP_userInclgravityAndCentrifugal Pg;
    
    double testing_frequency[9] = { 0.5,1,4,3,5,8,12,15,20 };
    double testing_Amplitude[7] = { 0.0001,0.002,0.01,0.02,0.03,0.9,1.2 };

    int freqIndex = 1;
    int aIndex = 2;
    std::cin >> freqIndex;
    std::cin >> aIndex;
    t = 0;
    auto start = std::chrono::high_resolution_clock::now();

    while (1)
    {
        auto end = std::chrono::high_resolution_clock::now();
        elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        t = (float)elapsed_time.count() * 1.0f / 1000000000.0f;
        sine = inputFromSineAccelaration(testing_Amplitude[aIndex], TZLIMIT * pow((2.0f * M_PI * testing_frequency[freqIndex]), 2), testing_frequency[freqIndex], t, dt, 0);
        Pp.setValues(sine, Pp.Tz);
        gforce6dCom << Pp;
        Sleep(5);

    }

        gforce6dCom.~udpCom();

}   



//Generate position packet in order to obatain spefici acceleration on motion axis 
float inputFromSineAccelaration(float accMax,float limit, float frequency, float time,float dt,char linearOrAngular)
{
    float acc;
    static float speed=0;
    static float pos=0;
   
    if (accMax > limit)
        accMax = limit;
    float posAmplitude = -accMax / pow((2.0f * M_PI * frequency), 2);
    pos = accMax * sin(2.0f * M_PI * frequency * time);
    speed = posAmplitude *(2.0f * M_PI * frequency)*sin(2.0f * M_PI * frequency * time);//speed phase +90 to avoid cosine discontinuuity on t=0 
    acc = -posAmplitude * pow((2.0f * M_PI * frequency),2) * sin(2.0f * M_PI * frequency * time);
   //std::cout <<time<<";"<< acc << ";" << speed << ";" << pos<<"\n";
    if (linearOrAngular == 0)
        return pos;
    else
        return speed;

}

std::vector<eP_userInclgravityAndCentrifugal> readUserPacktFromFile(std::string fileName)
{
    eP_userInclgravityAndCentrifugal res;
    std::vector<eP_userInclgravityAndCentrifugal> ResVect;
    using namespace std;
    string sectionName;
    ifstream myfile(fileName);
    if (myfile.is_open())
    {
        while (1)
        {
            char line[256];
            if (!myfile.getline(line, 256))
                break;
            const char* delim = ",";
            char* next_token;
            char* varValue = strtok_s(line, delim, &next_token);
            res.setValues(strtof(varValue, &varValue), res.Tx_dot2);
            varValue = strtok_s(next_token, delim, &next_token);
            res.setValues(strtof(varValue, &varValue), res.Ty_dot2);
            varValue = strtok_s(next_token, delim, &next_token);
            res.setValues(strtof(varValue, &varValue), res.Tz_dot2);
            varValue = strtok_s(next_token, delim, &next_token);
            res.setValues(strtof(varValue, &varValue), res.Rx_dot);
            varValue = strtok_s(next_token, delim, &next_token);
            res.setValues(strtof(varValue, &varValue), res.Ry_dot);
            varValue = strtok_s(next_token, delim, &next_token);
            res.setValues(strtof(varValue, &varValue), res.Rz_dot);
            ResVect.push_back(res);
        }
    }

        myfile.close();

        return ResVect;
}

std::vector<eP_plarformPosition> readPosPacktFromFile(std::string fileName)
{
    eP_plarformPosition res;
    std::vector<eP_plarformPosition> ResVect;
    using namespace std;
    string sectionName;
    ifstream myfile(fileName);
    if (myfile.is_open())
    {
        while (1)
        {
            char line[256];
            if (!myfile.getline(line, 256))
                break;
            const char* delim = ",";
            char* next_token;
            char* varValue = strtok_s(line, delim, &next_token);
            res.setValues(strtof(varValue, &varValue), res.Tx);
            varValue = strtok_s(next_token, delim, &next_token);
            res.setValues(strtof(varValue, &varValue), res.Ty);
            varValue = strtok_s(next_token, delim, &next_token);
            res.setValues(strtof(varValue, &varValue), res.Tz);
            varValue = strtok_s(next_token, delim, &next_token);
            res.setValues(strtof(varValue, &varValue), res.Rx);
            varValue = strtok_s(next_token, delim, &next_token);
            res.setValues(strtof(varValue, &varValue), res.Ry);
            varValue = strtok_s(next_token, delim, &next_token);
            res.setValues(strtof(varValue, &varValue), res.Rz);
            ResVect.push_back(res);
        }
    }

    myfile.close();

    return ResVect;
}
