/***********************************************************
 Smart Charger Safety Monitoring
 ESP32 + DHT22 + LCD + Relay + RGB + Buzzer + Blynk
************************************************************/

#define BLYNK_TEMPLATE_ID "TMPL6HKC_IQ8Z"
#define BLYNK_TEMPLATE_NAME "Smart Charger Safety Monitoring"
#define BLYNK_AUTH_TOKEN "00nD57TJe6Qof_nmteqh_rG4DEetY4GS"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <DHT.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define DHTPIN 4
#define DHTTYPE DHT22

#define RELAY_PIN 23
#define BUZZER_PIN 19

#define RED_PIN 18
#define GREEN_PIN 5
#define BLUE_PIN 17

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);

BlynkTimer timer;

float suhu;

String statusAlat;

bool notifSent=false;

void bacaSensor();
void tampilLCD();
void kirimBlynk();

void setup()
{

Serial.begin(115200);

pinMode(RELAY_PIN,OUTPUT);

pinMode(BUZZER_PIN,OUTPUT);

pinMode(RED_PIN,OUTPUT);
pinMode(GREEN_PIN,OUTPUT);
pinMode(BLUE_PIN,OUTPUT);

digitalWrite(RELAY_PIN,HIGH);

digitalWrite(RED_PIN,LOW);
digitalWrite(GREEN_PIN,LOW);
digitalWrite(BLUE_PIN,LOW);

noTone(BUZZER_PIN);

lcd.init();
lcd.backlight();

lcd.setCursor(0,0);
lcd.print("SMART CHARGER");

lcd.setCursor(0,1);
lcd.print("Connecting...");

dht.begin();

Blynk.begin(BLYNK_AUTH_TOKEN,ssid,pass);

lcd.clear();

timer.setInterval(1000L,bacaSensor);

}

void loop()
{

Blynk.run();

timer.run();

}
void bacaSensor()
{

  suhu = dht.readTemperature();

  if (isnan(suhu))
  {
    Serial.println("Sensor Error");
    return;
  }

  Serial.print("Suhu : ");
  Serial.println(suhu);

  //==============================
  // KONDISI AMAN
  //==============================

  if (suhu < 40)
  {

    statusAlat = "AMAN";

    digitalWrite(RELAY_PIN, HIGH);

    noTone(BUZZER_PIN);

    // RGB HIJAU
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);

    notifSent = false;

  }

  //==============================
  // KONDISI WARNING
  //==============================

  else if (suhu >= 40 && suhu < 45)
  {

    statusAlat = "WARNING";

    digitalWrite(RELAY_PIN, HIGH);

    // RGB KUNING
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);

    tone(BUZZER_PIN, 1200, 200);

  }

  //==============================
  // KONDISI OVERHEAT
  //==============================

  else
  {

    statusAlat = "OVERHEAT";

    digitalWrite(RELAY_PIN, LOW);

    // RGB MERAH
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);

    tone(BUZZER_PIN, 2000);

    if (!notifSent)
    {

      Blynk.logEvent("overheat");

      notifSent = true;

    }

  }

  tampilLCD();

  kirimBlynk();

}

void tampilLCD()
{

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.print(suhu,1);
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(0,1);

  if(statusAlat=="AMAN")
  {
      lcd.print("Status:AMAN");
  }

  else if(statusAlat=="WARNING")
  {
      lcd.print("Status:WARN");
  }

  else
  {
      lcd.print("Status:HOT!");
  }

}

void kirimBlynk()
{

  Blynk.virtualWrite(V0, suhu);

  Blynk.virtualWrite(V1, statusAlat);

  if(digitalRead(RELAY_PIN))
  {
    Blynk.virtualWrite(V2,1);
  }
  else
  {
    Blynk.virtualWrite(V2,0);
  }

}