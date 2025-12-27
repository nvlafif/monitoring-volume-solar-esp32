#include <ETH.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// =================== ETH CONFIG ===================
#define ETH_TYPE        ETH_PHY_LAN8720
#define ETH_ADDR        1
#define ETH_POWER       -1
#define ETH_MDC         23
#define ETH_MDIO        18
#define ETH_CLK_MODE    ETH_CLOCK_GPIO17_OUT

IPAddress ipStatic(10,27,30,110);
IPAddress ipGateway(10,27,30,1);
IPAddress ipSubnet(255,255,255,0);
IPAddress ipDns1(10,2,1,5);
IPAddress ipDns2(10,60,5,5);

bool staticIPApplied = false;

// =================== DIMENSI TANKI ===================
float panjangTanki = 0, lebarTanki = 0, tinggiTanki = 0;
bool dimFilled = false;

// =================== SENSOR ===================
#define TRIG_PIN 4
#define ECHO_PIN 33

// =================== WEB SERVER ===================
WebServer server(80);

// ==========================================================================
// =================== FUNGSI SIMPAN & LOAD IP CONFIG =======================
// ==========================================================================

void saveIPConfig() {
  DynamicJsonDocument doc(256);

  doc["ip"]      = ipStatic.toString();
  doc["gateway"] = ipGateway.toString();
  doc["subnet"]  = ipSubnet.toString();
  doc["dns1"]    = ipDns1.toString();
  doc["dns2"]    = ipDns2.toString();

  File file = LittleFS.open("/ipconfig.json", "w");
  if (!file) {
    Serial.println("Gagal menyimpan ipconfig.json");
    return;
  }

  serializeJsonPretty(doc, file);
  file.close();
  Serial.println("IP config saved.");
}

void loadIPConfig() {
  if (!LittleFS.exists("/ipconfig.json")) {
    Serial.println("ipconfig.json tidak ditemukan, pakai default.");
    return;
  }

  File file = LittleFS.open("/ipconfig.json", "r");
  DynamicJsonDocument doc(256);
  deserializeJson(doc, file);
  file.close();

  ipStatic.fromString(doc["ip"].as<String>());
  ipGateway.fromString(doc["gateway"].as<String>());
  ipSubnet.fromString(doc["subnet"].as<String>());
  ipDns1.fromString(doc["dns1"].as<String>());
  ipDns2.fromString(doc["dns2"].as<String>());

  Serial.println("IP config loaded.");
}

// ==========================================================================
// =================== EVENT ETH =======================
// ==========================================================================
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {

    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected. Applying Static IP...");
      if (!staticIPApplied) {
        if (ETH.config(ipStatic, ipGateway, ipSubnet, ipDns1, ipDns2)) {
          staticIPApplied = true;
          Serial.println("Static IP applied.");
        } else {
          Serial.println("Apply static IP FAILED!");
        }
      }
      break;

    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("ESP32 IP: ");
      Serial.println(ETH.localIP());
      break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
      staticIPApplied = false;
      Serial.println("ETH disconnected!");
      break;
  }
}

// ==========================================================================
// =================== SENSOR ULTRASONIK =======================
// ==========================================================================
float bacaJarak() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  float cm = duration * 0.0343 / 2.0;

  return (cm < 0 || cm > 500) ? 0 : cm;
}

// ==========================================================================
// =================== ROUTES: DIMENSI TANKI =======================
// ==========================================================================
void handleSetDim() {
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "POST only");

  panjangTanki = server.arg("p").toFloat();
  lebarTanki   = server.arg("l").toFloat();
  tinggiTanki  = server.arg("t").toFloat();

  dimFilled = true;
  server.send(200, "text/plain", "Dimensi tanki disimpan!");
}

void handleGetDim() {
  DynamicJsonDocument doc(128);

  if (!dimFilled) doc["status"] = "empty";
  else {
    doc["p"] = panjangTanki;
    doc["l"] = lebarTanki;
    doc["t"] = tinggiTanki;
  }

  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

// ==========================================================================
// =================== ROUTES: KONFIGURASI IP STATIC =======================
// ==========================================================================
void handleSetIP() {
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "POST only");

  ipStatic = IPAddress(server.arg("ip1").toInt(), server.arg("ip2").toInt(),
                       server.arg("ip3").toInt(), server.arg("ip4").toInt());

  ipGateway = IPAddress(server.arg("gw1").toInt(), server.arg("gw2").toInt(),
                        server.arg("gw3").toInt(), server.arg("gw4").toInt());

  ipSubnet = IPAddress(server.arg("sn1").toInt(), server.arg("sn2").toInt(),
                       server.arg("sn3").toInt(), server.arg("sn4").toInt());

  ipDns1 = IPAddress(server.arg("stdns1").toInt(), server.arg("stdns2").toInt(),
                     server.arg("stdns3").toInt(), server.arg("stdns4").toInt());

  ipDns2 = IPAddress(server.arg("nddns1").toInt(), server.arg("nddns2").toInt(),
                     server.arg("nddns3").toInt(), server.arg("nddns4").toInt());

  staticIPApplied = false;

  saveIPConfig();

  server.send(200, "text/plain", "IP diperbarui & disimpan permanen. Restarting...");
  delay(500);
  ESP.restart();
}

void handleGetIP() {
  DynamicJsonDocument doc(256);

  doc["ip"]      = ipStatic.toString();
  doc["gateway"] = ipGateway.toString();
  doc["subnet"]  = ipSubnet.toString();
  doc["dns1"]    = ipDns1.toString();
  doc["dns2"]    = ipDns2.toString();

  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

// ==========================================================================
// =================== ROUTE DATA SENSOR =======================
// ==========================================================================
void handleData() {
  if (!dimFilled)
    return server.send(200, "application/json", "{\"status\":\"empty\"}");

  float jarak = bacaJarak();
  float tinggiSolar = max(0.0f, tinggiTanki - jarak);

  float volumeSolar = (panjangTanki * lebarTanki * tinggiSolar) / 1000.0;
  float volumeMax   = (panjangTanki * lebarTanki * tinggiTanki) / 1000.0;

  float persenSolar = (volumeSolar / volumeMax) * 100.0;

  DynamicJsonDocument doc(256);
  doc["jarak"]  = jarak;
  doc["tinggi"] = tinggiSolar;
  doc["volume"] = volumeSolar;
  doc["persen"] = persenSolar;

  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

// ==========================================================================
// =================== FILE SERVER =======================
// ==========================================================================
String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  if (filename.endsWith(".css"))  return "text/css";
  if (filename.endsWith(".js"))   return "application/javascript";
  return "text/plain";
}

bool handleFileRead(String path) {
  if (path.endsWith("/")) path += "index.html";

  if (!LittleFS.exists(path)) return false;

  File file = LittleFS.open(path, "r");
  server.streamFile(file, getContentType(path));
  file.close();
  return true;
}

void handleNotFound() {
  if (!handleFileRead(server.uri()))
    server.send(404, "text/plain", "404 Not Found");
}

// ==========================================================================
// =================== SETUP =======================
// ==========================================================================
void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Mount filesystem
  if (!LittleFS.begin()) {
    Serial.println("LittleFS FAILED!");
    while (true) delay(1000);
  }

  // Load IP config
  loadIPConfig();

  // ETH
  WiFi.onEvent(WiFiEvent);
  ETH.begin(ETH_ADDR, ETH_POWER, ETH_MDC, ETH_MDIO, ETH_TYPE, ETH_CLK_MODE);

  // Buat history.txt dan history_index.txt jika belum ada
  if (!LittleFS.exists("/history.txt")) {
    File f = LittleFS.open("/history.txt", "w");
    // Buat 10 baris kosong agar struktur file tetap konsisten (opsional)
    for (int i = 0; i < 10; ++i) f.println("");
    f.close();
    Serial.println("Created /history.txt (10 empty lines)");
  }

  if (!LittleFS.exists("/history_index.txt")) {
    File f = LittleFS.open("/history_index.txt", "w");
    f.print("0");
    f.close();
    Serial.println("Created /history_index.txt");
  }

  // Routes
  server.onNotFound(handleNotFound);

  server.on("/setdim", HTTP_POST, handleSetDim);
  server.on("/getdim", HTTP_GET,  handleGetDim);

  server.on("/setIP",  HTTP_POST, handleSetIP);
  server.on("/getip",  HTTP_GET,  handleGetIP);

  server.on("/data",   HTTP_GET,  handleData);



  server.begin();
  Serial.println("Server started.");
}

// ==========================================================================
// =================== LOOP =======================
// ==========================================================================
void loop() {
  server.handleClient();
}
