import { createClient } from 'https://cdn.jsdelivr.net/npm/@supabase/supabase-js@2/+esm';

// Supabase配置
const SUPABASE_URL = 'YOUR_SUPABASE_URL';
const SUPABASE_ANON_KEY = 'YOUR_SUPABASE_ANON_KEY';

const supabase = createClient(SUPABASE_URL, SUPABASE_ANON_KEY);

// 获取DOM元素
const inputPhase = document.getElementById('inputPhase');
const paymentPhase = document.getElementById('paymentPhase');
const verifyingPhase = document.getElementById('verifyingPhase');
const successPhase = document.getElementById('successPhase');

const messageInput = document.getElementById('messageInput');
const submitBtn = document.getElementById('submitBtn');
const confirmBtn = document.getElementById('confirmBtn');
const countdownNum = document.getElementById('countdownNum');

let userMessage = '';

// 提交消息按钮
submitBtn.addEventListener('click', () => {
    userMessage = messageInput.value.trim();
    
    if (!userMessage) {
        alert('请输入您的消息');
        return;
    }
    
    // 切换到支付页面
    inputPhase.classList.add('hidden');
    paymentPhase.classList.remove('hidden');
});

// 确认支付按钮
confirmBtn.addEventListener('click', async () => {
    // 切换到验证页面
    paymentPhase.classList.add('hidden');
    verifyingPhase.classList.remove('hidden');
    
    // 3秒后发送RUN信号
    setTimeout(async () => {
        await sendRunSignal();
        
        // 显示成功页面
        verifyingPhase.classList.add('hidden');
        successPhase.classList.remove('hidden');
        
        // 倒计时后自动刷新
        let count = 3;
        const countdownInterval = setInterval(() => {
            count--;
            countdownNum.textContent = count;
            
            if (count <= 0) {
                clearInterval(countdownInterval);
                // 刷新页面
                window.location.reload();
            }
        }, 1000);
    }, 3000);
});

// 发送RUN信号到display端和ESP32
async function sendRunSignal() {
    try {
        // 通过Supabase Realtime广播
        const channel = supabase.channel('display-trigger');
        
        await channel.send({
            type: 'broadcast',
            event: 'run',
            payload: {
                message: userMessage,
                timestamp: new Date().toISOString()
            }
        });
        
        console.log('Run signal sent successfully');
    } catch (error) {
        console.error('Failed to send run signal:', error);
    }
}

console.log('Smile Please system initialized');
