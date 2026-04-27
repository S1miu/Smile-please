/**
 * SMILE PLEASE - 前端提交逻辑 (Fixed Version)
 * 修正了：Supabase URL、字段名、微信跳转链接
 */

import { createClient } from 'https://cdn.jsdelivr.net/npm/@supabase/supabase-js@2/+esm';

// 配置信息
const SUPABASE_URL = "https://ghkzhbfqcwzkgrmxwoww.supabase.co"; // 确保是 qcwz
const SUPABASE_KEY = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Imdoa3poYmZxY3d6a2dybXh3b3d3Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3NzY2NzEwNjksImV4cCI6MjA5MjI0NzA2OX0.YqEYNC5h_5RA6wOAEsQKBTnwAzbsFsptN82PEAWIJbk";
const WECHAT_URL = "https://mp.weixin.qq.com/s/n9DZljUjK9J5ErMHOw8TqA";

const supabase = createClient(SUPABASE_URL, SUPABASE_KEY);

const messageInput = document.getElementById('messageInput');
const submitBtn = document.getElementById('submitBtn');

async function submitMessage() {
    const userMessage = messageInput.value.trim();
    
    if (!userMessage) {
        alert('请输入内容');
        return;
    }
    
    // 禁用界面
    submitBtn.disabled = true;
    submitBtn.textContent = 'TRANSMITTING...';
    
    try {
        // 插入记录到 commands 表
        const { data, error } = await supabase
            .from('commands')
            .insert([
                {
                    text: userMessage, // 数据库列名
                    status: 'Run'      // 电机触发标记
                }
            ]);
        
        if (error) throw error;

        console.log('Success:', data);
        
        // 成功后跳转到微信链接
        window.location.href = WECHAT_URL;

    } catch (error) {
        console.error('Failed:', error.message);
        alert('发送失败，请检查网络: ' + error.message);
        submitBtn.disabled = false;
        submitBtn.textContent = 'SUBMIT TO SYSTEM';
    }
}

// 绑定按钮事件
submitBtn.addEventListener('click', submitMessage);