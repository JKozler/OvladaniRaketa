#include <Servo.h>

Servo myServo;

const int loadPin = 2;
const int firePin = 3;
const int stopPin = 21;

const bool onLOW = LOW;
const bool onHIGH = HIGH;

const int fuelPin = 10;
const int loxPin = 11;
const int ignitionPin = 12;
const int emergencyLightPin = 9;
const int ableToStopLightPin = 8;
const int infoToData = 6;

int load;                     //nazvy tlacitek
int fire;
int emergencyStop;

int testLength;               //zadavane promenne
int fuelCalibration;
int loxCalibration;

int fuelDelay;                //vypocitane delays
int loxDelay;

boolean lockLoadButton = false;   //omezeni tlacitek
boolean lockFireButton = true;

void setup() {

  pinMode(loadPin, INPUT);
  pinMode(firePin, INPUT);
  pinMode(stopPin, INPUT);
  
  pinMode(fuelPin, OUTPUT);
  pinMode(loxPin, OUTPUT);
  pinMode(7, OUTPUT);
  myServo.attach(7);
  
  pinMode(ignitionPin, OUTPUT);
  pinMode(emergencyLightPin, OUTPUT);
  pinMode(ableToStopLightPin, OUTPUT);
  pinMode(infoToData, OUTPUT);

  attachInterrupt(2, emergency, RISING);

  Serial.begin(9600);

  myServo.write(0);
  digitalWrite(ignitionPin, onLOW);
  digitalWrite(fuelPin, onLOW);

  
  Serial.println("Kalibrace serva... zkontrolujte uzavreni ventilu.");
  Serial.println("Vitejte v systemu. Pro nacteni hodnot stisknete LOAD.");


}

void loading() {
    digitalWrite(emergencyLightPin, LOW);
  Serial.println(" ");

  lockLoadButton = true;
  lockFireButton = true;
  Serial.println("TLACITKA UZAMCENA.");
  Serial.println("Zadejte delku testu v ms");

    while (Serial.available() == 0) {

    }

        testLength = Serial.parseInt();
        Serial.print("Delka nastavena na ");
        Serial.println(testLength);
        delay(100);
        
  Serial.println("Zadejte kalibraci paliva v ms (O kolik ms pred T=0 dojde k otevreni ventilu)");

    while (Serial.available() == 0) {

    }

        fuelCalibration = Serial.parseInt();
        Serial.print("Kalibrace paliva nastavena na ");
        Serial.println(fuelCalibration);
        delay(100);

  Serial.println("Zadejte kalibraci LOX v ms (O kolik ms pred T=0 dojde k otevreni ventilu)");

    while (Serial.available() == 0) {

    }

        loxCalibration = Serial.parseInt();
        Serial.print("Kalibrace paliva nastavena na ");
        Serial.println(loxCalibration);
        delay(100);

  Serial.println("_____________________________________________");
  Serial.print("Stisknete FIRE pro odpocet k testu o delce ");
  Serial.print(testLength);
  Serial.print(" ms s kalibraci FUEL ");
  Serial.print(fuelCalibration);
  Serial.print(" ms a kalibraci LOX ");
  Serial.print(loxCalibration);
  Serial.println(" ms");
  Serial.println("Pro nove zadani stisknete tlacitko LOAD.");

  lockFireButton = false;
  lockLoadButton = false;
  Serial.println("TLACITKA ODEMCENA.");

}



void fireSequence() {
  
  lockLoadButton = true;
  lockFireButton = true;
  
  Serial.println("TLACITKA UZAMCENA. ZACINA ODPOCET!");
  delay(3000);

  fuelDelay = 10000 - fuelCalibration;
  loxDelay = 10000 - loxCalibration;
  
  Serial.println("PROBIHA ODPOCET!");

  digitalWrite(ableToStopLightPin, HIGH);
  digitalWrite(infoToData, HIGH);             // signalizace odpoctu pro monitoring modul
  
  if (fuelDelay < loxDelay) {
    delay(fuelDelay);
    digitalWrite(fuelPin, onHIGH);
    delay(loxDelay-fuelDelay);
    myServo.write(90);
    digitalWrite(loxPin, HIGH);
    delay(10000-loxDelay);
    digitalWrite(ignitionPin, onHIGH);
  }
  
  if (fuelDelay > loxDelay) {
    delay(loxDelay);
    myServo.write(90);
    digitalWrite(loxPin, HIGH);
    delay(fuelDelay-loxDelay);
    digitalWrite(fuelPin, onHIGH);
    delay(10000-fuelDelay);
    digitalWrite(ignitionPin, onHIGH);
  }
  
  if (fuelDelay == loxDelay) {
    delay(fuelDelay);
    digitalWrite(fuelPin, onHIGH);
    myServo.write(90);
    digitalWrite(loxPin, HIGH);
    delay(10000-fuelDelay);
    digitalWrite(ignitionPin, onHIGH);
  }

    delay(testLength);
    digitalWrite(ignitionPin, onLOW);
        digitalWrite(fuelPin, onLOW);
            myServo.write(0);
            digitalWrite(loxPin, LOW);   

              digitalWrite(ableToStopLightPin, LOW);
                digitalWrite(infoToData, LOW);


    Serial.println("TEST UKONCEN. STISKNI LOAD PRO ZADANI NOVEHO TESTU.");

    lockLoadButton = false;
  }
    
void emergency() {
      digitalWrite(ignitionPin, onLOW);
        digitalWrite(fuelPin, onLOW);
                    myServo.write(0);
            digitalWrite(loxPin, LOW);
            
            stopstate();
}

void stopstate() {
  digitalWrite(emergencyLightPin, HIGH);
  Serial.println("STISKNUTO NOUZOVE TLACITKO!");
  Serial.println("RESTARTUJTE JEDNOTKU!");
  lockLoadButton = true;
  lockFireButton = true;
}


void loop() {
  load = digitalRead(loadPin);

  if (load == HIGH & lockLoadButton == false) {
    loading();
  }

    if (load == HIGH & lockLoadButton == true) {
    Serial.println("LOAD button locked");
  }

  fire = digitalRead(firePin);

  if (fire == HIGH & lockFireButton == false) {
    fireSequence();
  }

    if (fire == HIGH & lockFireButton == true) {
    Serial.println("FIRE button locked");
  }
  
}
