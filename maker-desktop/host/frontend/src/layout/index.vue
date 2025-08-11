<template>
  <div class="layout animate fade-in-up">
    <!-- 主内容区域 -->
    <div class="main-content animate fade-in-left delay-200">
      <router-view />
    </div>

    <!-- AI侧边栏 -->
    <AISidebar class="animate fade-in-right delay-300" />
  </div>
</template>

<script setup>
import AISidebar from '@/components/ai-sidebar/index.vue'
</script>

<style lang="scss" scoped>
.layout {
  width: 100%;
  height: 100%;
  display: flex;
  position: relative;
}
</style>
