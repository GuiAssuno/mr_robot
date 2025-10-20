#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <HCSR04.h>

#define motor_esquerdo 16
#define motor_direito 17
#define pwm_esquerdo 18
#define pwm_direito 19
#define trig 27
#define echo_esq 34
#define echo_dir 35
#define ledSonarEsquerdo 4
#define ledSonarDireito 25

const char* ssid = "Mr.Robot";
const char* password = "10203090";

WebServer server(80);

// Declaração de funções
void Back();
void stop();
void Parar();
void Frente();
void Direita();
int autosafe();
void Esquerda();
String getHTML();
void Gira(bool lado);
void comando(char inst);
char mapCommand(String cmd);

// Declarando as variaveis

HCSR04 distancia_esquerda(trig, echo_esq),  //variaveis de leitura do modulo HC-SR04
  distancia_direita(trig, echo_dir);        // trigger disparado juntos pelo mesmo pino

float M_distancia[7][2];

enum { FRENTE = 0,
       BACK = 1,
       PARAR = 2 };

unsigned long espera = 0,
              Millis = 0;

int x = 0,
    y = 0,
    pwm = 0,
    velo_atual = 0;

bool modo = true,
     curva = false,
     parado = true,
     press_l = false,
     press_r = false,
     liberado = false,
     flag = false,
     direction = false;

const int safe = 12,
          marcha = 50,
          base_vira = 15,
          delay_millis = 100,
          limite_speed = 200;
    
    int sensorIndex = 0;
const int numLeituras = 7; 

void setup() {

  Serial.begin(115200);
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

  // Iniciar servidor
  server.begin();
  Serial.println("✓ Servidor web iniciado na porta 80");
  Serial.println("=========================");
  Serial.println("ESP32 WROOM PRONTO!");
  Serial.println("Access Point: " + String(ssid));
  Serial.println("IP: " + WiFi.softAPIP().toString());
  Serial.println("Interface: http://" + WiFi.softAPIP().toString());
  Serial.println("=========================");

  // =========================
  //  CONFIGURAÇÃO DOS PINOS
  // =========================
  pinMode(motor_esquerdo, OUTPUT);
  pinMode(motor_direito, OUTPUT);
  pinMode(pwm_esquerdo, OUTPUT);
  pinMode(pwm_direito, OUTPUT);
  pinMode(echo_esq, INPUT);
  pinMode(echo_dir, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(ledSonarEsquerdo, OUTPUT);
  pinMode(ledSonarDireito, OUTPUT);

  // Estados inicial
  digitalWrite(ledSonarEsquerdo, LOW);
  digitalWrite(ledSonarDireito, LOW);
  stop();
}

void loop() {
  
  //Captura de comando da pagina web
  server.handleClient();
  if (millis() - lastSensorRead >= 10) {  // Fazer leitura a cada 50ms
    lastSensorRead = millis();
    
    M_distancia[sensorIndex][0] = distancia_direita.dist();
    M_distancia[sensorIndex][1] = distancia_esquerda.dist();
    
    sensorIndex++;
    
    if (sensorIndex >= numLeituras) {
      sensorIndex = 0;
      int locker = autosafe();
      if (locker == 1) {
        flag = true; 
        stop();
      } else {
        flag = false;
      }
    }
  }
}

int autosafe() {
  float esq = 0, dir = 0;

  for (int a = 0; a < numLeituras; a++) {
    // Ignorar leituras inválidas (0 ou muito grandes)
    if (M_distancia[a][0] > 0 && M_distancia[a][0] < 300) {
      dir += M_distancia[a][0];
    }
    if (M_distancia[a][1] > 0 && M_distancia[a][1] < 300) {
      esq += M_distancia[a][1];
    }
  }

  esq /= numLeituras;
  dir /= numLeituras;

  // Debug: exibir valores no monitor serial
  Serial.printf("Distância: Esq=%.2fcm, Dir=%.2fcm\n", esq, dir);

  if ((esq < safe) || (dir < safe)) {
    digitalWrite(ledSonarEsquerdo, esq < safe ? HIGH : LOW);
    digitalWrite(ledSonarDireito, dir < safe ? HIGH : LOW);
    
    if ((esq < safe) && (dir < safe)) {
      if (!liberado) return 1;
      else return 0;
    }
  } else {
    digitalWrite(ledSonarDireito, LOW);
    digitalWrite(ledSonarEsquerdo, LOW);
  }
  return 0;
}

  char mapCommand(String cmd) {
    if (cmd.equals("F")) return 'F';
    if (cmd.equals("B")) return 'B';
    if (cmd.equals("L")) return 'L';
    if (cmd.equals("R")) return 'R';
    if (cmd.equals("A")) return 'A';
    if (cmd.equals("S")) return 'S';
    if (cmd.equals("E")) return 'E';
    if (cmd.equals("D")) return 'D';
    if (cmd.equals("X")) return 'X';
    return '\0';
  }

  //   FUNÇÕES E CONTROLES
  void comando(char inst) {
    switch (inst) {
      case 'F': Frente(); break;
      case 'B': Back(); break;
      case 'S': Parar(); break;
      case 'L':
        press_l = true;
        if (modo) Esquerda();
        else Gira(0);
        break;
      case 'R':
        press_r = true;
        if (modo) Direita();
        else Gira(1);
        break;
      case 'E':
        press_l = false;
        if (press_r) break;
        if (modo) {
          if (direction) Back();
          else Frente();
          if (parado) stop();
          else if (direction) Back();
          else Frente();
        } else {
          if (direction) Back();
          else Frente();
          if (parado) stop();
          else if (direction) Back();
          else Frente();
        }
        break;
      case 'D':
        press_r = false;
        if (press_l) break;
        if (modo) {
          if (direction) Back();
          else Frente();
          if (parado) stop();
          else if (direction) Back();
          else Frente();
        } else {
          if (direction) Back();
          else Frente();
          if (parado) stop();
          else if (direction) Back();
          else Frente();
        }
        break;
      case 'A': modo = !modo; break;
      case 'X': liberado = true; break;
    }
  }

  void Frente() {
    int velo = velo_atual;
    direction = FRENTE;
    digitalWrite(motor_direito, FRENTE);
    digitalWrite(motor_esquerdo, FRENTE);

    if (curva && parado) {
      analogWrite(pwm_direito, limite_speed);
      analogWrite(pwm_esquerdo, limite_speed);
      curva = false;
      parado = false;
      velo_atual = limite_speed;
      return;
    }

    while (velo <= limite_speed) {
      if (millis() >= espera) {
        analogWrite(pwm_direito, velo);
        analogWrite(pwm_esquerdo, velo);
        velo += marcha;
        espera = (millis() + delay_millis);
      }
      velo_atual = velo;
    }

    if (velo < limite_speed || velo > limite_speed) {
      analogWrite(pwm_direito, limite_speed);
      analogWrite(pwm_esquerdo, limite_speed);
      velo_atual = limite_speed;
    }
    parado = false;
  }

  void Back() {
    int velo = velo_atual;
    direction = BACK;
    digitalWrite(motor_direito, BACK);
    digitalWrite(motor_esquerdo, BACK);

    if (curva && parado) {
      analogWrite(pwm_direito, limite_speed);
      analogWrite(pwm_esquerdo, limite_speed);
      velo_atual = limite_speed;
      curva = false;
      parado = false;
      return;
    }

    while (velo <= limite_speed) {
      if (millis() >= espera) {
        analogWrite(pwm_direito, velo);
        analogWrite(pwm_esquerdo, velo);
        velo += marcha;
        espera = (millis() + delay_millis);
      }
      velo_atual = velo;
    }

    if (velo < limite_speed || velo > limite_speed) {
      analogWrite(pwm_direito, limite_speed);
      analogWrite(pwm_esquerdo, limite_speed);
      velo_atual = limite_speed;
    }
    parado = false;
  }

  void Esquerda() {
    digitalWrite(motor_direito, direction);
    digitalWrite(motor_esquerdo, direction);

    if (press_r && parado) {
      analogWrite(pwm, base_vira);
      curva = false;
    } else if (press_r && !parado) {
      analogWrite(pwm, limite_speed);
      curva = false;
    }

    if (parado) {
      int velo = 0;
      pwm = pwm_direito;
      while (velo < base_vira) {
        if (millis() >= espera) {
          analogWrite(pwm, velo);
          velo += marcha;
          espera = (millis() + 60);
        }
      }
    } else {
      pwm = pwm_esquerdo;
      analogWrite(pwm, velo_atual * 0.075);
    }
    curva = true;
  }

  void Direita() {
    digitalWrite(motor_direito, direction);
    digitalWrite(motor_esquerdo, direction);

    if (press_l && parado) {
      analogWrite(pwm, base_vira);
      curva = false;
    } else if (press_l && !parado) {
      analogWrite(pwm, limite_speed);
      curva = false;
    }

    if (parado) {
      int velo = 0;
      pwm = pwm_esquerdo;
      while (velo < base_vira) {
        if (millis() >= espera) {
          analogWrite(pwm, velo);
          velo += marcha;
          espera = (millis() + 60);
        }
      }
    } else {
      pwm = pwm_direito;
      analogWrite(pwm, velo_atual * 0.075);
    }
    curva = true;
  }

  void Gira(bool lado) {
    if (lado) {
      digitalWrite(motor_esquerdo, BACK);
      digitalWrite(motor_direito, FRENTE);
      analogWrite(pwm_direito, limite_speed);
      analogWrite(pwm_esquerdo, limite_speed);
    } else {
      digitalWrite(motor_direito, BACK);
      digitalWrite(motor_esquerdo, FRENTE);
      analogWrite(pwm_direito, limite_speed);
      analogWrite(pwm_esquerdo, limite_speed);
    }
  }

  void Parar() {
    int velo = velo_atual;
    while (velo > 0) {
      if (millis() >= espera) {
        velo -= 50;
        analogWrite(pwm_direito, velo);
        analogWrite(pwm_esquerdo, velo);
        velo_atual = velo;
        espera = (millis() + 50);
      }
    }
    velo_atual = 0;
    parado = true;
  }

  void stop() {
    analogWrite(pwm_direito, 0);
    analogWrite(pwm_esquerdo, 0);
    velo_atual = 0;
    parado = true;
  }

  String getHTML() {
    return R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Controle ESP32</title>
    <style>
        /* Estilos otimizados para mobile/desktop */
        body {
            font-family: Tahoma, sans-serif;
            background: linear-gradient(#370063);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            margin: 0;
            padding: 10px;
            box-sizing: border-box;
            touch-action: manipulation;
            -webkit-tap-highlight-color: transparent;
        }
        
        .container {
            background: #c3dcbe;
            border-radius: 10px;
            padding: 15px;
            box-shadow: 0 10px 25px rgba(0, 0, 0, 0.5);
            max-width: 480px;
            width: 100%;
        }
        
        .header {
            text-align: center;
            margin-bottom: 15px;
        }
        
        .header h1 {
            color: #000000;
            font-size: 1.1rem;
            margin: 0;
        }

        .main-content {
            display: grid;
            gap: 12px;
            margin-bottom: 12px;
        }
        
        .mode-selector {
           display: flex;
            gap: 8px;
            margin-bottom: 15px;
            justify-content: center;
            flex-wrap: wrap;
        }
        
        .mode-option {
            padding: 10px 16px;
            background: rgba(255, 255, 255, 0.3);
            border-radius: 8px;
            font-size: 0.85rem;
            font-weight: bold;
            cursor: pointer;
            display: flex;
            align-items: center;
        }

        .mode-selector2 {
           display: flex;
            gap: 8px;
            margin-bottom: 15px;
            justify-content: center;
            flex-wrap: wrap;
        }
        
        .mode-option2 {
            padding: 10px 16px;
            background: rgba(255, 255, 255, 0.3);
            border-radius: 8px;
            font-size: 0.85rem;
            font-weight: bold;
            cursor: pointer;
            display: flex;
            align-items: center;
        }
        
        .mode-option input[type="radio"] {
            margin-right: 8px;
            transform: scale(1.3);
            
        }
        
        .mode-option2 input[type="radio"] {
            margin-right: 8px;
            transform: scale(1.3);
            
        }

        .video-container {
            background: #09004ed0;
            border-radius: 8px;
            padding: 8px;
            display: flex;
            flex-direction: column;
            margin-bottom: 12px;
            aspect-ratio: 4/3;
            width: 100%;
            position: relative;
            max-width: 320px;
            margin-left: auto;
            margin-right: auto;
        }
        
        .video-placeholder {
            background: #000;
            border-radius: 8px;
            display: flex;
            align-items: center;
            justify-content: center;
            width: 100%;
            height: 100%;
        }
        
        .cameraStream {
            width: 100%;
            height: 100%;
            object-fit: cover;
            border-radius: 5px;
        }

       .side-controls {
            display: flex;
            flex-direction: column;
            gap: 4px;
            align-items: center;
        }

        .controls-section {
            display: grid;
            grid-template-columns: 1fr 2fr 1fr;
            gap: 12px;
            align-items: center;
        }
        
        .controls-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            grid-template-rows: repeat(2, 1fr);
            gap: 6px;
            max-width: 200px;
            margin: 0 auto;
        }

        .controls-section2 {
            display: flex;
            justify-content: center;
            align-items: center;
        }
        
        .controls-grid2 {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            grid-template-rows: repeat(2, 1fr);
            gap: 8px;
            max-width: 280px;
            margin: 0 auto;
        }
        
        .control-btn { 
            width: 50px;
            height: 50px;
            border: 1px solid #000000;
            border-radius: 12px;
            font-size: 1.2rem;
            font-weight: bold;
            cursor: pointer;
            display: flex;
            justify-content: center;
            align-items: center;
            user-select: none;
            transition: transform 0.1s;
        }

        .control-btn:active {
            transform: scale(0.95);
        }
        
        .btn-mode {
            background: #e236cb;
            color: white;
        }
        
        .btn-stop {
            background: #37cf19;
            color: white;
        }
        
        .btn-up {
            grid-column: 2;
            grid-row: 1;
            background: #3e5cd4;
            color: white;
        }
        
        .btn-left {
            grid-column:1;
            grid-row: 2;
            background: #3e5cd4;
            color: white;
        }
        
        .btn-right {
            grid-column: 3;
            grid-row: 2;
            background: #3e5cd4;
            color: white;
        }
        
        .btn-down {
            grid-column: 2;
            grid-row: 2;
            background: #3e5cd4;
            color: white;
        }
        
        .control-label {
            text-align: center;
            font-weight: bold;
            color: #000000;
            margin-top: 4px;
            font-size: 0.8rem;
        }        
        
        .camera-offline {
            background-color: #ff4444;
        }

         /* Layout específico para mobile em orientação paisagem */
        @media screen and (max-width: 900px) and (orientation: landscape) {
            body {
                padding: 5px;
                height: 100vh;
                overflow: hidden;
            }
            
            .container {
                padding: 8px;
                height: 50vh;
                max-height: 100vh;
                display: flex;
                flex-direction: column;
                max-width: 100%;
            }
            
            .header {
                margin-bottom: 8px;
            }
            
            .header h1 {
                font-size: 1.1rem;
                margin: 0;
            }
            
            .mode-selector {
                margin-bottom: 8px;
                gap: 5px;
            }
            
            .mode-option {
                padding: 6px 12px;
                font-size: 0.8rem;
            }
            
            .main-content {
                flex: 1;
                display: flex;
                gap: 10px;
                align-items: center;
                justify-content: space-between;
                margin-bottom: 4px;
            }
            
            /* Esconde o layout de retrato */
            .main-content > .video-container,
            .controls-section,
            .side-controls,
            .control-label,
            .controls-grid,
            .mode-selector,
            .controls-grid2,
            .mode-selector,
            .mode-option{
                display: none;
            }

            /* Coluna esquerda - UP, DOWN, KeyA */
            .left-controls {
                display: flex;
                flex-direction: column;
                gap: 10px;
                align-items: center;
                min-width: 80px;
            }
            
            /* Coluna centro - Stream */
            .center-content {
                flex: 1;
                display: flex;
                flex-direction: column;
                align-items: center;
                max-height: 100%;
            }
            
            .video-container {
                margin: 0;
                flex: 1;
                max-height: 180px;
                width: 100%;
                aspect-ratio: 16/9;
            }
            
            /* Coluna direita - LEFT, RIGHT, KeyS */
            .right-controls2 {
                display: flex;
                align-items:end;
                
            }

            .right-controls {
                display: flex;
                flex-direction: column;
                gap: 10px;
                align-items: center;
                min-width: 80px;
            }            

            .right-control {
                display: flex;
                flex-direction:row-reverse;
                align-items: center;
                min-width: 80px;
            }            

            .control-btn {
                width: 50px;
                height: 50px;
                font-size: 1.3rem;
            }
            
            .control-label {
                font-size: 0.7rem;
                margin-top: 2px;
            }
            
        }
        
        /* Layout para mobile em orientação retrato */
        @media (max-width: 899px) and (orientation: portrait) {
            .container {
                padding: 5px;
            }
            
            .controls-section {
                display: flex;
                grid-template-columns: 1fr;
                gap: 5px; 

                justify-content: center;
                align-items: center;
            }
            
            .side-controls {
                display: flex;
                grid-template-columns: repeat(3, 1fr);
                grid-template-rows: repeat(2, 1fr);
                gap: 8px;
                max-width: 240px;
                margin: 0 auto;
                
            }
            
            .control-btn {
                width: 55px;
                height: 55px;
                font-size: 1.5rem;
            }
            
            .controls-grid2 {
                max-width: 240px;
            }
            
            .header h1 {
                font-size: 1.1rem;
            }
            
            /* Esconder layout paisagem no modo retrato */
            .left-controls,
            .right-controls,
            .right-controls2,
            .right-control,
            .control-label,
            .mode-selector2,
            .mode-option2,
            .side-controls,
            .controls-section
            .controls-grid,
            .center-content {
                display: none;
            }
        }
        
        /* Desktop - manter layout original */
        @media (min-width: 901px) {
            .left-controls,
            .right-control,
            .right-controls,
            .right-controls2,
            .mode-selector2,
            .mode-option2,
            .controls-section2,
            .controls-grid2,
            .center-content {
                display: none;
            }
        }

    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Controle ESP32</h1>
        </div>
        
        <div class="mode-selector2">
            <label class="mode-option2">
                <input type="radio" name="mode" value="curva" checked>
                <span>Curva</span>
            </label>
            <label class="mode-option2">
                <input type="radio" name="mode" value="gira">
                <span>Eixo </span>
            </label>
        </div>

        <div class="main-content">
            <div class="video-container">
                <div class="video-placeholder">
                    <img id="cameraStream" class="cameraStream" src='http://192.168.4.100:81/stream' alt="Stream da câmera">
                </div>
            </div>

            <div class="mode-selector">
                <label class="mode-option">
                    <input type="radio" name="mode" value="curva" checked>
                    <span>Curva</span>
                </label>
                <label class="mode-option">
                    <input type="radio" name="mode" value="gira">
                    <span>Eixo </span>
                </label>
            </div>
        
            <div class="controls-section">
                <div class="side-controls">
                    <button class="control-btn btn-mode" id="keyA">
                        <div>A</div>
                    </button>
                    <div class="control-label">MODO</div>
                </div>

                <div class="controls-grid">
                    <button class="control-btn btn-up" id="up">
                        <div>↑</div>
                    </button>
                    <button class="control-btn btn-left" id="left">
                        <div>←</div>
                    </button>
                    <button class="control-btn btn-down" id="down">
                        <div>↓</div>
                    </button>                  
                    <button class="control-btn btn-right" id="right">
                        <div>→</div>
                    </button>
                </div>
            
                <div class="side-controls">
                    <button class="control-btn btn-stop" id="keyS">
                        <div>S</div>
                    </button>
                    <div class="control-label">SOLTAR</div>
                </div>
            </div> 
        
            <div class="controls-section2">
                <div class="controls-grid2">
                    <!-- Linha 1 -->
                    <button class="control-btn btn-mode" id="keyA">A</button>
                    <button class="control-btn btn-up" id="up">↑</button>
                    <button class="control-btn btn-stop" id="keyS">S</button>
                    <!-- Linha 2 -->
                    <button class="control-btn btn-left" id="left">←</button>
                    <button class="control-btn btn-down" id="down">↓</button>                  
                    <button class="control-btn btn-right" id="right">→</button>
                </div>
            </div>

            <!-- Layout específico para mobile paisagem -->
            <!-- Coluna esquerda -->
            <div class="left-controls">
                <button class="control-btn btn-mode" id="keyA">
                    <div>A</div>
                </button>
                <button class="control-btn btn-up" id="up">
                    <div>↑</div>
                </button>
                <button class="control-btn btn-down" id="down">
                    <div>↓</div>
                </button>
                <div class="control-label">MODO</div>
            </div>
            
            <!-- Coluna centro -->
            <div class="center-content">
                <div class="video-container">
                    <div class="video-placeholder">
                        <img id="cameraStream" class="cameraStream" src="http://192.168.4.1:81/stream" alt="Stream da câmera">
                    </div>
                </div>
            </div>
            
            <!-- Coluna direita -->

            <div class="right-controls2">

                <div class="right-control">
                    <button class="control-btn btn-left" id="left">
                        <div>←</div>
                    </button>
                </div>

                <div class="right-controls">
                    <button class="control-btn btn-stop" id="keyS">
                        <div>S</div>
                    </button>
                
                    <button class="control-btn btn-right" id="right">
                        <div>→</div>
                    </button>
                </div>

                <div class="control-label">START</div>
            </div>
        </div>
    </div>

    <script>
        const cameraStream = document.getElementById('cameraStream');
        
        // Estado de controle
        let isSending = false;
        const sendDelay = 100;
        let commandQueue = [];
        let cameraConnected = false;
        
        // Elementos dos botões
        const keys = {
            'ArrowUp': document.getElementById('up'),
            'ArrowDown': document.getElementById('down'),
            'ArrowLeft': document.getElementById('left'),
            'ArrowRight': document.getElementById('right'),
            'KeyA': document.getElementById('keyA'),
            'KeyS': document.getElementById('keyS')
        };
        
        // Mapeamento de comandos
        const keyPress = {
            'ArrowUp': 'F',    
            'ArrowDown': 'B',  
            'ArrowLeft': 'L',  
            'ArrowRight': 'R', 
            'KeyA': 'A',
            'KeyS': 'X'
        };

        const keyRelease = {
            'ArrowUp': 'S',  
            'ArrowDown': 'S', 
            'ArrowLeft': 'E', 
            'ArrowRight': 'D',
            'KeyA': null,   
            'KeyS': null     
        };

        function toggleMode() {
            const currentMode = document.querySelector('input[name="mode"]:checked');
            if (currentMode.value === "curva") {
                document.querySelector('input[value="gira"]').checked = true;
            } else {
                document.querySelector('input[value="curva"]').checked = true;
            }
        }
        
        // Sistema de envio de comandos com fila que processa a ordem
        function processCommandQueue() {
            if (commandQueue.length > 0 && !isSending) {
                isSending = true;
                const commandToSend = commandQueue.shift(); // Pega o primeiro da fila
                
                fetch('/command?cmd=' + commandToSend)
                    .then(() => {
                        console.log(`Comando enviado: ${commandToSend}`);
                    })
                    .catch(error => {
                        console.error(`Erro ao enviar comando ${commandToSend}:`, error);
                    })
                    .finally(() => {
                        setTimeout(() => {
                            isSending = false;
                            processCommandQueue(); // Tenta processar o próximo da fila
                        }, sendDelay);
                    });
            }
        }

        // Eventos de teclado
        const pressedKeys = {};

        function handleKeyDown(e) {
            if (e.repeat) return;
            if (keyPress[e.code]) {
                e.preventDefault();
                if (e.code === 'KeyA' || e.code === 'KeyS') {
                    if (!pressedKeys[e.code]) {
                        if (keys[e.code]) keys[e.code].classList.add('active');
                        commandQueue.push(keyPress[e.code]);
                        processCommandQueue();
                        pressedKeys[e.code] = true;
                    }
                } else {
                    if (!pressedKeys[e.code]) {
                        if (keys[e.code]) keys[e.code].classList.add('active');
                        commandQueue.push(keyPress[e.code]);
                        processCommandQueue();
                        pressedKeys[e.code] = true;
                    }
                }
                
                if (e.code === 'KeyA') {
                    toggleMode();
                }
            }
        }
        
        function handleKeyUp(e) {
            if (keyRelease[e.code] && keyRelease[e.code] !== null) {
                e.preventDefault();
                if (keys[e.code]) keys[e.code].classList.remove('active'); 

                if (pressedKeys[e.code]) {
                    commandQueue.push(keyRelease[e.code]);
                    processCommandQueue();
                    delete pressedKeys[e.code];
                }
            }
            if ((e.code === 'KeyA' || e.code === 'KeyS') && pressedKeys[e.code]) {
                if (keys[e.code]) keys[e.code].classList.remove('active');
                delete pressedKeys[e.code];
            }
        }
        
        // Eventos de toque/botão
        function setupButtonEvents() {
            const buttons = {
                'up': { press: 'F', release: 'S' },
                'down': { press: 'B', release: 'S' },
                'left': { press: 'L', release: 'E' },
                'right': { press: 'R', release: 'D' },
                'keyA': { press: 'A', release: null },
                'keyS': { press: 'X', release: null }
            };
            
            Object.entries(buttons).forEach(([id, cmd]) => {
                const button = document.getElementById(id);
                if (button) {
                    button.addEventListener('mousedown', () => {
                        button.classList.add('active');
                        commandQueue.push(cmd.press);
                        processCommandQueue();
                        if (id === 'keyA') toggleMode();
                    });
                    
                    button.addEventListener('mouseup', () => {
                        button.classList.remove('active');
                        if (cmd.release) {
                            commandQueue.push(cmd.release);
                            processCommandQueue();
                        }
                    });
                    
                    button.addEventListener('touchstart', (e) => {
                        e.preventDefault();
                        button.classList.add('active');
                        commandQueue.push(cmd.press);
                        processCommandQueue();
                        if (id === 'keyA') toggleMode();
                    });
                    
                    button.addEventListener('touchend', (e) => {
                        e.preventDefault();
                        button.classList.remove('active');
                        if (cmd.release) {
                            commandQueue.push(cmd.release);
                            processCommandQueue();
                        }
                    });
                }
            });
        }
        
        // Inicialização
        document.addEventListener('DOMContentLoaded', () => {
            document.addEventListener('keydown', handleKeyDown);
            document.addEventListener('keyup', handleKeyUp);
            setupButtonEvents();
            
            // Reconexão da câmera
            cameraStream.onerror = () => {
                setTimeout(() => {
                    if (cameraConnected) {
                    cameraStream.src = 'http://192.168.4.100:81/stream?t=' + Date.now();
                    }
                }, 2000);
            };
        });
    </script>
</body>
</html>
)rawliteral";
  }