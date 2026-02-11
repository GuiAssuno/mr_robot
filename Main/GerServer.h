#ifndef GERSERVER_H
#define GERSERVER_H

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include "Config.h"
#include "WebPage.h"      
#include "HardwareCore.h" 

// Instância do WebSocket (porta 81)
WebSocketsServer webSocket = WebSocketsServer(81);
extern WebServer server;

// ==========================================
// 1. EVENTO DO WEBSOCKET (Ouve o Cliente)
// ==========================================
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    char* texto = (char*) payload;

    // --- JOYSTICK ---
    if (payload[0] == 'J') {
      int x, y;
      // Extrai os números do texto
      if (sscanf(texto, "J:%d:%d", &x, &y) == 2) {
        // Apenas ATUALIZA as variáveis globais (Config.h)
        // O HardwareCore (Núcleo 0) é quem vai ler isso e mover os motores
        joyX = x;
        joyY = y;
        
        // Avisa o HardwareCore que estamos no modo Joystick
        char cmd = 'J'; 
        xQueueSend(filaComandos, &cmd, 0);
         
      }
    }

    //--- VELOCIDADE ---
    else if (payload[0] == 'V') {
        int pct;
        // Lê o valor percentual (ex: "V:80" -> pct = 80)
        if (sscanf(texto, "V:%d", &pct) == 1) {
          // Trava entre 0 e 100 por segurança
          pct = constrain(pct, 0, 100);
          
          // Converte Porcentagem (0-100) para PWM Real (0-255)
          // e atualiza a variável global que o motor usa.
          limitePwmGlobal = map(pct, 0, 100, 0, 255);
          
          Serial.printf("Novo limite de velocidade: %d%% (PWM: %d)\n", pct, limitePwmGlobal);
        }
      }  

    // --- BOTÕES ---
    else {
      char cmd = (char)payload[0];
      xQueueSend(filaComandos, &cmd, 0);
    }
  }
}

// ==========================================
// 2. SETUP DA REDE E SERVIDORES
// ==========================================
// (Renomeado para bater com o Main.ino)
void setupRede() {

  // 1. Conexão Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Conectando ao Hotspot...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado! IP: " + WiFi.localIP().toString());

  // 2. Configura mDNS (http://superrobo.local)
  if (MDNS.begin("superrobo")) {
    Serial.println("MDNS iniciado! Acesse: http://superrobo.local");
  }

  // 3. Rota da Página Principal
  server.on("/", []() {
    server.send(200, "text/html", html_page);
  });

  // 4. Rota para comandos HTTP (Backup se WebSocket falhar)
  server.on("/command", []() {
    if (server.hasArg("cmd")) { // Usa "cmd" conforme seu Javascript novo
      String c = server.arg("cmd");
      char cmd = c.charAt(0); // Pega a primeira letra
      
      xQueueSend(filaComandos, &cmd, 0); // Envia para o Núcleo 0
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Erro");
    }
  });

  // 5. Inicia Serviços
  server.begin();
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  
  Serial.println("Servidor HTTP e WebSocket iniciados.");
}

// ==========================================
// 3. LOOP DA REDE (Mantém tudo vivo)
// ==========================================
void loopRede() {
  server.handleClient();
  webSocket.loop();
}

#endif