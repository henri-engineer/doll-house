#include <WiFi.h>
#include <WebServer.h> // Para criar o servidor HTTP
#include <Adafruit_NeoPixel.h>

// --- Configuração da Rede Wi-Fi ---
// SE ESTIVER USANDO WOKWI-GUEST PARA TESTE INTERNO, USE ESTAS CREDENCIAIS:
const char* ssid = "Wokwi-GUEST"; // AGORA COM HÍFEN!
const char* password = "";        // Senha vazia para Wokwi-GUEST

// --- Resto do Código (mesmo que o anterior, apenas com a ordem ajustada) ---

#define LED_PIN 4      // Pino GPIO ÚNICO para toda a cadeia de LEDs
#define TOTAL_LEDS 6   // 6 LEDs no total, um para cada cômodo

const int photoSensorPin = 2;
int photoSensorAnalogValue = analogRead(photoSensorPin);
const int externalLedPin = 26;

Adafruit_NeoPixel strip(TOTAL_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

struct Comodo {
  int led_index;
  String nome;
};

Comodo comodos[] = {
  {0, "Sala"},
  {1, "Cozinha"},
  {2, "Quarto 1"},
  {3, "Banheiro"},
  {4, "Quarto 2"},
  {5, "Closet"}
};
const int NUM_COMODOS = sizeof(comodos) / sizeof(comodos[0]);

void setComodoColor(int comodoIndex, uint32_t color) {
  if (comodoIndex >= 0 && comodoIndex < NUM_COMODOS) {
    int ledToControl = comodos[comodoIndex].led_index;
    strip.setPixelColor(ledToControl, color);
    strip.show();
    Serial.print("Comodo '");
    Serial.print(comodos[comodoIndex].nome);
    Serial.print("' (LED ");
    Serial.print(ledToControl);
    Serial.print(") set para cor: ");
    Serial.println(color, HEX);
  } else {
    Serial.println("Indice de comodo invalido.");
  }
}

WebServer server(80);

String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Smart Doll House</title>
<style>
  body { font-family: Arial, sans-serif; text-align: center; margin: 20px; background-color: #f0f0f0; }
  .container { max-width: 400px; margin: auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,.1); }
  h1 { color: #333; }
  .room-control { margin-bottom: 15px; padding: 10px; border: 1px solid #ddd; border-radius: 5px; background-color: #fafafa; }
  .room-control label { display: block; margin-bottom: 8px; font-weight: bold; }
  .button {
    background-color: #4CAF50;
    color: white;
    padding: 10px 15px;
    border: none;
    border-radius: 5px;
    cursor: pointer;
    font-size: 16px;
    margin: 5px;
    min-width: 80px;
  }
  .button.off { background-color: #f44336; }
  .button.on { background-color: #008CBA; }
  .button:hover { opacity: 0.8; }
  .color-input {
    width: 60px;
    height: 30px;
    vertical-align: middle;
    border: 1px solid #ccc;
    border-radius: 4px;
    cursor: pointer;
  }
  .global-control { margin-top: 20px; padding-top: 15px; border-top: 1px solid #eee; }
</style>
</head>
<body>
<div class="container">
  <h1>CP3 - Smart Doll House</h1>
  <label>Chechpoint 03 - Prof Morgantini</label>

  <div>
    <label>1st floor</label>
      <div class="room-control">
        <label>1 - Living Room</label>
        <button class="button on" onclick="setLight(0, 1)"><i class="fa fa-lightbulb-o"></i>On</button>
        <button class="button off" onclick="setLight(0, 0)">Off</button>
        Color: <input type="color" class="color-input" id="color0" onchange="setColor(0)">
      </div>

      <div class="room-control">
        <label>2 - Kitchen</label>
        <button class="button on" onclick="setLight(1, 1)">On</button>
        <button class="button off" onclick="setLight(1, 0)">Off</button>
        Color: <input type="color" class="color-input" id="color1" onchange="setColor(1)">
      </div>

      <div class="room-control">
        <label>3 - Small Dolls Room</label>
        <button class="button on" onclick="setLight(2, 1)">On</button>
        <button class="button off" onclick="setLight(2, 0)">Off</button>
        Color: <input type="color" class="color-input" id="color2" onchange="setColor(2)">
      </div>
  </div>

  <div>
    <label>2nd floor</label>
      <div class="room-control">
        <label>4 - Bathroom</label>
        <button class="button on" onclick="setLight(3, 1)">On</button>
        <button class="button off" onclick="setLight(3, 0)">Off</button>
        Color: <input type="color" class="color-input" id="color3" onchange="setColor(3)">
      </div>

      <div class="room-control">
        <label>5 - Big Doll Room</label>
        <button class="button on" onclick="setLight(4, 1)">On</button>
        <button class="button off" onclick="setLight(4, 0)">Off</button>
        Color: <input type="color" class="color-input" id="color4" onchange="setColor(4)">
      </div>

      <div class="room-control">
        <label>6 - Closet</label>
        <button class="button on" onclick="setLight(5, 1)">On</button>
        <button class="button off" onclick="setLight(5, 0)">Off</button>
        Color: <input type="color" class="color-input" id="color5" onchange="setColor(5)">
      </div>
  </div>
  

  <div class="global-control">
    <button class="button on" onclick="setAllLights(1)">All on</button>
    <button class="button off" onclick="setAllLights(0)">All of</button>
  </div>

</div>

<script>
  function hexToRgb(hex) {
    var r = parseInt(hex.substring(1, 3), 16);
    var g = parseInt(hex.substring(3, 5), 16);
    var b = parseInt(hex.substring(5, 7), 16);
    return { r, g, b };
  }

  function setLight(roomIndex, state) {
    var xhr = new XMLHttpRequest();
    var color = state === 1 ? "FFFFFF" : "000000";
    xhr.open("GET", "/control?room=" + roomIndex + "&color=" + color, true);
    xhr.send();
    console.log("Comando enviado: room=" + roomIndex + ", state=" + state + ", color=" + color);
  }

  function setColor(roomIndex) {
    var colorHex = document.getElementById("color" + roomIndex).value;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/control?room=" + roomIndex + "&color=" + colorHex.substring(1), true);
    xhr.send();
    console.log("Comando enviado: room=" + roomIndex + ", color=" + colorHex);
  }

  function setAllLights(state) {
    var color = state === 1 ? "FFFFFF" : "000000";
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/control_all?color=" + color, true);
    xhr.send();
    console.log("Comando enviado: all, state=" + state + ", color=" + color);
  }
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleControl() {
  if (server.hasArg("room") && server.hasArg("color")) {
    int roomIndex = server.arg("room").toInt();
    String colorHex = server.arg("color");

    long hexValue = strtol(colorHex.c_str(), NULL, 16);
    uint8_t r = (hexValue >> 16) & 0xFF;
    uint8_t g = (hexValue >> 8) & 0xFF;
    uint8_t b = hexValue & 0xFF;

    setComodoColor(roomIndex, strip.Color(r, g, b));
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Erro: Parametros faltando.");
  }
}

void handleControlAll() {
  if (server.hasArg("color")) {
    String colorHex = server.arg("color");
    long hexValue = strtol(colorHex.c_str(), NULL, 16);
    uint8_t r = (hexValue >> 16) & 0xFF;
    uint8_t g = (hexValue >> 8) & 0xFF;
    uint8_t b = hexValue & 0xFF;

    for(int i = 0; i < NUM_COMODOS; i++) {
      setComodoColor(i, strip.Color(r, g, b));
    }
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Erro: Cor faltando.");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Iluminacao Inteligente da Casa de Bonecas!");
  pinMode(photoSensorPin, INPUT_PULLUP);
  Serial.print("Valor lido pelo foto sensor: ");
  Serial.println(photoSensorAnalogValue);
  pinMode(externalLedPin, OUTPUT);
  
  strip.begin();
  strip.show();
  Serial.println("Fita de LEDs inicializada. Todos os LEDs desligados.");

  Serial.print("Conectando a rede Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int connectionAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && connectionAttempts < 30) {
    delay(500);
    Serial.print(".");
    connectionAttempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado com sucesso!");
    Serial.print("Endereco IP do ESP32: ");
    Serial.println(WiFi.localIP());

    // SOMENTE INICIA O SERVIDOR SE O WI-FI ESTIVER CONECTADO!
    server.on("/", handleRoot);
    server.on("/control", handleControl);
    server.on("/control_all", handleControlAll);
    server.begin();
    Serial.println("Servidor HTTP iniciado na porta 80.");
  } else {
    Serial.println("\nFalha na conexao WiFi!");
    Serial.println("Nao foi possivel conectar ao Access Point.");
    Serial.println("Verifique o SSID e a Senha (Wokwi-GUEST tem hifen e senha vazia!).");
    Serial.println("O servidor HTTP nao sera iniciado sem conexao Wi-Fi.");
  }
}

void loop() {
  // Processa as requisições HTTP SOMENTE se o Wi-Fi estiver conectado
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
    if(photoSensorAnalogValue < 1500) digitalWrite(externalLedPin, HIGH); 
  }
  delay(2000);
}