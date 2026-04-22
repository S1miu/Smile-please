# 🚀 快速配置指南

## 第一步：获取 Supabase 凭证

1. 访问 [Supabase](https://app.supabase.com)
2. 登录并创建新项目（或选择现有项目）
3. 进入 **Settings** → **API**
4. 复制以下两个值：
   - **Project URL**（例如：`https://xxxxx.supabase.co`）
   - **anon public key**（以 `eyJ` 开头的长字符串）

## 第二步：配置数据库

1. 在 Supabase 项目中，进入 **SQL Editor**
2. 复制并执行 `database_setup.sql` 中的 SQL 代码
3. 确认 `run_signals` 表已创建成功

## 第三步：更新配置文件

打开 `config.js`，将占位符替换为你的真实凭证：

```javascript
export const SUPABASE_CONFIG = {
    url: 'https://xxxxx.supabase.co',  // 你的 Project URL
    anonKey: 'eyJhbGc...'  // 你的 anon public key
};
```

## 第四步：本地测试

1. 在项目目录下打开终端
2. 启动本地服务器：
   ```bash
   python3 -m http.server 8000
   ```
3. 打开浏览器访问：
   - 观众端：`http://localhost:8000/index.html`
   - iPad端：`http://localhost:8000/display.html`

## 第五步：局域网部署

### 方法A：使用本机作为服务器

1. 获取你的本机IP地址：
   ```bash
   # macOS/Linux
   ifconfig | grep "inet "
   
   # 或使用
   ipconfig getifaddr en0
   ```

2. 确保防火墙允许端口 8000

3. 其他设备访问：
   - 观众端：`http://你的IP:8000/index.html`
   - iPad端：`http://你的IP:8000/display.html`

### 方法B：使用专业服务器

参考 `DEPLOYMENT.md` 中的部署方案

## 第六步：ESP32 配置（可选）

1. 在 `esp32_motor_control.ino` 中修改：
   - WiFi 名称和密码
   - Supabase URL 和 API Key
   
2. 使用 Arduino IDE 上传到 ESP32-S3 开发板

3. 确保 ESP32 与电脑在同一局域网

## 验证测试

1. 打开 display.html（iPad端），应显示"SYSTEM READY"
2. 打开 index.html（观众端）
3. 输入测试消息，点击提交
4. 点击"确认支付"
5. iPad端应立即触发30秒的闪烁效果
6. ESP32（如果已连接）应同时驱动电机30秒

## 常见问题

### Q: Display端没有反应？
- 检查 config.js 配置是否正确
- 打开浏览器控制台查看错误信息
- 确认 Supabase 数据库表已正确创建

### Q: 本地可以运行，但局域网访问不到？
- 检查防火墙设置
- 确认所有设备在同一WiFi网络
- 尝试关闭防火墙测试

### Q: ESP32 无法连接？
- 检查 WiFi 密码是否正确
- 确认 Supabase URL 使用 HTTPS
- 查看串口监视器的调试信息

## 技术支持

遇到问题请检查：
1. 浏览器控制台（F12）的错误信息
2. Supabase 项目的 Logs 页面
3. ESP32 串口监视器的输出

---

✨ **配置完成！开始创作你的交互艺术作品吧！**
