
#include <Stepper.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

int ledPin_Red = 5;
int ledPin_Green = 4;
int STEPPER_PIN_1 = 6;
int STEPPER_PIN_2 = 7;
int STEPPER_PIN_3 = 3;
int STEPPER_PIN_4 = 2;

bool Locked = false;
bool Unlocking = false;
const int Steps = 64;

Stepper mp(Steps, STEPPER_PIN_1,STEPPER_PIN_3,STEPPER_PIN_2,STEPPER_PIN_4);
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

String Registered_Cards[1] = {"33 3b f1 0d"};

void setup() {
	
  Serial.begin(9600);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
  pinMode(ledPin_Red, OUTPUT);
  pinMode(ledPin_Green, OUTPUT);
  mp.setSpeed(500);

	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  if (!Locked && !Unlocking){
    digitalWrite(ledPin_Green, HIGH);
    digitalWrite(ledPin_Red, LOW);

  }else{
    if (Locked){
      digitalWrite(ledPin_Green, LOW);
      digitalWrite(ledPin_Red, HIGH);
    }
  }

}

void UnlockDoor(){
  for (int i = 0; i<32;i++){
        mp.step(Steps);
    }
}

void LockDoor(){
  for (int i = 0; i<32;i++){
        mp.step(-Steps);
    }
}

void loop() {
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  bool access_granted = false;
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}


  String conteudo = "";

  for (byte i = 0; i < mfrc522.uid.size; i++){
    conteudo.concat(String(mfrc522.uid.uidByte[i] < HEX ? " 0": " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  for (String i : Registered_Cards){
    if (conteudo.substring(1) == i){
      access_granted = true;
      break;
    }
  }

  if (access_granted){
    if (Locked){
      UnlockDoor();
      Locked = false;
      digitalWrite(ledPin_Green, HIGH);
      digitalWrite(ledPin_Red, LOW);
    }else{
      LockDoor();
      Locked = true;
      digitalWrite(ledPin_Green, LOW);
      digitalWrite(ledPin_Red, HIGH);
    }
  }else{
    Serial.println("Acesso Negado");
  }

  

}
