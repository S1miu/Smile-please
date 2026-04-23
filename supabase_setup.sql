-- Smile Please 项目数据库设置
-- 请在 Supabase SQL Editor 中执行此脚本

-- 1. 创建 commands 表
CREATE TABLE IF NOT EXISTS public.commands (
    id BIGSERIAL PRIMARY KEY,
    message TEXT NOT NULL,
    processed BOOLEAN DEFAULT FALSE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

-- 2. 启用 Row Level Security (RLS)
ALTER TABLE public.commands ENABLE ROW LEVEL SECURITY;

-- 3. 创建允许公开插入的策略
CREATE POLICY "Allow public insert" 
ON public.commands 
FOR INSERT 
TO public 
WITH CHECK (true);

-- 4. 创建允许公开查询的策略
CREATE POLICY "Allow public select" 
ON public.commands 
FOR SELECT 
TO public 
USING (true);

-- 5. 创建允许公开更新的策略
CREATE POLICY "Allow public update" 
ON public.commands 
FOR UPDATE 
TO public 
USING (true);

-- 6. 启用 Realtime（实时订阅功能）
ALTER PUBLICATION supabase_realtime ADD TABLE public.commands;

-- 7. 验证设置
SELECT 'Setup completed! Table created with public access policies.' AS status;
