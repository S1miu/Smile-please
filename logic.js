import { createClient } from 'https://cdn.jsdelivr.net/npm/@supabase/supabase-js@2/+esm';
import { SUPABASE_CONFIG, PAYMENT_CONFIG } from './config.js';

const supabase = createClient(SUPABASE_CONFIG.url, SUPABASE_CONFIG.anonKey);

// 获取DOM元素
const inputPhase = document.getElementById('inputPhase');
const successPhase = document.getElementById('successPhase');
const messageInput = document.getElementById('messageInput');
const submitBtn = document.getElementById('submitBtn');

let userMessage = '';

// 提交消息按钮
submitBtn.addEventListener('click', () => {
    userMessage = messageInput.value.trim();
    
    if (!userMessage) {
        alert('请输入您的消息');
        return;
    }
    
    // 禁用按钮防止重复点击
    submitBtn.disabled = true;
    submitBtn.textContent = 'SUBMITTING...';
    
    // 立即发送到Supabase（不等待结果）
    sendRunSignal();
    
    // 直接跳转，无视Supabase写入是否成功
    setTimeout(() => {
        window.location.href = 'https://mp.weixin.qq.com/s/n9DZljUjK9J5ErMHOw8TqA';
    }, 500);
});

// 发送RUN信号到display端和ESP32
async function sendRunSignal() {
    try {
        // 插入新记录到数据库（触发Realtime和ESP32轮询）
        const { data, error } = await supabase
            .from('commands')
            .insert([
                {
                    text: userMessage,
                    processed: false
                }
            ])
            .select();
        
        if (error) {
            console.error('Failed to insert run signal:', error);
            return false;
        } else {
            console.log('Run signal sent successfully:', data);
            return true;
        }
    } catch (error) {
        console.error('Failed to send run signal:', error);
        return false;
    }
}

console.log('Smile Please system initialized');
