/*
 * SMILE PLEASE - ESP32-S3 艺术装置控制系统
 * 
 * 功能：
 * - 连接WiFi
 * - 轮询API检测RUN指令
 * - 触发时向GPIO 12和14发送30秒脉冲信号
 * - 控制86步进电机
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ========== WiFi 配置 ==========
const char* ssid = "YOUR_WIFI_SSID";           // 修改为你的WiFi名称
const char* password = "YOUR_WIFI_PASSWORD";   // 修改为你的WiFi密码

// ========== API 配置 ==========
// 选项1：使用 Supabase
const char* apiUrl = "YOUR_SUPABASE_URL/rest/v1/system_commands?select=*&order=created_at.desc&limit=1";
const char* apiKey = "YOUR_SUPABASE_ANON_KEY";

// 选项2：或使用自定义API端点
// const char* apiUrl = "http://YOUR_SERVER_IP:PORT/api/status";

// ========== GPIO 配置 ==========
const int MOTOR_PIN_1 = 12;  // GPIO 12 - 步进电机控制引脚1
const int MOTOR_PIN_2 = 14;  // GPIO 14 - 步进电机控制引脚2
const int LED_PIN = 2;        // GPIO 2 - 板载LED（可选，用于状态指示）

// ========== 运行参数 ==========
const int PULSE_DURATION = 30000;  // 30秒运行时间（毫秒）
const int POLL_INTERVAL = 1000;    // 1秒轮询间隔
const int PULSE_FREQUENCY = 200;   // 脉冲频率（Hz）
const int PULSE_WIDTH = 2;         // 脉冲宽度（毫秒）

// ========== 状态变量 ==========
bool isRunning = false;
unsigned long runStartTime = 0;
String lastCommandId = "";
unsigned long lastPollTime = 0;

void setup() {
  // 串口初始化
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=================================");
  Serial.println("SMILE PLEASE - ESP32-S3 启动中...");
  Serial.println("=================================");
  
  // GPIO初始化
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("[GPIO] 引脚初始化完成");
  
  // 连接WiFi
  connectWiFi();
  
  Serial.println("[系统] 初始化完成，开始轮询...\n");
}

void loop() {
  // 检查WiFi连接
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] 连接丢失，尝试重连...");
    connectWiFi();
    return;
  }
  
  // 如果正在运行，控制电机
  if (isRunning) {
    unsigned long elapsed = millis() - runStartTime;
    
    if (elapsed < PULSE_DURATION) {
      // 持续发送脉冲
      generatePulse();
      
      // 每5秒报告一次进度
      if (elapsed % 5000 < 100) {
        int remaining = (PULSE_DURATION - elapsed) / 1000;
        Serial.printf("[运行] 剩余时间: %d 秒\n", remaining);
      }
    } else {
      // 运行结束
      stopMotors();
    }
  } else {
    // 不在运行中，定时轮询API
    if (millis() - lastPollTime >= POLL_INTERVAL) {
      lastPollTime = millis();
      checkForCommand();
    }
  }
  
  delay(10);  // 短暂延迟避免CPU过载
}

// ========== WiFi 连接函数 ==========
void connectWiFi() {
  Serial.print("[WiFi] 正在连接到: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // LED闪烁
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] 连接成功！");
    Serial.print("[WiFi] IP地址: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_PIN, HIGH);  // LED常亮
  } else {
    Serial.println("\n[WiFi] 连接失败，将在10秒后重试...");
    digitalWrite(LED_PIN, LOW);
    delay(10000);
  }
}

// ========== API 轮询函数 ==========
void checkForCommand() {
  HTTPClient http;
  
  http.begin(apiUrl);
  
  // 如果使用Supabase，添加认证头
  if (strlen(apiKey) > 0) {
    http.addHeader("apikey", apiKey);
    http.addHeader("Authorization", String("Bearer ") + apiKey);
  }
  
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    
    // 解析JSON响应
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
      Serial.print("[API] JSON解析失败: ");
      Serial.println(error.c_str());
      http.end();
      return;
    }
    
    // 检查是否有新的RUN命令
    if (doc.size() > 0) {
      JsonObject command = doc[0];
      String commandId = command["id"].as<String>();
      String commandType = command["command"].as<String>();
      
      // 新命令且为RUN类型
      if (commandId != lastCommandId && commandType == "RUN") {
        lastCommandId = commandId;
        String message = command["message"].as<String>();
        
        Serial.println("\n=================================");
        Serial.println("[命令] 收到新的RUN指令！");
        Serial.printf("[消息] %s\n", message.c_str());
        Serial.println("=================================\n");
        
        startMotors();
      }
    }
  } else {
    Serial.printf("[API] HTTP错误: %d\n", httpCode);
  }
  
  http.end();
}

// ========== 电机控制函数 ==========
void startMotors() {
  if (isRunning) {
    Serial.println("[警告] 系统已在运行中");
    return;
  }
  
  isRunning = true;
  runStartTime = millis();
  
  Serial.println("[电机] 启动中...");
  digitalWrite(LED_PIN, HIGH);
}

void stopMotors() {
  isRunning = false;
  
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  
  Serial.println("\n[电机] 已停止");
  Serial.println("[系统] 恢复轮询状态\n");
  
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  digitalWrite(LED_PIN, HIGH);
}

void generatePulse() {
  // 生成交替脉冲信号
  // 这是一个基础的步进电机驱动逻辑
  // 根据你的具体电机型号可能需要调整
  
  static unsigned long lastPulseTime = 0;
  static bool pulseState = false;
  
  unsigned long currentTime = millis();
  
  if (currentTime - lastPulseTime >= PULSE_WIDTH) {
    lastPulseTime = currentTime;
    pulseState = !pulseState;
    
    if (pulseState) {
      digitalWrite(MOTOR_PIN_1, HIGH);
      digitalWrite(MOTOR_PIN_2, LOW);
    } else {
      digitalWrite(MOTOR_PIN_1, LOW);
      digitalWrite(MOTOR_PIN_2, HIGH);
    }
  }
}

// ========== 备选方案：使用本地HTTP服务器轮询 ==========
/*
 * 如果不使用Supabase，可以设置本地HTTP服务器
 * 修改checkForCommand()函数改为轮询本地服务器：
 * 
 * const char* apiUrl = "http://192.168.1.100:3000/api/status";
 * 
 * 服务器应返回JSON格式：
 * {
 *   "command": "RUN",
 *   "message": "用户消息",
 *   "timestamp": 1234567890
 * }
 */
