#include    <iostream>
#include    "Transmit.h"

using namespace std;

class Client_Data_Stream : public Transmit_Data{
    private: 
        int ClientSocket;
        uint16_t ClientPort;
        sockaddr_in ClientAddress;
        char ReceiveBuffer[BUFFER] = {0};
        char SendBuffer[BUFFER] = "Response from client"; 
        mutex MutexObject;
        int Receive_Stage;
        thread SendThread;
        thread ReceiveThread;
        string ClientName;
    public:

        // Config Client Socket
        void Config_Socket(short InternetProtocol, const char* IP_Address){
            ClientAddress.sin_family = InternetProtocol; // AF_INET
            ClientAddress.sin_port = htons(ClientPort);

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

        // Close Socket
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

        // Create Connect with sever
        void Client_Conneted(){
            if((connect(ClientSocket, (struct sockaddr*)&ClientAddress, sizeof(ClientAddress))) < 0){
                cerr << "Client Connected Error: "<< strerror(errno) << endl;
                exit(EXIT_FAILURE);
            }
            else{

                SendThread = thread(&Client_Data_Stream::Edit_Send, this, 0, ClientSocket);
                ReceiveThread = thread(&Client_Data_Stream::Receive_Data, this, 0, ClientSocket, ClientName);
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
                if(ClientName == ".") ClientName = "Unknow(You)";
                cout << ClientName << ": " << SendBuffer << endl;
                strcpy(SendBuffer, "");
        }
        
        // sent name of client
        void Send_Name(int Mode, int ClientSocket, const char* Buffer) override {
            unique_lock<mutex> lock(MutexObject);
            if((send(ClientSocket, Buffer, strlen(Buffer), Mode)) < 0){
                cerr << "Send Message Error: "<< strerror(errno) << endl;
                return;
            }
            else

                strcpy(SendBuffer, "");
        }

        void Edit_Send(int Mode, int ClientSocket) override{
            strcpy(ReceiveBuffer, ClientName.c_str());
            Send_Name(Mode, ClientSocket, ReceiveBuffer);
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
        void Receive_Data(int Mode, int ClientSocket, string Name) override{
            while(true){
                Receive_Stage = recv(ClientSocket, ReceiveBuffer, sizeof(ReceiveBuffer), Mode);
                if(Receive_Stage < 0){
                    cerr << "Receive Message Error: "<< strerror(errno) << endl;
                    return;
                }
                else if(Receive_Stage == 0){
                    cout << "Sever Disconnect " << endl;
                    close(ClientSocket);
                    return;
                }

                else{
                    lock_guard<mutex> lock(MutexObject);
                    cout<< ReceiveBuffer << endl;   //cout receive buffer
                    memset(ReceiveBuffer, 0, sizeof(ReceiveBuffer));

                }
                    

            }
            
        }

        // Enter Information of clients
        void Enter_Client_Information(){
            cout << "Enter Port: ";
            cin >> ClientPort; 
            cout << "Enter Client Name: ";
            cin >> ClientName;

        }
};  



int main() 
{
    
    Client_Data_Stream* Client = new Client_Data_Stream(AF_INET, SOCK_STREAM, 0);
    Client->Enter_Client_Information();
    Client->Config_Socket(AF_INET, "127.0.0.1");
    Client->Client_Conneted();
    delete Client;
    return 0;
}