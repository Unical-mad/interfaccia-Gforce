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
    
    double testing_frequency[7] = { 0.5,1,3,5,8,12,15 };
    double testing_Amplitude[5] = { 0.3,0.5,0.7,0.9,1.2 };

    int freqIndex = 0;
    int aIndex = 0;
    while (freqIndex < 8)
    {
        while (aIndex < 5)
        {
            while (k < 6)
            {
                switch (k)
                {

                case 0:
                    std::cout << "start Tx motion \n";
                    break;
                case 1:
                    std::cout << "start Ty motion \n";
                    break;
                case 2:
                    std::cout << "start Tz motion \n";
                    break;
                case 3:
                    std::cout << "start Rx motion\n";
                    break;
                case 4:
                    std::cout << "start Ry motion \n";
                    break;
                case 5:
                    std::cout << "start Rz motion \n";
                    break;
                }
                t = 0;
                dt = 0;

                while (t < 10)//i<10)
                {
                    auto start = std::chrono::high_resolution_clock::now();

                    switch (k)
                    {
                    case 0:
                        sine = inputFromSineAccelaration(testing_Amplitude[aIndex], TXLIMIT*pow((2.0f * M_PI * testing_frequency[freqIndex]), 2), testing_frequency[freqIndex], t, dt, 0);
                        dt = 0;
                        Pp.setValues(sine, Pp.Tx);
                        break;
                    case 1:
                        sine = inputFromSineAccelaration(testing_Amplitude[aIndex], TYLIMIT * pow((2.0f * M_PI * testing_frequency[freqIndex]), 2), testing_frequency[freqIndex], t, dt, 0);
                        dt = 0;
                        Pp.setValues(sine, Pp.Ty);
                        break;
                    case 2:
                        sine = inputFromSineAccelaration(testing_Amplitude[aIndex], TZLIMIT * pow((2.0f * M_PI * testing_frequency[freqIndex]), 2), testing_frequency[freqIndex], t, dt, 0);
                        dt = 0;
                        Pp.setValues(sine, Pp.Tz);
                        break;
                    case 3:
                        sine = inputFromSineAccelaration(testing_Amplitude[aIndex]*10 * M_PI / 180, RLIMIT * pow((2.0f * M_PI * testing_frequency[freqIndex]), 2), testing_frequency[freqIndex], t, dt, 1);
                        dt = 0;
                        Pp.setValues(sine, Pp.Rx);
                        break;
                    case 4:
                        sine = inputFromSineAccelaration(testing_Amplitude[aIndex]*10 * M_PI / 180, RLIMIT * pow((2.0f * M_PI * testing_frequency[freqIndex]), 2), testing_frequency[freqIndex], t, dt, 1);
                        dt = 0;
                        Pp.setValues(sine, Pp.Ry);
                        break;
                    case 5:
                        sine = inputFromSineAccelaration(testing_Amplitude[aIndex]*10 * M_PI / 180, RLIMIT * pow((2.0f * M_PI * testing_frequency[freqIndex]), 2), testing_frequency[freqIndex], t, dt, 1);
                        dt = 0;
                        Pp.setValues(2 * sine, Pp.Rz);
                        break;
                    }


                    while (dt < 0.01)
                    {
                        auto end = std::chrono::high_resolution_clock::now();
                        elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                        dt = (float)elapsed_time.count()*1.0 / 1000000000.0f;

                    }
                    t += dt;
                    gforce6dCom << Pp;
                    //   std::cout << "current time  " << t << "  dt:  " << dt << "\n";

                }
                auto start = std::chrono::high_resolution_clock::now();

                while (dt < 1)
                {
                    auto end = std::chrono::high_resolution_clock::now();
                    elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    dt = elapsed_time.count() / 1000000.0f;

                }
                k++;
            }
            k = 0;
            aIndex++;
        }
        freqIndex++;
        aIndex = 0;

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
    pos = posAmplitude * sin(2.0f * M_PI * frequency * time);
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
