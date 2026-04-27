/*
 * SMILE PLEASE - ESP32-S3 Motor Controller (Low Latency Version)
 * 功能：通过 WiFi 高频轮询 Supabase，控制步进电机运行 30 秒
 * 优化：大幅降低轮询延迟，提升响应速度
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ========== 配置参数 ==========
// WiFi设置
const char* WIFI_SSID = "smileplease";        // 你的WiFi名称
const char* WIFI_PASSWORD = "simiaobieku";    // 你的WiFi密码

// Supabase配置
const char* SUPABASE_URL = "https://ghkzhbfqcwzkgrmxwoww.supabase.co/rest/v1/commands?select=*&limit=1";
const char* SUPABASE_ANON_KEY = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Imdoa3poYmZxY3d6a2dybXh3b3d3Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3NzY2NzEwNjksImV4cCI6MjA5MjI0NzA2OX0.YqEYNC5h_5RA6wOAEsQKBTnwAzbsFsptN82PEAWIJbk";

// GPIO引脚
const int MOTOR_PIN_1 = 12;  // 脉冲引脚 1
const int MOTOR_PIN_2 = 14;  // 脉冲引脚 2

// 运行参数
const int MOTOR_DURATION = 30000;  // 电机运行时长（30秒）
const int PULSE_FREQUENCY = 200;   // 脉冲频率（Hz）
const int POLL_INTERVAL = 500;     // 优化：轮询间隔降低至 500ms，反应更灵敏

// 状态变量
bool isRunning = false;
unsigned long motorStartTime = 0;
unsigned long lastPollTime = 0;
String lastProcessedId = "";

// ========== 初始化设置 ==========
void setup() {
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\n========== SMILE PLEASE: LOW LATENCY MODE ==========");
  
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  
  connectWiFi();
}

// ========== 主循环 ==========
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
    return;
  }
  
  if (isRunning) {
    unsigned long elapsed = millis() - motorStartTime;
    if (elapsed < MOTOR_DURATION) {
      generatePulse();
    } else {
      stopMotor();
    }
  } 
  else if (millis() - lastPollTime > POLL_INTERVAL) {
    lastPollTime = millis();
    checkForNewMessage();
  }
}

// ========== WiFi连接 ==========
void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected! Status: Ready.");
  } else {
    Serial.println("\nConnection failed. Rebooting...");
    delay(1000);
    ESP.restart();
  }
}

// ========== 检查新消息 ==========
void checkForNewMessage() {
  WiFiClientSecure client;
  client.setInsecure(); // 跳过 SSL 验证提高速度
  
  HTTPClient http;
  
  // 构建带有 API Key 的 URL
  String url = String(SUPABASE_URL) + "&apikey=" + String(SUPABASE_ANON_KEY);

  http.begin(client, url);
  http.addHeader("apikey", SUPABASE_ANON_KEY);
  http.addHeader("Authorization", "Bearer " + String(SUPABASE_ANON_KEY));
  
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error && doc.size() > 0) {
      JsonObject message = doc[0];
      String messageId = message["id"].as<String>();
      String status = message["status"].as<String>();
      String content = message["text"].as<String>();
      
      // 检查是否为新指令
      if ((status == "Run" || status == "RUN") && messageId != lastProcessedId) {
        Serial.println("\n[SYNC] TRIGGER RECEIVED: " + content);
        lastProcessedId = messageId;
        startMotor();
      }
    } else {
      Serial.print("."); // 心跳监测
    }
  }
  http.end();
}

// ========== 启动电机 ==========
void startMotor() {
  Serial.println(">>> MOTOR STARTING <<<");
  isRunning = true;
  motorStartTime = millis();
  
  // 移除之前的 10ms 延迟，改为直接触发
  digitalWrite(MOTOR_PIN_1, HIGH);
  digitalWrite(MOTOR_PIN_2, HIGH);
  delayMicroseconds(100); 
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
}

// ========== 生成脉冲信号 ==========
void generatePulse() {
  int pulsePeriod = 1000000 / PULSE_FREQUENCY;
  int halfPeriod = pulsePeriod / 2;
  
  digitalWrite(MOTOR_PIN_1, HIGH);
  digitalWrite(MOTOR_PIN_2, HIGH);
  delayMicroseconds(halfPeriod);
  
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  delayMicroseconds(halfPeriod);
}

// ========== 停止电机 ==========
void stopMotor() {
  Serial.println(">>> MOTOR STOPPED <<<");
  isRunning = false;
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
}