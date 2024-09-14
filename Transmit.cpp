#include    <iostream>
#include    "Transmit.h"
using namespace std;


void Input_Clear::Clear_Input_CommandLine() {
    std::cout << "\033[A";
    std::cout << "\r";  
    std::cout << "\033[K"; 
}