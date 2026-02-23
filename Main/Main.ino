#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <WebSocketsServer.h> 
#include <HCSR04.h>
#include "Config.h"       // Setup de Configurações 
#include "Motores.h"      // Controle dos Motores
#include "Sensores.h"     // Sensores
#include "WebPage.h"      // Pagina HTML
#include "Server.h"       // Servidor
#include "HardwareCore.h" // Configurações do Nucleo

// Porta Servidor Web
WebServer server(80);

// Idendificador da tarefa
TaskHandle_t Task0;

// Identificador de Fila de Comandos
QueueHandle_t filaComandos;


void setup() {
    Serial.begin(115200);
    
    // Cria a fila
    filaComandos = xQueueCreate(10, sizeof(char));

    // Inicia Wi-Fi e WebSocket
    setupRede();

  // Chamando a tarefa do nucleo 0 (1 mudar nucleo depois par ver desempenho)
  xTaskCreatePinnedToCore(
    taskHardware,     // Nome da função
    "TarefaMotores",  // Nome debug
    10000,            // Tamanho da pilha
    NULL,             // Parametros
    1,                // Prioridade (1 = Alta)
    &Task0,           // Endereço do Identificador 
    0                 // Numero do nucleo
  );
}

// Chamando a tarefa do nucleo 0 adicionar depois para ver desempenho

void loop() {
    // mantem a comunicação
    loopRede();
    // delay para estabilidade 
    delay(2);
}

