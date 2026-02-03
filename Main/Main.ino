#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <WiFiClient.h> 
#include <HCSR04.h>
#include "HardwareCore.h"  // Traz as configurações do nucleo
#include "Config.h"       // Traz configurações
#include "Motors.h"      // Traz controle de motores
#include "Sensors.h"    // Traz sensores
#include "WebPage.h"   // Traz o HTML
#include "GerServer.h"// Traz o Servidor

WebServer server(80);

QueueHandle_t filaComandos;

void setup() {
    Serial.begin(115200);

    filaComandos = xQueueCreate(10, sizeof(char));
    setupRede();

  // O NÚCLEO 0
  xTaskCreatePinnedToCore(
    taskHardware,     // Nome da função no arquivo HardwareCore.h
    "TarefaMotores",  // Nome para debug
    10000,            // Tamanho da pilha
    NULL,             // Parâmetros
    1,                // Prioridade (1 = Alta)
    &.Task0,           // Handle
    0                 // <--- NÚMERO DO NÚCLEO (0)
  );
}

void loop() {
    loopRede();
    checkSafety();
}

