#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Configurações (Baseado no seu diagrama) ---
// Pinos I2C (Seu diagrama usa 8 e 9)
#define I2C_SDA_PIN 8
#define I2C_SCL_PIN 9

// LED (Seu diagrama usa 10)
#define LED_PIN 10

// Display OLED
#define SCREEN_WIDTH 128 // Largura do display
#define SCREEN_HEIGHT 64 // Altura do display
#define OLED_RESET -1    // Reset (não usado, -1)
#define SCREEN_ADDRESS 0x3C // Endereço I2C do SSD1306

// Sensor MPU6050
#define MPU_ADDRESS 0x68 // Endereço I2C do MPU6050

// Lógica da Aplicação
#define READ_INTERVAL 200 // Intervalo de leitura (200 ms) - Passo 2b
#define NUM_READINGS 10   // Número de leituras para a média - Passo 2c
#define DELTA_THRESHOLD 0.5 // Limite de delta para acender o LED (m/s^2) - Passo 2e

// --- Instâncias dos Objetos ---
Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Variáveis Globais ---
unsigned long previousMillis = 0; // Para controle de tempo

// Buffers para a média móvel
float readingsX[NUM_READINGS];
float readingsY[NUM_READINGS];
float readingsZ[NUM_READINGS];
int readIndex = 0; // Índice atual do buffer
float totalX = 0;
float totalY = 0;
float totalZ = 0;
float averageX = 0;
float averageY = 0;
float averageZ = 0;

// Variáveis para detectar a mudança (delta)
float lastAverageX = 0;
float lastAverageY = 0;
float lastAverageZ = 0;

// --- Função de Setup (executa uma vez) ---
void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); } // Espera o Serial (bom para debug)

  // Configura o pino do LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("Iniciando Atividade 7: Arduino no ESP32-S3");

  // 1. Inicializa o barramento I2C (Passo 2a)
  // No ESP32-S3, devemos passar os pinos para o Wire.begin()
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // Inicializa o MPU6050
  if (!mpu.begin(MPU_ADDRESS)) {
    Serial.println("Falha ao encontrar o MPU6050. Verifique as conexões.");
    while (1) { delay(10); }
  }
  Serial.println("MPU6050 Detectado!");
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G); // Configura o range do acelerômetro

  // Inicializa o Display SSD1306
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Falha ao encontrar o SSD1306. Verifique as conexões."));
    while (1) { delay(10); }
  }
  Serial.println("SSD1306 Detectado!");

  // Limpa o display e configura texto inicial
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Iniciando...");
  display.display();

  // Inicializa os buffers da média móvel com 0
  for (int i = 0; i < NUM_READINGS; i++) {
    readingsX[i] = 0;
    readingsY[i] = 0;
    readingsZ[i] = 0;
  }
  
  delay(1000); // Pequena pausa
}

// --- Função de Loop (executa repetidamente) ---
void loop() {
  // Controle de tempo não bloqueante (executa a cada 200ms)
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= READ_INTERVAL) {
    previousMillis = currentMillis;

    // 2. Ler dados do MPU6050 (Passo 2b)
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // 3. Calcular o valor médio (Média Móvel) (Passo 2c)
    
    // Subtrai o valor mais antigo do total
    totalX = totalX - readingsX[readIndex];
    totalY = totalY - readingsY[readIndex];
    totalZ = totalZ - readingsZ[readIndex];

    // Armazena a nova leitura no buffer
    readingsX[readIndex] = a.acceleration.x;
    readingsY[readIndex] = a.acceleration.y;
    readingsZ[readIndex] = a.acceleration.z;

    // Adiciona o novo valor ao total
    totalX = totalX + readingsX[readIndex];
    totalY = totalY + readingsY[readIndex];
    totalZ = totalZ + readingsZ[readIndex];

    // Avança o índice do buffer (circular)
    readIndex = (readIndex + 1) % NUM_READINGS;
    
    // Calcula a média
    averageX = totalX / NUM_READINGS;
    averageY = totalY / NUM_READINGS;
    averageZ = totalZ / NUM_READINGS;

    // 4. Apresentar no display os valores médios (Passo 2d)
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("--- Media Acel (m/s^2) ---");
    
    display.setCursor(0, 16);
    display.print("X: ");
    display.print(averageX, 2); // Imprime com 2 casas decimais

    display.setCursor(0, 32);
    display.print("Y: ");
    display.print(averageY, 2);

    display.setCursor(0, 48);
    display.print("Z: ");
    display.print(averageZ, 2);
    
    display.display(); // Envia o buffer para o display

    // Enviar informações via terminal serial (UART)
    Serial.print("Media X: "); Serial.print(averageX, 2);
    Serial.print(" m/s^2, Media Y: "); Serial.print(averageY, 2);
    Serial.print(" m/s^2, Media Z: "); Serial.print(averageZ, 2);
    Serial.println(" m/s^2");

    // 5. Acender o LED se houver mudança (delta > 0.5 m/s^2) (Passo 2e)
    
    // Calcula a variação absoluta (delta) desde a última medição
    float deltaX = abs(averageX - lastAverageX);
    float deltaY = abs(averageY - lastAverageY);
    float deltaZ = abs(averageZ - lastAverageZ);

    // Verifica se *qualquer* eixo ultrapassou o limite
    if (deltaX > DELTA_THRESHOLD || deltaY > DELTA_THRESHOLD || deltaZ > DELTA_THRESHOLD) {
      digitalWrite(LED_PIN, HIGH); // Acende o LED
    } else {
      digitalWrite(LED_PIN, LOW); // Apaga o LED
    }

    // Atualiza os valores "antigos" para a próxima comparação
    lastAverageX = averageX;
    lastAverageY = averageY;
    lastAverageZ = averageZ;
  }
}
