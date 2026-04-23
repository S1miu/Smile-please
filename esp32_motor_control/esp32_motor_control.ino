/*
 * SMILE PLEASE - ESP32-S3 Motor Control
 * 
 * 功能：
 * - 连接WiFi
 * - 轮询Supabase API检测RUN状态
 * - 触发时向GPIO 12和14发送30秒脉冲信号（控制86步进电机）
 * 
 * 硬件：ESP32-S3
 * 步进电机驱动：GPIO 12（STEP）, GPIO 14（DIR）
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ========== WiFi配置 ==========
const char* ssid = "YOUR_WIFI_SSID";           // 修改为你的WiFi名称
const char* password = "YOUR_WIFI_PASSWORD";   // 修改为你的WiFi密码

// ========== Supabase配置 ==========
const char* supabaseUrl = "YOUR_SUPABASE_URL";        // 例如：https://xxxxx.supabase.co
const char* supabaseKey = "YOUR_SUPABASE_ANON_KEY";   // 你的Supabase匿名密钥

// ========== GPIO配置 ==========
const int STEP_PIN = 12;    // 步进信号引脚
const int DIR_PIN = 14;     // 方向信号引脚

// ========== 电机参数 ==========
const int STEPS_PER_REV = 200;      // 86步进电机每转步数（1.8度步距角）
const int STEP_DELAY_US = 500;      // 步进脉冲延迟（微秒），控制速度
const unsigned long RUN_DURATION = 30000;  // 运行时长30秒

// ========== 系统状态 ==========
bool isRunning = false;
unsigned long lastCheckTime = 0;
const unsigned long CHECK_INTERVAL = 2000;  // 每2秒检查一次API
String lastMessageId = "";  // 记录最后处理的消息ID，避免重复触发

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n========================================");
    Serial.println("SMILE PLEASE - Motor Control System");
    Serial.println("========================================\n");
    
    // 初始化GPIO
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    digitalWrite(STEP_PIN, LOW);
    digitalWrite(DIR_PIN, LOW);
    
    Serial.println("GPIO initialized:");
    Serial.printf("  STEP_PIN: %d\n", STEP_PIN);
    Serial.printf("  DIR_PIN: %d\n\n", DIR_PIN);
    
    // 连接WiFi
    connectWiFi();
    
    Serial.println("\nSystem ready. Polling API...\n");
}

void loop() {
    // 确保WiFi连接
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected. Reconnecting...");
        connectWiFi();
        return;
    }
    
    // 如果正在运行，跳过检查
    if (isRunning) {
        return;
    }
    
    // 定时检查API
    unsigned long currentTime = millis();
    if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
        lastCheckTime = currentTime;
        checkForNewMessages();
    }
}

// 连接WiFi
void connectWiFi() {
    Serial.printf("Connecting to WiFi: %s\n", ssid);
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("Signal strength: %d dBm\n", WiFi.RSSI());
    } else {
        Serial.println("\nWiFi connection failed!");
        Serial.println("Please check your credentials and try again.");
        delay(5000);
        ESP.restart();
    }
}

// 检查新消息
void checkForNewMessages() {
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }
    
    HTTPClient http;
    
    // 构建API URL - 获取最新的RUN状态消息
    String url = String(supabaseUrl) + "/rest/v1/messages?status=eq.RUN&order=created_at.desc&limit=1";
    
    http.begin(url);
    http.addHeader("apikey", supabaseKey);
    http.addHeader("Authorization", String("Bearer ") + supabaseKey);
    
    int httpCode = http.GET();
    
    if (httpCode == 200) {
        String payload = http.getString();
        
        // 解析JSON
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, payload);
        
        if (!error && doc.size() > 0) {
            JsonObject message = doc[0];
            String messageId = message["id"].as<String>();
            String content = message["content"].as<String>();
            String status = message["status"].as<String>();
            
            // 检查是否是新消息且状态为RUN
            if (status == "RUN" && messageId != lastMessageId) {
                lastMessageId = messageId;
                
                Serial.println("\n========================================");
                Serial.println("NEW MESSAGE DETECTED!");
                Serial.println("========================================");
                Serial.printf("Message ID: %s\n", messageId.c_str());
                Serial.printf("Content: %s\n", content.c_str());
                Serial.println("Starting motor sequence...\n");
                
                // 触发电机运行
                runMotorSequence();
            }
        }
    } else {
        Serial.printf("HTTP Error: %d\n", httpCode);
    }
    
    http.end();
}

// 运行电机序列
void runMotorSequence() {
    isRunning = true;
    
    Serial.println("Motor sequence started");
    Serial.printf("Duration: %lu ms\n", RUN_DURATION);
    Serial.printf("Steps per revolution: %d\n", STEPS_PER_REV);
    Serial.printf("Step delay: %d μs\n\n", STEP_DELAY_US);
    
    unsigned long startTime = millis();
    unsigned long stepCount = 0;
    
    // 设置方向（可根据需要修改）
    digitalWrite(DIR_PIN, HIGH);
    
    // 运行30秒
    while (millis() - startTime < RUN_DURATION) {
        // 生成步进脉冲
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(STEP_DELAY_US);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(STEP_DELAY_US);
        
        stepCount++;
        
        // 每1000步打印一次进度
        if (stepCount % 1000 == 0) {
            unsigned long elapsed = millis() - startTime;
            float progress = (float)elapsed / RUN_DURATION * 100;
            Serial.printf("Progress: %.1f%% | Steps: %lu | Time: %lu ms\n", 
                         progress, stepCount, elapsed);
        }
    }
    
    // 停止电机
    digitalWrite(STEP_PIN, LOW);
    digitalWrite(DIR_PIN, LOW);
    
    Serial.println("\n========================================");
    Serial.println("Motor sequence completed!");
    Serial.println("========================================");
    Serial.printf("Total steps: %lu\n", stepCount);
    Serial.printf("Total revolutions: %.2f\n", (float)stepCount / STEPS_PER_REV);
    Serial.println("Returning to idle state...\n");
    
    isRunning = false;
}
