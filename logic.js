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
submitBtn.addEventListener('click', async () => {
    userMessage = messageInput.value.trim();
    
    if (!userMessage) {
        alert('请输入您的消息');
        return;
    }
    
    // 禁用按钮防止重复点击
    submitBtn.disabled = true;
    submitBtn.textContent = 'SUBMITTING...';
    
    // 发送RUN信号（触发iPad和ESP32）
    const success = await sendRunSignal();
    
    // 确保数据成功写入后再跳转
    if (success) {
        window.location.href = PAYMENT_CONFIG.url;
    } else {
        alert('消息发送失败，请重试');
        submitBtn.disabled = false;
        submitBtn.textContent = 'SUBMIT TO SYSTEM';
    }
});

// 发送RUN信号到display端和ESP32
async function sendRunSignal() {
    try {
        // 插入新记录到数据库（触发Realtime和ESP32轮询）
        const { data, error } = await supabase
            .from('run_signals')
            .insert([
                {
                    message: userMessage,
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
