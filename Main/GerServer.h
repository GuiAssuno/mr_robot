#ifndef GERSERVER_H
#define GERSERVER_H

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <DNSServer.h> // <--- Biblioteca Mágica do DNS
#include "Config.h"
#include "WebPage.h"      
#include "HardwareCore.h" 

// Instância do WebSocket
WebSocketsServer webSocket = WebSocketsServer(81);
extern WebServer server;

// Instância do Servidor DNS (Para o truque do nome)
DNSServer dnsServer;

// ==========================================
// 1. EVENTO DO WEBSOCKET
// ==========================================
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    char* texto = (char*) payload;

    // --- LÓGICA DO JOYSTICK ---
    if (payload[0] == 'J') {
      int x, y;
      if (sscanf(texto, "J:%d:%d", &x, &y) == 2) {
        joyX = x;
        joyY = y;
        char cmd = 'J'; 
        xQueueSend(filaComandos, &cmd, 0);
      }
    } 
    // --- LÓGICA DE VELOCIDADE (V) ---
    else if (payload[0] == 'V') {
        int pct;
        if (sscanf(texto, "V:%d", &pct) == 1) {
          pct = constrain(pct, 0, 100);
          limitePwmGlobal = map(pct, 0, 100, 0, 255);
          Serial.printf("Novo limite: %d%%\n", pct);
        }
    }  
    // --- LÓGICA DE BOTÕES ---
    else {
      char cmd = (char)payload[0];
      xQueueSend(filaComandos, &cmd, 0);
    }
  }
}

// ==========================================
// 2. SETUP DA REDE (MODO ACCESS POINT)
// ==========================================
void setupRede() {
  // Configura como Ponto de Acesso (Cria o Wi-Fi)
  WiFi.mode(WIFI_AP);
  
  // Nome da Rede e Senha (se quiser aberta, tire a senha)
  // Ex: "Mr Robot" e senha "robot1234"
  WiFi.softAP(ssid, password); 

  // Aguarda iniciar
  delay(100); 
  
  Serial.println("Rede Wi-Fi Criada!");
  Serial.print("Nome (SSID): "); Serial.println(ssid);
  Serial.print("IP do Robo: "); Serial.println(WiFi.softAPIP()); // Geralmente 192.168.4.1

  // === O TRUQUE DO DNS ===
  dnsServer.start(53, "*", WiFi.softAPIP());

  // 3. Rota da Página Principal
  server.on("/", []() {
    server.send(200, "text/html", html_page);
  });
  
  // Rota para Android detectar que é um portal (evita fechar sozinho)
  server.on("/generate_204", []() {
    server.send(200, "text/html", html_page);
  });

  // Rota para iOS detectar portal
  server.on("/hotspot-detect.html", []() {
    server.send(200, "text/html", html_page);
  });

  // Comandos HTTP Legado
  server.on("/command", []() {
    if (server.hasArg("cmd")) { 
      String c = server.arg("cmd");
      char cmd = c.charAt(0); 
      xQueueSend(filaComandos, &cmd, 0); 
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Erro");
    }
  });

  server.begin();
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  
  Serial.println("Sistemas Iniciados. Conecte no Wi-Fi do robo e acesse 192.168.4.1 ou http://robo.com");
}

// ==========================================
// 3. LOOP DA REDE
// ==========================================
void loopRede() {
  // Processa pedidos de DNS (Redireciona o nome para o IP)
  dnsServer.processNextRequest();
  
  server.handleClient();
  webSocket.loop();
}

#endif