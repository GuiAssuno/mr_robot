#ifndef GERSERVER_H
#define GERSERVER_H

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <DNSServer.h> 
#include "Config.h"
#include "WebPage.h"      
#include "HardwareCore.h" 

// Instância do WebSocket
WebSocketsServer webSocket = WebSocketsServer(81);
extern WebServer server;

// Instância do Servidor DNS
DNSServer dnsServer;


// WEBSOCKET
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    char* texto = (char*) payload;

    // ============== JOYSTICK =============
    if (payload[0] == 'J') {
      int x, y;
      if (sscanf(texto, "J:%d:%d", &x, &y) == 2) {
        joyX = x;
        joyY = y;
        char cmd = 'J'; 
        xQueueSend(filaComandos, &cmd, 0);
      }
    } 

    // ============= VELOCIDADE =============
    else if (payload[0] == 'V') {
        int pct;
        if (sscanf(texto, "V:%d", &pct) == 1) {
          pct = constrain(pct, 0, 100);
          limitePwmGlobal = map(pct, 0, 100, 0, 255);
          Serial.printf("Novo limite: %d%%\n", pct);
        }
    }  
    // ============== BOTÕES ==============
    else {
      char cmd = (char)payload[0];
      xQueueSend(filaComandos, &cmd, 0);
    }
  }
}

// ============ Setup da Rede ===========
void setupRede() {
  // Configura como Ponto de Acesso (Cria o Wi-Fi)
  WiFi.mode(WIFI_AP);
  
  // Nome da rede e senha
  WiFi.softAP(ssid, password); 

  // Aguarda iniciar
  delay(100); 
  
  //Serial.println("Rede Wi-Fi Criada!");
  //Serial.print("Nome (SSID): "); Serial.println(ssid);
  //Serial.print("IP do Robo: "); Serial.println(WiFi.softAPIP()); // 192.168.4.1

  dnsServer.start(53, "*", WiFi.softAPIP());

  // Página Principal
  server.on("/", []() {
    server.send(200, "text/html", html_page);
  });
  
  // para o android detectar evitar fechar sozinho
  server.on("/generate_204", []() {
    server.send(200, "text/html", html_page);
  });

  // para o ios detectar evitar fechar sozinho
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
  
  //Serial.println("acesse 192.168.4.1 ou http://robo.com");
}

// 
// ============ Loop da Rede ==============

void loopRede() {
  // Redireciona o nome para o IP
  dnsServer.processNextRequest();
  
  server.handleClient();
  webSocket.loop();
}

// ========= Tarefa do Nucleo 0 (adicionar depois para ver desempenho) =========== 
void taskRedeLoop(void *pvParameters) {
  for (;;) {

    loopRede(); 
    // Watchdog
    vTaskDelay(1 / portTICK_PERIOD_MS); 
  }
}

#endif