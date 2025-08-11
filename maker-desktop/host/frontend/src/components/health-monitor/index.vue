<template>
  <div class="health-monitor">
    <div class="module-header">
      <h3 class="title-animate">健康监测</h3>
      <div class="header-actions">
        <div class="button-container">
          <el-button
            size="small"
            @click="clearHealthData"
            :loading="healthStore.loading"
            class="clear-history-btn pulse-btn"
            type="warning"
            :disabled="healthStore.loading"
          >
            <span class="btn-text" v-if="!healthStore.loading">清空</span>
            <span class="btn-text" v-else>...</span>
          </el-button>
        </div>
      </div>
    </div>

    <!-- 健康数据图表 -->
    <div class="health-charts">
      <div class="chart-container">
        <h4>心率 (次/分)</h4>
        <div class="chart" ref="heartRateChart"></div>
      </div>

      <div class="chart-container">
        <h4>呼吸 (次/分)</h4>
        <div class="chart" ref="breathChart"></div>
      </div>

      <div class="chart-container">
        <h4>体温 (°C)</h4>
        <div class="chart" ref="temperatureChart"></div>
      </div>

      <div class="chart-container">
        <h4>距离 (cm)</h4>
        <div class="chart" ref="distanceChart"></div>
      </div>
    </div>

    <!-- 错误提示 -->
    <el-alert
      v-if="healthStore.error"
      :title="healthStore.error"
      type="error"
      show-icon
      closable
      @close="healthStore.error = null"
      class="error-alert slide-in"
    />

    <!-- 清空健康数据确认对话框 -->
    <el-dialog
      v-model="showClearDataDialog"
      title="确认清空健康数据"
      width="450px"
      :close-on-click-modal="false"
      class="clear-data-dialog animate-dialog"
    >
      <div class="clear-data-content">
        <div class="clear-icon-wrapper">
          <el-icon class="clear-icon"><Delete /></el-icon>
        </div>
        <div class="clear-message">
          <h4>确定要清空当前用户的所有健康数据吗？</h4>
          <p class="clear-warning">
            <el-icon><Warning /></el-icon>
            此操作不可恢复，清空后所有的心率、呼吸、体温、距离等健康数据将永久丢失。
          </p>
          <div class="data-summary" v-if="currentUserId !== -1">
            <p class="summary-title">即将清空的数据类型：</p>
            <div class="data-types">
              <span class="data-type-item">
                <el-icon><TrendCharts /></el-icon>
                心率数据
              </span>
              <span class="data-type-item">
                <el-icon><WindPower /></el-icon>
                呼吸数据
              </span>
              <span class="data-type-item">
                <el-icon><Monitor /></el-icon>
                体温数据
              </span>
              <span class="data-type-item">
                <el-icon><Location /></el-icon>
                距离数据
              </span>
            </div>
          </div>
        </div>
      </div>
      <template #footer>
        <div class="clear-actions">
          <el-button @click="showClearDataDialog = false" class="btn-animate hover-lift">
            取消
          </el-button>
          <el-button 
            type="warning" 
            @click="confirmClearData" 
            :loading="healthStore.loading"
            class="btn-animate hover-lift clear-btn"
          >
            <el-icon><Delete /></el-icon>
            确认清空
          </el-button>
        </div>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted, computed, watch, nextTick } from 'vue'
import { useHealthStore, useUserStore } from '@/stores'
import { ElMessage, ElMessageBox } from 'element-plus'
import { Delete, Warning, TrendCharts, WindPower, Monitor, Location } from '@element-plus/icons-vue'
import * as echarts from 'echarts'

const healthStore = useHealthStore()
const userStore = useUserStore()

// 图表引用
const heartRateChart = ref(null)
const breathChart = ref(null)
const temperatureChart = ref(null)
const distanceChart = ref(null)

// 图表实例
let heartRateChartInstance = null
let breathChartInstance = null
let temperatureChartInstance = null
let distanceChartInstance = null

// 计算当前用户ID
const currentUserId = computed(() => userStore.currentUserId)

// 清空数据确认对话框状态
const showClearDataDialog = ref(false)

// 清空健康数据
const clearHealthData = async () => {
  if (currentUserId.value === -1) {
    ElMessage.warning('请先选择用户')
    return
  }
  
  showClearDataDialog.value = true
}

// 确认清空健康数据
const confirmClearData = async () => {
  try {
    const success = await healthStore.clearHealthData(currentUserId.value)
    if (success) {
      ElMessage.success('健康数据已清空')
      showClearDataDialog.value = false
      // 重新获取数据
      await fetchHealthData()
    } else {
      ElMessage.error('清空健康数据失败')
    }
  } catch (error) {
    console.error('清空健康数据失败:', error)
    ElMessage.error('清空健康数据失败，请检查网络连接')
  }
}

// 添加测试数据
// const addTestData = () => {
//   const testData = {
//     heart: [60, 65, 70, 68, 72, 75, 73, 71, 69, 67],
//     breath: [12, 14, 13, 15, 16, 14, 13, 12, 15, 14],
//     temperature: [36.5, 36.8, 37.0, 36.9, 37.2, 37.1, 36.8, 36.7, 37.0, 36.9],
//     distance: [50, 45, 55, 60, 40, 65, 70, 55, 50, 45]
//   }

//   console.log('添加测试数据:', testData)

//   // 更新图表
//   updateChart(heartRateChartInstance, testData.heart, '心率')
//   updateChart(breathChartInstance, testData.breath, '呼吸')
//   updateChart(temperatureChartInstance, testData.temperature, '体温')
//   updateChart(distanceChartInstance, testData.distance, '距离')
// }

// 强制重新渲染图表
const forceResizeCharts = () => {
  setTimeout(() => {
    if (heartRateChartInstance) {
      heartRateChartInstance.resize()
    }
    if (breathChartInstance) {
      breathChartInstance.resize()
    }
    if (temperatureChartInstance) {
      temperatureChartInstance.resize()
    }
    if (distanceChartInstance) {
      distanceChartInstance.resize()
    }

    // 添加测试数据
    // addTestData()
  }, 200)
}

// 初始化图表
const initCharts = () => {
  // console.log('开始初始化图表...')

  // 确保容器有正确的尺寸
  const containers = [heartRateChart.value, breathChart.value, temperatureChart.value, distanceChart.value]
  containers.forEach((container, index) => {
    if (container) {
      // console.log(`容器${index}尺寸:`, container.offsetWidth, container.offsetHeight)
      // 强制设置容器尺寸
      container.style.height = '200px'
      container.style.width = '100%'
    }
  })

  // 添加图表点击事件处理
  const addChartEvents = (chartInstance, chartType) => {
    if (chartInstance) {
      chartInstance.on('click', function(params) {
        if (params.componentType === 'series') {
          console.log(`${chartType}图表点击:`, params)
          // 可以在这里添加点击后的操作，比如显示详细信息弹窗
        }
      })
    }
  }

  // 心率图表
  if (heartRateChart.value) {
    // console.log('初始化心率图表')
    if (heartRateChartInstance) {
      heartRateChartInstance.dispose()
    }
    heartRateChartInstance = echarts.init(heartRateChart.value)
    const heartRateOption = getChartOption('', '#ff1199')
    heartRateChartInstance.setOption(heartRateOption)
    addChartEvents(heartRateChartInstance, '心率')
  } else {
    console.error('心率图表容器未找到')
  }

  // 呼吸图表
  if (breathChart.value) {
    // console.log('初始化呼吸图表')
    if (breathChartInstance) {
      breathChartInstance.dispose()
    }
    breathChartInstance = echarts.init(breathChart.value)
    const breathOption = getChartOption('', '#11ff99')
    breathChartInstance.setOption(breathOption)
    addChartEvents(breathChartInstance, '呼吸')
  } else {
    console.error('呼吸图表容器未找到')
  }

  // 体温图表
  if (temperatureChart.value) {
    // console.log('初始化体温图表')
    if (temperatureChartInstance) {
      temperatureChartInstance.dispose()
    }
    temperatureChartInstance = echarts.init(temperatureChart.value)
    const temperatureOption = getChartOption('', '#9911ff')
    temperatureChartInstance.setOption(temperatureOption)
    addChartEvents(temperatureChartInstance, '体温')
  } else {
    console.error('体温图表容器未找到')
  }

  // 距离图表
  if (distanceChart.value) {
    // console.log('初始化距离图表')
    if (distanceChartInstance) {
      distanceChartInstance.dispose()
    }
    distanceChartInstance = echarts.init(distanceChart.value)
    const distanceOption = getChartOption('', '#222222')
    distanceChartInstance.setOption(distanceOption)
    addChartEvents(distanceChartInstance, '距离')
  } else {
    console.error('距离图表容器未找到')
  }

  // 强制重新渲染
  forceResizeCharts()
}

// 获取图表配置
const getChartOption = (title, color) => {
  return {
    title: {
      text: title,
      textStyle: {
        color: '#fff',
        fontSize: 14
      },
      left: 'center'
    },
    grid: {
      top: 40,
      left: 40,
      right: 20,
      bottom: 20,
      containLabel: true
    },
    tooltip: {
      trigger: 'axis',
      backgroundColor: 'rgba(128, 128, 128, 0.15)',
      borderColor: 'rgba(255, 255, 255, 0.3)',
      borderWidth: 1,
      textStyle: {
        color: '#fff',
        fontSize: 12
      },
      axisPointer: {
        type: 'cross',
        lineStyle: {
          color: color,
          width: 1,
          type: 'dashed'
        },
        crossStyle: {
          color: color,
          width: 1,
          type: 'dashed'
        },
        // 添加悬停时的指示器动画
        animation: true,
        animationDuration: 0.3,
        animationEasing: 'cubicOut'
      },
      formatter: function(params) {
        const data = params[0]
        const value = data.value
        const index = data.dataIndex
        
        // 根据图表类型返回不同的提示内容
        let unit = ''
        if (title.includes('心率')) {
          unit = '次/分'
        } else if (title.includes('呼吸')) {
          unit = '次/分'
        } else if (title.includes('体温')) {
          unit = '°C'
        } else if (title.includes('距离')) {
          unit = 'cm'
        }
        
        return `
          <div style="padding: 8px;">
            <div style="font-weight: bold; margin-bottom: 8px; color: ${color};">${title}</div>
            <div style="margin-bottom: 4px;">
              <span style="color: #e0e0e0;">数据点:</span> 
              <span style="color: #fff; font-weight: bold;">${index + 1}</span>
            </div>
            <div style="margin-bottom: 4px;">
              <span style="color: #e0e0e0;">数值:</span> 
              <span style="color: ${color}; font-weight: bold; font-size: 16px;">${value} ${unit}</span>
            </div>
          </div>
        `
      },
      // 添加浮现动画效果
      showDelay: 0,
      hideDelay: 100,
      transitionDuration: 0.3,
      // 响应式配置
      confine: true,
      appendToBody: false,
      extraCssText: `
        box-shadow: 0 8px 32px rgba(128, 128, 128, 0.3);
        border-radius: 12px;
        backdrop-filter: blur(20px);
        -webkit-backdrop-filter: blur(20px);
        animation: tooltipFadeIn 0.3s ease-out;
        border: 1px solid rgba(255, 255, 255, 0.2);
        max-width: 280px;
        word-wrap: break-word;
        z-index: 9999;
        pointer-events: auto;
        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        transition: all 0.3s ease;
        transform-origin: center bottom;
        user-select: none;
        cursor: default;
        overflow: hidden;
        background: rgba(128, 128, 128, 0.15) !important;
        // position: relative;
      `
    },
    xAxis: {
      type: 'category',
      show: false,
      data: [],
      inverse: true // 反转x轴方向，让新数据从右边加载，旧数据从左边移出
    },
    yAxis: {
      type: 'value',
      axisLine: {
        lineStyle: {
          color: 'rgba(255, 255, 255, 0.3)'
        }
      },
      axisLabel: {
        color: 'rgba(255, 255, 255, 0.8)'
      },
      splitLine: {
        lineStyle: {
          color: 'rgba(255, 255, 255, 0.1)'
        }
      }
    },
    series: [{
      data: [],
      type: 'line',
      smooth: true,
      symbol: 'none',
      symbolSize: 0,
      // 鼠标悬停时显示数据点
      emphasis: {
        focus: 'series',
        scale: true,
        symbolSize: 8,
        itemStyle: {
          color: color,
          borderColor: '#fff',
          borderWidth: 2,
          shadowColor: color,
          shadowBlur: 10,
          // 添加悬停时的数据点动画
          animation: true,
          animationDuration: 0.3,
          animationEasing: 'elasticOut'
        },
        lineStyle: {
          color: color,
          width: 3,
          shadowColor: color,
          shadowBlur: 15
        },
        areaStyle: {
          color: {
            type: 'linear',
            x: 0,
            y: 0,
            x2: 0,
            y2: 1,
            colorStops: [{
              offset: 0,
              color: color + '60'
            }, {
              offset: 1,
              color: color + '20'
            }]
          }
        }
      },
      lineStyle: {
        color: color,
        width: 2
      },
      areaStyle: {
        color: {
          type: 'linear',
          x: 0,
          y: 0,
          x2: 0,
          y2: 1,
          colorStops: [{
            offset: 0,
            color: color + '40'
          }, {
            offset: 1,
            color: color + '10'
          }]
        }
      },
      // 添加初始动画
      animation: true,
      animationDuration: 2000,
      animationEasing: 'elasticOut',
      // 添加数据点动画
      animationDelay: (idx) => idx * 100
    }],
    backgroundColor: 'transparent'
  }
}

// 更新图表数据
const updateChart = (chartInstance, data, title) => {
  if (!chartInstance) {
    console.error(`${title}图表实例未找到`)
    return
  }

  // console.log(`更新${title}图表数据:`, data)

  // 处理空数据情况
  if (!data || data.length === 0) {
    // console.log(`${title}图表数据为空，显示空图表`)
    const emptyOption = {
      xAxis: {
        data: []
      },
      series: [{
        data: []
      }]
    }
    chartInstance.setOption(emptyOption)
    return
  }

  const xData = Array.from({ length: data.length }, (_, i) => i + 1)
  
  // 添加数据更新动画效果
  const option = {
    xAxis: {
      data: xData,
      // 反转x轴方向，让新数据从右边加载，旧数据从左边移出
      inverse: true
    },
    series: [{
      data: data,
      // 添加数据更新动画
      animation: true,
      animationDuration: 1000,
      animationEasing: 'cubicOut',
      // 添加数据点动画
      animationDelay: (idx) => idx * 50
    }]
  }

  // console.log(`${title}图表更新选项:`, option)
  chartInstance.setOption(option)

  // 强制重新渲染
  forceResizeCharts()
}

// 获取健康数据
const fetchHealthData = async () => {
  if (currentUserId.value === -1) {
    // console.log('当前用户ID为-1，跳过获取健康数据')
    return
  }

  // console.log('开始获取健康数据，用户ID:', currentUserId.value)

  try {
    const success = await healthStore.fetchHealthData(currentUserId.value, 30)
    if (success) {
      // console.log('健康数据获取成功:', healthStore.healthData)
      // 更新图表
      updateChart(heartRateChartInstance, healthStore.healthData.heart, '心率')
      updateChart(breathChartInstance, healthStore.healthData.breath, '呼吸')
      updateChart(temperatureChartInstance, healthStore.healthData.temperature, '体温')
      updateChart(distanceChartInstance, healthStore.healthData.distance, '距离')
    } else {
      console.error('健康数据获取失败')
    }
  } catch (error) {
    console.error('获取健康数据失败:', error)
  }
}

// 监听用户变化
watch(currentUserId, (newUserId) => {
  // console.log('用户ID发生变化:', newUserId)
  if (newUserId !== -1) {
    fetchHealthData()
  } else {
    // 清空图表数据
    updateChart(heartRateChartInstance, [], '心率')
    updateChart(breathChartInstance, [], '呼吸')
    updateChart(temperatureChartInstance, [], '体温')
    updateChart(distanceChartInstance, [], '距离')
  }
})

// 监听健康数据变化
watch(() => healthStore.healthData, (newData) => {
  // console.log('健康数据发生变化:', newData)
  if (newData) {
    updateChart(heartRateChartInstance, newData.heart, '心率')
    updateChart(breathChartInstance, newData.breath, '呼吸')
    updateChart(temperatureChartInstance, newData.temperature, '体温')
    updateChart(distanceChartInstance, newData.distance, '距离')
  }
}, { deep: true })

// 监听窗口大小变化
const handleResize = () => {
  if (heartRateChartInstance) {
    heartRateChartInstance.resize()
  }
  if (breathChartInstance) {
    breathChartInstance.resize()
  }
  if (temperatureChartInstance) {
    temperatureChartInstance.resize()
  }
  if (distanceChartInstance) {
    distanceChartInstance.resize()
  }
}

onMounted(async () => {
  // console.log('健康监测组件已挂载')

  // 等待DOM渲染完成
  await nextTick()

  // 延迟初始化图表，确保容器有正确的尺寸
  setTimeout(() => {
    initCharts()
  }, 300)

  // 获取健康数据
  if (currentUserId.value !== -1) {
    await fetchHealthData()
  }

  // 设置定时刷新
  setInterval(() => {
    if (currentUserId.value !== -1) {
      fetchHealthData()
    }
  }, 2000) // 每2秒刷新

  // 监听窗口大小变化
  window.addEventListener('resize', handleResize)
})

onUnmounted(() => {
  // 移除窗口大小变化监听
  window.removeEventListener('resize', handleResize)

  // 销毁图表实例
  if (heartRateChartInstance) {
    heartRateChartInstance.dispose()
  }
  if (breathChartInstance) {
    breathChartInstance.dispose()
  }
  if (temperatureChartInstance) {
    temperatureChartInstance.dispose()
  }
  if (distanceChartInstance) {
    distanceChartInstance.dispose()
  }
})
</script>

<style lang="scss" scoped>
.health-monitor {
  flex: 2;
  background: rgba(255, 255, 255, 0.1);
  border-radius: 12px;
  padding: 20px;
  backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.2);
  overflow-y: auto;
  animation: fadeInUp 0.6s ease-out;
}

// 主容器淡入动画
@keyframes fadeInUp {
  from {
    opacity: 0;
    transform: translateY(30px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

.module-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
  flex-shrink: 0;
  position: relative;

  h3 {
    margin: 0;
    color: #fff;
    font-size: 20px;
    font-weight: 600;
  }
}

.header-actions {
  flex-shrink: 0;
  
  // 按钮容器样式
  .button-container {
    width: 80px !important;
    height: 32px !important;
    flex-shrink: 0 !important;
    display: flex !important;
    align-items: center !important;
    justify-content: center !important;
    overflow: hidden !important;
  }
  
  // 专门为清空历史按钮添加样式 - 使用!important确保优先级
  .clear-history-btn {
    width: 80px !important; // 强制固定宽度
    height: 32px !important; // 强制固定高度
    min-width: 80px !important; // 强制最小宽度
    min-height: 32px !important; // 强制最小高度
    max-width: 80px !important; // 强制最大宽度
    max-height: 32px !important; // 强制最大高度
    box-sizing: border-box !important; // 确保padding不影响总尺寸
    padding: 6px 12px !important; // 强制固定内边距
    font-size: 14px !important; // 强制固定字体大小
    line-height: 1 !important; // 强制固定行高
    white-space: nowrap !important; // 防止文字换行
    overflow: hidden !important; // 防止内容溢出
    text-overflow: ellipsis !important; // 文字溢出时显示省略号
    transition: all 0.3s ease !important; // 启用过渡效果
    position: relative !important;
    
    // 确保按钮内容居中
    display: flex !important;
    align-items: center !important;
    justify-content: center !important;
    
    // 悬停效果
    &:hover:not(.is-disabled) {
      transform: translateY(-2px) !important;
      box-shadow: 0 8px 25px rgba(255, 119, 0, 0.3) !important;
    }
    
    // 点击效果
    &:active:not(.is-disabled) {
      transform: translateY(0) !important;
      transition: all 0.1s ease !important;
    }
    
    // 覆盖Element Plus的默认样式
    &.el-button--small {
      width: 80px !important;
      height: 32px !important;
      min-width: 80px !important;
      min-height: 32px !important;
    }
    
    // 确保loading状态下的尺寸一致
    &.is-loading {
      width: 80px !important;
      height: 32px !important;
      min-width: 80px !important;
      min-height: 32px !important;
      
      // 隐藏Element Plus的默认loading图标
      .el-loading-spinner {
        display: none !important;
      }
      
      // 隐藏默认的loading文字
      .el-loading-text {
        display: none !important;
      }
    }
    
    // 按钮文字样式
    .btn-text {
      display: inline-block !important;
      width: 100% !important;
      text-align: center !important;
      line-height: 1 !important;
      font-size: 14px !important;
      color: inherit !important;
      margin: 0 !important;
      padding: 0 !important;
      white-space: nowrap !important;
      overflow: hidden !important;
      text-overflow: ellipsis !important;
    }
    
    // 禁用状态样式
    &.is-disabled {
      width: 80px !important;
      height: 32px !important;
      min-width: 80px !important;
      min-height: 32px !important;
      opacity: 0.6 !important;
    }
  }
}

// 脉冲按钮效果
.pulse-btn {
  &::before {
    content: '';
    position: absolute;
    top: 50%;
    left: 50%;
    width: 100%;
    height: 100%;
    background: rgba(255, 119, 0, 0.3);
    border-radius: inherit;
    transform: translate(-50%, -50%) scale(0);
    animation: pulse 2s infinite;
  }
}

@keyframes pulse {
  0% {
    transform: translate(-50%, -50%) scale(0);
    opacity: 1;
  }
  100% {
    transform: translate(-50%, -50%) scale(1.5);
    opacity: 0;
  }
}

.health-charts {
  // display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 20px;
  margin-top: 20px;
}

// 图表容器淡入动画
.chart-fade-in {
  animation: chartFadeIn 0.8s ease-out forwards;
  opacity: 0;
  transform: translateY(20px);
}

@keyframes chartFadeIn {
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

.chart-container {
  background: rgba(255, 255, 255, 0.05);
  border-radius: 8px;
  padding: 15px;
  border: 1px solid rgba(255, 255, 255, 0.1);
  transition: all 0.3s ease;
  position: relative;
  overflow: hidden;
  
  // 悬停效果
  &:hover {
    transform: translateY(-5px);
    box-shadow: 0 10px 30px rgba(0, 0, 0, 0.3);
    border-color: rgba(255, 255, 255, 0.3);
    background: rgba(255, 255, 255, 0.08);
    
    &::before {
      transform: translateX(100%);
    }
  }
  
  // 光效装饰
  &::before {
    content: '';
    position: absolute;
    top: 0;
    left: -100%;
    width: 100%;
    height: 100%;
    background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.1), transparent);
    transition: transform 0.6s ease;
  }

  h4 {
    margin-bottom: 10px;
    color: #fff;
    text-align: center;
  }
}

// 图表标题动画
.chart-title-animate {
  position: relative;
  transition: all 0.3s ease;
  
  &:hover {
    color: #11ff99;
    transform: scale(1.05);
  }
}

.chart {
  height: 250px;
  width: 100%;
  min-height: 200px;
  background: rgba(255, 255, 255, 0.02);
  border-radius: 4px;
  transition: all 0.3s ease;
  position: relative;
  
  &:hover {
    background: rgba(255, 255, 255, 0.05);
    box-shadow: inset 0 0 20px rgba(255, 255, 255, 0.1);
  }
}

// 图表加载状态
.chart-loading {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 10px;
  color: rgba(255, 255, 255, 0.8);
  font-size: 14px;
  
  .loading-spinner {
    width: 30px;
    height: 30px;
    border: 2px solid rgba(255, 255, 255, 0.2);
    border-top: 2px solid #11ff99;
    border-radius: 50%;
    animation: spin 1s linear infinite;
  }
  
  span {
    animation: fadeInOut 1.5s ease-in-out infinite;
  }
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

@keyframes fadeInOut {
  0%, 100% { opacity: 0.5; }
  50% { opacity: 1; }
}

// 错误提示动画
.error-alert {
  animation: slideIn 0.5s ease-out;
}

@keyframes slideIn {
  from {
    opacity: 0;
    transform: translateX(-100%);
  }
  to {
    opacity: 1;
    transform: translateX(0);
  }
}

// 清空数据确认对话框样式
.clear-data-dialog {
  .el-dialog__header {
    background: linear-gradient(135deg, #e6a23c, #f0c78a);
    color: white;
    border-radius: 8px 8px 0 0;
    
    .el-dialog__title {
      color: white;
      font-weight: 600;
    }
    
    .el-dialog__headerbtn .el-dialog__close {
      color: white;
      font-size: 18px;
      
      &:hover {
        color: #fdf6ec;
      }
    }
  }
  
  .el-dialog__body {
    padding: 0;
  }
  
  .clear-data-content {
    display: flex;
    align-items: flex-start;
    gap: 20px;
    padding: 30px;
    background: linear-gradient(135deg, #fefefe, #fdf6ec);
    border-radius: 0 0 8px 8px;
    border: 1px solid #ebeef5;
    position: relative;
    
    &::before {
      content: '';
      position: absolute;
      top: 0;
      left: 0;
      right: 0;
      height: 3px;
      background: linear-gradient(90deg, #e6a23c, #f0c78a);
    }
  }

  .clear-icon-wrapper {
    background: linear-gradient(135deg, #fdf6ec, #fef9f0);
    border-radius: 50%;
    width: 70px;
    height: 70px;
    display: flex;
    align-items: center;
    justify-content: center;
    box-shadow: 0 4px 12px rgba(230, 162, 60, 0.2);
    border: 2px solid #fdf6ec;
    animation: clearIconPulse 2s ease-in-out infinite;
    flex-shrink: 0;
  }

  .clear-icon {
    font-size: 40px;
    color: #e6a23c;
    filter: drop-shadow(0 2px 4px rgba(230, 162, 60, 0.3));
  }

  .clear-message {
    flex: 1;
    
    h4 {
      color: #303133;
      font-size: 18px;
      margin-bottom: 12px;
      font-weight: 600;
      line-height: 1.4;
    }
    
    .clear-warning {
      display: flex;
      align-items: flex-start;
      color: #e6a23c;
      font-size: 14px;
      margin-bottom: 20px;
      padding: 12px;
      background: rgba(230, 162, 60, 0.1);
      border-radius: 6px;
      border-left: 3px solid #e6a23c;
      
      .el-icon {
        margin-right: 8px;
        font-size: 16px;
        color: #e6a23c;
        margin-top: 2px;
        flex-shrink: 0;
      }
    }
    
    .data-summary {
      background: rgba(255, 255, 255, 0.8);
      border-radius: 8px;
      padding: 16px;
      border: 1px solid #ebeef5;
      
      .summary-title {
        color: #606266;
        font-size: 14px;
        font-weight: 600;
        margin-bottom: 12px;
        margin-top: 0;
      }
      
      .data-types {
        display: grid;
        grid-template-columns: repeat(2, 1fr);
        gap: 12px;
        
        .data-type-item {
          display: flex;
          align-items: center;
          gap: 8px;
          color: #606266;
          font-size: 13px;
          padding: 8px 12px;
          background: rgba(230, 162, 60, 0.05);
          border-radius: 6px;
          border: 1px solid rgba(230, 162, 60, 0.1);
          
          .el-icon {
            color: #e6a23c;
            font-size: 14px;
          }
        }
      }
    }
  }

  .clear-actions {
    display: flex;
    justify-content: flex-end;
    gap: 12px;
    padding: 20px 30px;
    background: #fafafa;
    border-top: 1px solid #ebeef5;
    border-radius: 0 0 8px 8px;
  }

  .clear-btn {
    background: linear-gradient(135deg, #e6a23c, #f0c78a);
    border: none;
    color: white;
    font-weight: 600;
    padding: 10px 20px;
    border-radius: 6px;
    transition: all 0.3s ease;
    
    &:hover {
      background: linear-gradient(135deg, #f0c78a, #f4d4a3);
      transform: translateY(-2px);
      box-shadow: 0 6px 20px rgba(230, 162, 60, 0.3);
    }
    
    &:active {
      transform: translateY(0);
    }
    
    .el-icon {
      margin-right: 6px;
    }
  }
}

// 清空图标脉冲动画
@keyframes clearIconPulse {
  0%, 100% {
    transform: scale(1);
    box-shadow: 0 4px 12px rgba(230, 162, 60, 0.2);
  }
  50% {
    transform: scale(1.05);
    box-shadow: 0 6px 20px rgba(230, 162, 60, 0.4);
  }
}

// 响应式设计
@media (max-width: 768px) {
  .health-charts {
    grid-template-columns: 1fr;
  }

  .chart {
    height: 150px;
  }
  
  // 移动端优化动画
  .chart-container:hover {
    transform: none;
  }
}
</style>
