#include <WiFi.h>

#include <HTTPClient.h>

#include <Bonezegei_DHT11.h>

#include <EEPROM.h>
int SERIAL_NUMBER=1234;
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP 10
#define EEPROM_SIZE 128
Bonezegei_DHT11 dht(18);
RTC_DATA_ATTR int bootCount = 0;
String URL = "http://192.168.0.102/szklarnia_1_project/szklarnia_1_test.php";
//String URL = "http://localhost/szklarnia_1_project/szklarnia_1_test.php";
String ssid_def = "janpawel3laczylmalesieci";
String password_def = "kuktas123";
#define MAX_STRING_LENGTH 31

void connectWifi(String log, String pass) {
  WiFi.mode(WIFI_OFF);
  delay(10);
  WiFi.mode(WIFI_STA);
  Serial.println("Próba łączenia do sieci z eeprom");
  WiFi.begin(log, pass);

  Serial.println("Connecting to Wifi");
  int licznik = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    licznik++;
    if (licznik > 10) {
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to: ");
    Serial.println(log);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return;
  } else {
    Serial.println("Próba łączenia do sieci default");
    WiFi.begin(ssid_def, password_def);
    licznik=0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
         licznik++;
    if (licznik > 10) {
      break;
    }
    }
    Serial.print("Connected to: ");
    Serial.println(ssid_def);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

}

void IRAM_ATTR buttonpressed() {
  bootCount = -1;
}

void saveStringToEEPROM(String ss, String pp) {
  for (int i = 0; i < MAX_STRING_LENGTH; i++) {
    char ch = ss.charAt(i);
    if (ch == '\0') {
      for (int j = i; j < MAX_STRING_LENGTH; j++) {
        EEPROM.write(j, '\0');
      }
      break;
    }
    EEPROM.write(i, ch);
  }

  for (int i = 0; i < MAX_STRING_LENGTH; i++) {
    char ch = pp.charAt(i);
    if (ch == '\0') {
      for (int j = i; j < MAX_STRING_LENGTH; j++) {
        EEPROM.write(j + MAX_STRING_LENGTH, '\0');
      }
      break;
    }
    EEPROM.write(MAX_STRING_LENGTH + i, ch);
  }

  EEPROM.write(2 * MAX_STRING_LENGTH, '\0'); // Dodajemy znak specjalny na końcu
  EEPROM.commit();
}

String readStringFromEEPROM(int x) {
  String result = "";
  char ch;
  int startAddress = x * (MAX_STRING_LENGTH); // Każdy napis ma MAX_STRING_LENGTH znaków plus znak końca

  for (int i = 0; i < MAX_STRING_LENGTH; i++) {
    ch = EEPROM.read(startAddress + i);
    if (ch == '\0') {
      break;
    }
    result += ch;
  }

  return result;
}

void setup() {
   
  delay(1000);
  EEPROM.begin(EEPROM_SIZE);

  bootCount++;
  Serial.println("bootCount: "+bootCount);
  pinMode(33, INPUT_PULLUP);
  pinMode(27, OUTPUT);
  attachInterrupt(33, buttonpressed, FALLING);
 esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  digitalWrite(27, HIGH);
  Serial.begin(9600);

  
  if (bootCount <= 0) {
    String ssid,password;
    Serial.println("Podaj login:");
    Serial.setTimeout(10000);
    ssid = Serial.readString();
    Serial.println("Podaj haslo:");
    password = Serial.readString();
    saveStringToEEPROM(ssid, password);
    Serial.println(ssid);
    Serial.println(password);
  }

  Serial.println("Odczytany EEPROM\n");
  Serial.println(readStringFromEEPROM(0));
  Serial.println(readStringFromEEPROM(1));


  dht.begin();

  if (WiFi.status() != WL_CONNECTED) {
    connectWifi(readStringFromEEPROM(0), readStringFromEEPROM(1));
  }

  if (dht.getData()) {
    float temperature = dht.getTemperature();
    int hum = dht.getHumidity();
  
  ///Sprawdzenie wartości
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");

    Serial.print("Humidity:");
    Serial.print(hum);
    Serial.println("%");
 
 
    String postData = "temperature=" + String(temperature) + "&humidity=" 
    + String(hum)+"&serial_number="+String(SERIAL_NUMBER)+"&insolation="+String(temperature*hum);
    HTTPClient http;
    http.begin(URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(postData);
    String payload = http.getString();
    ////////////////////////////////
    Serial.print("URL : ");
    Serial.println(URL);
    Serial.print("Data: ");
    Serial.println(postData);
    Serial.print("httpCode: ");
    Serial.println(httpCode);
    Serial.print("payload : ");
    Serial.println(payload);
    Serial.println("--------------------------------------------------");
    delay(20);
  }

  delay(3000);
  Serial.println("Lets s sleep");
  digitalWrite(27, LOW);
  esp_deep_sleep_start();
}

void loop() {}