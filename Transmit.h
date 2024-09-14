
#ifndef TRANSMIT_H
#define TRANSMIT_H

// User Libraries
#include    <iostream>
#include    <thread>
#include    <cstring>
#include    <netinet/in.h>
#include    <sys/socket.h>
#include    <unistd.h>
#include    <condition_variable>
#include    <mutex>
#include    <termios.h>

// User Define
#define PORT 4506
#define BUFFER 1024 

// User Global Variable 
extern std::mutex ExitFlagMutex;

// set namespace
using namespace std;

class Transmit_Data{
    public: 
        virtual void Send_Data(int Mode) = 0;
        virtual void Receive_Data(int Mode) = 0;
        virtual void Edit_Send(int Mode) = 0;
};

class Input_Clear{
    public:                           
        static void Clear_Input_CommandLine(); 
}

#endif