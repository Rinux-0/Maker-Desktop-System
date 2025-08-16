<template>
  <div class="dashboard animate fade-in-up">
    <!-- 用户管理模块 -->
    <UserManagement class="animate fade-in-left delay-200" />

    <!-- 健康监测模块 -->
    <HealthMonitor class="animate fade-in-right delay-400" />
  </div>
</template>

<script setup>
import UserManagement from '@/components/user-management/index.vue'
import HealthMonitor from '@/components/health-monitor/index.vue'
</script>

<style lang="scss" scoped>
.dashboard {
  width: 100%;
  height: 100%;
  padding: 20px;
  display: flex;
  gap: 20px;
  overflow: hidden;
}
</style>
