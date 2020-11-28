#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <OneButton.h>

#define RXPIN 2
#define TXPIN 3
#define RLED 11
#define BLED 12
#define BUT 7
#define BUZZ 8
#define BUFSIZE 10

bool state = 0; //0->GPS 1->police

int radius = 15;


//baudrate do GPS
#define GPSBAUD 4800

TinyGPSPlus gps;
SoftwareSerial uart_gps(RXPIN, TXPIN);
OneButton button(BUT, false);

bool lastBUT = HIGH;
double myLat=0;
double myLng=0;


void displayInfo();
void police();
void mode();
void signal();

void setup()
{
  Serial.begin(9600);
  uart_gps.begin(GPSBAUD);

  pinMode(RLED, OUTPUT);
  pinMode(BLED, OUTPUT);
  pinMode(BUT, INPUT);
  pinMode(BUZZ, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUT), mode, FALLING);

  digitalWrite(RLED, LOW);
  digitalWrite(BLED, LOW);
  digitalWrite(BUZZ, LOW);

  button.attachClick(mode);

  Serial.println(F("Obtendo um lock..."));
  Serial.print(F("TinyGPS++ versao. "));
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();
}

void loop() {


signal();

	if(state==1){
		police();
	}

button.tick();
}

void mode(){

	state=!state;
	if(state==1)
		Serial.println("Modo de controlo ativado...\n\r");
	else
		Serial.println("Modo de controlo desativado...\n\r");
}

void police(){

  digitalWrite(RLED, HIGH);
  delay(500);
  digitalWrite(RLED, LOW);
  digitalWrite(BLED, HIGH);
  delay(500);
  digitalWrite(BLED, LOW);


  if(myLat == 0)
	  myLat=gps.location.lat();
  if(myLng == 0)
	  myLng=gps.location.lng();


  while (uart_gps.available()){ //Listener

	if (gps.encode(uart_gps.read())){ //Lê e verifica a trama

		displayInfo();

	  if(gps.distanceBetween(myLat, myLng, gps.location.lat(), gps.location.lng()) > radius){

		  if (gps.location.isValid())  {

			  digitalWrite(BUZZ, HIGH);
			  delay(100);
			  digitalWrite(BUZZ, LOW);
			  delay(50);
		  }
	  }
	  break;
	}
  }
}

void signal(){

	while (uart_gps.available() && state == 0){ //Listener

		if (gps.encode(uart_gps.read())){ //Lê e verifica a trama
		  displayInfo();
		  break;
		}
	}
}

void displayInfo()
{

  Serial.print(F("Localizacao: "));  //Localização
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALIDO"));
  }
  Serial.println();
  Serial.print(F("Data/Tempo: "));
  if (gps.date.isValid())  //Data
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALIDO"));
  }

  Serial.print(F(" "));
  //Por causa do parsing é necessario voltar a adicionar o '0' caso o nr seja < 10
  //de modo a obter a motação hh:mm:ss
  if (gps.time.isValid())  //Tempo
  {
    if (gps.time.hour() < 10)
    	Serial.print(F("0")); //Coloca o '0'
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10)
    	Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10)
    	Serial.print(F("0"));
    Serial.print(gps.time.second());
    /*
    Serial.print(F("."));
    if (gps.time.centisecond() < 10)
    	Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
    */
  }
  else
  {
    Serial.print(F("INVALIDO"));
  }

  Serial.println();
  if (gps.satellites.isValid()) //Satelites
    {
	  Serial.print(F("Satelites ligados: "));
	  Serial.print(gps.satellites.value());
    }
  else
    {
  Serial.print(F("INVALIDO"));
    }
  Serial.println();
  if (gps.speed.isValid()) //Speed
	{
	  Serial.print(F("Velocidade (+/-0.36kmph) : "));
	  Serial.print(gps.speed.kmph());
	  Serial.print(F("kmph"));
	}
  else
   {
     Serial.print(F("INVALIDO"));
   }
  Serial.println();
  if (gps.hdop.isValid()) //Speed
	{
	  Serial.print(F("Qualidade do sinal(HDOP): "));
	  Serial.print(gps.hdop.value());
	}
  else
   {
     Serial.print(F("INVALIDO"));
   }

  Serial.println();
  Serial.println();
}
