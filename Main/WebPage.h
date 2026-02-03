#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <WebServer.h>
#include "Config.h"

// Nota: No WebPage.h não precisamos declarar 'server' se ele já é extern no GerServer.h
// Mas para evitar erro circular, deixamos sem declarar server aqui, apenas o HTML.

const char* html_page = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Controle Robô</title>
    <style>
        /* SEU CSS CONTINUA IDÊNTICO - MANTENHA O QUE VOCÊ JÁ FEZ */
        * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }
        body { font-family: Tahoma, sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh; display: flex; justify-content: center; align-items: center; margin: 0; padding: 5px; touch-action: none; overflow: hidden; }
        .container { background: rgba(255, 255, 255, 0.95); border-radius: 12px; padding: 12px; box-shadow: 0 10px 20px rgba(0, 0, 0, 0.2); max-width: 600px; width: 100%; }
        
        /* ... MANTENHA TODO O SEU CSS AQUI (Resumi para caber na resposta) ... */
        /* ... Copie do seu arquivo original ... */
        
        * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }
        body { font-family: Tahoma, sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh; display: flex; justify-content: center; align-items: center; margin: 0; padding: 5px; touch-action: none; overflow: hidden; }
        .container { background: rgba(255, 255, 255, 0.95); border-radius: 12px; padding: 12px; box-shadow: 0 10px 20px rgba(0, 0, 0, 0.2); max-width: 600px; width: 100%; }
        .header { text-align: center; margin-bottom: 12px; display: flex; justify-content: space-between; align-items: center; }
        .header h1 { color: #333; font-size: 1.0rem; margin: 0; }
        .status { background: #dc3545; color: white; padding: 4px 12px; border-radius: 15px; font-size: 0.8rem; }
        
        /* VÍDEO */
        .video-desktop, .video-placeholder { background: #000; border-radius: 8px; display: flex; align-items: center; justify-content: center; color: white; position: relative; width: 100%; aspect-ratio: 4/3; }
        .cameraStream { width: 100%; height: 100%; object-fit: contain; border-radius: 5px; }
        
        /* CONTROLES BÁSICOS */
        .controls-section { display: grid; grid-template-columns: 1fr 2fr 1fr; gap: 10px; align-items: center; margin: 15px 0; }
        .controls-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 5px; max-width: 160px; margin: 0 auto; }
        .control-btn { width: 45px; height: 45px; border-radius: 12px; border: 2px solid #333; background: #fff; font-weight: bold; font-size: 1.2rem; cursor: pointer; }
        .control-btn:active, .control-btn.active { background: #ddd; transform: scale(0.95); }       
        /* --- NOVO: ÁREA DE VELOCIDADE (Restaurado) --- */
        .speed-control { display: flex; align-items: center; justify-content: center; gap: 10px; margin: 10px 0; background: #f0f2f5; padding: 8px; border-radius: 10px; }
        .speed-label { font-weight: bold; color: #555; font-size: 0.9rem; }
        .speed-btn { width: 35px; height: 35px; font-size: 1.2rem; padding: 0;line-height: 0; }

        /* JOYSTICK */
        .joystick-container { position: relative; width: 100px; height: 100px; background: radial-gradient(circle, #f8f9fa 0%, #e9ecef 100%); border: 2px solid #adb5bd; border-radius: 50%; margin: 10px auto; touch-action: none; }
        .joystick-knob { position: absolute; width: 40px; height: 40px; background: radial-gradient(circle, #e91e63 0%, #c2185b 100%); border: 2px solid #fff; border-radius: 50%; top: 50%; left: 50%; transform: translate(-50%, -50%); pointer-events: none; }
        
        /* Layouts Responsivos */
        .landscape-layout, .portrait-layout { display: none; }
        
        @media screen and (orientation: portrait) {
            .video-desktop, .controls-section { display: none; }
            .portrait-layout { display: flex; flex-direction: column; gap: 10px; }
            .portrait-buttons { display: flex; justify-content: center; gap: 20px; }
        }
        @media screen and (orientation: landscape) {
             .video-desktop, .controls-section { display: none; }
             .landscape-layout { display: grid; grid-template-columns: 1fr 1.5fr 1fr; align-items: center; height: 90vh; }
             .landscape-controls { display: flex; flex-direction: column; align-items: center; justify-content: center; }
             .joystick-container { width: 80px; height: 80px; }
             .joystick-knob { width: 30px; height: 30px; }
        }
        @media (min-width: 900px) {
            .video-desktop, .controls-section { display: grid; }
            .portrait-layout, .landscape-layout { display: none; }
        }
    </style>
</head>
<body>
   <div class="container">        
        <div class="header">
            <h1>Controle Robô</h1>
            <span id="status" class="status">Desconectado</span>
        </div>
        
        <div class="speed-control">
            <button class="control-btn speed-btn" onclick="changeSpeed(-10)">-</button>
            <span class="speed-label">Velocidade Máx: <span id="speedPct">100</span>%</span>
            <button class="control-btn speed-btn" onclick="changeSpeed(+10)">+</button>
        </div>
        
        <div class="video-desktop">
            <img class="cameraStream" id="camDesktop" alt="Video">
        </div>
        <div class="controls-section">
            <div class="side-controls"><button class="control-btn" onclick="send('A')">M</button></div>
            <div class="controls-grid">
                <div></div><button class="control-btn" onmousedown="send('F')" onmouseup="send('S')">▲</button><div></div>
                <button class="control-btn" onmousedown="send('L')" onmouseup="send('S')">◄</button>
                <button class="control-btn" onmousedown="send('S')">■</button>
                <button class="control-btn" onmousedown="send('R')" onmouseup="send('S')">►</button>
                <div></div><button class="control-btn" onmousedown="send('B')" onmouseup="send('S')">▼</button><div></div>
            </div>
            <div class="side-controls"><button class="control-btn" onclick="send('X')">X</button></div>
        </div>
        
        <div class="portrait-layout">
            <div class="video-placeholder">
                <img class="cameraStream" id="camPortrait" alt="Video">
            </div>
            <div class="portrait-buttons">
                <button class="control-btn" onclick="send('A')">M</button>
                <button class="control-btn" onclick="send('X')">X</button>
                 <button class="control-btn speed-btn" onclick="changeSpeed(-10)">-</button>
                <button class="control-btn speed-btn" onclick="changeSpeed(+10)">+</button>
            </div>
            <div class="joystick-container" id="joyCenter">
                <div class="joystick-knob" id="knobCenter"></div>
            </div>
        </div>
        
        <div class="landscape-layout">
            <div class="landscape-controls">
                <button class="control-btn" onclick="send('A')" style="margin-bottom:20px">M</button>
                <div class="joystick-container" id="joyLeft">
                    <div class="joystick-knob" id="knobLeft"></div>
                </div>
            </div>
            <div class="video-placeholder">
                <img class="cameraStream" id="camLandscape" alt="Video">
            </div>
            <div class="landscape-controls">
                <button class="control-btn" onclick="send('X')" style="margin-bottom:20px">X</button>
                <div class="joystick-container" id="joyRight">
                    <div class="joystick-knob" id="knobRight"></div>
                </div>
            </div>
        </div>
    </div>

<script>
    // ===== WEBSOCKET (O Segredo da Velocidade) =====
    var ws;
    var lastSend = 0;
    var currentSpeedPct = 100;

    function changeSpeed(delta) {
        currentSpeedPct += delta;
        // Trava entre 10% e 100%
        if (currentSpeedPct > 100) currentSpeedPct = 100;
        if (currentSpeedPct < 10) currentSpeedPct = 10;
        
        // Atualiza na tela
        document.getElementById('speedPct').innerText = currentSpeedPct;
        
        // Envia para o ESP32 via WebSocket (Protocolo "V:xx")
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send("V:" + currentSpeedPct);
        }
    }

    // Variáveis Globais de Estado dos Joysticks
    // (Precisamos guardar o estado de cada um para combinar no landscape)
    var joyState = {
        center: { x: 0, y: 0 },
        left:   { x: 0, y: 0 },
        right:  { x: 0, y: 0 }
    };

    function initWS() {
        // Conecta no IP atual (host) porta 81
        ws = new WebSocket('ws://' + location.hostname + ':81/');
        
        ws.onopen = function() {
            document.getElementById('status').innerText = 'Conectado';
            document.getElementById('status').style.background = '#28a745';
        };
        ws.onclose = function() {
            document.getElementById('status').innerText = 'Desconectado';
            document.getElementById('status').style.background = '#dc3545';
            setTimeout(initWS, 1000); // Tenta reconectar
        };
    }

    // Função única para enviar comandos (Botão ou Joystick)
    function send(cmd) {
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send(cmd);
        }
    }

    // Função Throttle para Joystick (Evita floodar o ESP32)
    function sendJoy(x, y) {
        var now = Date.now();
        if (now - lastSend > 50) { // Max 20 envios por segundo
            if (ws && ws.readyState === WebSocket.OPEN) {
                // Formato: "J:x:y" (Ex: "J:0:100")
                ws.send("J:" + Math.round(x) + ":" + Math.round(y));
            }
            lastSend = now;
        }
    }

    // ===== LÓGICA DE MULTITOUCH ROBUSTA =====
    
    // Configura um joystick
    function setupJoystick(containerId, knobId, type) {
        const container = document.getElementById(containerId);
        const knob = document.getElementById(knobId);
        
        let touchId = null; // ID do dedo que "donou" este joystick
        let startX, startY;
        const maxDist = 35; // Raio máximo de movimento (px)

        // Função interna para mover o knob e calcular valores
        function update(clientX, clientY) {
            const rect = container.getBoundingClientRect();
            const centerX = rect.width / 2;
            const centerY = rect.height / 2;
            
            let dx = clientX - rect.left - centerX;
            let dy = clientY - rect.top - centerY;
            
            // Limitadores por Tipo (Trava Eixo)
            if (type === 'left') dx = 0;   // Só Y (Frente/Ré)
            if (type === 'right') dy = 0;  // Só X (Esq/Dir)
            
            // Limita ao círculo
            const distance = Math.sqrt(dx*dx + dy*dy);
            if (distance > maxDist) {
                dx = (dx / distance) * maxDist;
                dy = (dy / distance) * maxDist;
            }
            
            // Move a bolinha (Visual)
            knob.style.transform = `translate(-50%, -50%) translate(${dx}px, ${dy}px)`;
            
            // Calcula porcentagem (-100 a 100)
            // Nota: Y do HTML é invertido (cima é negativo), invertemos aqui para ficar intuitivo
            let valX = (dx / maxDist) * 100;
            let valY = -(dy / maxDist) * 100;
            
            // Atualiza estado global
            joyState[type] = { x: valX, y: valY };
            
            // DECISÃO DE ENVIO:
            // Se estiver em Landscape, combina Left(Y) + Right(X)
            if (window.innerHeight < window.innerWidth && window.innerWidth < 900) {
                 // Landscape: Pega Y da Esquerda e X da Direita
                 sendJoy(joyState.right.x, joyState.left.y);
            } else {
                 // Portrait/Center: Manda o próprio
                 sendJoy(valX, valY);
            }
        }

        function reset() {
            touchId = null;
            knob.style.transform = `translate(-50%, -50%)`;
            joyState[type] = { x: 0, y: 0 };
            
            // Se soltou, manda parar (J:0:0) Imediatamente
            // Mas verifica se o OUTRO joystick ainda está ativo no modo Landscape
            if (window.innerHeight < window.innerWidth && window.innerWidth < 900) {
                 sendJoy(joyState.right.x, joyState.left.y);
            } else {
                 sendJoy(0, 0);
            }
        }

        // --- EVENT LISTENERS (TOUCH) ---
        container.addEventListener('touchstart', function(e) {
            e.preventDefault();
            // Pega o último toque que caiu aqui
            const touch = e.changedTouches[e.changedTouches.length - 1];
            touchId = touch.identifier;
            update(touch.clientX, touch.clientY);
        }, {passive: false});

        container.addEventListener('touchmove', function(e) {
            e.preventDefault();
            // Procura o toque que tem o ID do nosso joystick
            for (let i = 0; i < e.changedTouches.length; i++) {
                if (e.changedTouches[i].identifier === touchId) {
                    update(e.changedTouches[i].clientX, e.changedTouches[i].clientY);
                    break;
                }
            }
        }, {passive: false});

        container.addEventListener('touchend', function(e) {
            for (let i = 0; i < e.changedTouches.length; i++) {
                if (e.changedTouches[i].identifier === touchId) {
                    reset();
                    break;
                }
            }
        });
        
        // --- EVENT LISTENERS (MOUSE - Para teste no PC) ---
        // (Só funciona se clicar e arrastar, não suporta multi-mouse claro)
        let isMouseDown = false;
        container.addEventListener('mousedown', function(e) { isMouseDown = true; update(e.clientX, e.clientY); });
        window.addEventListener('mousemove', function(e) { if(isMouseDown) update(e.clientX, e.clientY); });
        window.addEventListener('mouseup', function(e) { if(isMouseDown) { isMouseDown = false; reset(); } });
    }

    // ===== INICIALIZA TUDO =====
    window.onload = function() {
        initWS();
        
        // Configura Câmeras (mDNS ou IP atual)
        // Se usar mDNS, mude para: 'http://cam-robo.local:81/stream'
        // Se usar IP fixo, mude aqui.
        // Se usar IP dinâmico (mesmo do controle), use location.hostname:
        var camURL = 'http://' + location.hostname + ':81/stream'; 
        
        document.getElementById('camDesktop').src = camURL;
        document.getElementById('camPortrait').src = camURL;
        document.getElementById('camLandscape').src = camURL;

        // Inicia Joysticks
        setupJoystick('joyCenter', 'knobCenter', 'center'); // Retrato (Livre)
        setupJoystick('joyLeft',   'knobLeft',   'left');   // Paisagem (Só Y)
        setupJoystick('joyRight',  'knobRight',  'right');  // Paisagem (Só X)
        
        // Teclado (PC)
        document.addEventListener('keydown', function(e) {
            if(e.repeat) return;
            if(e.key === "ArrowUp") send('F');
            if(e.key === "ArrowDown") send('B');
            if(e.key === "ArrowLeft") send('L');
            if(e.key === "ArrowRight") send('R');
            if(e.key === " ") send('S');
        });
        document.addEventListener('keyup', function(e) {
            send('S');
        });
    };
</script>
</body>
</html>
)rawliteral";

#endif