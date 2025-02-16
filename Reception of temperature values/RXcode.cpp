/*****Multiceiver wireless network*****/

#include "mbed.h"
#include "ESP8266.h"
#include "nRF24L01P.h"
 
Serial pc(USBTX, USBRX); // tx, rx
 
nRF24L01P my_nrf24l01p(PTD2, PTD3, PTD1, PTD0, PTD5, PTD4);    // mosi, miso, sck, csn, ce, irq
ESP8266 wifi(PTE0, PTE1, 115200); 

char snd[255],resp[1000];
char http_cmd[300], comm[300];
int timeout = 5000;
DigitalOut myled(PTA5);

#define SSID "yash"
#define PASS "12345678"

//Remote IP
#define IP "34.231.200.253"

//ldrvalue global variable
int pot1Val = 0;
    

//Public and private keys for phant
char* Update_Key = "T2UOHRRW96LYCTE8";
//blue for pipe1 and green for pipe0
DigitalOut GreenLED(LED2);
DigitalOut RedLED(LED1);

//Wifi init function
void wifi_initialize(void){
    
    pc.printf("\n******** Resetting wifi module ********\r\n");
    wifi.Reset();
    
    //wait for 5 seconds for response, else display no response receiveed
    if (wifi.RcvReply(resp, 5000))    
        pc.printf("%s",resp);    
    else
        pc.printf("No response");
    
    pc.printf("******** Setting Station mode of wifi with AP ********\r\n");
    wifi.SetMode(1);    // set transparent  mode
    if (wifi.RcvReply(resp, timeout))    //receive a response from ESP
        pc.printf("%s",resp);    //Print the response onscreen
    else
        pc.printf("No response while setting mode. \r\n");
    
    pc.printf("******** Joining network with SSID and PASS ********\r\n");
    wifi.Join(SSID, PASS);     
    if (wifi.RcvReply(resp, timeout))    
        pc.printf("%s",resp);   
    else
        pc.printf("No response while connecting to network \r\n");
        
    pc.printf("******** Getting IP and MAC of module ********\r\n");
    wifi.GetIP(resp);     
    if (wifi.RcvReply(resp, timeout))    
        pc.printf("%s",resp);    
    else
        pc.printf("No response while getting IP \r\n");
    
    pc.printf("******** Setting WIFI UART passthrough ********\r\n");
    wifi.setTransparent();          
    if (wifi.RcvReply(resp, timeout))    
        pc.printf("%s",resp);    
    else
        pc.printf("No response while setting wifi passthrough. \r\n");
    wait(1);    
    
    pc.printf("******** Setting single connection mode ********\r\n");
    wifi.SetSingle();             
    wifi.RcvReply(resp, timeout);
    if (wifi.RcvReply(resp, timeout))    
        pc.printf("%s",resp);    
    else
        pc.printf("No response while setting single connection \r\n");
    wait(1);
}

void wifi_send(void){
    
    pc.printf("******** Starting TCP connection on IP and port ********\r\n");
    wifi.startTCPConn(IP, 80);    //cipstart
    wifi.RcvReply(resp, timeout);
    if (wifi.RcvReply(resp, timeout))    
        pc.printf("%s",resp);    
    else
        pc.printf("No response while starting TCP connection \r\n");
    wait(1);
    
    //create link 
    sprintf(http_cmd,"/update?key=%s&field1=%d",Update_Key,pot1Val); 
    pc.printf(http_cmd);
    
    pc.printf("******** Sending URL to wifi ********\r\n");
    wifi.sendURL(http_cmd, comm);   //cipsend and get command
    if (wifi.RcvReply(resp, timeout))    
        pc.printf("%s",resp);    
    else
        pc.printf("No response while sending URL \r\n");
    
    //wifi.SendCMD("AT+CIPCLOSE"); //Close the connection to server
    //wifi.RcvReply(resp, timeout);
    //pc.printf("%s", resp);
}
 
int main()
 {

    char count[1];
    char RxDataCnt_PIPE0, RxDataCnt_PIPE1;
    char temp;
    
    //specifying address same as transmitter for pipe0 and pipe1
    long long RxAddress_PIPE0 = 0xC2C2C2C2C2;
    
    wifi_initialize();
    my_nrf24l01p.powerUp();
    my_nrf24l01p.setRfFrequency(2492);
    
    //set rx address with default address and for specified pipe
    my_nrf24l01p.setRxAddress(RxAddress_PIPE0, DEFAULT_NRF24L01P_ADDRESS_WIDTH, NRF24L01P_PIPE_P0);
 
    // Display the (default) setup of the nRF24L01+ chip
    pc.printf( "nRF24L01+ Frequency    : %d MHz\r\n",  my_nrf24l01p.getRfFrequency() );
    pc.printf( "nRF24L01+ Output power : %d dBm\r\n",  my_nrf24l01p.getRfOutputPower() );
    pc.printf( "nRF24L01+ Data Rate    : %d kbps\r\n", my_nrf24l01p.getAirDataRate() );
    
    //display rx address for both pipes
    pc.printf( "nRF24L01+ RX Address - PIPE0  : 0x%010llX\r\n", my_nrf24l01p.getRxAddress(NRF24L01P_PIPE_P0) ); 
    pc.printf( "Wireless Sensor Network - Multiceiver\r\n" );
    
    RxDataCnt_PIPE0 = 1;
    
    //set transfer size explicitly for both pipes
    my_nrf24l01p.setTransferSize(RxDataCnt_PIPE0, NRF24L01P_PIPE_P0);
    
    my_nrf24l01p.setReceiveMode();
    my_nrf24l01p.enable();
 
    while (1) 
    {
        //check if data is available in pipe0
        if ( my_nrf24l01p.readable(NRF24L01P_PIPE_P0) ) {
 
            // ...read the data into the receive buffer
            temp = my_nrf24l01p.read( NRF24L01P_PIPE_P0, count, RxDataCnt_PIPE0 );
            
            pot1Val = count[0];
 
            pc.printf("Received: %d bytes from PIPE0; Temperature = %d\r\n",temp, count[0]);
            
            // Toggle LED2 (to help debug nRF24L01+ -> Host communication)
            GreenLED = !GreenLED;
        }
        pc.printf("%d \n\r",pot1Val);
        
        wifi_send();
        if(pot1Val>80) {
           myled=1; 
        }
        else {
            myled=0;
        }
        wait(5);
    }
}