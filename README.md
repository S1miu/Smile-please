# SMILE PLEASE - 交互艺术装置控制系统

> 极致黑白极简主义 + 故障艺术

## 📋 项目概述

这是一个三端联动的艺术装置系统：
- **观众端**：输入消息
- **支付模拟端**：扫码确认
- **iPad展示端**：极致视觉呈现
- **ESP32硬件控制**：驱动86步进电机

---

## 🎯 快速开始

详细配置请查看 [QUICKSTART.md](QUICKSTART.md)

### 核心配置（3步完成）

#### 1. 创建 Supabase 项目

1. 访问 [supabase.com](https://supabase.com) 并注册
2. 创建新项目，在 SQL Editor 中执行：

```sql
CREATE TABLE run_signals (
  id BIGSERIAL PRIMARY KEY,
  message TEXT NOT NULL,
  processed BOOLEAN DEFAULT false,
  created_at TIMESTAMPTZ DEFAULT NOW()
);

ALTER PUBLICATION supabase_realtime ADD TABLE run_signals;
```

3. 获取凭据：Settings > API 中复制 Project URL 和 Anon public key

#### 2. 配置文件

编辑 `config.js`：

```javascript
const SUPABASE_URL = 'https://你的项目ID.supabase.co';
const SUPABASE_ANON_KEY = '你的anon-key';
```

#### 3. 运行项目

**在本地测试：**
```bash
# 方法1：使用 Python
python3 -m http.server 8000

# 方法2：使用 Node.js
npx http-server -p 8000

# 方法3：使用 VS Code
# 安装 Live Server 插件，右键点击 index.html → Open with Live Server
```

**访问地址：**
- 观众端：http://localhost:8000
- iPad展示端：http://localhost:8000/display.html

---

### 方案B：使用本地 Node.js 服务器

#### 1. 创建服务器

创建 `server.js`：

```javascript
const express = require('express');
const cors = require('cors');
const app = express();

app.use(cors());
app.use(express.json());
app.use(express.static(__dirname));

let currentCommand = null;

// 获取命令
app.get('/api/status', (req, res) => {
  res.json(currentCommand || { command: 'IDLE', message: '' });
});

// 发送命令
app.post('/api/command', (req, res) => {
  currentCommand = {
    id: Date.now().toString(),
    command: req.body.command,
    message: req.body.message,
    timestamp: Date.now()
  };
  res.json({ success: true });
});

app.listen(3000, () => {
  console.log('Server running on http://localhost:3000');
});
```

#### 2. 安装依赖并运行

```bash
npm init -y
npm install express cors
node server.js
```

#### 3. 修改前端代码

在 `index.html`、`payment.html` 和 `display.html` 中，替换 Supabase 代码为：

```javascript
// 发送命令
async function sendCommand(message) {
  await fetch('http://localhost:3000/api/command', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ command: 'RUN', message })
  });
}

// 轮询状态（display.html）
setInterval(async () => {
  const response = await fetch('http://localhost:3000/api/status');
  const data = await response.json();
  if (data.command === 'RUN') {
    triggerDisplay(data.message);
  }
}, 1000);
```

---

## 🔧 ESP32-S3 硬件配置

### 1. 安装 Arduino IDE 库

在 Arduino IDE 中安装以下库：
- **WiFi**（ESP32内置）
- **HTTPClient**（ESP32内置）
- **ArduinoJson**（通过库管理器安装）

### 2. 配置 ESP32

打开 `esp32_control.ino`，修改以下参数：

```cpp
// WiFi配置
const char* ssid = "你的WiFi名称";
const char* password = "你的WiFi密码";

// API配置（选择一种）
// 方案A：Supabase
const char* apiUrl = "https://你的项目ID.supabase.co/rest/v1/system_commands?select=*&order=created_at.desc&limit=1";
const char* apiKey = "你的anon key";

// 方案B：本地服务器
// const char* apiUrl = "http://192.168.1.100:3000/api/status";
```

### 3. 硬件连接

```
ESP32-S3 引脚连接：
├─ GPIO 12 → 步进电机驱动器 IN1
├─ GPIO 14 → 步进电机驱动器 IN2
├─ GPIO 2  → 板载LED（状态指示）
└─ GND     → 共地
```

### 4. 上传代码

1. 选择开发板：工具 → 开发板 → ESP32 → ESP32-S3 Dev Module
2. 选择端口：工具 → 端口 → (选择对应COM口)
3. 点击"上传"按钮

---

## 🎨 使用流程

### 完整体验流程：

1. **观众输入**
   - 打开 `index.html`
   - 输入消息（例如："HELLO WORLD"）
   - 点击 "SUBMIT TO SYSTEM"
   - 看到 "MESSAGE TRANSMITTED" 确认

2. **iPad 展示**
   - `display.html` 检测到新消息
   - 立即触发 30 秒视觉效果：
     - ⚡ 黑白闪烁（Strobe Effect）
     - 📜 消息滚动（Glitch 像素风）
     - ⏱️ 实时倒计时
   - 自动恢复静默状态

3. **硬件响应**
   - ESP32 检测到新消息
   - 向 GPIO 12/14 发送 30 秒脉冲
   - 步进电机开始运转
   - 30秒后自动停止

---

## 📱 部署建议

### iPad 展示端设置

1. **全屏模式**
   - Safari：点击分享 → 添加到主屏幕
   - 从主屏幕启动即为全屏

2. **防止息屏**
   - 设置 → 显示与亮度 → 自动锁定 → 永不

3. **固定页面**
   - 设置 → 辅助功能 → 引导式访问 → 开启
   - 三击侧边按钮启动引导式访问

### 局域网部署

```bash
# 1. 找到电脑IP地址
# macOS/Linux:
ifconfig | grep "inet "

# Windows:
ipconfig

# 2. 启动服务器
python3 -m http.server 8000

# 3. 其他设备访问
# http://你的IP:8000/index.html
# 例如：http://192.168.1.100:8000/index.html
```

---

## 🎭 视觉效果说明

### 色彩方案
- **主色调**：纯黑 (#000) + 纯白 (#fff)
- **无灰色过渡**：极致对比

### 字体
- **主字体**：Courier Prime（等宽字体）
- **备选**：Courier New, Monospace

### 核心动画

1. **Glitch Effect（故障艺术）**
   - 文字切断位移
   - RGB 色彩分离
   - 随机扰动

2. **Strobe Effect（频闪）**
   - 0.1秒黑白快速切换
   - ⚠️ 警告：可能引起光敏癫痫

3. **文字滚动**
   - 像素风格渲染
   - 无限循环滚动
   - 实时 Glitch 叠加

---

## ⚙️ 自定义参数

### 修改运行时长

**前端（display.html）：**
```javascript
const DISPLAY_DURATION = 30000; // 改为你想要的毫秒数
```

**ESP32（esp32_control.ino）：**
```cpp
const int PULSE_DURATION = 30000; // 改为你想要的毫秒数
```

### 修改轮询频率

**前端：**
```javascript
setInterval(checkStatus, 1000); // 改为你想要的间隔（毫秒）
```

**ESP32：**
```cpp
const int POLL_INTERVAL = 1000; // 改为你想要的间隔（毫秒）
```


---

## 🐛 常见问题

### Q1: iPad 展示端没有反应？
**A:** 检查以下项目：
1. 确认 iPad 和其他设备在同一WiFi
2. 打开浏览器控制台查看是否有错误
3. 确认 Supabase 配置正确
4. 尝试手动刷新页面

### Q2: ESP32 无法连接WiFi？
**A:** 
1. 检查 WiFi 名称和密码是否正确
2. 确保 WiFi 为 2.4GHz（ESP32 不支持 5GHz）
3. 查看串口监视器的错误信息

### Q3: 电机不转动？
**A:**
1. 检查 GPIO 引脚连接
2. 确认步进电机驱动器供电
3. 使用串口监视器查看是否收到 RUN 指令
4. 调整 `generatePulse()` 函数中的脉冲宽度


---

## 📦 文件结构

```
smile-please/
├── index.html              # 观众入口页
├── display.html            # iPad展示页
├── logic.js                # 核心逻辑
├── config.js               # Supabase配置
├── style.css               # 统一样式表
├── esp32_motor_control.ino # ESP32控制代码
├── README.md               # 本文档
├── QUICKSTART.md           # 快速开始指南
├── SETUP.md                # 详细设置说明
└── DEPLOYMENT.md           # 部署指南
```

---

## 🎬 演出前检查清单

- [ ] 所有设备连接同一WiFi
- [ ] Supabase 配置正确并正常运行
- [ ] iPad 全屏模式设置完成
- [ ] iPad 已关闭自动锁屏
- [ ] ESP32 已上传代码并启动
- [ ] 电机硬件连接正常
- [ ] 测试完整流程至少一次
- [ ] 浏览器控制台无错误信息

---

## 📄 开源协议

MIT License - 自由使用和修改

---

## 🙏 鸣谢

- **字体**：Courier Prime by Quote-Unquote Apps
- **通讯方案**：Supabase
- **硬件**：ESP32-S3

---

## 💡 提示

这是一个艺术装置项目，Strobe Effect（频闪效果）可能对光敏感人群造成不适。在公共展示时请：

1. 在入口处放置警示标识
2. 降低频闪频率（修改 CSS 中的 `0.1s` 为更大的值）
3. 提供无频闪的备用模式

---

**祝你的艺术装置大获成功！** 🎉
