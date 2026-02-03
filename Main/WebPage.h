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
        /* === SEU CSS ORIGINAL DO WEB.HTML (INTACTO) === */
        * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }
        body { font-family: Tahoma, sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh; display: flex; justify-content: center; align-items: center; margin: 0; padding: 5px; touch-action: manipulation; }
        .container { background: rgba(255, 255, 255, 0.95); border-radius: 12px; padding: 12px; box-shadow: 0 10px 20px rgba(0, 0, 0, 0.2); max-width: 600px; width: 100%; }
        .header { text-align: center; margin-bottom: 12px; }
        .header h1 { color: #333; font-size: 1.0rem; margin: 0; }
        .status { background: #dc3545; color: white; padding: 4px 12px; border-radius: 15px; font-size: 0.8rem; display: inline-block; } /* Começa vermelho */
        .status.connected { background: #28a745; } /* Verde quando conecta */
        
        /* VIDEO */
        .video-placeholder { background: #000; border-radius: 8px; display: flex; align-items: center; justify-content: center; color: white; font-size: 0.8rem; aspect-ratio: 4/3; margin: 5px 0; position: relative; }
        .video-desktop { background: #000; border-radius: 8px; display: flex; align-items: center; justify-content: center; color: white; font-size: 0.9rem; aspect-ratio: 4/3; margin: 10px 0; position: relative; max-width: 320px; margin-right: auto; margin-left: auto; }
        .cameraStream { width: 100%; height: 100%; object-fit: cover; border-radius: 5px; }
        .video-status { position: absolute; background: rgba(0, 0, 0, 0.8); color: white; padding: 4px 8px; border-radius: 4px; font-size: 0.7rem; }

        /* MODOS */
        .mode-display { text-align: center; margin: 10px 0; padding: 8px; background: #f8f9fa; border-radius: 6px; }
        .mode-text { font-size: 0.9rem; font-weight: bold; color: #495057; }
        
        /* GRIDS DE CONTROLE */
        .controls-section { display: grid; grid-template-columns: 1fr 2fr 1fr; gap: 10px; align-items: center; margin: 15px 0; }
        .controls-grid { display: grid; grid-template-columns: repeat(3, 1fr); grid-template-rows: repeat(2, 1fr); gap: 5px; max-width: 160px; margin: 0 auto; }
        .side-controls { display: flex; flex-direction: column; gap: 4px; align-items: center; }
        
        /* BOTÕES */
        .control-btn { width: 45px; height: 45px; border: 2px solid #333; border-radius: 12px; font-size: 1rem; font-weight: bold; cursor: pointer; display: flex; justify-content: center; align-items: center; user-select: none; transition: all 0.15s ease; background: #fff; color: #333; }
        .control-btn:active, .control-btn.active { transform: scale(0.9); box-shadow: inset 0 2px 6px rgba(0, 0, 0, 0.2); }
        .btn-mode { background: #e91e63; color: white; border-color: #c2185b; }
        .btn-stop { background: #4caf50; color: white; border-color: #388e3c; }
        .btn-up, .btn-down, .btn-left, .btn-right { background: #2196f3; color: white; border-color: #1976d2; }
        .btn-up { grid-column: 2; grid-row: 1; }
        .btn-left { grid-column: 1; grid-row: 2; }
        .btn-right { grid-column: 3; grid-row: 2; }
        .btn-down { grid-column: 2; grid-row: 2; }
        .control-label { text-align: center; font-weight: bold; color: #666; margin-top: 3px; font-size: 0.7rem; }

        /* JOYSTICK */
        .joystick-container { position: relative; width: 90px; height: 90px; background: radial-gradient(circle, #f8f9fa 0%, #e9ecef 100%); border: 2px solid #adb5bd; border-radius: 50%; margin: 10px auto; box-shadow: inset 0 2px 8px rgba(0, 0, 0, 0.1); transition: border-color 0.2s ease; touch-action: none; }
        .joystick-container.active { border-color: #e91e63; box-shadow: inset 0 2px 8px rgba(0, 0, 0, 0.1), 0 0 10px rgba(233, 30, 99, 0.2); }
        .joystick-knob { position: absolute; width: 35px; height: 35px; background: radial-gradient(circle, #e91e63 0%, #c2185b 100%); border: 2px solid #fff; border-radius: 50%; cursor: pointer; box-shadow: 0 2px 6px rgba(0, 0, 0, 0.2); transition: all 0.2s ease; top: 50%; left: 50%; transform: translate(-50%, -50%); touch-action: none; }
        .joystick-knob:active { transform: translate(-50%, -50%) scale(1.1); }
        .joystick-label { text-align: center; margin-top: 8px; font-weight: 600; color: #495057; font-size: 0.75rem; }
        
        /* VELOCIDADE */
        .speed-section { background: #f8f9fa; border-radius: 10px; padding: 10px; margin: 10px 0; text-align: center; }
        .speed-display { margin-bottom: 8px; }
        .speed-label { font-weight: bold; color: #495057; margin-right: 8px; font-size: 0.9rem; }
        .speed-value { font-size: 1rem; font-weight: bold; color: #e91e63; }
        .btn-speed { background: #17a2b8; color: white; border-color: #138496; width: 40px; height: 40px; font-size: 1.1rem; }
        
        /* RESPONSIVIDADE (Portait/Landscape) */
        .landscape-layout, .portrait-layout { display: none; }
        
        @media screen and (max-width: 768px) and (orientation: portrait) {
            .controls-section, .speed-section, .video-desktop { display: none; }
            .video-placeholder { max-width: none; }
            .portrait-layout { display: flex; flex-direction: column; gap: 12px; }
            .portrait-buttons { display: flex; justify-content: space-around; gap: 15px; margin: 8px 0; }
            .portrait-controls { display: flex; justify-content: center; }
        }
        
        @media screen and (max-width: 900px) and (orientation: landscape) {
            .controls-section, .speed-section, .video-desktop { display: none; }
            .landscape-layout { display: grid; grid-template-columns: 1fr 1.5fr 1fr; gap: 8px; align-items: center; }
            .landscape-controls { display: flex; flex-direction: column; gap: 10px; align-items: center; }
            .container { max-width: 100%; height: auto; min-height: 95vh; border-radius: 0; padding: 8px; }
            .joystick-container { width: 80px; height: 80px; margin: 8px auto; }
            .joystick-knob { width: 30px; height: 30px; }
            .video-placeholder { max-height: 460px; margin: 5px 0; }
            .control-btn { width: 38px; height: 38px; font-size: 0.9rem; }
            .control-label { font-size: 0.65rem; }
        }
    </style>
</head>
<body>
    <div class="container">        
        <div class="mode-display">
            <div class="header">
                <h1>Controle ESP32 <span id="status" class="status">Off</span></h1>
            </div>
            <div class="mode-text"><span id="modeText">Curva</span></div>
        </div>
        
        <div class="video-desktop">
            <img id="cameraStreamDesktop" class="cameraStream" alt="Stream da câmera">
            <div id="videoStatusDesktop" class="video-status">Conectando...</div>
        </div>
        
        <div class="speed-section">
            <div class="speed-display">
                <span class="speed-label">Velocidade:</span>
                <span id="speedValue" class="speed-value">100%</span>
            </div>
        </div>
        
        <div class="controls-section">
            <div class="side-controls">
                <button class="control-btn btn-mode" id="btnMode">M</button>
                <div class="control-label">MODO</div>
                <button class="control-btn btn-speed" id="btnSpeedUpAlt">⇧</button>
                <div class="control-label">ACELERAR</div>
            </div>
            <div class="controls-grid">
                <button class="control-btn btn-up" id="btnUp">↑</button>
                <button class="control-btn btn-left" id="btnLeft">←</button>
                <button class="control-btn btn-down" id="btnDown">↓</button>
                <button class="control-btn btn-right" id="btnRight">→</button>
            </div>
            <div class="side-controls">
                <button class="control-btn btn-stop" id="btnStop">X</button>
                <div class="control-label">DESBLOQ</div>
                <button class="control-btn btn-speed" id="btnSpeedDownAlt">⇩</button>
                <div class="control-label">REDUZIR</div>
            </div>
        </div>
        
        <div class="portrait-layout">
            <div class="video-placeholder">
                <img id="cameraStreamPortrait" class="cameraStream" alt="Stream da câmera">
                <div id="videoStatusPortrait" class="video-status">Carregando...</div>
            </div>
            <div class="portrait-buttons">
                <button class="control-btn btn-mode" id="btnModePortrait">M</button>
                <button class="control-btn btn-stop" id="btnStopPortrait">X</button>
                <button class="control-btn btn-speed" id="btnSpeedUpMob">+</button>
                <button class="control-btn btn-speed" id="btnSpeedDownMob">-</button>
            </div>
            <div class="portrait-controls">
                <div class="joystick-container" id="centerJoystick">
                    <div class="joystick-knob" id="centerKnob"></div>
                </div>
            </div>
        </div>
        
        <div class="landscape-layout">
            <div class="landscape-controls">
                <button class="control-btn btn-mode" id="btnModeLandscape">M</button>
                <div class="joystick-container" id="leftJoystick">
                    <div class="joystick-knob" id="leftKnob"></div>
                    <div class="joystick-label">Frente/Ré</div>
                </div>
            </div>
            <div class="video-placeholder">
                <img id="cameraStreamLandscape" class="cameraStream" alt="Stream da câmera">
                <div id="videoStatusLandscape" class="video-status">Carregando...</div>
            </div>
            <div class="landscape-controls">
                <button class="control-btn btn-stop" id="btnStopLandscape">X</button>
                <div class="joystick-container" id="rightJoystick">
                    <div class="joystick-knob" id="rightKnob"></div>
                    <div class="joystick-label">Esq/Dir</div>
                </div>
            </div>
        </div>
    </div>

<script>
    // ===== WEBSOCKET ENGINE (O Cérebro Rápido) =====
    var ws;
    var lastSend = 0;
    var currentSpeedPct = 100;
    var joyState = { center: { x: 0, y: 0 }, left: { x: 0, y: 0 }, right: { x: 0, y: 0 } };

    // Inicia Conexão
    function initWS() {
        ws = new WebSocket('ws://' + location.hostname + ':81/');
        ws.onopen = function() {
            var s = document.getElementById('status');
            s.innerText = 'On';
            s.classList.add('connected');
        };
        ws.onclose = function() {
            var s = document.getElementById('status');
            s.innerText = 'Off';
            s.classList.remove('connected');
            setTimeout(initWS, 1000);
        };
    }

    // Envia comando simples (Letra)
    function send(cmd) {
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send(cmd);
        }
    }

    // Controle de Velocidade
    function changeSpeed(delta) {
        currentSpeedPct += delta;
        if (currentSpeedPct > 100) currentSpeedPct = 100;
        if (currentSpeedPct < 10) currentSpeedPct = 10;
        
        document.getElementById('speedValue').innerText = currentSpeedPct + "%";
        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send("V:" + currentSpeedPct);
        }
    }

    // Envia Joystick com Throttle (50ms)
    function sendJoy(x, y) {
        var now = Date.now();
        if (now - lastSend > 50) { 
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send("J:" + Math.round(x) + ":" + Math.round(y));
            }
            lastSend = now;
        }
    }

    // ===== LÓGICA DO JOYSTICK (Multitouch Real) =====
    function setupJoystick(containerId, knobId, type) {
        const container = document.getElementById(containerId);
        const knob = document.getElementById(knobId);
        let touchId = null; 
        const maxDist = 35; // Raio do joystick no CSS

        function update(clientX, clientY) {
            const rect = container.getBoundingClientRect();
            const centerX = rect.width / 2;
            const centerY = rect.height / 2;
            let dx = clientX - rect.left - centerX;
            let dy = clientY - rect.top - centerY;
            
            if (type === 'left') dx = 0;   // Trava Eixo X
            if (type === 'right') dy = 0;  // Trava Eixo Y
            
            const distance = Math.sqrt(dx*dx + dy*dy);
            if (distance > maxDist) {
                dx = (dx / distance) * maxDist;
                dy = (dy / distance) * maxDist;
            }
            
            knob.style.transform = `translate(-50%, -50%) translate(${dx}px, ${dy}px)`;
            
            // Converte para -100 a +100
            let valX = (dx / maxDist) * 100;
            let valY = -(dy / maxDist) * 100; // Inverte Y HTML
            
            joyState[type] = { x: valX, y: valY };
            
            // Se for Landscape, combina os dois joysticks
            if (window.innerHeight < window.innerWidth && window.innerWidth < 900) {
                 sendJoy(joyState.right.x, joyState.left.y);
            } else {
                 sendJoy(valX, valY);
            }
        }

        function reset() {
            touchId = null;
            knob.style.transform = `translate(-50%, -50%)`;
            joyState[type] = { x: 0, y: 0 };
            
            if (window.innerHeight < window.innerWidth && window.innerWidth < 900) {
                 sendJoy(joyState.right.x, joyState.left.y);
            } else {
                 sendJoy(0, 0);
            }
        }

        // Touch Events
        container.addEventListener('touchstart', function(e) {
            e.preventDefault();
            const touch = e.changedTouches[e.changedTouches.length - 1];
            touchId = touch.identifier;
            update(touch.clientX, touch.clientY);
        }, {passive: false});

        container.addEventListener('touchmove', function(e) {
            e.preventDefault();
            for (let i = 0; i < e.changedTouches.length; i++) {
                if (e.changedTouches[i].identifier === touchId) {
                    update(e.changedTouches[i].clientX, e.changedTouches[i].clientY);
                    break;
                }
            }
        }, {passive: false});

        container.addEventListener('touchend', function(e) {
            for (let i = 0; i < e.changedTouches.length; i++) {
                if (e.changedTouches[i].identifier === touchId) { reset(); break; }
            }
        });
        
        // Mouse Events
        let isMouseDown = false;
        container.addEventListener('mousedown', function(e) { isMouseDown = true; update(e.clientX, e.clientY); });
        window.addEventListener('mousemove', function(e) { if(isMouseDown) update(e.clientX, e.clientY); });
        window.addEventListener('mouseup', function(e) { if(isMouseDown) { isMouseDown = false; reset(); } });
    }

    // ===== INICIALIZAÇÃO =====
    window.onload = function() {
        initWS();
        
        // Configura Câmera Dinâmica (IP Automático)
        var camURL = 'http://' + location.hostname + ':81/stream'; 
        document.getElementById('cameraStreamDesktop').src = camURL;
        document.getElementById('videoStatusDesktop').style.display = 'none'; // Esconde status se carregar
        document.getElementById('cameraStreamPortrait').src = camURL;
        document.getElementById('cameraStreamLandscape').src = camURL;

        // Inicia Joysticks
        setupJoystick('centerJoystick', 'centerKnob', 'center'); 
        setupJoystick('leftJoystick',   'leftKnob',   'left');   
        setupJoystick('rightJoystick',  'rightKnob',  'right');  

        // Conecta Botões Físicos (Desktop) aos Comandos
        // Helper para conectar evento de pressionar/soltar
        function btnLink(id, press, release) {
            var el = document.getElementById(id);
            if(!el) return;
            el.onmousedown = function() { send(press); el.classList.add('active'); };
            el.onmouseup   = function() { if(release) send(release); el.classList.remove('active'); };
            el.ontouchstart= function(e) { e.preventDefault(); send(press); el.classList.add('active'); };
            el.ontouchend  = function(e) { e.preventDefault(); if(release) send(release); el.classList.remove('active'); };
        }
        
        // Helper para botões de clique simples
        function clickLink(id, cmd) {
            var el = document.getElementById(id);
            if(!el) return;
            el.onclick = function() { send(cmd); };
        }
        
        // Helper para Velocidade
        function spdLink(id, delta) {
            var el = document.getElementById(id);
            if(!el) return;
            el.onclick = function() { changeSpeed(delta); };
        }

        // Mapeamento dos Botões do SEU HTML
        btnLink('btnUp', 'F', 'S');
        btnLink('btnDown', 'B', 'S');
        btnLink('btnLeft', 'L', 'S');
        btnLink('btnRight', 'R', 'S');
        
        clickLink('btnMode', 'A'); // A = Alternar Modo
        clickLink('btnStop', 'X'); // X = Destravar
        clickLink('btnModePortrait', 'A');
        clickLink('btnStopPortrait', 'X');
        clickLink('btnModeLandscape', 'A');
        clickLink('btnStopLandscape', 'X');

        // Botões de Velocidade
        spdLink('btnSpeedUpAlt', 10);
        spdLink('btnSpeedDownAlt', -10);
        spdLink('btnSpeedUpMob', 10);
        spdLink('btnSpeedDownMob', -10);
        
        // Teclado PC
        document.addEventListener('keydown', function(e) {
            if(e.repeat) return;
            if(e.key === "ArrowUp") send('F');
            if(e.key === "ArrowDown") send('B');
            if(e.key === "ArrowLeft") send('L');
            if(e.key === "ArrowRight") send('R');
            if(e.key === " ") send('S');
        });
        document.addEventListener('keyup', function(e) { send('S'); });
    };
</script>
</body>
</html>
)rawliteral";

#endif