#ifndef GerSERVER_H
#define GerSERVER_H

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "Config.h"
#include "WebPage.h"      // Precisa incluir para acessar a variável 'server' e 'html_page'
#include "HardwareCore.h" // Para enviar comandos para a fila (xQueue)

// Instância do WebSocket (porta 81)
WebSocketsServer webSocket = WebSocketsServer(81);

// ==========================================
// FUNÇÃO DE EVENTO DO WEBSOCKET
// ==========================================
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    char cmd = (char)payload[0];
    
    // Envia para a Fila do FreeRTOS (Para o Núcleo 0 processar)
    // Se você ainda não implementou a fila, use a variável global aqui
    xQueueSend(filaComandos, &cmd, 0); 
  }
}

// ==========================================
// FUNÇÃO PRINCIPAL DE CONFIGURAÇÃO (SETUP)
// ==========================================
void setupNetworkAndServer() {
  
  // 1. Configurar Wi-Fi (Station Mode - Conecta no Celular/Note)
  WiFi.mode(WIFI_STA);
  // Se quiser IP Fixo, configure aqui antes do begin
  // WiFi.config(local_IP, gateway, subnet); 
  WiFi.begin(ssid, password);

  Serial.print("Conectando ao Hostpot...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado! IP: " + WiFi.localIP().toString());

  // 2. Configurar Rotas do Servidor Web
  server.on("/", []() {
    // Aqui usamos o HTML que está em WebPage.h
    // Dica: Se quiser injetar o IP da câmera dinamicamente:
    String pagina = html_page;
    // pagina.replace("SEU_IP_CAMERA", WiFi.localIP().toString()); 
    server.send(200, "text/html", pagina);
  });

  server.on("/command", []() {
    if (server.hasArg("cmd")) {
      String c = server.arg("cmd");
      char cmd = c.charAt(0);
      xQueueSend(filaComandos, &cmd, 0); // Envia para fila
      server.send(200, "text/plain", "OK");
    }
  });

  // 3. Iniciar Serviços
  server.begin();
  
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  
  Serial.println("Servidor HTTP e WebSocket iniciados!");

























  // ==============================
  //  CONFIGURAÇÃO DO ACCESS POINT
  // ==============================
  WiFi.mode(WIFI_OFF);
  delay(1000);

  WiFi.mode(WIFI_AP);
  delay(500);

  WiFi.softAPConfig(
    IPAddress(192, 168, 4, 1),   // IP Local
    IPAddress(192, 168, 4, 1),   // Gateway
    IPAddress(255, 255, 255, 0)  // Subnet
  );

  bool apResult = WiFi.softAP(ssid, password);

  // para feedback via monitor serial
  if (apResult) {
    Serial.println("✓ Access Point criado com sucesso!");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Senha: ");
    Serial.println(password);
    Serial.print("IP do AP: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("MAC do AP: ");
    Serial.println(WiFi.softAPmacAddress());
  } else {
    Serial.println("✗ Falha ao criar Access Point!");
    return;
  }

  // ==========================
  //  CONFIGURAÇÃO DO SERVIDOR
  // ==========================
  Serial.println("Configurando servidor web...");

  //interface de controle
  server.on("/", []() {
    server.send(200, "text/html", getHTML());
  });

  //comandos recebidos via rota http
  server.on("/command", HTTP_GET, []() {
    if (server.hasArg("cmd")) {
      char command = mapCommand(server.arg("cmd"));

      if ((flag) && (command != 'X')) return;

      if (command != '\0') {
        comando(command);
        Serial.println(command);
        server.send(200, "text/plain", "OK");
      } else {
        server.send(400, "application/json", "{\"status\":\"invalid_command\"}");
      }
    } else {
      server.send(400, "application/json", "{\"status\":\"no_command\"}");
    }
  });

  // === Rota para joystick ===
  server.on("/joystick", HTTP_GET, handleJoystick);

  // Iniciar servidor
  server.begin();
  Serial.println("✓ Servidor web iniciado na porta 80");
  Serial.println("=========================");
  Serial.println("ESP32 WROOM PRONTO!");
  Serial.println("Access Point: " + String(ssid));
  Serial.println("IP: " + WiFi.softAPIP().toString());
  Serial.println("Interface: http://" + WiFi.softAPIP().toString());
  Serial.println("=========================");
    filaComandos = xQueueCreate(10, sizeof(char));
    // Configurações do WebSocket
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
    setupWebServer();

}

// Função para manter os serviços vivos (Chamar no Loop do Main.ino)
void loopNetwork() {
  server.handleClient();
  webSocket.loop();
}

#endif