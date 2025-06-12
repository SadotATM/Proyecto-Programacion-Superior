#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "RWR";        // ← Cambiá por el nombre de tu red Wi-Fi
const char* password = "12345678"; // ← Cambiá por tu contraseña

const int ledPin = 2; // LED en GPIO 2
WebServer server(80);

// Página HTML con botones para controlar el LED
String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>Control LED ESP32</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial; text-align: center; margin-top: 50px; }
      button { font-size: 20px; padding: 15px 30px; margin: 10px; border-radius: 10px; }
    </style>
  </head>
  <body>
    <h1>Control de LED con ESP32</h1>
    <button onclick="location.href='/on'">ENCENDER</button>
    <button onclick="location.href='/off'">APAGAR</button>
  </body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Conexión a tu red Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Conectado a WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP()); // Anotá esta IP

  // Rutas web
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", htmlPage);
  });

  server.on("/on", HTTP_GET, []() {
    digitalWrite(ledPin, HIGH);
    server.send(200, "text/plain", "LED ENCENDIDO");
  });

  server.on("/off", HTTP_GET, []() {
    digitalWrite(ledPin, LOW);
    server.send(200, "text/plain", "LED APAGADO");
  });

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}
