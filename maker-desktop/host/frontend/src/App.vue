<template>
  <div id="app" class="animate fade-in-up">
    <router-view />
  </div>
</template>

<script setup>
import { onMounted, onUnmounted, ref } from 'vue'
import { useUserStore } from '@/stores'

// 获取用户store
const userStore = useUserStore()

// 启动轮询
const startPolling = async () => {
  try {
    // 初始化用户数据
    await userStore.initUserData()
    
    // 启动轮询
    userStore.startPolling()
    console.log('轮询已启动')
  } catch (error) {
    console.error('启动轮询失败:', error)
  }
}

// 应用启动时启动轮询
onMounted(() => {
  startPolling()
})
</script>

<style lang="scss">
#app {
  width: 100vw;
  height: 100vh;
  overflow: hidden;
}
</style>
