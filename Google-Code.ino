#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN 22
#define SS_PIN 21
#define BUZZER 15
#define GREEN_LED 2
#define RED_LED 4

String names[] = {"Fawzy","Ahmad"};
int size = 2;

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

int blockNum = 2;

byte bufferLen = 18;
byte readBlockData[18];

String card_holder_name;
const String sheet_url = "https://script.google.com/macros/s/AKfycbwOJ0k-DOaenpYN-UgS7wLSvfVDGTE00XNzGw9o9ItXthUyuy5o6s_7XCO9emjjq12dKQ/exec?name=";

#define WIFI_SSID "Fawzy"
#define WIFI_PASSWORD "Yafcr0000000000#"
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
  Wire.begin(16, 17); // SDA, SCL
  lcd.init();
  delay(100);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Welcome!");
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  pinMode(BUZZER, OUTPUT);


  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW); // مفيش كارت

  SPI.begin();
}

void loop() {
  mfrc522.PCD_Init();
  if (!mfrc522.PICC_IsNewCardPresent()) { return; }
  if (!mfrc522.PICC_ReadCardSerial()) { return; }
  Serial.println();
  Serial.println(F("Reading last data from RFID..."));
  ReadDataFromBlock(blockNum, readBlockData);
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  //Print the data read from block
  Serial.println();
  Serial.print(F("Last data in RFID:"));
  Serial.print(blockNum);
  Serial.print(F(" --> "));
  for (int j = 0; j < 16; j++) {
    Serial.write(readBlockData[j]);
  }
  Serial.println();
  // بعد ما تقرأ البلوك
  ReadDataFromBlock(blockNum, readBlockData);  // قراءة البلوك
  String card_name = String((char*)readBlockData); // تحويل البيانات لـ String
  card_name.trim(); // تنظف أي حروف زايدة أو فراغات

  bool found = false;

  for (int i = 0; i < size; i++) {
    if (card_name == names[i]) {
      found = true;
      break;
    }
  }

  if (found) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Access accepted");
    lcd.setCursor(0,1);
    lcd.print(card_name);
    digitalWrite(BUZZER, LOW);
    digitalWrite(GREEN_LED, HIGH);
    delay(200);
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    digitalWrite(GREEN_LED, LOW);
    delay(3000);
    lcd.clear();
  } else {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Access denied");
    lcd.setCursor(0,1);
    digitalWrite(BUZZER, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(200);
    digitalWrite(RED_LED, LOW);
    delay(3000);
    lcd.clear();

  }

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    card_holder_name = sheet_url + String((char*)readBlockData);
    card_holder_name.trim();
    Serial.println(card_holder_name);
    HTTPClient https;
    Serial.print(F("[HTTPS] begin...\n"));

    if (https.begin(client, (String)card_holder_name)) {
      Serial.print(F("[HTTPS] GET...\n"));
      // start connection and send HTTP header
      int httpCode = https.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been sent and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
      delay(1000);
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}


void ReadDataFromBlock(int blockNum, byte readBlockData[]) {
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for Read: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Authentication success");
  }
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Block was read successfully");
  }
}