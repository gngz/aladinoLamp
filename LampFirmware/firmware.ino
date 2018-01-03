#include <ESP8266WiFi.h>
#include "Led.h"
#include "RTCDriver.h"
#include "TempSensor.h"
#include <Wire.h>

#define HOME_TEST
#define DEBUG

// Network Defines

#ifdef GROUP_TEST
#define WIFINAME "aladinos"
#define WIFIPASS "1234567890"
#endif

#ifdef HOME_TEST
#define WIFINAME "SSID"
#define WIFIPASS "PWD"
#endif


#define SOCKET_PORT 10101

// Hardware Defines

#define TEMP_ADDRESS 0b01001000

// Color Defines

#define DARKBLUE 0,0,255,100
#define MEDIUMBLUE 0,0,81,100
#define LIGHTBLUE 0,150,255,100
#define LIGHTERBLUE1 0,200,255,100
#define LIGHTERBLUE2 0,255,255,100
#define GREEN 0,255,0,100
#define GREENISH 205,255,0,100
#define YELLOW 255,255,0,100
#define ORANGE 255,165,0,100
#define RED 255,0,0,100


// Modes Defines

#define SETCOLOR_MODE  	0
#define TEMPCOLOR_MODE 	1
#define RANDOM_MODE    	2

#define STATE_MANTAINCOLOR  0
#define STATE_CHANGECOLOR   1



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
Led led(D5, D6, D8); // Configuração do Servidor

uint8_t mode = SETCOLOR_MODE;
uint8_t random_state = STATE_MANTAINCOLOR;

unsigned long latest_mantain_millis;
unsigned long latest_change_millis;



const int MANTAIN_TIME = 10000;
const int CHANGE_TIME = 10;


bool rtc_present = false;
bool temp_present = false;

float temperatura;

uint8_t r, g , b, i;
uint8_t new_r, new_g, new_b;
bool new_color = false;
bool new_delay = false;


void setup() {

	// Inicializar o Serial
	Serial.begin(9600);

	// Inicializar o I2C
	Wire.begin(D3, D4);

	if (rtc.detect())
	{
#ifdef DEBUG
		Serial.println("RTC Detetado!");
#endif
		rtc_present = true;
	}
#ifdef DEBUG
	else
	{
		Serial.println("RTC não Detetado!");
	}
#endif


	if (temp.detect()) {
#ifdef DEBUG
		Serial.println("Sensor Temperatura encontrado!");
#endif
		temp_present = true;
	}
#ifdef DEBUG
	else {
		Serial.println("Sensor Temperatura não encontrado!");
	}
#endif

	// WiFi.disconnectAP();
	//WiFi.softAP("aladinoLamp", "genioslampada");

	WiFi.begin(WIFINAME, WIFIPASS);

	led.setColor(255, 0, 0, 100);

	while (WiFi.status() != WL_CONNECTED)
	{


		delay(100);
	// TO DO: Método para reconnectar noutra configuração
	}

	led.setColor(0, 255, 0, 100);

#ifdef DEBUG
	Serial.println();
	Serial.println("Connected!");
	Serial.print("IP Address:");
	Serial.println(WiFi.localIP());
#endif


	temp.setConfig();
	temp.startConvert();

	server.begin();

}



void loop() {

	WiFiClient client = server.available();
	DateTime current_time;

	if (temp_present)
	{
		temperatura = temp.getTemp();
	}


	if (rtc_present)
	{
		current_time = rtc.getDateTime();
	}

	if (client)
	{

		if (!client.connected())
		{
			client.flush();
			client.stop();
		}
#ifdef DEBUG
		else
		{
			Serial.println("Cliente Conectado!");
		}
#endif

		while (client.connected()) {

  // obter tempo atual
			if (rtc_present)
			{
				current_time = rtc.getDateTime();
			}

			byte command;

			if (client.available() > 0) {

				command = client.read();
			}

			if (command == ECHO) {
				client.write(ECHO_RESPONSE);
			}

			if (command == SETMODE)
			{

				while (!client.available() >= 1);

				if (mode >= 0 && mode <= 2)
				{
					mode = client.read();
#ifdef DEBUG
					if (mode == SETCOLOR_MODE)
						Serial.println("Modo Selecionar Cor!");

					if (mode == TEMPCOLOR_MODE )
						Serial.println("Modo Temperatura!");

					if (mode == RANDOM_MODE)
						Serial.println("Modo Rainbow!");
#endif
				}


			}



			if (command == SETCOLOR) {
    // Comando setColor


    			while (!client.available() >= 4 ); // Espera os 3 paramêtros

			    r = client.read();
			    g = client.read();
			    b = client.read();
			    i = client.read();

#ifdef DEBUG
			    Serial.write("R:");
			    Serial.println(r);
			    Serial.write("G:");
			    Serial.println(g);
			    Serial.write("B:");
			    Serial.println(b);
#endif

			    if (mode == SETCOLOR_MODE)
			    {
			    	led.setColor(r, g, b, i);
			    }
			}

			if (command == SETDATE)
			{
				DateTime date;
				byte d, m , y;

				while (!client.available() >= 3);

				d = client.read();
				m = client.read();
				y = client.read();

				current_time.setDate(d, m, y + 2000);

				if (rtc_present)
				{
					rtc.setDateTime(current_time);
				}

			}

			if (command == SETTIME) {
				DateTime date;
				byte h, m , s;

				while (!client.available() >= 3);

				h = client.read();
				m = client.read();
				s = client.read();

				current_time.setTime(h, m, s);

				if (rtc_present)
				{
					rtc.setDateTime(current_time);
				}


			}

			if (command == REQUESTTEMP)
			{
				client.write(SENDTEMP);

				if (temperatura < 0)
				{
					client.write((byte) 1);
				}
				else
				{
					client.write((byte) 0);
				}

				byte temp_int = (byte) temperatura;
				byte temp_decimal = (temperatura - temp_int) * 100;
#ifdef DEBUG
				Serial.print("Temperature Sent: ");
				Serial.print(temp_int);
				Serial.print(".");
				Serial.println(temp_decimal);
#endif
				client.write(temp_int);
				client.write(temp_decimal);


				


			}

			modesFunction();
			command = 255;
			delay(20);




		}
	}

	modesFunction();

}


void modesFunction() {
	if (mode == TEMPCOLOR_MODE)
	{
		setColorByTemp(temperatura);
		
	}

	if (mode == RANDOM_MODE)
	{

		if (random_state == STATE_MANTAINCOLOR)
		{


			if (new_delay == false)
			{
				Serial.println("Estado Manter Cor Inicio");
				latest_mantain_millis = millis();
				new_delay = true;
			}
			else
			{
				if ( (millis() - latest_mantain_millis) >= MANTAIN_TIME)
				{
					Serial.println("Fim");

					random_state = STATE_CHANGECOLOR;
					Serial.println("Delay 5000");
					new_delay = false;

				}
			}



		}
		else if (random_state == STATE_CHANGECOLOR)
		{

			if (!new_color)
			{
				Serial.println("Nova Cor");


				while (new_r == r && new_g == g && new_b == b )
				{
					getRandomColor(&new_r, &new_g, &new_b);
				}

				new_color = true;

				Serial.print("rgb(");
				Serial.print(new_r);
				Serial.print(",");
				Serial.print(new_g);
				Serial.print(",");
				Serial.print(new_b);
				Serial.println(")");
			}

			if ((millis() - latest_change_millis) >= CHANGE_TIME)
			{
				latest_change_millis = millis();

				if (r < new_r)
					r++;
				if (r > new_r)
					r--;
				if (g < new_g)
					g++;
				if (g > new_g)
					g--;
				if (b < new_b)
					b++;
				if (b > new_b)
					b--;



				led.setColor(r, g, b, 100);

			}


			if (r == new_r && g == new_g && b == new_b)
			{

				Serial.println("Fim nova Cor");

				Serial.print("rgb(");
				Serial.print(r);
				Serial.print(",");
				Serial.print(g);
				Serial.print(",");
				Serial.print(b);
				Serial.println(")");
				new_color = false;
				random_state = STATE_MANTAINCOLOR;
				led.setColor(r, g, b, 100);
			}


		}

	}

}





void setColorByTemp(float temp) {
	if (temp < -10)
	{
		led.setColor(DARKBLUE);
	}

	if (temp >= -10  && temp < 0)
	{
		led.setColor(MEDIUMBLUE);
	}

	if (temp >= 0 && temp < 15)
	{
		led.setColor(LIGHTBLUE);
	}

	if (temp >= 15 && temp < 18)
	{
		led.setColor(LIGHTERBLUE1);
	}

	if (temp >= 18 && temp < 21)
	{
		led.setColor(LIGHTERBLUE2);
	}

	if (temp >= 21 && temp < 23)
	{
		led.setColor(GREEN);
	}

	if (temp >= 23 && temp < 25)
	{
		led.setColor(GREENISH);
	}

	if (temp >= 25 && temp < 35)
	{
		led.setColor(YELLOW);
	}

	if (temp >= 35 && temp < 45)
	{
		led.setColor(ORANGE);
	}

	if (temp >= 45)
	{
		led.setColor(RED);
	}
}


void getRandomColor(byte *r, byte *g, byte *b) {
	int color = random(10);
	switch (color)
	{
		case 0:
			*r = 255;
			*g = 0;
			*b = 0;
			break;
		case 1:
			*r = 0;
			*g = 255;
			*b = 0;
			break;
		case 2:
			*r = 0;
			*g = 0;
			*b = 255;
			break;
		case 3:
			*r = 255;
			*g = 255;
			*b = 0;
			break;
		case 4:
			*r = 255;
			*g = 0;
			*b = 255;
			break;
		case 5:
			*r = 0;
			*g = 255;
			*b = 255;
			break;
		case 6:
			*r = 255;
			*g = 255;
			*b = 255;
			break;
		case 7:
			*r = 255;
			*g = 128;
			*b = 0;
			break;
		case 8:
			*r = 255;
			*g = 0;
			*b = 128;
			break;
		case 9:
			*r = 0;
			*g = 255;
			*b = 128;
			break;
		case 10:
			*r = 0;
			*g = 128;
			*b = 255;
			break;
		default:
			*r = 255;
			*g = 255;
			*b = 255;

	}

}

