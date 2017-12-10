#include <ESP8266WiFi.h>
#include "Led.h"
#include "RTCDriver.h"
#include "TempSensor.h"
#include <Wire.h>


// Network Defines

#define WIFINAME "^^ B"
#define WIFIPASS "joanapassos"
#define SOCKET_PORT 10101

// Hardware Defines

#define TEMP_ADDRESS 0b01001000

// Modes

#define SETCOLOR_MODE  	0
#define TEMPCOLOR_MODE 	1
#define TIMECOLOR_MODE 	2
#define RANDOM_MODE    	3

// Commands

#define ECHO 			0
#define ECHO_RESPONSE 	1
#define SETMODE 		2
#define SETCOLOR 		3
#define SETDATE			4
#define SETTIME			5
#define REQUESTTEMP		6
#define SENDTEMP		7


WiFiServer server(SOCKET_PORT);

TempSensor temp(TEMP_ADDRESS);
RTCDriver rtc;
Led led(D5,D6,D8);  // Configuração do Servidor

uint8_t mode = 0;

bool rtc_present = false;
bool temp_present = false;


void setup() {

	// Inicializar o Serial
	Serial.begin(9600);

	// Inicializar o I2C
	Wire.begin(D3,D4);

	if(rtc.detect())
	{
		Serial.println("RTC Detetado!");
		rtc_present = true;
	}
	else
	{
		Serial.println("RTC não Detetado!");
	}

	if(temp.detect()) {
		Serial.println("Sensor Temperatura encontrado!");    
		temp_present = true;
	}
	else {
	 	Serial.println("Sensor Temperatura não encontrado!");    
	}

 // WiFi.disconnectAP();
  //WiFi.softAP("aladinoLamp", "genioslampada");
  
	WiFi.begin(WIFINAME,WIFIPASS);

  led.setColor(255,0,0,100);
  
	while(WiFi.status() != WL_CONNECTED)
	{


    delay(100);
		// TO DO: Método para reconnectar noutra configuração
	}

  led.setColor(0,255,0,100);

	Serial.println();
	Serial.println("Connected!");
	Serial.print("IP Address:");
	Serial.println(WiFi.localIP());

  
 temp.setConfig();
 temp.startConvert();
	 
	server.begin();

}



void loop() {

	WiFiClient client = server.available();
	DateTime current_time;



	if(rtc_present)
	{
		current_time = rtc.getDateTime();
	}

	if(client)
	{
		while(client.connected()) {

			float temperatura = temp.getTemp();


			// LOOP de Trabalho


			// obter tempo atual
			if(rtc_present)
			{
				current_time = rtc.getDateTime();
			}

			byte command; 

			if(client.available() > 0) {

				command = client.read();
			}

			if(command == ECHO) {
				client.write(ECHO_RESPONSE);
			}

			if(command == SETMODE)
			{
				// TODO: Validações
				byte m;
				while(!client.available() >= 1);

				m = client.read();

			}


			if(command == SETCOLOR) {
				// Comando setColor
				byte r, g ,b, i;

				while(!client.available() >= 3 ); // Espera os 3 paramêtros

       	//i = client.read();  // provisorio
				r = client.read();
				g = client.read();
				b = client.read();
        i = client.read();

        Serial.write("R:");
        Serial.println(r);
        Serial.write("G:");
        Serial.println(g);
        Serial.write("B:");
        Serial.println(b);
				
				if(mode == SETCOLOR_MODE)
				{
					led.setColor(r,g,b,i);
				}
			}

			if(command == SETDATE)
			{
				DateTime date;
				byte d, m , y;

				while(!client.available() >= 3);

				d = client.read();
				m = client.read();
				y = client.read();

				current_time.setDate(d,m,y+2000);

				if(rtc_present)
				{
					rtc.setDateTime(current_time);
				}

			}

			if(command == SETTIME) {
				DateTime date;
				byte h, m , s;

				while(!client.available() >= 3);

				h = client.read();
				m = client.read();
				s = client.read();

				current_time.setTime(h,m,s);

				if(rtc_present)
				{
					rtc.setDateTime(current_time);
				}


			}

			if(command == REQUESTTEMP)
			{
				client.write(SENDTEMP);

				if(temperatura <0)
				{
					client.write((byte) 1);
				}
				else
				{
					client.write((byte) 0);
				}

				byte temp_int = (byte) temperatura;
    			byte temp_decimal = (temperatura - temp_int) * 100;

    			Serial.println("Temp: ");
    			Serial.print(temp_int);
    			Serial.print(".");
    			Serial.println(temp_decimal);

    			client.write(temp_int);
    			client.write(temp_decimal);


			}

     command = 255;

     delay(20);




		}
	}
	







}
