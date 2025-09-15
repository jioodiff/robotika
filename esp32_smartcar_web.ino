#include <WiFi.h>
#include <WebServer.h>

/* ======================== WiFi Mode ===========================
   Default: ESP32 menjadi Access Point (lebih simpel)
   SSID: ESP32-Car   Password: 12345678  → buka http://192.168.4.1
   Jika ingin mode Station (terhubung ke router), set USE_AP=false
   dan isi SSID/PASS di bawah.
================================================================*/
#define USE_AP true
const char* AP_SSID = "ESP32-Car";
const char* AP_PASS = "12345678";

const char* STA_SSID = "YourWiFi";
const char* STA_PASS = "YourPassword";

/* ======================== Motor Pins (L298N) ================== */
const int ENA = 25;  // PWM channel A
const int IN1 = 26;
const int IN2 = 27;

const int ENB = 33;  // PWM channel B
const int IN3 = 32;
const int IN4 = 14;

/* Balik arah motor bila terpasang terbalik tanpa bongkar kabel */
const bool LEFT_REVERSED  = false;  // motor kiri (IN1/IN2/ENA)
const bool RIGHT_REVERSED = false;  // motor kanan (IN3/IN4/ENB)

/* ======================== PWM config (ESP32 LEDC) ============= */
const int PWM_FREQ = 15000;     // Hz
const int PWM_RES  = 8;         // 8-bit (0..255)
const int CH_A     = 0;         // channel for ENA
const int CH_B     = 1;         // channel for ENB

/* ======================== Server ============================== */
WebServer server(80);

/* ======================== State =============================== */
int speedVal = 200;   // 0..255 default
String lastCmd = "stop";

/* ======================== Helpers ============================= */
void setLeft(int dir, int pwm) {
  // dir: 1=fwd, -1=back, 0=stop
  if (LEFT_REVERSED) dir = -dir;
  if (dir > 0) {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  } else if (dir < 0) {
    digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  } else {
    digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
  }
  ledcWrite(CH_A, dir == 0 ? 0 : pwm);
}

void setRight(int dir, int pwm) {
  if (RIGHT_REVERSED) dir = -dir;
  if (dir > 0) {
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  } else if (dir < 0) {
    digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
  }
  ledcWrite(CH_B, dir == 0 ? 0 : pwm);
}

void goStop()    { setLeft(0,0);          setRight(0,0);          lastCmd="stop"; }
void goForward() { setLeft( 1,speedVal);  setRight( 1,speedVal);  lastCmd="fwd";  }
void goBackward(){ setLeft(-1,speedVal);  setRight(-1,speedVal);  lastCmd="back"; }
void goLeft()    { setLeft(-1,speedVal);  setRight( 1,speedVal);  lastCmd="left"; }  // pivot kiri
void goRight()   { setLeft( 1,speedVal);  setRight(-1,speedVal);  lastCmd="right"; } // pivot kanan

/* ======================== Web UI ============================== */
const char HTML_PAGE[] PROGMEM = R"HTML(
<!doctype html><html lang="id"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 2WD SmartCar</title>
<style>
  :root{--bg:#0b1220;--panel:#111a2f;--text:#e7efff;--muted:#9bb0d0;--accent:#61dafb;--ok:#3fb950;--err:#ff6b6b;}
  *{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--text);font:15px/1.5 system-ui,Segoe UI,Roboto,Helvetica,Arial,sans-serif}
  .wrap{max-width:700px;margin:20px auto;padding:16px}
  .card{background:var(--panel);border:1px solid #1b2b48;border-radius:16px;box-shadow:0 10px 30px rgba(0,0,0,.25);padding:16px}
  h1{margin:0 0 12px;font-size:18px}
  .grid{display:grid;grid-template-columns:repeat(3,1fr);gap:10px;margin-top:8px}
  button{border:0;border-radius:14px;padding:16px;font-weight:700;background:#1a2a4a;color:#e7efff;cursor:pointer}
  button:active{transform:scale(.98)}
  .full{grid-column:1 / -1}
  .stop{background:#432032}
  .muted{color:var(--muted)}
  .row{display:flex;align-items:center;gap:10px;margin-top:10px}
  input[type=range]{width:100%}
  .badge{display:inline-block;padding:4px 10px;border-radius:999px;background:#0f1c34;border:1px solid #22375e;margin-left:6px}
</style>
</head><body><div class="wrap">
  <div class="card">
    <h1>ESP32 2WD SmartCar <span id="status" class="badge">...</span></h1>
    <div class="row">
      <label>Kecepatan: <span id="spdVal">200</span></label>
      <input id="spd" type="range" min="0" max="255" value="200">
    </div>
    <div class="grid" style="margin-top:14px">
      <div></div>
      <button id="fwd">MAJU</button>
      <div></div>

      <button id="left">KIRI</button>
      <button id="stop" class="stop">STOP</button>
      <button id="right">KANAN</button>

      <div></div>
      <button id="back" class="full">MUNDUR</button>
    </div>
    <p class="muted">Tekan & tahan tombol arah untuk bergerak, lepas = berhenti.</p>
  </div>
</div>
<script>
const send = (url) => fetch(url).catch(()=>{});
const down = (id, cmd) => {
  const el = document.getElementById(id);
  ['mousedown','touchstart','pointerdown'].forEach(ev=>el.addEventListener(ev, e=>{
    e.preventDefault(); send('/cmd?d='+cmd);
  },{passive:false}));
  ['mouseup','mouseleave','touchend','touchcancel','pointerup','blur'].forEach(ev=>el.addEventListener(ev, e=>{
    e.preventDefault(); send('/cmd?d=stop');
  },{passive:false}));
};
down('fwd','fwd'); down('back','back'); down('left','left'); down('right','right');
document.getElementById('stop').addEventListener('click', ()=>send('/cmd?d=stop'));
const spd = document.getElementById('spd'), spdVal = document.getElementById('spdVal');
spd.addEventListener('input', ()=>{ spdVal.textContent = spd.value; send('/spd?val='+spd.value); });
fetch('/status').then(r=>r.json()).then(j=>{
  document.getElementById('status').textContent = j.ip + ' · ' + j.mode;
  spd.value=j.speed; spdVal.textContent=j.speed;
});
</script>
</body></html>
)HTML";

/* ======================== HTTP Handlers ======================= */
void handleRoot() {
  server.send_P(200, "text/html; charset=utf-8", HTML_PAGE);
}

void handleCmd() {
  if (!server.hasArg("d")) { server.send(400, "text/plain", "missing d"); return; }
  String d = server.arg("d");
  if      (d == "fwd")  goForward();
  else if (d == "back") goBackward();
  else if (d == "left") goLeft();
  else if (d == "right")goRight();
  else                  goStop();
  server.send(200, "text/plain", "ok");
}

void handleSpd() {
  if (server.hasArg("val")) {
    int v = server.arg("val").toInt();
    speedVal = constrain(v, 0, 255);
  }
  server.send(200, "text/plain", String(speedVal));
}

void handleStatus() {
  IPAddress ip = USE_AP ? WiFi.softAPIP() : WiFi.localIP();
  String mode = USE_AP ? "AP" : "STA";
  String j = "{";
  j += "\"ip\":\"" + ip.toString() + "\"";
  j += ",\"mode\":\"" + mode + "\"";
  j += ",\"speed\":" + String(speedVal);
  j += ",\"last\":\"" + lastCmd + "\"";
  j += "}";
  server.send(200, "application/json", j);
}

/* ======================== Setup/Loop ========================== */
void setup() {
  // Motor pins
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  ledcSetup(CH_A, PWM_FREQ, PWM_RES); ledcAttachPin(ENA, CH_A);
  ledcSetup(CH_B, PWM_FREQ, PWM_RES); ledcAttachPin(ENB, CH_B);
  goStop();

  // WiFi
  if (USE_AP) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
  } else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(STA_SSID, STA_PASS);
    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis()-t0 < 15000) delay(100);
  }

  // Routes
  server.on("/", handleRoot);
  server.on("/cmd", handleCmd);   // /cmd?d=fwd|back|left|right|stop
  server.on("/spd", handleSpd);   // /spd?val=0..255
  server.on("/status", handleStatus);
  server.begin();
}

void loop() {
  server.handleClient();
}
