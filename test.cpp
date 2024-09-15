// Online C++ compiler to run C++ program online
#include <iostream>
#include <cstring>
using namespace std;
#define BUFFER 1024
int main() {
    char additionalText[BUFFER] = "Admin: ";
    char SendBuffer[BUFFER];
    // Write C++ code here
    cin.getline(SendBuffer, BUFFER);
    strcat(additionalText, SendBuffer);
    std::cout << additionalText << endl;

    return 0;
}