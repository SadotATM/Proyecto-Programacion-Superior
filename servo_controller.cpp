#include "servo_control.h"

// WiFi
const char* ssid = "RWR";
const char* password = "12345678";

// Pines de los servos
const int SERVO1_PIN = 12;
const int SERVO2_PIN = 14;
const int SERVO3_PIN = 27;
const int SERVO4_PIN = 26; // GARRA

// Pin del pulsador (garra)
const int BUTTON_PIN = 33; // Ajusta según el pin que uses

// Servos
Servo servo1, servo2, servo3, servo4;

// Ángulos actuales
int angle1 = 90;
int angle2 = 90;
int angle3 = 90;
int angle4 = 0;

WebServer server(80);

// HTML para interfaz web
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Control de 4 Servos</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial;
      text-align: center;
      padding-top: 20px;
      background: #f9f9f9;
    }
    input[type=range] {
      width: 300px;
      height: 30px;
    }
    .contenedor-deslizante {
      margin: 10px auto;
      max-width: 95%;
      background: white;
      padding: 12px 15px;
      border-radius: 10px;
      box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
      font-size: 1.2em;
    }
    button {
      margin-top: 10px;
      font-size: 18px;
      padding: 10px 25px;
      border: none;
      border-radius: 6px;
      background: #007bff;
      color: white;
      cursor: pointer;
    }
    button:hover {
      background: #0056b3;
    }
    #brazo {
      width: 100%;
      max-width: 600px;
      height: 400px;
      margin: 20px auto;
      display: block;
      background: white;
      border-radius: 10px;
      touch-action: none;
    }
    circle.articulacion, circle.articulacion-muneca {
      fill: #6e6e6e;
      cursor: grab;
    }
    circle.articulacion:active, circle.articulacion-muneca:active {
      cursor: grabbing;
    }
  </style>
</head>
<body>
  <h2>Control de 4 Servos</h2>

  <div class="contenedor-deslizante">
    <label>Servo 1: <span id="val1">90</span>°</label><br />
    <input type="range" min="0" max="180" value="45" id="slider1">
  </div>
  <div class="contenedor-deslizante">
    <label>Servo 2 (Inverso): <span id="val2">90</span>°</label><br />
    <input type="range" min="0" max="180" value="90" id="slider2">
  </div>
  <div class="contenedor-deslizante">
    <label>Servo 3: <span id="val3">90</span>°</label><br />
    <input type="range" min="0" max="180" value="90" id="slider3">
  </div>
  <div class="contenedor-deslizante">
    <label>Servo 4 (0-90°): <span id="val4">Cerrada</span></label><br />
    <button id="botonAlternarGarra">Abrir / Cerrar</button>
  </div>

  <svg id="brazo" viewBox="0 0 400 400" preserveAspectRatio="xMidYMid meet">
    <rect x="20" y="20" width="360" height="360" rx="30" fill="black" opacity="0.85" />
    <line id="lineaBase" x1="200" y1="350" x2="200" y2="300" stroke="black" stroke-width="14" />
    <circle id="articulacionBase" class="articulacion" cx="200" cy="300" r="15" />
    <line id="lineaHombro" x1="200" y1="300" x2="200" y2="240" stroke="#d6d6d6" stroke-width="14" />
    <circle id="articulacionHombro" class="articulacion" cx="200" cy="240" r="15" />
    <line id="lineaCodo" x1="200" y1="240" x2="200" y2="180" stroke="white" stroke-width="14" />
    <circle id="articulacionCodo" class="articulacion" cx="200" cy="180" r="15" />
    <line id="lineaMuneca" x1="200" y1="180" x2="200" y2="140" stroke="orange" stroke-width="10" />
    <circle id="articulacionMuneca" class="articulacion-muneca" cx="200" cy="140" r="0" />
    <line id="garraIzquierda" x1="200" y1="140" x2="185" y2="120" stroke="white" stroke-width="8" />
    <line id="garraDerecha" x1="200" y1="140" x2="215" y2="120" stroke="white" stroke-width="8" />
  </svg>

  <script>
    const longitudes = { baseAHombro: 100, hombroACodo: 80, codoAMuneca: 30, munecaAGarra: 0 };
    let angulos = { base: 90, hombro: 45, codo: 45, garraAbierta: false };

    const baseX = 200, baseY = 350;

    function gradosARadianes(grados) { return grados * Math.PI / 180; }

    function actualizarBrazo() {
      const aB = gradosARadianes(angulos.base);
      const x1 = baseX + longitudes.baseAHombro * Math.cos(aB);
      const y1 = baseY - longitudes.baseAHombro * Math.sin(aB);

      lineaBase.setAttribute("x2", x1); lineaBase.setAttribute("y2", y1);

      const aH = gradosARadianes(angulos.hombro);
      const x2 = x1 + longitudes.hombroACodo * Math.cos(aH);
      const y2 = y1 - longitudes.hombroACodo * Math.sin(aH);

      lineaHombro.setAttribute("x1", x1); lineaHombro.setAttribute("y1", y1);
      lineaHombro.setAttribute("x2", x2); lineaHombro.setAttribute("y2", y2);

      const aC = gradosARadianes(angulos.codo);
      const x3 = x2 + longitudes.codoAMuneca * Math.cos(aC);
      const y3 = y2 - longitudes.codoAMuneca * Math.sin(aC);

      lineaCodo.setAttribute("x1", x2); lineaCodo.setAttribute("y1", y2);
      lineaCodo.setAttribute("x2", x3); lineaCodo.setAttribute("y2", y3);

      lineaMuneca.setAttribute("x1", x3); lineaMuneca.setAttribute("y1", y3);
      lineaMuneca.setAttribute("x2", x3); lineaMuneca.setAttribute("y2", y3 - longitudes.munecaAGarra);

      articulacionBase.setAttribute("cx", baseX); articulacionBase.setAttribute("cy", baseY);
      articulacionHombro.setAttribute("cx", x1); articulacionHombro.setAttribute("cy", y1);
      articulacionCodo.setAttribute("cx", x2); articulacionCodo.setAttribute("cy", y2);
      articulacionMuneca.setAttribute("cx", x3); articulacionMuneca.setAttribute("cy", y3);

      const g = angulos.garraAbierta ? 30 : 10, l = 20, baseG = -angulos.codo;
      const gx1 = x3 + l * Math.cos(gradosARadianes(baseG - g));
      const gy1 = y3 + l * Math.sin(gradosARadianes(baseG - g));
      const gx2 = x3 + l * Math.cos(gradosARadianes(baseG + g));
      const gy2 = y3 + l * Math.sin(gradosARadianes(baseG + g));

      garraIzquierda.setAttribute("x1", x3); garraIzquierda.setAttribute("y1", y3);
      garraIzquierda.setAttribute("x2", gx1); garraIzquierda.setAttribute("y2", gy1);
      garraDerecha.setAttribute("x1", x3); garraDerecha.setAttribute("y1", y3);
      garraDerecha.setAttribute("x2", gx2); garraDerecha.setAttribute("y2", gy2);

      val1.textContent = angulos.base.toFixed(0);
      val2.textContent = angulos.hombro.toFixed(0);
      val3.textContent = angulos.codo.toFixed(0);
      val4.textContent = angulos.garraAbierta ? 'Abierta' : 'Cerrada';
    }

    function limitarAngulo(a) { return Math.min(180, Math.max(0, a)); }

    function setupSlider(id, valId, num, prop) {
      const s = document.getElementById(id);
      const v = document.getElementById(valId);
      s.oninput = () => {
        angulos[prop] = Number(s.value);
        v.textContent = s.value;
        fetch(/servo?num=${num}&angle=${s.value});
        actualizarBrazo();
      };
    }

    setupSlider("slider1", "val1", 1, "base");
    setupSlider("slider2", "val2", 2, "hombro");
    setupSlider("slider3", "val3", 3, "codo");

    botonAlternarGarra.onclick = () => {
      angulos.garraAbierta = !angulos.garraAbierta;
      fetch(/servo?num=4&angle=${angulos.garraAbierta ? 90 : 0});
      actualizarBrazo();
    };

    let arrastrando = null;
    let inicioX = 0, inicioY = 0;

    const svg = document.getElementById("brazo");
    svg.addEventListener("pointerdown", e => {
      if (e.target.classList.contains("articulacion")) {
        arrastrando = e.target.id;
        inicioX = e.clientX;
        inicioY = e.clientY;
        e.target.setPointerCapture(e.pointerId);
      }
    });

    svg.addEventListener("pointermove", e => {
      if (!arrastrando) return;
      const dx = e.clientX - inicioX;
      inicioX = e.clientX;
        if (arrastrando === "articulacionBase") {
        angulos.base = limitarAngulo(angulos.base + dx * 0.5);
        slider1.value = angulos.base;
        fetch(/servo?num=1&angle=${angulos.base});
      } else if (arrastrando === "articulacionHombro") {
        angulos.hombro = limitarAngulo(angulos.hombro + dx * 0.5);
        slider2.value = angulos.hombro;
        fetch(/servo?num=2&angle=${angulos.hombro});
      } else if (arrastrando === "articulacionCodo") {
        angulos.codo = limitarAngulo(angulos.codo + dx * 0.5);
        slider3.value = angulos.codo;
        fetch(/servo?num=3&angle=${angulos.codo});
      }
      actualizarBrazo();
    });

    svg.addEventListener("pointerup", e => {
      if (arrastrando) {
        e.target.releasePointerCapture(e.pointerId);
        arrastrando = null;
      }
    });
    svg.addEventListener("pointercancel", e => arrastrando = null);

    actualizarBrazo();
  </script>
</body>
</html>
)rawliteral";

// Función para mover suavemente el servo
void smoothMove(Servo& servo, int& currentAngle, int targetAngle, int servoNum) {
  int step = (targetAngle > currentAngle) ? 1 : -1;
  for (int i = currentAngle; i != targetAngle; i += step) {
    // Si es el servo de la garra (número 4), verificar el pulsador
    if (servoNum == 4 && digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("Garra cerrada: pulsador presionado, deteniendo movimiento");
      break;
    }
    servo.write(i);
    delay(15);
  }
  // Al final, ajustar al ángulo más cercano alcanzado
  currentAngle = servo.read();
}

void setupAll() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP); // Pulsador conectado a GND

  servo1.setPeriodHertz(50);
  servo1.attach(SERVO1_PIN, 500, 2500);

  servo2.setPeriodHertz(50);
  servo2.attach(SERVO2_PIN, 500, 2500);

  servo3.setPeriodHertz(50);
  servo3.attach(SERVO3_PIN, 500, 2500);

  servo4.setPeriodHertz(50);
  servo4.attach(SERVO4_PIN, 500, 2500);

  servo1.write(angle1);
  servo2.write(angle2);
  servo3.write(angle3);
  servo4.write(angle4);

  WiFi.begin(ssid, password);
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi");
  Serial.println("IP: " + WiFi.localIP().toString());

  server.on("/", []() {
    server.send(200, "text/html", htmlPage);
  });

  server.on("/servo", []() {
    if (server.hasArg("num") && server.hasArg("angle")) {
      int num = server.arg("num").toInt();
      int angle = server.arg("angle").toInt();

      angle = constrain(angle, 0, (num == 4 ? 90 : 180));

      switch (num) {
        case 1:
          smoothMove(servo1, angle1, angle, 1);
          break;
        case 2:
          smoothMove(servo2, angle2, 180 - angle, 2); // Inverso
          break;
        case 3:
          smoothMove(servo3, angle3, angle, 3);
          break;
        case 4:
          smoothMove(servo4, angle4, angle, 4);
          break;
      }

      server.send(200, "text/plain", "Servo " + String(num) + " → " + String(angle) + "°");
    } else {
      server.send(400, "text/plain", "Faltan parámetros");
    }
  });

  server.begin();
}

void loopAll() {
  server.handleClient();
}