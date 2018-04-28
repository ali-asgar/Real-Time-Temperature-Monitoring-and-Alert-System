/****PIPE0 Transmitter****/

#include "mbed.h"
#include "nRF24L01P.h"
#include "Dht11.h"
 
Serial pc(USBTX, USBRX); // tx, rx
 Dht11 sensor(PTD7);
 
nRF24L01P my_nrf24l01p(PTD2, PTD3, PTD1, PTD0, PTD5, PTD4);    // mosi, miso, sck, csn, ce, irq
DigitalOut RedLED(LED1);
 
int main()
{
    char count[1];
    char TxDataCnt;
    char temp;
    
    //set tx and rx address for pipe0: 5 bytes long; different for every pipe
    long long TxAddress_PIPE0 = 0xC2C2C2C2C2;
    long long RxAddress_PIPE0 = 0xC2C2C2C2C2;
    
    my_nrf24l01p.powerUp();
    my_nrf24l01p.setRfFrequency(2492);
    
    //setting tx and rx address
    my_nrf24l01p.setTxAddress(TxAddress_PIPE0);
    
    //set rx address with default width and pipe number
    my_nrf24l01p.setRxAddress(RxAddress_PIPE0, DEFAULT_NRF24L01P_ADDRESS_WIDTH, NRF24L01P_PIPE_P0);
    
    // Display the (default) setup of the nRF24L01+ chip
    pc.printf( "nRF24L01+ Frequency    : %d MHz\r\n",  my_nrf24l01p.getRfFrequency() );
    pc.printf( "nRF24L01+ Output power : %d dBm\r\n",  my_nrf24l01p.getRfOutputPower() );
    pc.printf( "nRF24L01+ Data Rate    : %d kbps\r\n", my_nrf24l01p.getAirDataRate() );
    pc.printf( "nRF24L01+ TX Address - PIPE0  : 0x%010llX\r\n", my_nrf24l01p.getTxAddress() );
    
    pc.printf( "Wirelesss sensor network \r\n");
        
    TxDataCnt = 1;
    
    //set transfer size for specified pipe
    my_nrf24l01p.setTransferSize(TxDataCnt, NRF24L01P_PIPE_P0);
    my_nrf24l01p.enable();
    
    char pot_val;
    
    while (1) {
        
        //adjusting data to 0-255 with char data type
        sensor.read();
        pc.printf("Current Temperature is %f degrees in fahrenheit and Humidity is %d%\r\n", sensor.getFahrenheit(), sensor.getHumidity());
        pot_val = sensor.getFahrenheit();
        count[0] = pot_val;
        
        //transmit on specified pipe
        temp = my_nrf24l01p.write( NRF24L01P_PIPE_P0,count, TxDataCnt );
 
        pc.printf( "Sending %d bytes from Transmitter FRDM Board to Receiver FRDM Board through PIPE0; Temperature = %d\r\n",temp,count[0]);
 
        // Toggle LED1 (to help debug Host -> nRF24L01+ communication)
        RedLED = !RedLED;
          
        wait(2);
    }
}