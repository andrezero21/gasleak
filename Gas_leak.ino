#include <SoftwareSerial.h>
#include "SevSeg.h"
#include <EEPROM.h>
/** the current address in the EEPROM (i.e. which byte we're going to write to next) **/
///Inicio Variables EEPROM///
int addr = 0;
byte value;
String prueba="Prueba de Verdad";
String mostrar;
///FIN Variables EEPROM///
int valor_7seg=0;
int k1=0;
int  t1=0;
int ti2=0; //identificador timer 2
int  ti3=0; //identificador timer 3
int flag_tb1=0; //Flag Timer 1
int  flag_tb2=0; // Flag Timer 2
int flag_c=0;
int flag_b1=0;
int flag_conf=0;
/////FIN Constantes y Flags para entrar en modo configuración/////
/////Constantes y Flags para lectura de dos teclas/////
const unsigned long period = 50;
unsigned long prevMillis = 0;
int boton =0;
int pulsado=0;
int numero=0;
String numero_telefono;
String numero_telefono_temp;
int concat=0;
int k4=0;
String inicio="AT+CMGS=\"+593";
//////////////INICIO SIM900/////////////////////////
//Create software serial object to communicate with SIM900
SoftwareSerial mySerial(7, 8); //SIM900 Tx & Rx is connected to Arduino #7 & #8
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
int k3=0;
int flag_GSM = 0;
boolean newData = false;
int k2=0; //Contador de mensajes recibidos
int flag_2=0; //Número de Mensaje Con Información GSM Recibido.
//////////////FIN SIM900/////////////////////////
//////////////////////////////////////////////////////////////
SevSeg sevseg; // 7 segmentos
int sietesegevent; // Evento de 7 Segmentos. Debe durar 5 minutos
int gasValue = A0; // smoke / gas sensor connected with analog pin A1 of the arduino / mega.
int data = 0;
int data_max=400;
int flag_1 = 0;
int ti1 =0; //indentificador timer de flag de alarma cada 5 minutos.
int ti4 =0; //identificador timer de flag de comprobación de GSM
/*This is the code to show the alphabetical characters on a 7 segment LED display*/
/* INICIO LETRAS*/
/////FIN Constantes y Flags para lectura de dos teclas/////
///INICIO LETRAS Y NÚMEROS////
const byte alphabet[15] = {
		B1111110, //0 0
		B0110000, //1 1
		B1101101, //2 2
		B1111001, //3 3
		B0110011, //4 4
		B1011011, //5 5
		B1011111, //6 6
		B1110000, //7 7
		B1111111, //8 8
		B1111011, //9 9
		B1011111, //G 10
		B1110111, //A 11
		B1011011, //S 12
		B1001111, //E 13
		B0000000, // OFF 14 shows nothing
};

//pins for each segment (a-g) on the 7 segment LED display with the corresponding arduino connection
const int segmentPins[7] = {10, 9, 4, 13, 12, 5, 6 }; //{ 11, 10, 9, 4, 3, 2, 5, 6 };
/* FIN LETRAS*/

void stoptimer()
{
	TCCR1B &= ~(1<< CS12);
	TCCR1B &= ~(1<< CS11);
	TCCR1B &= ~(1<< CS10);
}
void recvWithEndMarker() {
	static byte ndx = 0;
	char endMarker = '\n';
	char rc;

	while (mySerial.available() > 0 && newData == false) {
		rc = mySerial.read();

		if (rc != endMarker) {
			receivedChars[ndx] = rc;
			ndx++;
			if (ndx >= numChars) {
				ndx = numChars - 1;
			}
		}
		else {

			receivedChars[ndx -1] = '\0'; // terminate the string
			k3=ndx;
			newData = true;
			ndx=0;
		}

	}
}
void showNewData() {

	if(strcmp(receivedChars, "AT+CSQ") == 0)
	{
		k2=0;
	}
	if (newData == true) {
		Serial.print("K2:");
		Serial.println(receivedChars);
		k2++;
		newData = false;
		if(k2==3)
		{
			Serial.print("Dato a Usar:");
			Serial.println(receivedChars);
			flag_2=flag_2+1;
			Serial.print("Flag:");
			Serial.println(flag_2);
			k2=0;
			switch (flag_2) {
			case 1:
				Serial.print("Entro 1:");
				Serial.print("receivedChars=[");
				Serial.print(receivedChars);
				Serial.print("]  length=");
				Serial.println(strlen(receivedChars));

				if(strcmp(receivedChars, "OK") == 0)
				{
					Serial.println("FUNCO");
					stoptimer();
					showDigit(11); // Muestra A
					delay(1000);
					showDigit(14); // Apaga 7 segementos
				}
				break;
			case 2:
				Serial.print("Entro 2:");
				Serial.print("receivedChars=[");
				Serial.print(receivedChars);
				Serial.print("]  length=");
				Serial.println(strlen(receivedChars));
				if(strcmp(receivedChars, "ERROR") == 0)
				{
					Serial.println("ERROR CHIP");
					showDigit(13);
					delay(1000);
					showDigit(14);

				}
				else
				{
					Serial.println("CHIP CONECTADO");
					showDigit(11);
					delay(1000);
					showDigit(14);
				}
				break;

			case 3:
				Serial.print("Entro 3:");
				char coma = ',';

				Serial.print("receivedChars=[");
				Serial.print(receivedChars);
				Serial.print("]  length=");
				Serial.println(strlen(receivedChars));
				char temporal[2];

				temporal[0]=receivedChars[6];
				if(receivedChars[7]!=coma)
				{temporal[1]=receivedChars[7];}
				int int_output;
				int_output = atoi(temporal);
				Serial.print(int_output);
				if(int_output < 10)
				{
					Serial.println("Señal Mala");
					showDigit(1); // Muestra 1
					delay(3000);
					showDigit(14); // Muestra 0
					flag_2=0;
					flag_GSM=1;

				}
				else
				{
					if(int_output > 10 && int_output < 20)
					{
						Serial.println("Señal Media");
						showDigit(2); //Muestra 2
						delay(3000);
						showDigit(14); // Muestra 0
						flag_2=0;
						flag_GSM=1;
					}
					else
					{
						Serial.println("Señal Excelente");
						showDigit(3); //Muestra 3
						delay(3000);
						showDigit(14); // Muestra 0
						flag_2=0;
						flag_GSM=1;

					}

				}
				break;

			}

		}
	}



}

void mostrar_numero()
{
	Serial.println("Mostrar numero");
	boton=0;
	numero++;
	if (numero<10)
	{
		showDigit(numero);
	}
	else
	{
		numero=0;
		showDigit(numero);
	}
}

void grabar_numero()
{
	boton=0;
	Serial.println("Grabar Numero");
	if(concat<=8)
	{
		showDigit(11);
		delay(500);
		showDigit(numero);
		Serial.println("Concat:");
		Serial.println(concat);
		concat++;
		numero_telefono_temp=numero_telefono_temp + numero;
		Serial.println(numero_telefono_temp);

		if(concat==9)

		{
			for(int i=0; i<=8;i++)
					{
						showDigit(numero_telefono_temp.charAt(i) - 48);
						delay(1000);
						EEPROM.write(addr+i,(byte)numero_telefono_temp.charAt(i));
						delay(100);
					}
					numero_telefono_temp=inicio + numero_telefono_temp;
					numero_telefono_temp=numero_telefono_temp + "\"" + "\r";
					numero_telefono=numero_telefono_temp;
					Serial.println(numero_telefono_temp);
					flag_conf=0;
					attachInterrupt(digitalPinToInterrupt(2), velocidadMenos, RISING);
					attachInterrupt(digitalPinToInterrupt(3), velocidadMas, RISING);
					concat=0;
					flag_1 = 0;
				}

		}
		}
bool readKeypad()
{
	//Serial.println("Leer Boton");
	bool rst = false;
	// Barrido de columnas
	for (byte c = 2; c <= 3; c++)
	{
		if (digitalRead(c) == HIGH)
		{
			pulsado=1;
			k4=c;
		}
		if (digitalRead(k4) == LOW && pulsado==1)
		{
			pulsado=0;
			boton = k4;
			rst = true;
		}
	}

	return rst;
}

void setup()
{
	mySerial.begin(19200); // Setting the baud rate of GSM Module
	Serial.begin(19200); // Setting the baud rate of Serial Monitor (Arduino)
	///INICIO ENCENDER SIM900///
	pinMode(11, OUTPUT);
	pinMode(A1, OUTPUT);
	digitalWrite(A1, HIGH);
	delay(1000);
	digitalWrite(A1, LOW);
	delay(5000);
	Serial.println("SIM900 ENCENDIDO");

	///FIN ENCENDER SIM900///

	///INICIO LECTURA EEPROM///
	for(int i=0;i<=8;i++)
	{
		value=EEPROM.read(addr+i);
		Serial.println("Mostrar:");
		Serial.println((char)value);
		mostrar += (char)(value);
		delay(100);
	}
	//mostrar[prueba.length()]='\0';
	Serial.println("Mostrar Total:");
	Serial.println(mostrar);
	numero_telefono=inicio + mostrar;

	numero_telefono=numero_telefono + "\"" + "\r";
	Serial.println(numero_telefono);
	///FIN LECTURA EEPROM///

	/* INICIO LETRAS*/

	for (int i = 0; i < 7; i++)
	{
		pinMode(segmentPins[i], OUTPUT);
	}

	/* FIN LETRAS*/

	// Configuramos los pines de interrupciones para que
	// detecten un cambio de bajo a alto
	attachInterrupt(digitalPinToInterrupt(2), velocidadMenos, RISING);
	attachInterrupt(digitalPinToInterrupt(3), velocidadMas, RISING);
	pinMode(2, INPUT);
	pinMode(3, INPUT);
	/* INICIO LECTURA GAS*/
	randomSeed(analogRead(0));
	pinMode(gasValue, INPUT);
	/* FIN LECTURA GAS*/
	///////////////INICIO CONFIGURACION TIMER 1 //////////////
	TCCR1A = 0x00;  //TC1 in normal counting mode
	TCCR1B = 0x00;   //TC1 is OFF
	TCNT1 = 49755;  //pre-set value for 1-sec time delay
	TIMSK1 = 0x01;   //TC1's interrupt logic is enabled
	interrupts();
	stoptimer();
	///////////////FIN CONFIGURACION TIMER 1 //////////////
	///////////////////////////////////////////////////////////////////
	//////////////INICIO DE LECTURA DE ESTADO GSM/////////////////////
	delay(15000);//180000 son 3 minutos para que se enganche modulo gsm
	Serial.println("Initializing...");
	delay(500);
	mySerial.println("AT"); //Handshaking with SIM900
	delay(300);
	mySerial.println("AT+CCID");
	delay(300);
	mySerial.println("AT+CSQ");
	TCCR1B = 0x05;
	ti4=1;
	while (flag_GSM == 0 ) {  //Lectura de Estado
		recvWithEndMarker();
		showNewData();
		if(t1==45 && ti4==1)
		{
			flag_GSM =1;
			ti4=0;
			showDigit(13);
			delay(3000);
			showDigit(14);
			stoptimer();

		}
	}
}
void loop()
{
	numero_telefono_temp="";
	////INICIO LECTURA BOTONES///
	if(t1>=1 && ti3==1) //loop interrupción 3
	{
		//Serial.println("Ejecucion de t3");
		flag_tb2=1;
		t1=0;
	}

	///LOOP DE CONFIGURACIÓN///
	while(flag_conf==1)
	{
			if(boton==2)
			{
				delay(300);
				mostrar_numero();
			}
			else
			{
				if(boton==3)
				{
					delay(300);
					grabar_numero();
				}
		}
	}

	if(t1>=15 && ti2==1) //loop interrupción 2
	{
		Serial.println("Ejecucion de t2");
		flag_tb1=1;
		t1=0;
		ti2=0;
		flag_c=0;
		stoptimer();
	}
	////FIN LECTURA BOTONES///
	if(flag_conf==0)
	{
		data = analogRead(gasValue);
		Serial.print("Gas Level: ");
		Serial.println(data);
		Serial.print("t1:");
		Serial.println(t1);

	}

	///Timer para Flag de alarma cada 5 minutos
	if(t1>=300 && ti1==1)
	{
		Serial.print("SMS Activado");
		flag_1 = 0;
		t1=0;
		ti1=0;
		stoptimer();
	}
	///FIN Timer para Flag de alarma cada 5 minutos
	if ( data > data_max)
	{
		if (flag_1 == 0)
		{
			flag_1 = 1;
			SendMessage();
			Serial.println("Gas detect alarm");
			delay(500);
			TCCR1B = 0x05;
			ti1=1;
		}
		//delay(1000); //after LED segment shuts off, there is a 2-second delay
		digitalWrite(11, HIGH);
		Serial.println("Sirena Activada");//Se debe activar SIRENA EN ESTE LUGAR
		for (int i = 10; i <= 12; i++)
				{
					showDigit(i);
					delay(1000);
				}
	}
	else
	{
		valor_7seg= (data-190)/21;
		showDigit(valor_7seg);
		delay(300);
		//Serial.println(valor_7seg);
		Serial.println("Gas Level Low");
		Serial.println("Sirena Desactivada");
		//showDigit(14);
		digitalWrite(11, LOW);
		//delay(500);
	}
}

void SendMessage()
{
	Serial.println("I am in send");
	mySerial.println("AT+CMGF=1"); //Sets the GSM Module in Text Mode
	delay(500); // Delay of 1000 milli seconds or 1 second
	mySerial.println(numero_telefono); // Replace x with mobile number
	delay(500);
	mySerial.println("Excess Gas Detected. Open Windows");// The SMS text you want to send
	delay(100);
	mySerial.println((char)26);// ASCII code of CTRL+Z
	delay(500);
}

void showDigit (int number)
{
	boolean isBitSet;
	for (int segment = 0; segment < 7; segment++)
	{
		isBitSet = bitRead(alphabet[number], segment);
		digitalWrite(segmentPins[segment], isBitSet);
	}
}

void velocidadMenos()
{
	Serial.println("Presiono Botón 1");
	boton=2;
	flag_b1=1;
	flag_tb1=0;
	if(flag_c==0)
	{
		///Se activa el timer
		TCCR1B = 0x05;
		ti2=1;
	}
	else //modo configuracion
	{
		flag_conf=1;

	}
}
void velocidadMas()
{
	boton=3;
	Serial.println("Presiono Botón 2");
	if(flag_b1==1 && flag_tb1==0)

	{
		//Serial.println("C1 activado");
		flag_c=1;
		flag_tb1=1;
	}

}

ISR(TIMER1_OVF_vect) //ISR for Timer1 Overflow
{
	//Serial.println("Interrupcion Timer 1");
	TCNT1 = 49755;
	t1 = t1+1;
}

