/*
 * PROJETO MQTT - ESP32 COM LDR
 * Características:
 * - Arquitetura não-bloqueante (sem delay())
 * - Reconexão automática (WiFi & MQTT)
 * - LWT (Última Vontade e Testamento) para monitoramento de estado
 * - Serialização de dados JSON
 * - Tratamento remoto de comandos JSON
 * - Leitura de sensor LDR com calibração
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ==========================================
// 1. CONFIGURAÇÕES (Edite estas)
// ==========================================
const char* ssid = SSID;
const char* password = PASSWORD;

// Configurações do Broker MQTT
const char* mqtt_server = "broker.emqx.io";  // Broker público
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_pass = "";

// ID único do dispositivo
const char* device_id = "ESP32_LDR_Unit_01";

// Tópicos
const char* topic_telemetry = "iot/esp32/ldr";      // Dados do sensor
const char* topic_command = "esp32/unit01/cmd";     // Comandos recebidos
const char* topic_status = "esp32/unit01/status";   // LWT

// ==========================================
// 2. PINOS E VARIÁVEIS GLOBAIS
// ==========================================
#define LDR_PIN 0        
#define LED_PIN 2

WiFiClient espClient;
PubSubClient client(espClient);

// Variáveis do LDR
int luzBruta = 0;
int luzPorcentagem = 0;
String statusLuz = "";

// Timers
unsigned long ultimaLeituraLDR = 0;
unsigned long ultimoEnvioMQTT = 0;
const long intervaloLeitura = 1000;    // Ler LDR a cada 1 segundo
const long intervaloEnvio = 5000;      // Enviar dados a cada 5 segundos

// Valores de calibração do LDR (ajuste conforme seu sensor)
int LDR_ESCURO = 4095;     // Valor máximo (totalmente coberto)
int LDR_CLARO = 0;         // Valor mínimo (luz forte)
// NOTA: O valor real varia com o circuito. Faça testes!

// ==========================================
// 3. CONFIGURAÇÃO DO WIFI
// ==========================================
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// ==========================================
// 4. CALIBRAR LDR - Execute esta função uma vez
// ==========================================
void calibrarLDR() {
  Serial.println("\n=== CALIBRAÇÃO DO LDR ===");
  Serial.println("1. Cubra completamente o LDR e aguarde...");
  delay(3000);
  LDR_ESCURO = analogRead(LDR_PIN);
  Serial.print("Valor no ESCURO: ");
  Serial.println(LDR_ESCURO);
  
  Serial.println("2. Exponha o LDR à luz forte e aguarde...");
  delay(3000);
  LDR_CLARO = analogRead(LDR_PIN);
  Serial.print("Valor no CLARO: ");
  Serial.println(LDR_CLARO);
  
  Serial.println("=== CALIBRAÇÃO CONCLUÍDA ===\n");
}

// ==========================================
// 5. LEITURA E PROCESSAMENTO DO LDR
// ==========================================
void lerLDR() {
  luzBruta = analogRead(LDR_PIN);
  
  // IMPORTANTE: Restringir valores dentro do range calibrado
  if (luzBruta < LDR_CLARO) luzBruta = LDR_CLARO;
  if (luzBruta > LDR_ESCURO) luzBruta = LDR_ESCURO;
  
  // Mapear invertido: valor alto = escuro, valor baixo = claro
  luzPorcentagem = map(luzBruta, LDR_ESCURO, LDR_CLARO, 0, 100);
  
  // Garantir limites de 0-100%
  luzPorcentagem = constrain(luzPorcentagem, 0, 100);
  
  // Determinar status
  if (luzPorcentagem < 20) {
    statusLuz = "Escuro";
  } else if (luzPorcentagem < 50) {
    statusLuz = "Meia-luz";
  } else if (luzPorcentagem < 80) {
    statusLuz = "Claro";
  } else {
    statusLuz = "Muito Claro";
  }
}

// ==========================================
// 6. CALLBACK (Agora suporta JSON!)
// ==========================================
void callback(char* topico, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topico);
  Serial.print("] ");
  
  char mensagem[length + 1];
  for (int i = 0; i < length; i++) {
    mensagem[i] = (char)payload[i];
  }
  mensagem[length] = '\0';
  Serial.println(mensagem);
  
  // Verificar se é JSON
  JsonDocument doc;
  DeserializationError erro = deserializeJson(doc, mensagem);
  
  if (erro) {
    // Se não for JSON válido, trata como texto simples
    Serial.print("Erro ao parsear JSON: ");
    Serial.println(erro.c_str());
    
    // Para compatibilidade com texto simples
    String msgSimples = String(mensagem);
    msgSimples.trim();
    
    if (String(topico) == topic_command) {
      if (msgSimples == "ON" || msgSimples == "{\"msg\": \"ON\"}") {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED LIGADO via texto simples");
        client.publish(topic_telemetry, "{\"led\": \"LIGADO\"}");
      } else if (msgSimples == "OFF" || msgSimples == "{\"msg\": \"OFF\"}") {
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED DESLIGADO via texto simples");
        client.publish(topic_telemetry, "{\"led\": \"DESLIGADO\"}");
      }
    }
  } else {
    // É JSON válido
    if (String(topico) == topic_command) {
      String comando = doc["msg"] | "";  // Procura pela chave "msg"
      
      if (comando == "ON") {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED LIGADO via JSON");
        client.publish(topic_telemetry, "{\"led\": \"LIGADO\"}");
      } else if (comando == "OFF") {
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED DESLIGADO via JSON");
        client.publish(topic_telemetry, "{\"led\": \"DESLIGADO\"}");
      } else {
        Serial.print("Comando não reconhecido: ");
        Serial.println(comando);
      }
    }
  }
}

// ==========================================
// 7. RECONEXÃO
// ==========================================
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    
    if (client.connect(device_id, mqtt_user, mqtt_pass, topic_status, 1, true, "offline")) {
      Serial.println("conectado");
      
      // Publicar status online
      client.publish(topic_status, "online", true);
      
      // Inscrever-se
      client.subscribe(topic_command);
      Serial.println("Inscrito no tópico: " + String(topic_command));
      
      // Publicar mensagem de inicialização
      client.publish(topic_telemetry, "{\"status\": \"Sistema iniciado\"}");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

// ==========================================
// 8. SETUP PRINCIPAL
// ==========================================
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  
  setup_wifi();
  
  // Descomente na primeira execução para calibrar:
  // calibrarLDR();
  
  // Valores estimados (ajuste conforme seus testes):
  LDR_ESCURO = 3500;   // Valor quando totalmente coberto
  LDR_CLARO = 500;     // Valor com luz ambiente
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  // Piscar LED para indicar inicialização
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

// ==========================================
// 9. LOOP PRINCIPAL
// ==========================================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long agora = millis();

  // Leitura do LDR
  if (agora - ultimaLeituraLDR > intervaloLeitura) {
    ultimaLeituraLDR = agora;
    lerLDR();
    
    Serial.print("LDR: ");
    Serial.print(luzBruta);
    Serial.print(" (");
    Serial.print(luzPorcentagem);
    Serial.print("% - ");
    Serial.print(statusLuz);
    Serial.println(")");
  }

  // Envio de telemetria
  if (agora - ultimoEnvioMQTT > intervaloEnvio) {
    ultimoEnvioMQTT = agora;
    
    JsonDocument doc;
    doc["device_id"] = device_id;
    doc["raw"] = luzBruta;
    doc["percent"] = luzPorcentagem;
    doc["status"] = statusLuz;
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["uptime"] = millis() / 1000;

    char buffer[256];
    serializeJson(doc, buffer);

    Serial.print("Publicando: ");
    Serial.println(buffer);
    client.publish(topic_telemetry, buffer);
  }
}