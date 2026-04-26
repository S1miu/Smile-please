/*
 * SMILE PLEASE - ESP32-S3 Motor Controller (Fixed Version)
 * 功能：修正了 URL 拼接错误和函数作用域错误
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ========== 配置参数 ==========
const char* WIFI_SSID = "smileplease";        
const char* WIFI_PASSWORD = "simiaobieku";    

// 修正：SUPABASE_URL 只保留主域名
const char* SUPABASE_URL = "https://ghkzhbfqcwzkgrmxwoww.supabase.co"; 
const char* SUPABASE_ANON_KEY = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Imdoa3poYmZxY3d6a2dybXh3b3d3Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3NzY2NzEwNjksImV4cCI6MjA5MjI0NzA2OX0.YqEYNC5h_5RA6wOAEsQKBTnwAzbsFsptN82PEAWIJbk"; 

const int MOTOR_PIN_1 = 12;  
const int MOTOR_PIN_2 = 14;  
const int LED_PIN = 2;

const int MOTOR_DURATION = 30000;  
const int PULSE_FREQUENCY = 200;   
const int POLL_INTERVAL = 2000;    

bool isRunning = false;
unsigned long motorStartTime = 0;
unsigned long lastPollTime = 0;
String lastProcessedId = "";

// 函数声明，确保编译器能找到它们
void connectWiFi();
void checkForNewMessage();
void startMotor();
void stopMotor();
void generatePulse();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n========== SMILE PLEASE Motor Controller ==========");
  
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  
  connectWiFi();
}

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

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    digitalWrite(LED_PIN, HIGH); // 联网成功灯亮
  } else {
    Serial.println("\nWiFi connection failed! Restarting...");
    delay(2000);
    ESP.restart();
  }
}

void checkForNewMessage() {
  WiFiClientSecure client;
  client.setInsecure(); // 跳过证书验证
  
  HTTPClient http;
  // 修正后的 URL 拼接
  String url = String(SUPABASE_URL) + "/rest/v1/commands?select=*&order=created_at.desc&limit=1";

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
      String textContent = message["text"].as<String>();
      
      // 兼容 Run 和 RUN，同时对比 ID
      if ((status == "Run" || status == "RUN") && messageId != lastProcessedId) {
        Serial.println("\n--- NEW COMMAND RECEIVED ---");
        Serial.print("Text: "); Serial.println(textContent);
        lastProcessedId = messageId;
        startMotor();
      } else {
        Serial.print("."); // 心跳监测
      }
    }
  } else {
    Serial.print("HTTP Error: ");
    Serial.println(httpCode);
  }
  http.end();
}

void startMotor() {
  Serial.println(">>> MOTOR STARTING <<<");
  isRunning = true;
  motorStartTime = millis();
}

void stopMotor() {
  Serial.println(">>> MOTOR STOPPED <<<");
  isRunning = false;
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
}

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