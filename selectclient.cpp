#include    <iostream>
#include    "Transmit.h"

using namespace std;

class Client_Data_Stream : public Transmit_Data{
    private: 
        int ClientSocket; 
        sockaddr_in ClientAddress;
        char ReceiveBuffer[BUFFER] = {0};
        char SendBuffer[BUFFER] = "Response from client"; 
        mutex MutexObject;
        int Stage;
        thread SendThread;
        thread ReceiveThread;
    public:

        // Config Client Socket
        void Config_Socket(short InternetProtocol, uint16_t Port, const char* IP_Address){
            ClientAddress.sin_family = InternetProtocol; // AF_INET
            ClientAddress.sin_port = htons(Port);

            if(inet_pton(InternetProtocol, IP_Address, &ClientAddress.sin_addr) <= 0) {
                cerr << "Invalid address / Address not supported" << endl;
                exit(EXIT_FAILURE);
            }
            
        }
        
        // Create Socket
        Client_Data_Stream(int Domain, int TypeSocket, int InternetProtocol){
            ClientSocket = socket(Domain, TypeSocket, InternetProtocol);// InternetProtocol = 0, Domain = AF_INET, TypeSocket = SOCK_STREAM,
             if(ClientSocket < 0) {
                cerr << "Socket creation error: " << strerror(errno) << endl;
                exit(EXIT_FAILURE);
            }
        }
        //using Sever_Data_Stream::Sever_Data_Stream
        
        ~Client_Data_Stream(){
                
            // Waiting Thread End
            if (SendThread.joinable()) {
                SendThread.join();
            }
            if (ReceiveThread.joinable()) {
                ReceiveThread.join();
            }
            close(ClientSocket);
            
        }

        void Client_Conneted(){
            if((connect(ClientSocket, (struct sockaddr*)&ClientAddress, sizeof(ClientAddress))) < 0){
                cerr << "Client Connected Error: "<< strerror(errno) << endl;
                exit(EXIT_FAILURE);
            }
            else{
                SendThread = thread(&Client_Data_Stream::Edit_Send, this, 0, ClientSocket);
                ReceiveThread = thread(&Client_Data_Stream::Receive_Data, this, 0, ClientSocket);
            }
        }
        
        // Send Text
        void Send_Data(int Mode, int ClientSocket) override{
            unique_lock<mutex> lock(MutexObject);
            if((send(ClientSocket, SendBuffer, strlen(SendBuffer), Mode)) < 0){
                cerr << "Send Message Error: "<< strerror(errno) << endl;
                return;
            }
            else
                cout << "Client: " << SendBuffer << endl;
                strcpy(SendBuffer, "");
        }
        

        void Edit_Send(int Mode, int ClientSocket) override{
            while(true){
                
                cin.getline(SendBuffer, BUFFER);
                Input_Clear::Clear_Input_CommandLine();
                if(cin.fail()){
                    cerr << "Input Error: "<< strerror(errno) << endl;
                    cin.clear(); 
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    return;
                }
                else{
                    if(strcmp(SendBuffer, "~") == 0){
                        close(ClientSocket);
                        return;
                    } 
                    Send_Data(Mode, ClientSocket);
                }
                 
                
            }
        }
        // Receive Text
        void Receive_Data(int Mode, int ClientSocket) override{
            while(true){
                Stage = recv(ClientSocket, ReceiveBuffer, sizeof(ReceiveBuffer), Mode);
                if(Stage < 0){
                    cerr << "Receive Message Error: "<< strerror(errno) << endl;
                    return;
                }
                else if(Stage == 0){
                    cout << "Sever Disconnect " << endl;
                    close(ClientSocket);
                    return;
                }

                else{
                    lock_guard<mutex> lock(MutexObject);
                    cout << "Sever: " << ReceiveBuffer << endl;
                    memset(ReceiveBuffer, 0, sizeof(ReceiveBuffer));

                }
                    

            }
            
        }
};  



int main() 
{
    int ClientPort;
    Client_Data_Stream* Client = new Client_Data_Stream(AF_INET, SOCK_STREAM, 0);
    cout << "Hay Nhap Port: ";
    cin >> ClientPort; 
    Client->Config_Socket(AF_INET, ClientPort, "127.0.0.2");
    Client->Client_Conneted();
  
    

    delete Client;


    return 0;
}