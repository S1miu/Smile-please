/*
 * SMILE PLEASE - ESP32-S3 步进电机控制程序
 * 功能：监听Supabase RUN信号，驱动GPIO 12和14的步进电机30秒
 * 硬件：ESP32-S3 + 86步进电机驱动器
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ===== WiFi配置 =====
const char* ssid = "YOUR_WIFI_SSID";           // 替换为你的WiFi名称
const char* password = "YOUR_WIFI_PASSWORD";   // 替换为你的WiFi密码

// ===== Supabase配置 =====
const char* supabaseUrl = "YOUR_SUPABASE_URL";  // 例如：https://xxxxx.supabase.co
const char* supabaseKey = "YOUR_SUPABASE_ANON_KEY";
const char* tableName = "run_signals";  // 数据表名

// ===== GPIO引脚定义 =====
const int MOTOR1_STEP_PIN = 12;  // 步进电机1的脉冲引脚
const int MOTOR2_STEP_PIN = 14;  // 步进电机2的脉冲引脚
const int MOTOR1_DIR_PIN = 13;   // 方向引脚（可选）
const int MOTOR2_DIR_PIN = 15;   // 方向引脚（可选）

// ===== 电机控制参数 =====
const int PULSE_WIDTH = 500;      // 脉冲宽度（微秒）
const int PULSE_INTERVAL = 1000;  // 脉冲间隔（微秒）
const long RUN_DURATION = 30000;  // 运行持续时间（30秒）

// ===== 状态变量 =====
bool isRunning = false;
unsigned long runStartTime = 0;
String lastProcessedId = "";

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=================================");
    Serial.println("SMILE PLEASE - ESP32 Motor Control");
    Serial.println("=================================\n");
    
    // 初始化GPIO引脚
    pinMode(MOTOR1_STEP_PIN, OUTPUT);
    pinMode(MOTOR2_STEP_PIN, OUTPUT);
    pinMode(MOTOR1_DIR_PIN, OUTPUT);
    pinMode(MOTOR2_DIR_PIN, OUTPUT);
    
    digitalWrite(MOTOR1_STEP_PIN, LOW);
    digitalWrite(MOTOR2_STEP_PIN, LOW);
    digitalWrite(MOTOR1_DIR_PIN, HIGH);  // 设置方向
    digitalWrite(MOTOR2_DIR_PIN, HIGH);
    
    Serial.println("GPIO initialized: Pins 12, 13, 14, 15");
    
    // 连接WiFi
    connectWiFi();
}

void loop() {
    // 检查WiFi连接
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected. Reconnecting...");
        connectWiFi();
        return;
    }
    
    // 如果正在运行，继续发送脉冲
    if (isRunning) {
        unsigned long currentTime = millis();
        
        if (currentTime - runStartTime < RUN_DURATION) {
            // 发送脉冲到两个电机
            sendPulse();
        } else {
            // 30秒结束，停止电机
            stopMotors();
            Serial.println("Motor run completed (30s)");
        }
    } else {
        // 轮询检查新的RUN信号
        checkForRunSignal();
        delay(1000);  // 每秒检查一次
    }
}

// ===== WiFi连接函数 =====
void connectWiFi() {
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi connection failed!");
    }
}

// ===== 检查RUN信号 =====
void checkForRunSignal() {
    if (WiFi.status() != WL_CONNECTED) return;
    
    HTTPClient http;
    
    // 构建Supabase REST API URL
    String url = String(supabaseUrl) + "/rest/v1/" + tableName + "?select=*&order=created_at.desc&limit=1";
    
    http.begin(url);
    http.addHeader("apikey", supabaseKey);
    http.addHeader("Authorization", String("Bearer ") + supabaseKey);
    
    int httpCode = http.GET();
    
    if (httpCode == 200) {
        String payload = http.getString();
        
        // 解析JSON
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);
        
        if (doc.size() > 0) {
            String recordId = doc[0]["id"].as<String>();
            bool processed = doc[0]["processed"] | false;
            
            // 检查是否为新记录且未处理
            if (!processed && recordId != lastProcessedId) {
                Serial.println("New RUN signal detected!");
                Serial.print("Message: ");
                Serial.println(doc[0]["message"].as<String>());
                
                // 启动电机
                startMotors();
                lastProcessedId = recordId;
                
                // 标记为已处理（可选）
                markAsProcessed(recordId);
            }
        }
    } else {
        Serial.print("HTTP Error: ");
        Serial.println(httpCode);
    }
    
    http.end();
}

// ===== 启动电机 =====
void startMotors() {
    isRunning = true;
    runStartTime = millis();
    Serial.println("Motors started - Running for 30 seconds...");
}

// ===== 停止电机 =====
void stopMotors() {
    isRunning = false;
    digitalWrite(MOTOR1_STEP_PIN, LOW);
    digitalWrite(MOTOR2_STEP_PIN, LOW);
}

// ===== 发送脉冲信号 =====
void sendPulse() {
    // 同时向两个电机发送脉冲
    digitalWrite(MOTOR1_STEP_PIN, HIGH);
    digitalWrite(MOTOR2_STEP_PIN, HIGH);
    delayMicroseconds(PULSE_WIDTH);
    
    digitalWrite(MOTOR1_STEP_PIN, LOW);
    digitalWrite(MOTOR2_STEP_PIN, LOW);
    delayMicroseconds(PULSE_INTERVAL);
}

// ===== 标记信号为已处理 =====
void markAsProcessed(String recordId) {
    HTTPClient http;
    
    String url = String(supabaseUrl) + "/rest/v1/" + tableName + "?id=eq." + recordId;
    
    http.begin(url);
    http.addHeader("apikey", supabaseKey);
    http.addHeader("Authorization", String("Bearer ") + supabaseKey);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Prefer", "return=minimal");
    
    String payload = "{\"processed\": true}";
    int httpCode = http.PATCH(payload);
    
    if (httpCode == 204) {
        Serial.println("Signal marked as processed");
    }
    
    http.end();
}
