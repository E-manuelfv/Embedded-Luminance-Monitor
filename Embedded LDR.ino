#include <WiFi.h>
#include <WebServer.h>

// --- Configuração de Rede ---
const char* ssid = "SSID";      
const char* password = "PASSWORD"; 

// Cria o objeto do servidor na porta 80
WebServer server(80);

// --- Configuração do Sensor LDR ---
// No ESP32-C3 SuperMini, usamos o GPIO 2 (ADC1_CH2)
#define AO_PIN 2  

// Variáveis para armazenar os dados
int luzBruta = 0;
int luzPorcentagem = 0;
String statusLuz = "--";

// --- DECLARAÇÃO DE FUNÇÕES (A CORREÇÃO ESTÁ AQUI) ---
// Isso avisa o compilador que essas funções existem lá embaixo
void handle_OnConnect();
void handle_NotFound();

// --- Template HTML (Armazenado na memória flash para economizar RAM) ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Monitor LDR ESP32</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        body { background-color: #eef1f5; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
        .sidebar { width: 240px; min-height: 100vh; background: linear-gradient(180deg, #0d2b45, #092035); color: white; position: fixed; }
        .sidebar h4 { margin-top: 20px; color: #fff; }
        .sidebar a { color: #cfd8dc; text-decoration: none; display: block; padding: 10px 20px; transition: 0.3s; }
        .sidebar a:hover { background-color: #12395c; color: white; border-left: 4px solid #0d6efd; }
        .content { margin-left: 240px; padding: 20px; }
        .card-metric { border-left: 5px solid #0d6efd; transition: transform 0.2s; }
        .card-metric:hover { transform: translateY(-5px); }
        @media (max-width: 768px) {
            .sidebar { width: 100%; height: auto; position: relative; min-height: auto; }
            .content { margin-left: 0; }
        }
    </style>
</head>
<body>

<div class="d-flex flex-column flex-md-row">
    <div class="sidebar d-flex flex-column align-items-center text-center p-3">
        <h4>ESP32 Monitor</h4>
        <small class="text-white-50">Firmware v1.0</small>
        <hr class="w-100 border-light">
        <nav class="w-100 text-start">
            <a href="/">Dashboard</a>
            <a href="#">Configurações</a>
            <a href="#">Sobre</a>
        </nav>
    </div>

    <div class="content flex-grow-1">
        <h3 class="mb-4 text-dark border-bottom pb-2">Dashboard – Sensor de Luminosidade</h3>

        <div class="row mb-4">
            <div class="col-md-4 mb-3">
                <div class="card card-metric shadow-sm">
                    <div class="card-body">
                        <small class="text-muted">Leitura Bruta (0-4095)</small>
                        <h2 class="text-primary">{{VALOR_BRUTO}}</h2>
                    </div>
                </div>
            </div>

            <div class="col-md-4 mb-3">
                <div class="card card-metric shadow-sm" style="border-left-color: #198754;">
                    <div class="card-body">
                        <small class="text-muted">Porcentagem de Luz</small>
                        <h2 class="text-success">{{VALOR_PORCENTO}}%</h2>
                    </div>
                </div>
            </div>

            <div class="col-md-4 mb-3">
                <div class="card card-metric shadow-sm" style="border-left-color: #ffc107;">
                    <div class="card-body">
                        <small class="text-muted">Status Atual</small>
                        <h2 class="text-warning">{{STATUS_LUZ}}</h2>
                    </div>
                </div>
            </div>
        </div>

        <div class="row">
            <div class="col-md-8 mb-3">
                <div class="card shadow-sm h-100">
                    <div class="card-header bg-white fw-bold">Nível de Luz (Visual)</div>
                    <div class="card-body">
                        <label>Intensidade da Luz:</label>
                        <div class="progress" style="height: 30px;">
                            <div class="progress-bar bg-info" role="progressbar" style="width: {{VALOR_PORCENTO}}%;" aria-valuenow="{{VALOR_PORCENTO}}" aria-valuemin="0" aria-valuemax="100">{{VALOR_PORCENTO}}%</div>
                        </div>
                        <p class="mt-3 text-muted">Nota: Atualize a página (F5) para ler o novo valor do sensor.</p>
                    </div>
                </div>
            </div>

            <div class="col-md-4 mb-3">
                <div class="card shadow-sm h-100">
                    <div class="card-header bg-white fw-bold">Gráfico de Rosca</div>
                    <div class="card-body">
                        <canvas id="donutChart"></canvas>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<script>
    const ctx = document.getElementById('donutChart').getContext('2d');
    const myChart = new Chart(ctx, {
        type: 'doughnut',
        data: {
            labels: ['Luz (%)', 'Escuridão (%)'],
            datasets: [{
                data: [{{VALOR_PORCENTO}}, 100 - {{VALOR_PORCENTO}}],
                backgroundColor: ['#0d6efd', '#e9ecef'],
                borderWidth: 1
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: { legend: { position: 'bottom' } }
        }
    });
</script>

</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Configura a leitura analógica
  analogSetAttenuation(ADC_11db); // Permite ler até 3.3V

  Serial.println("Conectando ao Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nWi-Fi Conectado!");
  Serial.print("Endereço IP: ");  
  Serial.println(WiFi.localIP());

  // Agora o compilador já sabe que essas funções existem
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}

// --- Implementação das Funções ---

void handle_OnConnect() {
  // 1. LER O SENSOR
  luzBruta = analogRead(AO_PIN); // Lê de 0 a 4095
  
  // 2. CONVERTER PARA PORCENTAGEM (Invertido: 0=Escuro, 100=Claro)
  luzPorcentagem = map(luzBruta, 4095, 0, 0, 100); 

  // Corrige limites se passar um pouco (ruído)
  if (luzPorcentagem < 0) luzPorcentagem = 0;
  if (luzPorcentagem > 100) luzPorcentagem = 100;

  // 3. DEFINIR STATUS TEXTUAL
  if (luzPorcentagem < 20) statusLuz = "Muito Escuro";
  else if (luzPorcentagem < 50) statusLuz = "Penumbra";
  else if (luzPorcentagem < 80) statusLuz = "Claro";
  else statusLuz = "Muito Claro";

  // 4. PREPARAR O HTML
  String html = index_html; // Copia o template original

  // 5. SUBSTITUIR OS PLACEHOLDERS PELOS VALORES REAIS
  html.replace("{{VALOR_BRUTO}}", String(luzBruta));
  html.replace("{{VALOR_PORCENTO}}", String(luzPorcentagem));
  html.replace("{{STATUS_LUZ}}", statusLuz);

  // 6. ENVIAR PARA O NAVEGADOR
  server.send(200, "text/html", html); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Pagina nao encontrada");
}