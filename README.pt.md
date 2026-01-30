# Monitor de LDR Embarcado – ESP32 (Servidor Web Local)

[English](README.md) | [Português](README.pt.md)

## Visão Geral
Este projeto é um sistema embarcado baseado no ESP32 que lê os níveis de luz ambiente usando um sensor LDR e expõe os dados através de uma interface web local hospedada diretamente no microcontrolador.

O objetivo principal do estado atual é validar a aquisição do sensor, a conectividade Wi-Fi e a visualização web embarcada de uma maneira simples, funcional e inspecionável.

## Objetivos
- Praticar a aquisição de sensores analógicos usando o ADC do ESP32.
- Compreender o escalonamento do ADC e a normalização de sinais.
- Implementar conectividade Wi-Fi em um dispositivo embarcado.
- Servir um dashboard HTML dinâmico diretamente do ESP32.
- Visualizar dados do sensor usando tecnologias web padrão.

## Hardware
- ESP32 C3 Mini
- Módulo LDR (Resistor Dependente de Luz)
- Protoboard e jumpers

## Stack de Software
- Framework Arduino para ESP32
- Bibliotecas WiFi e WebServer
- HTML, CSS e JavaScript
- Bootstrap 5 para layout
- Chart.js para visualização de dados

## Arquitetura do Firmware
O firmware está organizado em torno de um modelo de execução síncrona simples:

- **Fase de Inicialização**
  - Configuração da interface serial
  - Configuração do ADC
  - Conexão Wi-Fi
  - Inicialização do servidor HTTP

- **Fase de Execução (Runtime)**
  - Tratamento de requisições HTTP
  - Aquisição do sensor sob demanda
  - Geração dinâmica de HTML com valores atuais do sensor

As leituras do sensor ocorrem quando o endpoint raiz HTTP (`/`) é acessado por um cliente.

## Processamento do Sensor
- Os valores brutos do ADC são lidos na faixa `0–4095`.
- Os valores são mapeados para uma escala percentual (`0–100%`).
- Uma classificação simples baseada em regras define o status da luz:
  - Muito Escuro (Very Dark)
  - Pouca Luz (Low Light)
  - Claro (Bright)
  - Muito Claro (Very Bright)

## Interface Web
A interface web embarcada fornece:
- Exibição do valor bruto do ADC
- Porcentagem do nível de luz
- Status textual da condição de luz
- Barra de progresso visual
- Gráfico de rosca (Doughnut chart) representando luz vs escuridão

O dashboard é responsivo e pode ser acessado de qualquer dispositivo conectado à mesma rede local.

## Funcionalidades Atuais
- Leitura do sensor LDR via ADC do ESP32
- Normalização e classificação de dados
- Conectividade Wi-Fi
- Servidor HTTP embarcado
- Geração dinâmica de páginas HTML
- Visualização de dashboard em rede local

## Limitações Conhecidas
- Os dados do sensor não são armazenados.
- Sem registro de data/hora (timestamp) das medições.
- A leitura do sensor e a lógica do servidor web estão acopladas.
- Necessário atualizar a página para ver novos valores.
- Sem API externa ou integração com a nuvem.

## Status do Projeto
🟢 Protótipo funcional – monitoramento local do sensor via servidor web embarcado.

## Modificações Futuras
- Desacoplar a aquisição do sensor do tratamento de requisições HTTP.
- Implementar amostragem periódica do sensor.
- Adicionar endpoint de API REST para consumidores de dados externos.
- Introduzir buffer de dados e timestamps.
- Preparar o firmware para integração com backend na nuvem.
- Melhorar a modularização do código e a separação de responsabilidades.