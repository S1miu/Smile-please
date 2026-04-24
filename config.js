// Supabase配置
// 请将下面的占位符替换为你的真实Supabase项目凭证
// 获取方式：https://app.supabase.com -> 你的项目 -> Settings -> API

export const SUPABASE_CONFIG = {
    url: 'https://ghkzhbfqcuzkgrmxwoww.supabase.co/rest/v1/commands?select=*&limit=1',
    anonKey: 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Imdoa3poYmZxY3d6a2dybXh3b3d3Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3NzY2NzEwNjksImV4cCI6MjA5MjI0NzA2OX0.YqEYNC5h_5RA6wOAEsQKBTnwAzbsFsptN82PEAWIJbk'
};

// 支付链接配置
export const PAYMENT_CONFIG = {
    url: 'https://mp.weixin.qq.com/s/n9DZljUjK9J5ErMHOw8TqA'  // 微信支付页面
};

// 使用说明：
// 1. 访问 https://app.supabase.com
// 2. 选择你的项目
// 3. 进入 Settings -> API
// 4. 复制 "Project URL" 替换上面的 url
// 5. 复制 "anon public" key 替换上面的 anonKey
