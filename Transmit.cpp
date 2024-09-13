#include    <iostream>
#include    "Transmit.h"
using namespace std;

// Hàm để tắt hiển thị đầu vào
void Clear_Input_CommandLine() {
    std::cout << "\033[A";
    std::cout << "\r";  
    std::cout << "\033[K"; 
}

