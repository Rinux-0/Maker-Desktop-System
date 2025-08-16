<template>
  <div class="ai-sidebar sidebar-toggle" :class="{ collapsed: isCollapsed }">
    <div class="sidebar-header animate fade-in-down">
      <h4 v-show="!isCollapsed" class="animate scale-in delay-200">AI助手</h4>
      <el-button size="small" @click="toggleCollapse" v-show="!isCollapsed" class="btn-animate hover-lift magic-button">
        收起
      </el-button>
    </div>

    <!-- 收起状态下的点击区域 -->
    <div v-if="isCollapsed" class="collapsed-area animate bounce-in" @click="toggleCollapse">
      <div class="collapsed-icon animate float magic-icon">
        <el-icon><ChatDotRound /></el-icon>
      </div>
      <!-- <div class="collapsed-text">AI助手</div> -->
    </div>

    <div class="sidebar-content" v-show="!isCollapsed">
      <!-- 对话区域 -->
      <div class="chat-area" ref="chatArea">
        <div
          v-for="(message, index) in aiStore.messages"
          :key="index"
          class="message message-bubble"
          :class="[message.type, 'user-message-enter']"
          :style="{ animationDelay: `${index * 0.1}s` }"
        >
          <div class="message-time">{{ formatTime(message.timestamp) }}</div>
          <div class="message-content">
            {{ message.content }}
            <span v-if="message.isAnalysis" class="analysis-badge animate sparkle magic-badge">健康分析</span>
          </div>
        </div>

        <!-- 流式响应显示 -->
        <div v-if="aiStore.isStreaming" class="message ai animate message-enter message-bubble">
          <div class="message-content">
            <span>{{ currentStreamContent }}</span>
            <span class="typing-indicator animate pulse magic-typing">▋</span>
          </div>
          <div class="message-time animate fade-in-up delay-200">正在分析...</div>
        </div>
      </div>

      <!-- 输入区域 -->
      <div class="input-area">
        <div class="input-controls">
          <!-- 语音输入按钮 -->
          <el-button
            :type="recordStatus ? 'danger' : 'primary'"
            size="small"
            @click="toggleVoiceInput"
            :loading="aiStore.loading"
            :disabled="aiStore.isStreaming"
            class="voice-button btn-animate hover-lift magic-voice-btn"
            :class="{ 'recording': recordStatus }"
          >
            <el-icon><Microphone /></el-icon>
            {{ recordStatus ? '结束语音' : '语音输入' }}
          </el-button>

          <el-button
            type="success"
            size="small"
            @click="analyzeHealth"
            :loading="aiStore.loading"
            :disabled="aiStore.isStreaming || !userStore.currentUser"
            class="btn-animate hover-lift magic-analyze-btn"
          >
            <el-icon><DataAnalysis /></el-icon>
            健康分析
          </el-button>
        </div>

        <el-input
          v-model="inputMessage"
          type="textarea"
          :rows="3"
          placeholder="输入消息... (Ctrl+Enter发送)"
          @keyup.ctrl.enter="sendMessage"
          :disabled="aiStore.isStreaming"
          class="input-focus magic-input"
        />

        <div class="input-actions animate fade-in-up delay-200">
          <el-button
            type="primary"
            @click="sendMessage"
            :loading="aiStore.loading"
            :disabled="aiStore.isStreaming || !inputMessage.trim()"
            class="btn-animate hover-lift magic-send-btn"
          >
            发送
          </el-button>
          <el-switch v-model="aiStore.ttsEnabled" active-text="语音播报" class="hover-glow magic-switch" />
        </div>
      </div>

      <!-- 错误提示 -->
      <el-alert
        v-if="aiStore.error"
        :title="aiStore.error"
        type="error"
        show-icon
        closable
        @close="aiStore.error = null"
        class="animate shake magic-alert"
      />
    </div>
  </div>
</template>

<script setup>
import { ref, nextTick, onMounted } from 'vue'
import { useAIStore, useUserStore } from '@/stores'
import { ElMessage } from 'element-plus'
import { Microphone, DataAnalysis, ChatDotRound } from '@element-plus/icons-vue'

const aiStore = useAIStore()
const userStore = useUserStore()

// 状态
const isCollapsed = ref(false)
const inputMessage = ref('')
const chatArea = ref(null)
const currentStreamContent = ref('')

// 全局录音状态哨兵
const recordStatus = ref(false)

// 切换收起/展开
const toggleCollapse = () => {
  isCollapsed.value = !isCollapsed.value
}

// 切换语音输入
const toggleVoiceInput = async () => {
  if (recordStatus.value) {
    // 结束录音
    await stopRecording()
  } else {
    // 开始录音
    await startRecording()
  }
}

// 开始录音
const startRecording = async () => {
  try {
    const response = await fetch('/api/ai_aide/record/', {
      method: 'POST'
    })

    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`)
    }

    // 更新录音状态
    recordStatus.value = true
    ElMessage.success('开始录音，请说话...')

  } catch (error) {
    console.error('启动录音失败:', error)
    ElMessage.error('启动录音失败: ' + error.message)
  }
}

// 结束录音
const stopRecording = async () => {
  try {
    // 1. 调用后端结束录音
    const stopResponse = await fetch('/api/ai_aide/record/', {
      method: 'PUT'
    })

    if (!stopResponse.ok) {
      throw new Error(`停止录音失败: ${stopResponse.status}`)
    }

    // 更新录音状态
    recordStatus.value = false
    ElMessage.info('录音已停止，正在识别...')

    // 2. 调用后端进行ASR识别（流式）
    const asrResponse = await fetch('/api/ai_aide/asr/', {
      method: 'POST'
    })

    if (!asrResponse.ok) {
      throw new Error(`ASR识别失败: ${asrResponse.status}`)
    }

    // 3. 实时将流式获取的文本追加到输入框
    const reader = asrResponse.body.getReader()
    const decoder = new TextDecoder()
    let asrText = ''

    while (true) {
      const { done, value } = await reader.read()
      if (done) break

      // 1. 解码二进制数据为字符串
      const chunk = decoder.decode(value, { stream: true });

      // 2. 去除前6个字符（假设前6个字符是协议头等无用信息）
      const trimmedChunk = chunk.replace("data: ", "").replace("\n\n", "").trim();

      // 3. 解析为JSON对象
      const jsonData = JSON.parse(trimmedChunk);

      // 4. 提取content字段的值
      asrText += jsonData.content;

      // 5. 用最终的文本覆盖输入框
      inputMessage.value = asrText
    }

    // 4. 调用后端恢复标点（非流式）
    if (asrText.trim()) {
      const punctuationResponse = await fetch('/api/ai_aide/asr/?str_input=' + asrText, {
        method: 'PUT',
        headers: {
          'Content-Type': 'application/json'
        },
        // body: JSON.stringify({ text: asrText })
      })

      if (punctuationResponse.ok) {
        const result = await punctuationResponse.json()
        // 5. 用最终的文本覆盖输入框
        inputMessage.value = result.content || asrText
        ElMessage.success('语音识别完成')
      } else {
        ElMessage.warning('标点恢复失败，使用原始识别结果')
      }
    } else {
      ElMessage.warning('未识别到语音内容')
    }

  } catch (error) {
    console.error('停止录音或识别失败:', error)
    ElMessage.error('语音识别失败: ' + error.message)
    recordStatus.value = false
  }
}

// 查询录音状态
const checkRecordStatus = async () => {
  try {
    const response = await fetch('/api/ai_aide/record/', {
      method: 'GET'
    })

    if (response.ok) {
      const result = await response.json()
      recordStatus.value = result.status || false
    }
  } catch (error) {
    console.error('查询录音状态失败:', error)
    recordStatus.value = false
  }
}

// 发送普通消息
const sendMessage = async () => {
  if (!inputMessage.value.trim() || aiStore.isStreaming) return

  const userMessage = inputMessage.value
  inputMessage.value = ''

  // 添加用户消息
  aiStore.addMessage({
    type: 'user',
    content: userMessage,
    timestamp: new Date()
  })

  // 滚动到底部
  scrollToBottom()

  // 清空当前流式内容
  currentStreamContent.value = ''

  // 流式发送消息
  try {
    await aiStore.sendMessageStream(userMessage, (chunk) => {
      currentStreamContent.value += chunk
      scrollToBottom()
    })

    // 流式响应完成后，添加到消息列表
    if (currentStreamContent.value) {
      aiStore.addMessage({
        type: 'ai',
        content: currentStreamContent.value,
        // timestamp: new Date()
      })

      // TTS播报
      if (aiStore.ttsEnabled) {
        aiStore.textToSpeech(currentStreamContent.value)
      }

      currentStreamContent.value = ''
      scrollToBottom()
    }
  } catch (error) {
    console.error('发送消息失败:', error)
    ElMessage.error('发送消息失败')
  }
}

// 分析健康数据
const analyzeHealth = async () => {
  if (!userStore.currentUser) {
    ElMessage.warning('请先选择用户')
    return
  }

  // 清空当前流式内容
  currentStreamContent.value = ''

  // 流式分析健康数据
  try {
    await aiStore.analyzeHealthStream(userStore.currentUser.id, (chunk) => {
      currentStreamContent.value += chunk
      scrollToBottom()
    })

    // 流式响应完成后，添加到消息列表
    if (currentStreamContent.value) {
      aiStore.addMessage({
        type: 'ai',
        content: currentStreamContent.value,
        timestamp: new Date(),
        isAnalysis: true
      })

      // TTS播报分析结果
      if (aiStore.ttsEnabled) {
        await aiStore.textToSpeech(currentStreamContent.value)
      }

      currentStreamContent.value = ''
      scrollToBottom()
    }
  } catch (error) {
    console.error('分析健康数据失败:', error)
    ElMessage.error('分析健康数据失败')
  }
}

// 滚动到底部
const scrollToBottom = () => {
  nextTick(() => {
    if (chatArea.value) {
      chatArea.value.scrollTop = chatArea.value.scrollHeight
    }
  })
}

// 添加消息
const addMessage = (type, content) => {
  aiStore.addMessage({
    type,
    content,
    timestamp: new Date()
  })
  scrollToBottom()
}

// 格式化时间
const formatTime = (timestamp) => {
  if (!timestamp) return ''
  const date = new Date(timestamp)
  return date.toLocaleTimeString('zh-CN', {
    hour: '2-digit',
    minute: '2-digit'
  })
}

// 组件挂载时初始化
onMounted(async () => {
  // 添加欢迎消息
  if (aiStore.messages.length === 0) {
    aiStore.addMessage({
      type: 'ai',
      content: '你好！我是你的AI助手，有什么可以帮助你的吗？',
      timestamp: new Date()
    })
  }

  // 前端启动时，更新录音状态
  await checkRecordStatus()
})
</script>

<style lang="scss" scoped>
.ai-sidebar {
  position: fixed;
  right: 0;
  top: 0;
  bottom: 0; // 占据整个右侧高度
  width: 350px;
  background: rgba(255, 255, 255, 0.1);
  backdrop-filter: blur(10px);
  border-left: 1px solid rgba(255, 255, 255, 0.2);
  transition: all 0.4s cubic-bezier(0.4, 0, 0.2, 1);
  display: flex;
  flex-direction: column;
  transform-origin: right center;

  &.collapsed {
    width: 50px;
    transform: scaleX(0.95);
    
    .sidebar-content {
      opacity: 0;
      transform: translateX(20px);
    }
  }
}

.sidebar-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 15px;
  border-bottom: 1px solid rgba(255, 255, 255, 0.1);
  flex-shrink: 0;
  position: relative;

  h4 {
    margin: 0;
    color: #fff;
    font-size: 18px;
    font-weight: 600;
  }
}

.sidebar-content {
  flex: 1;
  display: flex;
  flex-direction: column;
  overflow: hidden;
  transition: all 0.4s cubic-bezier(0.4, 0, 0.2, 1);
}

.chat-area {
  flex: 1;
  overflow-y: auto;
  padding: 15px;
  scroll-behavior: smooth;
}

.message {
  margin-bottom: 15px;

  &.user {
    text-align: right;

    .message-content {
      background: #409eff;
      color: white;
      border-radius: 15px 15px 0 15px;
      text-align: left; // 用户消息内容左对齐
    }
  }

  &.ai {
    text-align: left;

    .message-content {
      background: rgba(255, 255, 255, 0.1);
      border-radius: 15px 15px 15px 0;
      text-align: left; // AI消息内容左对齐
    }
  }
}

.message-content {
  display: inline-block;
  max-width: 80%;
  padding: 10px 15px;
  word-wrap: break-word;
  line-height: 1.4;
  white-space: pre-wrap;
  position: relative;
  text-align: left; // 确保所有消息内容都是左对齐
}

.analysis-badge {
  position: absolute;
  top: -8px;
  right: -8px;
  background: #67c23a;
  color: white;
  font-size: 10px;
  padding: 2px 6px;
  border-radius: 10px;
  font-weight: bold;
}

.message-time {
  font-size: 12px;
  color: rgba(255, 255, 255, 0.6);
  margin-bottom: 5px;
  text-align: center;
}

.typing-indicator {
  animation: blink 1s infinite;
  color: #409eff;
}

@keyframes blink {
  0%, 50% { opacity: 1; }
  51%, 100% { opacity: 0; }
}

.input-area {
  padding: 15px;
  border-top: 1px solid rgba(255, 255, 255, 0.1);
  flex-shrink: 0;
}

.input-controls {
  display: flex;
  gap: 10px;
  margin-bottom: 10px;
}

.voice-button {
  transition: all 0.3s ease;
  position: relative;
  overflow: hidden;

  &:before {
    content: '';
    position: absolute;
    top: 0;
    left: -100%;
    width: 100%;
    height: 100%;
    background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.3), transparent);
    transition: left 0.6s ease;
  }

  &:hover:before {
    left: 100%;
  }

  &.recording {
    background: rgba(245, 108, 108, 0.8) !important;
    border-color: rgba(245, 108, 108, 0.8) !important;
    animation: pulse 1.5s infinite;
    
    &:after {
      content: '';
      position: absolute;
      top: 50%;
      left: 50%;
      width: 20px;
      height: 20px;
      background: rgba(255, 255, 255, 0.6);
      border-radius: 50%;
      transform: translate(-50%, -50%);
      animation: sparkle 1s ease-in-out infinite;
    }
  }
}

@keyframes pulse {
  0% { transform: scale(1); opacity: 0.8; }
  50% { transform: scale(1.05); opacity: 1; }
  100% { transform: scale(1); opacity: 0.8; }
}

.input-actions {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-top: 10px;
}

.tts-settings {
  padding: 15px;
  border-top: 1px solid rgba(255, 255, 255, 0.1);
  flex-shrink: 0;

  h5 {
    margin: 0 0 10px 0;
    color: #fff;
  }
}

.tts-controls {
  display: flex;
  flex-direction: column;
  gap: 10px;
}

.volume-control {
  display: flex;
  align-items: center;
  gap: 10px;

  span {
    color: #fff;
    font-size: 14px;
  }

  .el-slider {
    flex: 1;
  }
}

.collapsed-area {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 20px 10px;
  cursor: pointer;
  transition: all 0.3s ease;
  height: 100%;
  min-height: 200px;
  border-radius: 8px;
  margin: 10px;

  &:hover {
    background-color: rgba(255, 255, 255, 0.1);
    transform: scale(1.05);
    box-shadow: 0 5px 15px rgba(0, 0, 0, 0.2);
  }
}

.collapsed-icon {
  font-size: 28px;
  margin-bottom: 10px;
  color: #fff;
  opacity: 0.8;
  transition: all 0.3s ease;
  
  &:hover {
    transform: scale(1.2) rotate(5deg);
    opacity: 1;
    text-shadow: 0 0 10px rgba(255, 255, 255, 0.5);
  }
}

.collapsed-text {
  color: #fff;
  font-size: 14px;
  font-weight: bold;
  opacity: 0.8;
  writing-mode: vertical-rl;
  text-orientation: mixed;
  transform: rotate(180deg);
}

// 响应式设计
@media (max-width: 768px) {
  .ai-sidebar {
    width: 100%;
    bottom: 0;
  }

  .input-controls {
    flex-direction: column;
  }

  .input-actions {
    flex-direction: column;
    gap: 10px;
  }
}



.magic-button {
  position: relative;
  overflow: hidden;
  background: linear-gradient(45deg, #667eea, #764ba2);
  border: none;
  color: white;
  font-weight: bold;
  transition: all 0.3s ease;
  
  &:hover {
    transform: translateY(-3px) scale(1.05);
    box-shadow: 0 10px 25px rgba(102, 126, 234, 0.4);
    
    &:before {
      transform: translateX(100%);
    }
  }
  
  &:before {
    content: '';
    position: absolute;
    top: 0;
    left: -100%;
    width: 100%;
    height: 100%;
    background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.3), transparent);
    transition: transform 0.6s ease;
  }
}

.magic-icon {
  position: relative;
  
  &:hover {
    animation: iconBounce 0.6s ease-in-out;
    
    &:after {
      opacity: 1;
      transform: scale(1.5);
    }
  }
  
  &:after {
    content: '';
    position: absolute;
    top: 50%;
    left: 50%;
    width: 40px;
    height: 40px;
    background: radial-gradient(circle, rgba(255, 255, 255, 0.3), transparent);
    border-radius: 50%;
    transform: translate(-50%, -50%) scale(0);
    opacity: 0;
    transition: all 0.3s ease;
    pointer-events: none;
  }
}

@keyframes iconBounce {
  0%, 20%, 50%, 80%, 100% { transform: translateY(0) scale(1.2) rotate(5deg); }
  40% { transform: translateY(-10px) scale(1.3) rotate(10deg); }
  60% { transform: translateY(-5px) scale(1.25) rotate(7deg); }
}

.message-bubble {
  position: relative;
  transition: all 0.3s ease;
  
  &:hover {
    transform: translateY(-2px) scale(1.02);
    
    .message-content {
      box-shadow: 0 8px 25px rgba(0, 0, 0, 0.15);
    }
  }
  
  &:before {
    content: '';
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: linear-gradient(45deg, transparent, rgba(255, 255, 255, 0.05), transparent);
    opacity: 0;
    transition: opacity 0.3s ease;
    border-radius: inherit;
  }
  
  &:hover:before {
    opacity: 1;
  }
}

.magic-badge {
  background: linear-gradient(45deg, #67c23a, #85ce61);
  box-shadow: 0 4px 15px rgba(103, 194, 58, 0.4);
  animation: badgeFloat 2s ease-in-out infinite;
  
  &:hover {
    animation: badgeSpin 0.8s ease-in-out;
  }
}

@keyframes badgeFloat {
  0%, 100% { transform: translateY(0) rotate(0deg); }
  50% { transform: translateY(-3px) rotate(2deg); }
}

@keyframes badgeSpin {
  0% { transform: rotate(0deg) scale(1); }
  50% { transform: rotate(180deg) scale(1.2); }
  100% { transform: rotate(360deg) scale(1); }
}

.magic-typing {
  color: #409eff;
  text-shadow: 0 0 10px rgba(64, 158, 255, 0.6);
  animation: typingGlow 1.5s ease-in-out infinite;
}

@keyframes typingGlow {
  0%, 100% { 
    opacity: 1; 
    text-shadow: 0 0 10px rgba(64, 158, 255, 0.6);
  }
  50% { 
    opacity: 0.7; 
    text-shadow: 0 0 20px rgba(64, 158, 255, 0.8);
  }
}

.magic-voice-btn {
  background: linear-gradient(45deg, #409eff, #67c23a);
  border: none;
  color: white;
  font-weight: bold;
  
  &:hover {
    transform: translateY(-2px) scale(1.05);
    box-shadow: 0 8px 20px rgba(64, 158, 255, 0.4);
  }
  
  &.recording {
    background: linear-gradient(45deg, #f56c6c, #e6a23c) !important;
    animation: recordingPulse 1.5s ease-in-out infinite;
    
    &:after {
      animation: recordingRipple 2s ease-in-out infinite;
    }
  }
}

@keyframes recordingPulse {
  0%, 100% { 
    transform: scale(1); 
    box-shadow: 0 0 20px rgba(245, 108, 108, 0.6);
  }
  50% { 
    transform: scale(1.05); 
    box-shadow: 0 0 30px rgba(245, 108, 108, 0.8);
  }
}

@keyframes recordingRipple {
  0% { 
    transform: translate(-50%, -50%) scale(0.5);
    opacity: 1;
  }
  100% { 
    transform: translate(-50%, -50%) scale(2);
    opacity: 0;
  }
}

.magic-analyze-btn {
  background: linear-gradient(45deg, #67c23a, #85ce61);
  border: none;
  color: white;
  font-weight: bold;
  
  &:hover {
    transform: translateY(-2px) scale(1.05);
    box-shadow: 0 8px 20px rgba(103, 194, 58, 0.4);
    background: linear-gradient(45deg, #85ce61, #95d475);
  }
}

.magic-input {
  transition: all 0.3s ease;
  
  &:focus-within {
    transform: scale(1.02);
    box-shadow: 0 0 20px rgba(64, 158, 255, 0.3);
    
    .el-textarea__inner {
      border-color: #409eff;
      box-shadow: 0 0 15px rgba(64, 158, 255, 0.2);
    }
  }
}

.magic-send-btn {
  background: linear-gradient(45deg, #409eff, #67c23a);
  border: none;
  color: white;
  font-weight: bold;
  position: relative;
  overflow: hidden;
  
  &:hover {
    transform: translateY(-2px) scale(1.05);
    box-shadow: 0 8px 20px rgba(64, 158, 255, 0.4);
    
    &:before {
      transform: translateX(100%);
    }
  }
  
  &:before {
    content: '';
    position: absolute;
    top: 0;
    left: -100%;
    width: 100%;
    height: 100%;
    background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.3), transparent);
    transition: transform 0.6s ease;
  }
}

.magic-switch {
  .el-switch__core {
    background: linear-gradient(45deg, #409eff, #67c23a);
    transition: all 0.3s ease;
    
    &:hover {
      transform: scale(1.05);
      box-shadow: 0 0 15px rgba(64, 158, 255, 0.4);
    }
  }
}

.magic-alert {
  border: none;
  background: linear-gradient(45deg, #f56c6c, #e6a23c);
  color: white;
  box-shadow: 0 8px 25px rgba(245, 108, 108, 0.3);
  
  &:hover {
    transform: translateY(-2px);
    box-shadow: 0 12px 30px rgba(245, 108, 108, 0.4);
  }
}



.collapsed-area {
  &:hover {
    .magic-icon {
      animation: iconFloat 1s ease-in-out infinite;
    }
  }
}

@keyframes iconFloat {
  0%, 100% { transform: translateY(0) scale(1.05); }
  50% { transform: translateY(-5px) scale(1.1); }
}

// 消息进入动画增强
.user-message-enter {
  animation: userMessageFlyIn 0.6s ease-out forwards;
  opacity: 0;
  transform: translateY(50px);
}

@keyframes userMessageFlyIn {
  0% {
    opacity: 0;
    transform: translateY(50px) scale(0.8);
  }
  50% {
    opacity: 0.8;
    transform: translateY(-5px) scale(1.05);
  }
  100% {
    opacity: 1;
    transform: translateY(0) scale(1);
  }
}

// 输入框聚焦动画
.input-focus {
  .el-textarea__inner {
    transition: all 0.3s ease;
    border: 2px solid transparent;
    background: rgba(255, 255, 255, 0.05);
    
    &:focus {
      border-color: #409eff;
      background: rgba(255, 255, 255, 0.1);
      box-shadow: 0 0 20px rgba(64, 158, 255, 0.2);
    }
  }
}
</style>
