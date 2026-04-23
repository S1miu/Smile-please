# 🔧 数据写入故障排查指南

## 问题：手机端点击后数据未写入 Supabase

### ✅ 检查清单

#### 1️⃣ 验证 config.js 配置

打开 `config.js` 文件，确认以下内容已正确填写：

```javascript
export const SUPABASE_CONFIG = {
    url: 'https://你的项目id.supabase.co',    // ❌ 不应该是 'YOUR_SUPABASE_URL'
    anonKey: 'eyJhb...很长的字符串'              // ❌ 不应该是 'YOUR_SUPABASE_ANON_KEY'
};
```

**如何获取正确的值：**
1. 访问 https://app.supabase.com
2. 选择你的项目
3. 点击左侧菜单 **Settings** → **API**
4. 复制 **Project URL** → 粘贴到 `url`
5. 复制 **anon public** key → 粘贴到 `anonKey`

---

#### 2️⃣ 检查表名称

打开浏览器控制台（F12），查看是否有如下错误：

```
relation "public.commands" does not exist
```

**如果出现此错误：**
- 说明表名不对，请到 Supabase 确认表名
- 如果表名不是 `commands`，修改 `logic.js` 第 50 行

```javascript
const { data, error } = await supabase
    .from('你的实际表名')  // 这里改成实际的表名
    .insert([...])
```

---

#### 3️⃣ 验证 API Key 权限

**测试步骤：**

1. 在 Supabase 控制台，进入 **Table Editor** → **commands** 表
2. 手动点击 **Insert row** 插入一条测试数据
3. 如果能成功插入，说明表存在且有权限

**检查 RLS (Row Level Security) 政策：**

1. 在 Supabase 控制台，进入 **Authentication** → **Policies**
2. 找到 `commands` 表
3. 确认有以下策略（或类似的允许 INSERT 的策略）：

```sql
-- 应该看到类似的策略
Policy: Enable insert for anon users
Check: true
Using: true
```

**如果没有政策或政策限制过严：**

执行以下 SQL（在 SQL Editor 中）：

```sql
-- 允许匿名用户插入数据
CREATE POLICY "Allow anon insert" ON commands
FOR INSERT TO anon
WITH CHECK (true);

-- 允许匿名用户读取数据
CREATE POLICY "Allow anon select" ON commands
FOR SELECT TO anon
USING (true);
```

---

#### 4️⃣ 浏览器控制台调试

1. **打开 index.html**
2. **按 F12 打开开发者工具**
3. **切换到 Console 标签**
4. **输入消息并点击按钮**

**查看控制台输出：**

✅ **成功的输出应该是：**
```
Smile Please system initialized
Run signal sent successfully: [{...}]
```

❌ **失败的输出可能是：**
```
Failed to insert run signal: {message: "...", details: "..."}
```

**常见错误及解决方案：**

| 错误信息 | 原因 | 解决方案 |
|---------|------|---------|
| `Invalid API key` | API Key 错误 | 重新复制正确的 anon key |
| `relation does not exist` | 表不存在 | 检查表名是否为 `commands` |
| `permission denied` | 权限不足 | 检查 RLS 政策 |
| `Failed to fetch` | 网络问题 | 检查网络连接，确认 URL 正确 |

---

#### 5️⃣ 实时测试脚本

在浏览器控制台（F12 → Console）粘贴以下代码进行快速测试：

```javascript
// 测试 Supabase 连接
import { createClient } from 'https://cdn.jsdelivr.net/npm/@supabase/supabase-js@2/+esm';

const testUrl = '你的 Supabase URL';  // 替换
const testKey = '你的 Anon Key';      // 替换

const testClient = createClient(testUrl, testKey);

// 尝试插入测试数据
const testInsert = async () => {
    const { data, error } = await testClient
        .from('commands')
        .insert([{ 
            message: '测试消息', 
            processed: false 
        }])
        .select();
    
    if (error) {
        console.error('❌ 插入失败:', error);
    } else {
        console.log('✅ 插入成功:', data);
    }
};

testInsert();
```

---

## 🎯 快速修复步骤

### 方案 A：重新配置 config.js

1. 获取正确的 Supabase URL 和 anon key
2. 打开 `config.js`
3. 替换占位符为真实值
4. 保存文件
5. 刷新页面重试

### 方案 B：临时关闭 RLS

如果只是测试，可以临时关闭 Row Level Security：

```sql
-- 在 Supabase SQL Editor 执行
ALTER TABLE commands DISABLE ROW LEVEL SECURITY;
```

⚠️ **警告：生产环境不推荐关闭 RLS**

---

## 📞 仍然无法解决？

请提供以下信息：

1. **浏览器控制台的完整错误信息**（截图或文本）
2. **Supabase 项目的 Table Editor 截图**（确认表名和结构）
3. **config.js 的内容**（隐藏敏感信息）

---

## ✨ 验证成功

当一切正常时，你应该看到：

1. ✅ 点击按钮后，控制台显示 `Run signal sent successfully`
2. ✅ Supabase Table Editor 中 `commands` 表有新记录
3. ✅ 页面自动跳转到支付链接
4. ✅ iPad 展示端立即触发动画
