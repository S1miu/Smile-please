/*
 * SMILE PLEASE - ESP32-S3 Motor Controller
 * 功能：监听Supabase API，触发步进电机30秒运行
 * 硬件：ESP32-S3 + 86步进电机
 * GPIO：12和14输出脉冲信号
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ========== 配置参数 ==========
// WiFi设置
const char* WIFI_SSID = "smileplease";        // 修改为你的WiFi名称
const char* WIFI_PASSWORD = "simiaobieku";    // 修改为你的WiFi密码

// Supabase配置（从config.js中复制）
const char* SUPABASE_URL = "https://ghkzhbfqcuzkgrmxwoww.supabase.co/rest/v1/commands?select=*&limit=1";           // 例如：https://xxx.supabase.co
const char* SUPABASE_ANON_KEY = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Imdoa3poYmZxY3d6a2dybXh3b3d3Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3NzY2NzEwNjksImV4cCI6MjA5MjI0NzA2OX0.YqEYNC5h_5RA6wOAEsQKBTnwAzbsFsptN82PEAWIJbk"; // 从config.js复制

// GPIO引脚
const int MOTOR_PIN_1 = 12;  // 步进电机控制引脚1
const int MOTOR_PIN_2 = 14;  // 步进电机控制引脚2

// 运行参数
const int MOTOR_DURATION = 30000;  // 电机运行时长（30秒）
const int PULSE_FREQUENCY = 200;   // 脉冲频率（Hz），可根据步进电机调整
const int POLL_INTERVAL = 2000;    // API轮询间隔（2秒）

// 状态变量
bool isRunning = false;
unsigned long motorStartTime = 0;
unsigned long lastPollTime = 0;
String lastProcessedId = "";

// ========== 初始化设置 ==========
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n========== SMILE PLEASE Motor Controller ==========");
  Serial.println("Initializing ESP32-S3...");
  
  // 配置GPIO引脚
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  
  Serial.println("GPIO pins configured (12, 14)");
  
  // 连接WiFi
  connectWiFi();
}

// ========== 主循环 ==========
void loop() {
  // 检查WiFi连接
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, reconnecting...");
    connectWiFi();
    return;
  }
  
  // 如果电机正在运行
  if (isRunning) {
    unsigned long elapsed = millis() - motorStartTime;
    
    if (elapsed < MOTOR_DURATION) {
      // 继续发送脉冲
      generatePulse();
    } else {
      // 30秒到达，停止电机
      stopMotor();
    }
  } 
  // 如果电机未运行，轮询API
  else if (millis() - lastPollTime > POLL_INTERVAL) {
    lastPollTime = millis();
    checkForNewMessage();
  }
}

// ========== WiFi连接 ==========
void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\nWiFi connection failed!");
    Serial.println("Retrying in 5 seconds...");
    delay(5000);
    ESP.restart();
  }
}

// ========== 检查新消息 ==========
void checkForNewMessage() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  // 使用WiFiClientSecure来处理HTTPS
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    // 跳过SSL证书验证（适用于开发/测试环境）
    client->setInsecure();
    
    HTTPClient http;
    
    // 构建API URL - 查询最新的RUN状态记录
    String url = String(SUPABASE_URL) + "/rest/v1/messages?status=eq.RUN&order=created_at.desc&limit=1";
    
    http.begin(*client, url);
    http.addHeader("apikey", SUPABASE_ANON_KEY);
    http.addHeader("Authorization", String("Bearer ") + SUPABASE_ANON_KEY);
    
    int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    
    // 解析JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error && doc.size() > 0) {
      JsonObject message = doc[0];
      String messageId = message["id"].as<String>();
      String status = message["status"].as<String>();
      String content = message["content"].as<String>();
      
      // 检查是否是新消息
      if (status == "RUN" && messageId != lastProcessedId) {
        Serial.println("\n========== NEW MESSAGE DETECTED ==========");
        Serial.print("ID: ");
        Serial.println(messageId);
        Serial.print("Content: ");
        Serial.println(content);
        Serial.println("==========================================");
        
        // 记录已处理的ID
        lastProcessedId = messageId;
        
        // 启动电机
        startMotor();
      }
    } else {
      Serial.print(".");  // 表示正在监听
    }
  } else {
    Serial.print("\nHTTP Error: ");
    Serial.println(httpCode);
    if (httpCode == -1) {
      Serial.println("Connection failed. Check:");
      Serial.println("1. WiFi connection");
      Serial.println("2. Supabase URL format");
      Serial.println("3. Firewall settings");
    }
  }
  
  http.end();
  delete client;
  }
}

// ========== 启动电机 ==========
void startMotor() {
  Serial.println("\n>>> MOTOR STARTING <<<");
  Serial.println("Duration: 30 seconds");
  
  isRunning = true;
  motorStartTime = millis();
  
  // 可选：立即发送一个启动脉冲
  digitalWrite(MOTOR_PIN_1, HIGH);
  digitalWrite(MOTOR_PIN_2, HIGH);
  delay(10);
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
}

// ========== 生成脉冲信号 ==========
void generatePulse() {
  // 计算脉冲周期（微秒）
  int pulsePeriod = 1000000 / PULSE_FREQUENCY;
  int halfPeriod = pulsePeriod / 2;
  
  // 发送脉冲到两个引脚
  digitalWrite(MOTOR_PIN_1, HIGH);
  digitalWrite(MOTOR_PIN_2, HIGH);
  delayMicroseconds(halfPeriod);
  
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  delayMicroseconds(halfPeriod);
}

// ========== 停止电机 ==========
void stopMotor() {
  Serial.println("\n>>> MOTOR STOPPED <<<");
  Serial.println("Ready for next message...\n");
  
  isRunning = false;
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
}
