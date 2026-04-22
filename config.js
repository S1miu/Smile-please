// Supabase配置
// 请将下面的占位符替换为你的真实Supabase项目凭证
// 获取方式：https://app.supabase.com -> 你的项目 -> Settings -> API

export const SUPABASE_CONFIG = {
    url: 'YOUR_SUPABASE_URL',           // 替换为你的 Supabase Project URL
    anonKey: 'YOUR_SUPABASE_ANON_KEY'   // 替换为你的 Anon/Public Key
};

// 支付链接配置
export const PAYMENT_CONFIG = {
    url: 'YOUR_PAYMENT_URL'  // 替换为你的支付链接（如微信/支付宝收款页面）
};

// 使用说明：
// 1. 访问 https://app.supabase.com
// 2. 选择你的项目
// 3. 进入 Settings -> API
// 4. 复制 "Project URL" 替换上面的 url
// 5. 复制 "anon public" key 替换上面的 anonKey
