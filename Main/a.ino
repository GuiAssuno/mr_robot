#include <WiFi.h>
#include <WebServer.h>
#include "Config.h"   // Traz configurações
#include "Motors.h"   // Traz controle de motores
#include "Sensors.h"  // Traz sensores
#include "WebPage.h"  // Traz o HTML

WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Inicializa Módulos
  setupMotors();
  setupSensors();

  // Conexão Wi-Fi (Modo Cliente)
  WiFi.mode(WIFI_STA);
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid, password);

  Serial.print("Conectando ao Notebook...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado! IP: " + WiFi.localIP().toString());

  // Configura Rotas do Servidor
  server.on("/", []() {
    server.send(200, "text/html", html_page);
  });

  server.on("/command", []() {
    if (server.hasArg("cmd")) {
      String c = server.arg("cmd");
      char comando = c.charAt(0);
      
      // Traduz comando Web para Ação Motor
      switch(comando) {
        case 'F': setMovimento(FRENTE); break;
        case 'B': setMovimento(TRAS); break;
        case 'L': setMovimento(ESQUERDA); break;
        case 'R': setMovimento(DIREITA); break;
        case 'S': setMovimento(PARADO); break;
      }
      server.send(200, "text/plain", "OK");
    }
  });

  server.begin();
}

void loop() {
  // 1. Processa Wi-Fi
  server.handleClient();

  // 2. Verifica Segurança (Prioridade Alta)
  if (checkSafety()) {
    // Se detectar obstáculo, sobrescreve qualquer comando e para
    pararImediatamente(); 
  } else {
    // 3. Se seguro, gerencia a rampa de velocidade dos motores
    loopMotors(); 
  }
}