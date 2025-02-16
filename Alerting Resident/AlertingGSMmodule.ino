/* ALERTING - GSM Module
* RX is digital pin 10 (connect to TX of GSM Modem)
* TX is digital pin 11 (connect to RX of GSM Modem) */


#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX
const int pina = 7;
int val = 0;


void setup() {

pinMode(pina,INPUT);

// Open serial communications and wait for port to open:

Serial.begin(9600);

Serial.println("Calling through GSM Modem");

}


void loop() {

val = digitalRead(pina);

if (val == HIGH) {

// set the data rate for the SoftwareSerial port
            mySerial.begin(9600);
            delay(2000);
            mySerial.println("ATD+18628720305;");

                        Serial.println("Calling 8628720305");

// print response over serial port
             if (mySerial.available())
             Serial.write(mySerial.read());
             delay(10000);
  }

}
