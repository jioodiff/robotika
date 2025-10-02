#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ================== Konfigurasi WiFi AP ==================
const char* AP_SSID = "KelompokGila";   // Nama WiFi
const char* AP_PASS = "Kelompok11";       // Password min 8 karakter

ESP8266WebServer server(80);

IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway (192, 168, 4, 1);
IPAddress subnet  (255, 255, 255, 0);

// ================== Pin Motor L298N ==================
int ENA = D1;   // Enable A (motor kiri, PWM)
int IN1 = D2;   // Motor kiri maju
int IN2 = D3;   // Motor kiri mundur

int ENB = D5;   // Enable B (motor kanan, PWM)
int IN3 = D6;   // Motor kanan maju
int IN4 = D7;   // Motor kanan mundur

// ================== Halaman Web ==================
void handleRoot() {
  String html = F(
    "<!doctype html><html><head><meta charset='utf-8'/>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'/>"
    "<title>ESP8266 Robot Sumo</title>"
    "<style>"
    "body{font-family:Arial;text-align:center;background:#f0f0f0;margin:0;padding:20px;}"
    "h1{color:#333;}"
    ".btn{display:inline-block;padding:20px 30px;margin:10px;font-size:20px;font-weight:bold;"
    "color:#fff;background:#007BFF;border:none;border-radius:10px;text-decoration:none;}"
    ".btn:hover{background:#0056b3;}"
    ".grid{display:grid;grid-template-columns:100px 100px 100px;justify-content:center;align-items:center;}"
    ".grid div{margin:5px;}"
    ".stop{background:#dc3545;}"
    "</style></head><body>"
    "<h1>Kontrol Robot Sumo</h1>"
    "<div class='grid'>"
    "  <div></div><div><a class='btn' href='/maju'>&uarr; Maju</a></div><div></div>"
    "  <div><a class='btn' href='/kiri'>&larr; Kiri</a></div>"
    "  <div><a class='btn stop' href='/stop'>&#9632; Stop</a></div>"
    "  <div><a class='btn' href='/kanan'>Kanan &rarr;</a></div>"
    "  <div></div><div><a class='btn' href='/mundur'>&darr; Mundur</a></div><div></div>"
    "</div>"
    "</body></html>"
  );
  server.send(200, "text/html", html);
}

// ================== Setup ==================
void setup() {
  Serial.begin(115200);

  // Setup pin motor
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  berhenti();

  // Setup WiFi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  bool ap_ok = WiFi.softAP(AP_SSID, AP_PASS);

  Serial.println(ap_ok ? "AP started" : "AP failed");
  Serial.print("AP SSID: "); Serial.println(AP_SSID);
  Serial.print("AP IP  : "); Serial.println(WiFi.softAPIP());

  // Routing Web Server
  server.on("/", handleRoot);
  server.on("/maju", [](){ maju(800,800); server.send(200,"text/plain","Maju"); });
  server.on("/mundur", [](){ mundur(800,800); server.send(200,"text/plain","Mundur"); });
  server.on("/kiri", [](){ belokKiri(800,800); server.send(200,"text/plain","Kiri"); });
  server.on("/kanan", [](){ belokKanan(800,800); server.send(200,"text/plain","Kanan"); });
  server.on("/stop", [](){ berhenti(); server.send(200,"text/plain","Stop"); });

  server.begin();
  Serial.println("Web server started");
}

// ================== Loop ==================
void loop() {
  server.handleClient();
}

// ================== Fungsi Gerakan Motor ==================
void maju(int speedL, int speedR) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speedL);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, speedR);
}

void mundur(int speedL, int speedR) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speedL);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, speedR);
}

void belokKiri(int speedL, int speedR) {
  // Motor kiri berhenti
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  // Motor kanan maju
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, speedR);
}

void belokKanan(int speedL, int speedR) {
  // Motor kiri maju
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, speedL);

  // Motor kanan berhenti
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}

void berhenti() {
  // Pastikan semua pin off dan PWM 0
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}
