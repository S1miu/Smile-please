# SMILE PLEASE - 部署指南

## 📋 项目概述
极致黑白极简主义交互艺术装置控制系统，包含观众端、支付端、iPad展示端和ESP32硬件控制。

---

## 🚀 快速开始

### 1. Supabase 配置（5分钟）

#### 1.1 创建Supabase项目
1. 访问 [supabase.com](https://supabase.com)
2. 创建新项目
3. 记录以下信息：
   - `Project URL`: https://xxxxx.supabase.co
   - `anon public key`: eyJhbGciOi...（在Settings > API找到）

#### 1.2 创建数据表
在Supabase SQL编辑器中执行：

```sql
-- 创建run_signals表
CREATE TABLE run_signals (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    message TEXT NOT NULL,
    processed BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

-- 启用实时订阅
ALTER TABLE run_signals REPLICA IDENTITY FULL;

-- 设置表权限（允许匿名访问）
ALTER TABLE run_signals ENABLE ROW LEVEL SECURITY;

CREATE POLICY "Allow all operations" ON run_signals
FOR ALL USING (true) WITH CHECK (true);
```

#### 1.3 启用Realtime
1. 进入 Database > Replication
2. 找到 `run_signals` 表
3. 启用 Realtime

---

### 2. 前端配置（2分钟）

编辑 `logic.js` 文件，替换以下两行：

```javascript
const SUPABASE_URL = 'YOUR_SUPABASE_URL';  // 替换为你的Project URL
const SUPABASE_KEY = 'YOUR_SUPABASE_KEY';  // 替换为你的anon public key
```

---

### 3. 本地测试

#### 3.1 启动本地服务器
```bash
# 使用Python（Mac自带）
python3 -m http.server 8000

# 或使用Node.js
npx http-server -p 8000
```

#### 3.2 测试三个端点
- **观众端**: http://localhost:8000/index.html
- **iPad展示端**: http://localhost:8000/display.html

#### 3.3 测试流程
1. 在观众端输入测试文字
2. 点击 SUBMIT TO SYSTEM
3. 显示收款码页面
4. 点击 CONFIRM PAYMENT
5. 观察：
   - 观众端显示验证动画→SUCCESS
   - iPad展示端立即触发全屏Glitch效果
   - 倒计时30秒后自动恢复

---

### 4. 局域网部署

#### 4.1 获取本机IP
```bash
# Mac
ifconfig | grep "inet " | grep -v 127.0.0.1

# 输出示例：192.168.1.100
```

#### 4.2 局域网访问
- **观众端**: http://192.168.1.100:8000/index.html
- **iPad展示端**: http://192.168.1.100:8000/display.html

⚠️ **重要**：所有设备必须连接同一WiFi网络

---

### 5. ESP32-S3 硬件配置

#### 5.1 安装Arduino IDE
1. 下载 [Arduino IDE](https://www.arduino.cc/en/software)
2. 安装ESP32开发板支持：
   - 打开 Arduino IDE > Preferences
   - Additional Board Manager URLs添加：
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Tools > Board > Boards Manager
   - 搜索"ESP32"并安装

#### 5.2 安装依赖库
Tools > Manage Libraries，搜索并安装：
- `ArduinoJson` (by Benoit Blanchon)

#### 5.3 配置代码
打开 `esp32_motor_control.ino`，修改：

```cpp
// WiFi配置
const char* ssid = "YOUR_WIFI_SSID";           // 你的WiFi名称
const char* password = "YOUR_WIFI_PASSWORD";   // 你的WiFi密码

// Supabase配置
const char* supabaseUrl = "YOUR_SUPABASE_URL";      // 如：https://xxxxx.supabase.co
const char* supabaseKey = "YOUR_SUPABASE_ANON_KEY"; // Supabase anon key
```

#### 5.4 接线图
```
ESP32-S3          步进电机驱动器
GPIO 12  -------> STEP1 (电机1脉冲)
GPIO 13  -------> DIR1  (电机1方向)
GPIO 14  -------> STEP2 (电机2脉冲)
GPIO 15  -------> DIR2  (电机2方向)
GND      -------> GND
5V       -------> VCC (根据驱动器要求)
```

#### 5.5 上传程序
1. 连接ESP32-S3到电脑
2. Tools > Board > ESP32 Arduino > ESP32S3 Dev Module
3. Tools > Port > 选择对应端口
4. 点击Upload按钮
5. 打开Serial Monitor (115200 baud)查看连接状态

---

## 🎨 展览现场部署

### iPad设置
1. 全屏浏览器：Safari或Chrome
2. 开启"请勿打扰"模式
3. 禁用自动锁屏：Settings > Display & Brightness > Auto-Lock > Never
4. 访问：http://[服务器IP]:8000/display.html
5. 进入全屏模式（Safari：点击分享按钮 > 添加到主屏幕）

### 观众端设置
- 可使用任何手机/平板浏览器
- 推荐二维码快速访问
- 生成二维码：http://[服务器IP]:8000/index.html

### 收款码设置
- 确保 `收款码.jpg` 在项目根目录
- 图片建议尺寸：800x800px以上
- 格式：JPG/PNG

---

## 🔧 参数调整

### 视觉效果强度
编辑 `display.html` 中的CSS：

```css
/* Strobe频率（当前：0.1s = 每秒10次） */
@keyframes strobe {
    0%, 49% { background: #000; }
    50%, 100% { background: #fff; }
}
animation: strobe 0.1s infinite;  /* 修改这里调整频率 */

/* Glitch强度 */
transform: translate(...);  /* 增大数值增强位移 */
text-shadow: 5px 0 #ff0000;  /* 增大数值增强效果 */
```

### 倒计时时长
编辑 `display.html` 的JavaScript：

```javascript
const COUNTDOWN_DURATION = 30;  // 修改秒数
```

### 电机运行时间
编辑 `esp32_motor_control.ino`：

```cpp
const long RUN_DURATION = 30000;  // 毫秒，30000 = 30秒
```

### 电机速度
编辑 `esp32_motor_control.ino`：

```cpp
const int PULSE_WIDTH = 500;      // 脉冲宽度（微秒）
const int PULSE_INTERVAL = 1000;  // 脉冲间隔（增大=变慢）
```

---

## 🐛 故障排查

### 前端问题

**iPad展示端没有响应**
- [ ] 检查Supabase Realtime是否启用
- [ ] 在浏览器控制台查看错误信息
- [ ] 确认logic.js中的Supabase配置正确
- [ ] 刷新页面重新建立连接

**支付确认后没有触发**
- [ ] 检查浏览器控制台的网络请求
- [ ] 确认Supabase表权限正确设置
- [ ] 验证数据是否成功写入数据表

### 硬件问题

**ESP32无法连接WiFi**
- [ ] 确认SSID和密码正确
- [ ] 检查WiFi是否为2.4GHz（ESP32不支持5GHz）
- [ ] 查看Serial Monitor的详细输出

**电机不转动**
- [ ] 检查接线是否正确
- [ ] 确认驱动器电源是否充足
- [ ] 验证GPIO引脚是否正确配置
- [ ] 使用万用表测量引脚输出

**ESP32收不到信号**
- [ ] 确认ESP32和服务器在同一网络
- [ ] 检查Supabase URL和Key是否正确
- [ ] 查看Serial Monitor的HTTP响应状态码

---

## 📱 移动端优化

所有页面已针对移动设备优化：
- 响应式布局
- 触摸友好的按钮尺寸
- 自适应字体大小

---

## 🔒 安全建议

**生产环境部署：**
1. 使用Supabase的Row Level Security（RLS）
2. 限制API密钥的权限范围
3. 启用HTTPS（使用Vercel/Netlify等平台）
4. 定期清理Supabase历史数据

**示例RLS策略：**
```sql
-- 限制写入频率（防止刷屏）
CREATE POLICY "Rate limit inserts" ON run_signals
FOR INSERT WITH CHECK (
    NOT EXISTS (
        SELECT 1 FROM run_signals
        WHERE created_at > NOW() - INTERVAL '5 seconds'
    )
);
```

---

## 📊 监控与日志

### Supabase Dashboard
- 实时查看 `run_signals` 表的新记录
- 监控API请求量
- 查看错误日志

### ESP32 Serial Monitor
- 实时监控WiFi连接状态
- 查看接收到的RUN信号
- 调试电机控制逻辑

---

## 🎯 性能优化

### 减少延迟
- iPad展示端使用Realtime订阅（几乎零延迟）
- ESP32使用HTTP轮询（1秒间隔，可调整）

### 提升稳定性
- 自动重连WiFi机制
- 错误处理和重试逻辑
- 状态管理防止重复触发

---

## 📞 技术支持

遇到问题？检查顺序：
1. 浏览器控制台（F12）查看错误
2. Supabase Dashboard查看数据
3. ESP32 Serial Monitor查看日志
4. 检查网络连接状态

---

## 📄 文件清单

```
smile please/
├── index.html              # 观众端（入口+支付）
├── display.html            # iPad展示端
├── logic.js                # Supabase通讯逻辑
├── style.css               # 共享样式表
├── esp32_motor_control.ino # ESP32硬件代码
├── 收款码.jpg              # 支付二维码图片
├── README.md               # 项目说明
└── DEPLOYMENT.md           # 本部署指南
```

---

## 🎨 视觉风格说明

本项目采用：
- **极简主义**：纯黑白配色，无多余元素
- **故障艺术**：Glitch效果模拟数字错误美学
- **像素风格**：等宽字体，技术感界面
- **Strobe效果**：高频黑白闪烁，营造强烈视觉冲击

---

祝你的艺术装置展览成功！🎉
