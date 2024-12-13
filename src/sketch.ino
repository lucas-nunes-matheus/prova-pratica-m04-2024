#include <WiFi.h>
#include <HTTPClient.h>

#define led_verde 41 // Pino utilizado para controle do led verde
#define led_vermelho 40 // Pino utilizado para controle do led vermelho
#define led_amarelo 9 // Pino utilizado para controle do led amarelo

const int pinoBotao = 18;  // O número do pino do botão
int estadoBotao = LOW;  // Variável para armazenar/ler o estado do botão

const int pinoLdr = 4;  // Pino utilizado para controle do sensor ldr
int estadoLdr = 0;  // Variável para armazenar/ler o estado do sensor Ldr
int limiarDeLuminosidade = 600; // Limite numérico para configuração do sensor ldr

// Configuração do debounce do Botão
unsigned long ultimoTempoDebounce = 0;
int tempoDelayDebounce = 50;
int leituraEstadoBotao;

// Configuracao da requisicao http
int codigoRespostaHttp;
int estadoLedVermelho = LOW;
int contadorBotao = 0;

// Configuracao para substituicao do delay
int tempoAtualMillis;
int ultimoTempoMillis = 0;

void setup() {

  // Configuração inicial dos pinos para controle dos leds como OUTPUTs (saídas) do ESP32
  pinMode(led_verde, OUTPUT);
  pinMode(led_vermelho, OUTPUT);
  pinMode(led_amarelo, OUTPUT);

  // Inicialização das entradas
  pinMode(pinoBotao, INPUT); // Inicializa o pino do botão como uma entrada (`input`)

  digitalWrite(led_verde, LOW); // Configura o led verde como apagado inicialmente
  digitalWrite(led_vermelho, LOW); // Configura o led vermelho como apagado inicialmente
  digitalWrite(led_amarelo, LOW); // Configura o led amarelo como apagado inicialmente
  Serial.begin(9600); // Configuração para debug por interface serial entre ESP e computador com baud rate de 9600

  WiFi.begin("Wokwi-GUEST", ""); // Conexão à rede WiFi aberta com SSID Wokwi-GUEST

  while (WiFi.status() == WL_CONNECT_FAILED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi com sucesso!"); // Considerando que saiu do loop acima, o ESP32 agora está conectado ao WiFi (outra opção é colocar este comando dentro do if abaixo)

  if (WiFi.status() == WL_CONNECTED) { // Se o ESP32 estiver conectado à Internet
    HTTPClient http;

    String rotaServidor = "http://www.google.com.br/"; // Endpoint da requisição HTTP

    http.begin(rotaServidor.c_str());

    codigoRespostaHttp = http.GET(); // Código do Resultado da Requisição HTTP

    if (codigoRespostaHttp > 0) {
      Serial.print("Código da Resposta HTTP: ");
      Serial.println(codigoRespostaHttp);
      String conteudo = http.getString(); // Obtém-se o conteúdo ou `payload`
      Serial.println(conteudo);
    }
    else {
      Serial.print("Código de erro: ");
      Serial.println(codigoRespostaHttp);
    }
    http.end();
  }

  else {
    Serial.println("WiFi Desconectado");
  }
}

void loop() {
  estadoLdr = analogRead(pinoLdr);

  tempoAtualMillis = millis();

  leituraEstadoBotao = digitalRead(pinoBotao);
  if (leituraEstadoBotao != estadoBotao) {
    ultimoTempoDebounce = millis();
  }

  if ((millis() - ultimoTempoDebounce) > tempoDelayDebounce) {
    estadoBotao = leituraEstadoBotao;
    contadorBotao++;
  }

  if (estadoLdr < limiarDeLuminosidade) {
    Serial.print("Está escuro, acende o LED");
    Serial.println(estadoLdr);
    digitalWrite(led_amarelo, HIGH);
    delay(1000);
    digitalWrite(led_amarelo, LOW);
    delay(1000);

  } else {
    Serial.print("Está claro, apague o LED");
    Serial.println(estadoLdr);

    digitalWrite(led_verde, HIGH);
    delay(3000);
    digitalWrite(led_verde, LOW);

    digitalWrite(led_amarelo, HIGH);
    delay(2000);
    digitalWrite(led_amarelo, LOW);

    estadoLedVermelho = HIGH;
    digitalWrite(led_vermelho, estadoLedVermelho);
    if((tempoAtualMillis-ultimoTempoMillis) > 5000) {
      ultimoTempoMillis = tempoAtualMillis;
      estadoLedVermelho = !estadoLedVermelho;
    }
    if (estadoLedVermelho && estadoBotao) {
      int ultimoTempoLedVermelhoMillis = 0;
      if (millis()-ultimoTempoLedVermelhoMillis > 1000) {
        estadoLedVermelho = !estadoLedVermelho;
      }
    }
    digitalWrite(led_vermelho, estadoLedVermelho);

    if (estadoLedVermelho && contadorBotao >= 3) {
      HTTPClient http;
      String rotaTeste = "http://www.google.com.br/";
      http.begin(rotaTeste.c_str());
      int codigoRespostaHttpTeste;
      codigoRespostaHttpTeste = http.GET();

      if (codigoRespostaHttpTeste > 0) {
        Serial.print("Código da Resposta HTTP de teste: ");
        Serial.println(codigoRespostaHttpTeste);
      }
      else {
        Serial.print("Código de erro do teste: ");
        Serial.println(codigoRespostaHttpTeste);
      }
      http.end();
    }
  }
}