#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// ==========================================
// 1. CONFIGURAÇÕES DE REDE
// ==========================================
// Substitua pelo nome e senha do Hotspot do seu celular
const char* ssid = "mr robot";      
const char* password = "robot1234"; // Confirme se a senha é esta mesmo

// ==========================================
// 2. PINAGEM EXATA DA AI THINKER
// ==========================================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Inicia servidor na porta 81 (padrão para streaming simples)
WebServer server(81);

// ==========================================
// 3. FUNÇÃO DE STREAMING DE VÍDEO
// ==========================================
void handleStream() {
  WiFiClient client = server.client();
  
  // Cabeçalho HTTP para stream MJPEG
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  server.sendContent(response);

  while (client.connected()) {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Erro: Falha na captura do frame");
      delay(100); // Espera um pouco para tentar de novo
      continue;
    }
    
    // Envia o frame atual
    String header = "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: " + String(fb->len) + "\r\n\r\n";
    server.sendContent(header);
    client.write(fb->buf, fb->len);
    server.sendContent("\r\n");
    
    // Libera a memória do frame
    esp_camera_fb_return(fb);
  }
}

// Rota raiz para teste simples
void handleRoot() {
  server.send(200, "text/plain", "ESP32-CAM Funcionando! Use a rota /stream");
}

// ==========================================
// 4. SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  Serial.println();

  // --- Configuração da Câmera ---
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  
  // --- CORREÇÃO CRÍTICA PARA ERRO 0x106 ---
  // Baixamos a frequência para 10MHz. Isso resolve 90% dos problemas de cabo/energia.
  config.xclk_freq_hz = 10000000; 
  
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Resolução QVGA (320x240) é rápida e não trava o Wi-Fi
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12; // Qualidade (0-63, menor é melhor)
  config.fb_count = 2;      // Buffer duplo

  // Inicializa a Câmera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("ERRO CRITICO: Camera init failed with error 0x%x\n", err);
    Serial.println("1. Verifique se o cabo flat laranja esta bem encaixado.");
    Serial.println("2. Verifique se a placa selecionada no Arduino e 'AI Thinker ESP32-CAM'.");
    // Fica travado aqui se der erro
    while(true) { delay(1000); }
  }

  // --- Conexão Wi-Fi ---
  WiFi.begin(ssid, password);
  Serial.print("Conectando no Wi-Fi: ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Wi-Fi Conectado!");

  // Inicia o Servidor
  server.on("/", handleRoot);
  server.on("/stream", handleStream);
  server.begin();

  // --- MOSTRA O IP PARA VOCÊ COPIAR ---
  Serial.println("=====================================");
  Serial.print("IP DA CAMERA: ");
  Serial.println(WiFi.localIP());
  Serial.println("=====================================");
  Serial.println("Copie esse IP e coloque no campo 'IP Camera' do controle do robo.");
}

void loop() {
  server.handleClient();
}