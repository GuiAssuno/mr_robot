#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <WebSocketsServer.h> 
#include <HCSR04.h>
#include "Config.h"       // Configurações Gerais
#include "Motors.h"      // Controle de motores
#include "Sensors.h"    // Sensoresensores
#include "WebPage.h"      // Pagina HTML
#include "GerServer.h"    // Servidor
#include "HardwareCore.h" // Configurações do nucleo

// Porta Servidor Web
WebServer server(80);

// O handle da tarefa
TaskHandle_t Task0;

// Fila de Comandos
QueueHandle_t filaComandos;


void setup() {
    Serial.begin(115200);
    
    // Cria a fila
    filaComandos = xQueueCreate(10, sizeof(char));

    // Inicia Wi-Fi e WebSocket
    setupRede();

  // Chamando a tarefa do nucleo 0
  xTaskCreatePinnedToCore(
    taskHardware,     // Nome da função
    "TarefaMotores",  // Nome debug
    10000,            // Tamanho da pilha
    NULL,             // Parametros
    1,                // Prioridade (1 = Alta)
    &Task0,           // Handle
    0                 // Numero do nucleo
  );
}

void loop() {
    // mantem a comunicação
    loopRede();

    // delay para estabilidade 
    delay(2);
}

