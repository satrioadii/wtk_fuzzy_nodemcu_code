#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> //BEN ISO AKSES HTTPS

//#define LED_PIN 5 
#define MOTOR 0
#define MOBIL 1
#define LED_HIJAU D0
#define LED_MERAH D1

int mobil, motor;
String hasil;
float fuzzy;

int get_sensor(short int p);
float puzzy(int motor_in, int mobil_in);
float trapezoidLeft(float x, float c, float d);
float trapezoidRight(float x, float a, float b);

//Konfigurasi WiFi
const char *ssid = "Redmi Note 9";
const char *password = "adsfhgjlk";
 
//IP Address Server yang terpasang XAMPP
const char *host = "kopasdri.com";
WiFiClientSecure klien;   //BEN ISO AKSES HTTPS

const int httpPort = 443;

bool Parsing = false;
String dataPHP, dataxx[8];

void setup() {
  Serial.begin(115200);
  
  pinMode(A0, INPUT);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  digitalWrite(D0, HIGH);
  digitalWrite(D1, HIGH);
  
  //pinMode(5, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
 
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  //Jika koneksi berhasil, maka akan muncul address di serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  klien.setInsecure(); //BEN ISO AKSES HTTPS
  //pinMode(LED_PIN, OUTPUT);
  //digitalWrite(LED_PIN, LOW);
}
 
//int value = 0;
 
void loop() {
  // Proses Pengiriman -----------------------------------------------------------
  delay(1000);
  //++value;
 
  // Membaca Sensor Analog -------------------------------------------------------
  mobil = get_sensor(MOBIL);
  motor = get_sensor(MOTOR);
  fuzzy = puzzy(motor, mobil);
  
  Serial.print("connecting to ");
  Serial.println(host);
 
// Mengirimkan ke alamat host dengan port 80 -----------------------------------
  
  if (!klien.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // Isi Konten yang dikirim adalah alamat ip si esp -----------------------------
  String url = "/fuzzy/add_data.php?jarak=";
  url += mobil;
  url += "&kecepatan=";
  url += motor;
  url += "&fuzzy=";
  url += fuzzy;
  url += "&hasil=";
  url += hasil;
  url += "&id=1";
 
  Serial.print("Requesting URL: ");
  Serial.println(url);
 
// Mengirimkan Request ke Server -----------------------------------------------
  klien.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  
  unsigned long timeout = millis();
  while (klien.available() == 0) {
    if (millis() - timeout > 1000) {
      Serial.println(">>> Client Timeout !");
      klien.stop();
      return;
    }
  }
 
// Read all the lines of the reply from server and print them to Serial
  while (klien.available()) {
    //String line = klien.readStringUntil('\r');
    dataPHP = klien.readStringUntil('\n');
    int q = 0;
    Serial.print("Data Masuk : ");
    Serial.print(dataPHP);
    Serial.println();

    dataxx[q] = "";
    for (int i = 0; i < dataPHP.length(); i++) {
        if (dataPHP[i] == '#') {
            q++;
            dataxx[q] = "";
        }
        else {
            dataxx[q] = dataxx[q] + dataPHP[i];
        }
    }
    Serial.println(dataxx[1].toInt());
    //digitalWrite(LED_PIN, dataxx[1].toInt());
    Parsing = false;
    dataPHP = "";
  }
  
  Serial.println();
  Serial.println("closing connection");
}

/*------------------------------------------------------------------------------------------------------------------------------------------------*/

int get_sensor(short int p){
  int jml;
  if(p==0){
    digitalWrite(D4, LOW);
    digitalWrite(D3, LOW);
    jml=50;
  }else{
    digitalWrite(D4, HIGH);
    digitalWrite(D3, LOW);
    jml=30;
  }
  
  return ((float)analogRead(A0))/1024*jml;
}

float trapezoidLeft(float x, float c, float d)
{
  float u;
  if (x > d)
    u = 0;
  else if (c <= x <= d)
    u = (d - x) / (d - c);
  else
    u = 1;
  return u;

}

float trapezoidRight(float x, float a, float b)
{
  float u;
  if (x < a)
    u = 0;
  else if (a <= x <= b)
    u = (x - a) / (b - a);
  else
    u = 1;
  return u;
}

float puzzy(int motor_in, int mobil_in){
  float motorSedikit[50], motorNormal[50], motorBanyak[50];
  float carSedikit[30], carBanyak[30], uCar[2], uMotor[3];
  float Light1[25], Light2[25], LightAll[25];
  float merah[26], hijau[26];
  float merahmax, hijaumax;
  float ruleTable[2][3], data1, data2;

  Serial.print(motor_in);
  Serial.print(" ");
  Serial.println(mobil_in);
  // Motorcycle
  // Motorcycle Sedikit
  for (int i = 0; i <= 10; i++) {
    motorSedikit[i] = 1.0;
  }
  for (int i = 10; i <= 20; i++) {
    motorSedikit[i] = ((20.0 - i) / (20.0 - 10.0));
  }
  for (int i = 20; i <= 50; i++) {
    motorSedikit[i] = 0.0;
  }
  
  //Motorcycle Normal
  for (int i = 0; i <= 15; i++) {
    motorNormal[i] = 0.0;
  }
  for (int i = 16; i <= 25; i++) {
    motorNormal[i] = ((i - 15.0) / (25.0 - 15.0));
  }
  for (int i = 26; i <= 35; i++) {
    motorNormal[i] = ((35.0 - i) / (35.0 - 25.0));
  }

  //Motorcycle Banyak
  for (int i = 0; i <= 30; i++) {
    motorBanyak[i] = 0.0;
  }
  for (int i = 31; i <= 40; i++) {
    motorBanyak[i] = ((i - 30.0) / (40.0 - 30.0));
  }
  for (int i = 41; i <= 50; i++) {
    motorBanyak[i] = 1.0;
  }

  //Car
  //Car Sedikit
  for (int i = 0; i <= 10; i++) {
    carSedikit[i] = 1.0;
  }
  for (int i = 10; i <= 20; i++) {
    carSedikit[i] = ((20.0 - i) / (20.0 - 10.0));
  }
  for (int i = 20; i <= 30; i++) {
    carSedikit[i] = 0.0;
  }
  
  //Car Banyak
  for (int i = 0; i <= 10; i++) {
    carBanyak[i] = 0.0;
  }
  for (int i = 11; i <= 20; i++) {
    carBanyak[i] = ((i - 10.0) / (20.0 - 10.0));
  }
  for (int i = 21; i <= 30; i++) {
    carBanyak[i] = 1.0;
  }

  // Light
  // Light Merah
  for (int i = 0; i <= 5; i++) {
    merah[i] = 1.0;
  }
  for (int i = 6; i <= 15; i++) {
    merah[i] = ((15.0 - i) / (15.0 - 5.0));
  }
  for (int i = 15; i <= 25; i++) {
    merah[i] = 0.0;
  }

  // Light Hijau
  for (int i = 0; i <= 5; i++) {
    hijau[i] = 0.0;
  }
  for (int i = 6; i <= 15; i++) {
    hijau[i] = ((i - 5.0) / (15.0 - 5.0));
  }
  for (int i = 16; i <= 25; i++) {
    hijau[i] = 1.0;
  }

  //Fuzzyfikasi
  uMotor[0] = motorSedikit[motor_in];
  uMotor[1] = motorNormal[motor_in];
  uMotor[2] = motorBanyak[motor_in];
  uCar[0] = carSedikit[mobil_in];
  uCar[1] = carBanyak[mobil_in];

  //impliklasi
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++)
      ruleTable[i][j] = min(uMotor[j], uCar[i]);

  merahmax = ruleTable[0][0];
  merahmax = max(merahmax, ruleTable[0][1]);
  
  hijaumax = ruleTable[0][2];
  hijaumax = max(hijaumax, ruleTable[1][0]);
  hijaumax = max(hijaumax, ruleTable[1][1]);
  hijaumax = max(hijaumax, ruleTable[1][2]);

  for (int i = 0; i <= 25; i++){
    Light1[i] = min(merah[i], merahmax);
    Light2[i] = min(hijau[i], hijaumax);
    LightAll[i] = max(Light1[i], Light2[i]);
  }

  float out = 0, out1 = 0, out2 = 0;
  for (int i = 0; i < 25; i++)
  {
    out1 += LightAll[i] * i;
    out2 += LightAll[i];
  }

  out = out1 / out2;

  data1 = trapezoidLeft(out, 5.0, 15.0);
  data2 = trapezoidRight(out, 15.0, 25.0);

  if (data1 > data2){
    digitalWrite(LED_MERAH, LOW);
    digitalWrite(LED_HIJAU, HIGH);
    hasil = "Berhenti";
  }
  else{
    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(LED_HIJAU, LOW);
    hasil = "Jalan";
  }

  return out;
}
