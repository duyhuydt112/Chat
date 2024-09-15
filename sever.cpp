#include    <iostream>
#include    "Transmit.h"

using namespace std;

// Global Variable


class Sever_Data_Stream : public Transmit_Data{
    private:
        int SeverSocket, ClientSocket; // Create socket prototype
        sockaddr_in ServerAddress;
        sockaddr_in ClientAddress;
        static Sever_Data_Stream* Object;
        string NameObject;
        mutex MutexObject;
        char ReceiveBuffer[BUFFER] = {0};
        char SendBuffer[BUFFER] = "Response from client"; 
        int Stage, Option = 1;
        vector <thread> ClientThreadS;
        vector <int> ClientSockets;
        vector <string> ClientNames;
        socklen_t ClientAddressSize = sizeof(ClientAddress);

        // Create socket connection
        Sever_Data_Stream(int Domain, int TypeSocket, int InternetProtocol, string Name){
            SeverSocket = socket(AF_INET, SOCK_STREAM, InternetProtocol); // InternetProtocol = 0, Domain = AF_INET, TypeSocket = SOCK_STREAM,
             if(SeverSocket < 0) {
                cerr << "Socket creation error: " << strerror(errno) << endl;
                exit(EXIT_FAILURE);
            }
            NameObject = Name;
        }
        
    public:
       
        // Prevent Create a Copy of Construction
         Sever_Data_Stream(const Sever_Data_Stream&) = delete;

        // Prevent Assign 
        void operator=(const Sever_Data_Stream&) = delete;

        // Close socket connection
         ~Sever_Data_Stream(){
            for(auto& Wait : ClientThreadS){
                if(Wait.joinable()){
                    Wait.join();
                }
            }
            close(ClientSocket);
            close(SeverSocket);
            
            
         }
    
        

        // Setting Sever Socket prototype
        void Config_Socket(short InternetProtocol, uint16_t Port, in_addr_t Adress){
            ServerAddress.sin_family = InternetProtocol; // AF_INET
            ServerAddress.sin_port = htons(Port);
            ServerAddress.sin_addr.s_addr = Adress; //INADDR_ANY
        }

        void Set_Socket(){
            if (setsockopt(SeverSocket, SOL_SOCKET, SO_REUSEADDR, &Option, sizeof(Option)) < 0) {
                cerr << "Set socket option error: " << strerror(errno) << endl;
                close(SeverSocket);
                exit(EXIT_FAILURE);
            }
        }

        // Binding local socket addresss to socket prototype address
        void Sever_Bind(){
            if((bind(SeverSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress))) < 0){
                 cerr << "Socket Binding Error: "<< strerror(errno) << endl;
                 close(SeverSocket);
                 exit(EXIT_FAILURE);
             }
         }

 
         // Waiting connect from clients
        void Sever_Listen(int NumberConnect){
            if(listen(SeverSocket, NumberConnect) < 0 ){ // max number connects= 0-4095
            cerr << "Sever Listen Error"<< strerror(errno) << endl;
            close(SeverSocket);
            exit(EXIT_FAILURE);
            }
         }

        // Accept Connection with clients
        void Sever_Accept(){
            int loop = 0, NumOfCLient = 1;
            char Index[10], Unknow[11];
            while(true){
                ClientSocket = accept(SeverSocket, (struct sockaddr*)&ClientAddress, &ClientAddressSize);
                if(ClientSocket < 0){
                    cerr << "Sever Accept Error"<< strerror(errno) << endl;
                    continue;
                }
                else{
                    if(loop < 1){
                        ClientThreadS.emplace_back(&Sever_Data_Stream::Edit_Send, this, 0, 0);
                        loop++;
                    }
                    lock_guard<mutex> lock(MutexObject);
                    ClientSockets.push_back(ClientSocket);
                    strcpy(Unknow, "Unknown_");
                    sprintf(Index, "%d", NumOfCLient++);
                    ClientThreadS.emplace_back(&Sever_Data_Stream::Receive_Data, this, 0, ClientSocket, strcat(Unknow,Index));

                }
               
            }
            
        }
        
        // Send Text
        void Send_Data(int Mode, int ClientSocket) override{
            lock_guard<mutex> lock(MutexObject);
            if((send(ClientSocket, SendBuffer, strlen(SendBuffer), Mode)) < 0){
                cerr << "Send Message Error"<< strerror(errno) << endl;
                return;
            }
            else
                
                return;
        }

        // Send Name
        void Send_Name(int Mode, int ClientSocket, const char* Buffer) override {
            unique_lock<mutex> lock(MutexObject);
            if((send(ClientSocket, Buffer, strlen(Buffer), Mode)) < 0){
                cerr << "Send Message Error: "<< strerror(errno) << endl;
                return;
            }
            else
                cout <<": " << SendBuffer << endl;
                strcpy(SendBuffer, "");
        }


        // receive message from client and send that data too all client
        void Receive_Send_Message(int Mode, int ClientSocket, const char* Buffer){
            lock_guard<mutex> lock(MutexObject);
            if((send(ClientSocket, Buffer, strlen(Buffer), Mode)) < 0){
                cerr << "Send Message Error"<< strerror(errno) << endl;
                return;
            }
            else
                return;
        }
        

        // // Function for check name of client
        // int Check_Client_Name(int ClientSocket){
        //     int IndexNumber;
        //     for(size_t index = 0; index < ClientSockets.size(); index++){
        //         if(ClientSockets[index] == ClientSocket){
        //             IndexNumber = index;
        //         }
        //     }
        //     return IndexNumber;
        // }



        // Receive Text
        void Receive_Data(int Mode, int ClientSocket, string Name) override{
     
            int loop = 0;
            string Temp_Name = Name;
            while(true){
                
                Stage = recv(ClientSocket, ReceiveBuffer, sizeof(ReceiveBuffer), Mode);
                if(Stage  < 0){
                    cerr << "Receive Message Error: "<< strerror(errno) << endl;
                    break;
                }
                 else if(Stage == 0){   // cach de 1 client dong thi ko bao loi trong vong loop tiep theo
                    cout << "Client Disconnect " << endl;
                    close(ClientSocket);
                }

                else{
                    if(loop++ == 0){
                        Temp_Name = ReceiveBuffer;
                        if(Temp_Name == "."){
                            Temp_Name = Name;
                            ClientNames.push_back(Temp_Name);
                        }
                        continue;
                        ClientNames.push_back(Temp_Name);
                    }

                    Temp_Name = Temp_Name + ": " + ReceiveBuffer;
                    cout << Temp_Name << endl;
                    strcpy(ReceiveBuffer, Temp_Name.c_str());
                    for(size_t i = 0; i < ClientSockets.size(); i++){
                        if(ClientSockets[i] != ClientSocket){
                            
                            Receive_Send_Message(0, ClientSockets[i], ReceiveBuffer); 
                        } 
                    }
                    memset(ReceiveBuffer, 0, sizeof(ReceiveBuffer));

                }
            }
            
        }
        
        // Create Method
        static  Sever_Data_Stream* Create_Object(string Name){
            if(nullptr == Object){
                Object = new Sever_Data_Stream(AF_INET, SOCK_STREAM, 0, Name);
            }
            return Object;
        }

        // Enter the sentence and change string from sever to all clients
        void Edit_Send(int Mode, int ClientSocket) override{
            char AdditionalText[BUFFER];
            while(true){
                memset(SendBuffer, 0, sizeof(SendBuffer));
                cin.getline(AdditionalText, BUFFER);
                strcat(SendBuffer, "Admin: ");
                strcat(SendBuffer, AdditionalText);
                Input_Clear::Clear_Input_CommandLine();
                if(cin.fail()){
                    cerr << "Input Error: "<< strerror(errno) << endl;
                    cin.clear(); 
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    return;
                }
                else{
                    if(strcmp(SendBuffer, "~") == 0){
                        for(size_t i = 0; i < ClientSockets.size(); i++){
                            close(ClientSockets[i]);
                        }
                        close(SeverSocket);
                    }
                    for(size_t i = 0; i < ClientSockets.size(); i++){
                        Send_Data(0, ClientSockets[i]);
                    }
                    cout << SendBuffer << endl; 
                    memset(AdditionalText, 0, sizeof(AdditionalText));
              
                }
                
                
            }

        }

};



// User Golbal Variable
Sever_Data_Stream* Sever_Data_Stream::Object = nullptr;

int main()
{   
    string SeverName;
    // cout << "Enter Sever Name: ";
    // cin >> SeverName;
    Sever_Data_Stream* Sever = Sever_Data_Stream::Create_Object("Hello");
    
    Sever->Config_Socket(AF_INET, PORT, INADDR_ANY);
    
    Sever->Set_Socket();
    Sever->Sever_Bind();
    Sever->Sever_Listen(5);
    Sever->Sever_Accept();
    delete Sever;
    return 0;
}