# SMILE PLEASE - 交互艺术装置控制系统

> 极致黑白极简主义 + 故障艺术风格的三端联动系统

## 🎯 系统概述

这是一个基于Web的艺术装置控制系统，通过局域网实现：
- **观众端** (index.html) - 输入信息
- **触发端** (payment.html) - 显示二维码系统
- **触发端** (payment.html) - 显示二维码并触发系统
- **展示端** (display.html) - iPad全屏展示Glitch效果
- **硬件端** (ESP32-S3) - 控制步进电机30秒运行

## 🚀 5分钟快速启动

### 1. 配置Supabase（云端实时通讯）

```bash
# 访问 https://supabase.com 创建免费账号
# 创建新项目，获取URL和API密钥
```

在项目中找到：
- **Project URL**: https://xxx.supabase.co
- **Anon Key**: eyJhbGc...（很长的字符串）

### 2. 配置文件

编辑 `config.js`：
```javascript
export const SUPABASE_CONFIG = {
    url: '你的Project URL',
    anonKey: '你的Anon Key'
};
```

### 3. 创建数据库表

在Supabase SQL编辑器中运行 `supabase_setup.sql` 的内容。

### 4. 启动系统

```bash
# 方法1: 使用Python简单服务器
python3 -m http.server 8000

# 方法2: 使用npm的http-server
npm install -g http-server
http-server -p 8000
```

访问：http://localhost:8000

### 5. 配置ESP32硬件（可选）

1. 用Arduino IDE打开 `esp32_motor_controller.ino`
2. 修改WiFi和Supabase配置
3. 上传到ESP32-S3
4. 连接GPIO 12和14到步进电机驱动器

## 📱 使用流程

2. **端**显示二维码 → 点击TRIGGER SYSTEM
3. **观众端**输入文字 → 点击SUBMIT
2. **触发端**显示二维码 → 点击TRIGGER SYSTEM
3. **展示端**（iPad）自动触发：
   - 全屏黑白频闪30秒
4  - 滚动显示观众文字
   - 倒计时归零后恢复
4. **ESP32**同步驱动电机30秒

## 📄 文件说明

| 文件 | 说明 |页 |
| `payment.html` | 二维码
|------|------|
| `index.html` | 观众输入页 |
| `payment.html` | 
| `esp32_motor_controller.ino` | ESP32代码 |
| `QUICKSTART.md` | 详细快速启动指南 |
| `TROUBLESHOOTING.md` | 问题排查指南 |

## 🎨 视觉特点

- **极简主义**: 纯黑白配色，Courier New字体
- **故障艺术**: 高频闪烁、像素错位、色彩分离
- **终端美学**: 命令行风格交互界面

## 🔧 技术栈

- **前端**: 纯HTML5/CSS3/JavaScript
- **通讯**: Supabase Realtime Database
- **硬件**: ESP32-S3 + 86步进电机

## 📞 支持

详细文档：
- `QUICKSTART.md` - 完整设置流程
- `TROUBLESHOOTING.md` - 常见问题解决
- `DEPLOYMENT.md` - 生产环境部署

## ⚡ 关键特性

✅ 无需后端服务器（使用Supabase云端）  
✅ 局域网内实时同步  
✅ iPad/手机全屏沉浸式体验  
✅ ESP32硬件联动支持  
✅ 极简安装配置

---

**License**: MIT  
**Created for**: SMILE PLEASE Interactive Art Installation
