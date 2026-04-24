# 🎨 SMILE PLEASE - 最终部署指南

## ⚡ 快速启动（3分钟）

### 1️⃣ 配置 Supabase（首次使用）

```bash
# 访问 https://supabase.com 创建项目
# 复制 URL 和 anon key 到 config.js
```

**config.js 示例：**
```javascript
export const SUPABASE_CONFIG = {
    url: 'https://你的项目ID.supabase.co',
    anonKey: '你的anon-key'
};
```

**执行 SQL：**
- 在 Supabase SQL Editor 中运行 `supabase_setup.sql`

---

### 2️⃣ 启动本地服务器

```bash
# 方法1：使用 Python
python3 -m http.server 8000

# 方法2：使用 Node.js
npx http-server -p 8000

# 方法3：使用 PHP
php -S localhost:8000
```

---

### 3️⃣ 打开三个页面

| 设备 | URL | 用途 |
|------|-----|------|
| **手机（观众）** | `http://你的电脑IP:8000/index.html` | 输入消息并支付 |
| **iPad（展示）** | `http://你的电脑IP:8000/display.html` | 全屏显示效果 |
| **电脑（监控）** | Supabase Dashboard | 查看数据库记录 |

**查找电脑 IP：**
```bash
# macOS/Linux
ifconfig | grep "inet " | grep -v 127.0.0.1

# Windows
ipconfig
```

---

## 🧪 测试流程

### A. 基础测试（不需要硬件）

1. **iPad 打开 display.html**
   - 应显示：黑屏 + 右下角"SYSTEM READY_"闪烁

2. **手机打开 index.html**
   - 输入：`你好世界`
   - 点击：SUBMIT TO SYSTEM
   - 应显示：收款码页面

3. **手机点击 CONFIRM PAYMENT**
   - 应显示：3秒验证动画 → SUCCESS页面

4. **iPad 应立即触发：**
   - 黑白闪烁（strobe effect）
   - 文字滚动/居中显示
   - 30秒倒计时
   - 结束后恢复待机

### B. 硬件测试（ESP32-S3）

```arduino
// 上传 esp32_motor_controller/esp32_motor_controller.ino
// 修改 WiFi 和 Supabase 配置
// 观察串口输出
```

---

## 🎯 预期效果

### 短文字（≤5字）
- **居中固定显示**
- 例如："你好" 将居中不动

### 长文字（>5字）
- **从右向左滚动**
- 速度：每秒3个字符
- 循环播放直到30秒结束

### Glitch 效果
- **黑白闪烁频率：** 每0.1秒切换
- **文字抖动：** CSS动画自动实现

---

## 🔧 常见问题

### iPad 没有反应？
```javascript
// 在 display.html 的浏览器控制台测试
window.testDisplay('测试');
```

### 收款码不显示？
- 检查 `收款码.jpg` 是否在项目根目录
- 路径是否正确：`./收款码.jpg`

### WebSocket 连接失败？
1. 检查 config.js 配置
2. 确认 Supabase 项目已启用 Realtime
3. 查看浏览器 Console 错误信息

---

## 📱 设备布局建议

```
┌─────────────────────────────────────┐
│  iPad (全屏显示)                     │
│  display.html                        │
│                                      │
│  [黑屏 + SYSTEM READY_]              │
│                                      │
└─────────────────────────────────────┘

┌──────────┐    ┌──────────┐
│ 观众手机  │    │ ESP32-S3 │
│ index.html│────│ 步进电机  │
└──────────┘    └──────────┘
```

---

## 🚀 部署检查清单

- [ ] Supabase 项目已创建
- [ ] config.js 已配置正确的 URL 和 Key
- [ ] SQL 表已创建（commands 表）
- [ ] 收款码.jpg 已放置在项目根目录
- [ ] 本地服务器已启动
- [ ] iPad 已打开 display.html（全屏模式）
- [ ] 手机已连接同一 WiFi
- [ ] ESP32（可选）已上传代码并连接 WiFi

---

## 💡 优化建议

### 性能优化
- iPad 使用 Safari 全屏模式
- 关闭 iPad 自动锁屏
- 确保 WiFi 信号稳定

### 视觉优化
- 调整 display.html 中的字体大小（80px → 你想要的尺寸）
- 修改闪烁频率（0.1s → 其他值）
- 自定义倒计时时长（30秒 → 其他值）

---

## 📞 技术支持

遇到问题？
1. 查看 `TROUBLESHOOTING.md`
2. 检查浏览器控制台（F12）
3. 查看 Supabase Logs

---

**© 2024 SMILE PLEASE - Minimalist Glitch Art Installation**
