/*
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS      SDA(SS)      10            53        D10        10               10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

#include <SPI.h>
//It is important that you download the below header file or any other header file your respective reader
#include <MFRC522.h>

#include <math.h>

/*
  we had some really strong, expensive servo motors lying around, so we used them.
  However, a linear actuator would be a lot more practical.
  This code can be easily modified for linear actuators
*/
#include <Servo.h>

int pos = 0;
Servo myservo;

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

/*
chk_lst is the array where we stored the access levels and the following
RFID card numbers
*/
const char chk_lst[][] = {

}


void setup() {
  Serial.begin(9600);                                           //
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  Serial.println(F("Read personal data on a MIFARE PICC:"));    //shows in serial that it is ready to read
  myservo.attach(3);
  myservo.write(0);
}

void loop() {

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;


  // Reset the loop if no new card present on the reader.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("  Card Detected:  "));

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

  Serial.print(F("Name: "));

  byte buffer1[18];

  block = 4;
  len = 18;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT FIRST NAME
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      Serial.write(buffer1[i]);
    }
  }
  Serial.print(" ");


  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  for (int i = 0; i < 13; i++) {
    Serial.write(buffer2[i]);
  }

  int i = 0;
  int tmp;
  // replace 'n1' according to your array
  for (int chk_cnt = 0; chk_cnt <= n1; chk_cnt++){

// replace 'n2' according to your array
    for (i = 0; i < n2; i++) {

      if (buffer2[i+1] == chk_lst[chk_cnt][i]) {

      }
      else
        break;
      tmp = i;
    }
    if (tmp==11){
      Serial.println("\nAccess Granted");
      myservo.write(139);
    delay(30000);
      break;
    }

  }
  if(tmp<11)
  Serial.println("\nAccess Denied");


myservo.write(0);
mfrc522.PICC_HaltA();
mfrc522.PCD_StopCrypto1();
}
