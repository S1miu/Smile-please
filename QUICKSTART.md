# Smile Please 快速配置指南

## 1. Supabase 设置（5分钟）

### 创建项目
1. 访问 https://supabase.com
2. 创建新项目
3. 记录下：
   - Project URL（形如 https://xxxxx.supabase.co）
   - Anon public key（在 Settings > API 中）

### 创建数据表
在 SQL Editor 中执行：

```sql
CREATE TABLE run_signals (
  id BIGSERIAL PRIMARY KEY,
  message TEXT NOT NULL,
  processed BOOLEAN DEFAULT false,
  created_at TIMESTAMPTZ DEFAULT NOW()
);

-- 启用实时功能
ALTER PUBLICATION supabase_realtime ADD TABLE run_signals;
```

### 启用Realtime
1. 进入 Database > Replication
2. 找到 `run_signals` 表
3. 启用 "INSERT" 事件

## 2. 配置代码（2分钟）

### 更新 logic.js
```javascript
const SUPABASE_URL = 'https://你的项目ID.supabase.co';
const SUPABASE_ANON_KEY = '你的anon-key';
```

### 更新 display.html
```javascript
const SUPABASE_URL = 'https://你的项目ID.supabase.co';
const SUPABASE_ANON_KEY = '你的anon-key';
```

### 更新 esp32_motor_control.ino
```cpp
const char* supabaseUrl = "https://你的项目ID.supabase.co";
const char* supabaseKey = "你的anon-key";
const char* ssid = "你的WiFi名称";
const char* password = "你的WiFi密码";
```

## 3. 测试系统（3分钟）

### 启动本地服务器
```bash
python3 -m http.server 8000
```

### 测试流程
1. **浏览器1** - 打开 http://localhost:8000
   - 输入消息
   - 点击提交
   - 点击确认支付

2. **浏览器2** - 打开 http://localhost:8000/display.html
   - 应该看到 "SYSTEM READY_"
   - 当支付确认后，应该立即触发显示

3. **检查控制台**
   - F12 打开开发者工具
   - 查看 Console 是否有错误

## 4. 部署到设备

### iPad 展示端
1. 在 iPad 上打开 Safari
2. 访问 `http://你的电脑IP:8000/display.html`
3. 点击 "分享" > "添加到主屏幕"
4. 设置为全屏模式

### ESP32 上传
1. 安装 Arduino IDE
2. 安装 ESP32 开发板支持
3. 安装库：WiFi, HTTPClient, ArduinoJson
4. 上传 `esp32_motor_control.ino`
5. 打开串口监视器查看连接状态

## 5. 常见问题

### Display 端没有反应？
- 检查 Supabase Realtime 是否启用
- 在浏览器控制台查看错误
- 确认 URL 和 Key 配置正确

### ESP32 连接失败？
- 检查 WiFi 密码
- 确认网络允许设备连接
- 查看串口监视器的错误信息

### 支付页面卡住？
- 检查网络连接
- 查看浏览器控制台错误
- 确认 Supabase 配置正确

## 6. 生产环境建议

### 安全性
- 使用环境变量存储密钥
- 启用 Supabase RLS（Row Level Security）
- 限制 API 访问频率

### 稳定性
- 使用稳定的 WiFi 网络
- 为 ESP32 提供稳定电源
- 定期清理数据库旧记录

### 性能优化
```sql
-- 定期清理旧记录（可设置为定时任务）
DELETE FROM run_signals 
WHERE created_at < NOW() - INTERVAL '1 day';
```

## 完成！

系统现在应该可以正常运行了。祝你的艺术装置展览顺利！

如有问题，请检查各端的控制台日志获取详细错误信息。
