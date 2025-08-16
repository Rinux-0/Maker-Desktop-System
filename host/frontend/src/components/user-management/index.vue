<template>
  <div class="user-management">
    <div class="module-header animate fade-in-down">
      <h3 class="animate scale-in delay-200">用户管理</h3>
      <div class="header-actions animate fade-in-right delay-300">

        
        <el-button
          type="primary"
          size="small"
          @click="refreshUserList"
          :loading="userStore.loading"
          :disabled="userStore.loading"
          class="btn-animate hover-lift"
        >
          <el-icon><Refresh /></el-icon>
          刷新
        </el-button>
        <el-button type="primary" size="small" @click="showAddDialog = true" class="btn-animate hover-lift">
          <el-icon><Plus /></el-icon>
          添加用户
        </el-button>
      </div>
    </div>

    <!-- 当前用户信息 -->
    <div class="current-user animate fade-in-up delay-400" v-if="userStore.currentUser">
      <h4 class="animate scale-in delay-500">当前用户</h4>
      <div class="user-card current card-hover animate bounce-in delay-600">
        <div class="user-info">
          <div class="user-name animate fade-in-left delay-700">{{ userStore.currentUser.name }}</div>
          <div class="user-details animate fade-in-up delay-800">
            <span class="animate scale-in delay-900">ID: {{ userStore.currentUser.id }}</span>
            <span class="animate scale-in delay-1000">年龄: {{ userStore.currentUser.age }}岁</span>
            <span class="animate scale-in delay-1100">身高: {{ userStore.currentUser.height }}cm</span>
            <span class="animate scale-in delay-1200">体重: {{ userStore.currentUser.weight }}kg</span>
            <span class="animate scale-in delay-1300">血型: {{ userStore.currentUser.blood }}</span>
            <span class="animate scale-in delay-1400">电话: {{ userStore.currentUser.phone }}</span>
          </div>
          <div class="user-intro animate fade-in-up delay-1500" v-if="userStore.currentUser.intro">
            简介: {{ userStore.currentUser.intro }}
          </div>
        </div>
      </div>
    </div>

    <!-- 用户列表 -->
    <div class="user-list animate fade-in-up delay-600">
      <h4 class="animate scale-in delay-700">用户列表</h4>
      <div class="users-grid">
        <div
          v-for="(user, index) in userStore.users"
          :key="user.id"
          class="user-card list-item card-hover"
          :class="{ active: userStore.currentUser?.id === user.id }"
          :style="{ animationDelay: `${0.8 + index * 0.1}s` }"
        >
          <div class="user-header">
            <span class="user-name animate fade-in-left" :style="{ animationDelay: `${0.9 + index * 0.1}s` }">{{ user.name }}</span>
            <div class="user-actions animate fade-in-right" :style="{ animationDelay: `${1.0 + index * 0.1}s` }">
              <el-button
                size="small"
                type="primary"
                @click="switchToUser(user.id)"
                :disabled="userStore.loading"
                class="btn-animate hover-lift"
              >
                <el-icon v-if="userStore.currentUser?.id === user.id">
                  <Refresh />
                </el-icon>
                <span v-else>切换</span>
              </el-button>
              <el-button
                size="small"
                @click="editUser(user)"
                :disabled="userStore.loading"
                class="btn-animate hover-lift"
              >
                编辑
              </el-button>
              <el-button
                size="small"
                type="danger"
                @click="deleteUser(user.id)"
                :disabled="userStore.loading"
                class="btn-animate hover-lift"
              >
                X
              </el-button>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- 错误提示 -->
    <el-alert
      v-if="userStore.error"
      :title="userStore.error"
      type="error"
      show-icon
      closable
      @close="userStore.error = null"
      class="animate shake"
    />



    <!-- 添加用户对话框 -->
    <el-dialog
      v-model="showAddDialog"
      title="添加用户"
      width="250px"
      :close-on-click-modal="false"
      @close="handleCloseAddDialog"
      class="animate-dialog"
    >
      <UserForm
        ref="addFormRef"
        :user-data="addFormData"
        @update:user-data="(data) => Object.assign(addFormData, data)"
      />
      <template #footer>
        <el-button @click="showAddDialog = false" class="btn-animate hover-lift">取消</el-button>
        <el-button type="primary" @click="handleAddUser" :loading="userStore.loading" class="btn-animate hover-lift">
          确定
        </el-button>
      </template>
    </el-dialog>

    <!-- 编辑用户对话框 -->
    <el-dialog
      v-model="showEditDialog"
      title="编辑用户"
      width="250px"
      :close-on-click-modal="false"
      @close="handleCloseEditDialog"
      class="animate-dialog"
    >
      <UserForm
        ref="editFormRef"
        :user-data="editFormData"
        :is-edit="true"
        @update:user-data="(data) => Object.assign(editFormData, data)"
      />
      <template #footer>
        <el-button @click="showEditDialog = false" class="btn-animate hover-lift">取消</el-button>
        <el-button type="primary" @click="handleEditUser" :loading="userStore.loading" class="btn-animate hover-lift">
          确定
        </el-button>
      </template>
    </el-dialog>

    <!-- 删除用户确认对话框 -->
    <el-dialog
      v-model="showDeleteDialog"
      title="确认删除用户"
      width="400px"
      :close-on-click-modal="false"
      class="delete-confirm-dialog animate-dialog"
    >
      <div class="delete-confirm-content">
        <div class="delete-icon-wrapper">
          <el-icon class="delete-icon"><Delete /></el-icon>
        </div>
        <div class="delete-message">
          <h4>确定要删除用户 "{{ deleteUserData.name }}" 吗？</h4>
          <p class="delete-warning">
            <el-icon><Warning /></el-icon>
            此操作不可恢复，删除后用户的所有数据将永久丢失。
          </p>
        </div>
      </div>
      <template #footer>
        <div class="delete-actions">
          <el-button @click="showDeleteDialog = false" class="btn-animate hover-lift">
            取消
          </el-button>
          <el-button 
            type="danger" 
            @click="confirmDeleteUser" 
            :loading="userStore.loading"
            class="btn-animate hover-lift delete-btn"
          >
            <el-icon><Delete /></el-icon>
            确认删除
          </el-button>
        </div>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { useUserStore } from '@/stores'
import { ElMessage, ElMessageBox } from 'element-plus'
import { Plus, Refresh, Delete, Warning } from '@element-plus/icons-vue'
import UserForm from './UserForm.vue'

const userStore = useUserStore()

// 对话框状态
const showAddDialog = ref(false)
const showEditDialog = ref(false)
const showDeleteDialog = ref(false) // 新增删除确认对话框状态

// 表单引用
const addFormRef = ref(null)
const editFormRef = ref(null)

// 表单数据
const addFormData = reactive({
  name: '',
  age: 25,
  height: 170,
  weight: 60,
  blood: '',
  phone: '',
  intro: ''
})

const editFormData = reactive({
  id: null,
  name: '',
  age: 25,
  height: 170,
  weight: 60,
  blood: '',
  phone: '',
  intro: ''
})

// 删除用户数据
const deleteUserData = reactive({
  id: null,
  name: ''
})

// 初始化
onMounted(async () => {
  try {
    await userStore.initUserData()
  } catch (error) {
    console.error('用户数据初始化失败:', error)
    ElMessage.error('连接后端服务失败，请检查网络连接')
  }
})

// 组件卸载时不需要停止轮询，因为轮询是全局的
// onUnmounted(() => {
//   // 停止轮询
//   userStore.stopPolling()
// })

// 刷新用户列表
const refreshUserList = async () => {
  try {
    await userStore.fetchUsers()
    ElMessage.success('用户列表已刷新')
  } catch (error) {
    console.error('刷新用户列表失败:', error)
    ElMessage.error('刷新用户列表失败，请检查网络连接')
  }
}



// 切换用户
const switchToUser = async (userId) => {
  const success = await userStore.switchUser(userId)
  if (success) {
    ElMessage.success('用户切换成功')
  } else {
    ElMessage.error('用户切换失败')
  }
}

// 编辑用户
const editUser = (user) => {
  // console.log('编辑用户原始数据:', user)

  // 确保数据类型正确
  Object.assign(editFormData, {
    id: user.id,
    name: user.name || '',
    age: Number(user.age) || 25,
    height: Number(user.height) || 170,
    weight: Number(user.weight) || 60,
    blood: user.blood || '',
    phone: user.phone || '',
    intro: user.intro || ''
  })

  // console.log('编辑用户表单数据:', editFormData)
  showEditDialog.value = true
}

// 删除用户
const deleteUser = async (userId) => {
  const userToDelete = userStore.users.find(user => user.id === userId);
  if (userToDelete) {
    deleteUserData.id = userId;
    deleteUserData.name = userToDelete.name;
    showDeleteDialog.value = true;
  } else {
    ElMessage.error('未找到要删除的用户');
  }
}

// 确认删除用户
const confirmDeleteUser = async () => {
  if (!deleteUserData.id) return;

  try {
    const success = await userStore.deleteUser(deleteUserData.id);
    if (success) {
      ElMessage.success('用户删除成功');
      showDeleteDialog.value = false;
      refreshUserList(); // 刷新列表以更新当前用户
    } else {
      ElMessage.error(userStore.error || '用户删除失败');
    }
  } catch (error) {
    console.error('删除用户失败:', error);
    ElMessage.error('删除用户失败，请检查网络连接');
  }
};


// 添加用户
const handleAddUser = async () => {
  if (!addFormRef.value) return

  const isValid = await addFormRef.value.validate()
  if (!isValid) {
    ElMessage.error('请检查表单信息')
    return
  }

  const success = await userStore.createUser(addFormData)
  if (success) {
    ElMessage.success('用户添加成功')
    showAddDialog.value = false
    handleCloseAddDialog()
  } else {
    ElMessage.error(userStore.error || '用户添加失败')
  }
}

// 编辑用户
const handleEditUser = async () => {
  if (!editFormRef.value) return

  const isValid = await editFormRef.value.validate()
  if (!isValid) {
    ElMessage.error('请检查表单信息')
    return
  }

  // console.log('提交编辑用户数据:', editFormData)
  const success = await userStore.updateUser(editFormData.id, editFormData)
  if (success) {
    ElMessage.success('用户信息更新成功')
    showEditDialog.value = false
    handleCloseEditDialog()
  } else {
    ElMessage.error(userStore.error || '用户信息更新失败')
  }
}

// 关闭添加对话框
const handleCloseAddDialog = () => {
  if (addFormRef.value) {
    addFormRef.value.resetForm()
  }
  Object.assign(addFormData, {
    name: '',
    age: 25,
    height: 170,
    weight: 60,
    blood: '',
    phone: '',
    intro: ''
  })
}

// 关闭编辑对话框
const handleCloseEditDialog = () => {
  if (editFormRef.value) {
    editFormRef.value.resetForm()
  }
  Object.assign(editFormData, {
    id: null,
    name: '',
    age: 25,
    height: 170,
    weight: 60,
    blood: '',
    phone: '',
    intro: ''
  })
}

// 获取最后检查时间
const getLastCheckTime = () => {
  if (userStore.lastCheckTime) {
    const date = new Date(userStore.lastCheckTime)
    return date.toLocaleString()
  }
  return '从未检查'
}


</script>

<style lang="scss" scoped>
.user-management {
  flex: 1;
  background: rgba(255, 255, 255, 0.1);
  border-radius: 12px;
  padding: 20px;
  backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.2);
  overflow-y: auto;
}

.module-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
  position: relative;

  h3 {
    margin: 0;
    color: #fff;
    font-size: 20px;
    font-weight: 600;
  }

  .header-actions {
    display: flex;
    gap: 10px;
    align-items: center;
  }
}

.current-user {
  margin-bottom: 30px;

  h4 {
    margin-bottom: 15px;
    color: #fff;
  }
}

.user-card {
  background: rgba(255, 255, 255, 0.1);
  border-radius: 8px;
  padding: 15px;
  margin-bottom: 15px;
  border: 1px solid rgba(255, 255, 255, 0.2);
  transition: all 0.3s ease;

  &.current {
    background: rgba(64, 158, 255, 0.2);
    border-color: rgba(64, 158, 255, 0.4);
  }

  &.active {
    background: rgba(103, 194, 58, 0.2);
    border-color: rgba(103, 194, 58, 0.4);
  }

  &:hover {
    transform: translateY(-2px);
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
  }
}

.user-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.user-name {
  font-size: 16px;
  font-weight: 600;
  color: #fff;
}

.user-details {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
  margin: 10px 0;

  span {
    background: rgba(255, 255, 255, 0.1);
    padding: 4px 8px;
    border-radius: 4px;
    font-size: 12px;
  }
}

.user-intro {
  margin-top: 10px;
  color: rgba(255, 255, 255, 0.8);
  font-size: 14px;
}

.user-actions {
  display: flex;
  gap: 8px;
}

.users-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
  gap: 15px;
}

// 响应式设计
@media (max-width: 768px) {
  .module-header {
    flex-direction: column;
    gap: 10px;
    align-items: flex-start;
  }

  .header-actions {
    width: 100%;
    justify-content: space-between;
  }

  .users-grid {
    grid-template-columns: 1fr;
  }

  .user-actions {
    flex-direction: column;
    gap: 4px;
  }
}

// 删除确认对话框样式
.delete-confirm-dialog {
  .el-dialog__header {
    background: linear-gradient(135deg, #f56c6c, #f78989);
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
        color: #fde2e2;
      }
    }
  }
  
  .el-dialog__body {
    padding: 0;
  }
  
  .delete-confirm-content {
    display: flex;
    align-items: center;
    gap: 20px;
    padding: 30px;
    background: linear-gradient(135deg, #fefefe, #f5f7fa);
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
      background: linear-gradient(90deg, #f56c6c, #f78989);
    }
  }

  .delete-icon-wrapper {
    background: linear-gradient(135deg, #fde2e2, #fef0f0);
    border-radius: 50%;
    width: 70px;
    height: 70px;
    display: flex;
    align-items: center;
    justify-content: center;
    box-shadow: 0 4px 12px rgba(245, 108, 108, 0.2);
    border: 2px solid #fde2e2;
    animation: deleteIconPulse 2s ease-in-out infinite;
  }

  .delete-icon {
    font-size: 40px;
    color: #f56c6c;
    filter: drop-shadow(0 2px 4px rgba(245, 108, 108, 0.3));
  }

  .delete-message {
    flex: 1;
    
    h4 {
      color: #303133;
      font-size: 18px;
      margin-bottom: 12px;
      font-weight: 600;
      line-height: 1.4;
    }
    
    .delete-warning {
      display: flex;
      align-items: center;
      color: #e6a23c;
      font-size: 14px;
      margin-top: 8px;
      padding: 12px;
      background: rgba(230, 162, 60, 0.1);
      border-radius: 6px;
      border-left: 3px solid #e6a23c;
      
      .el-icon {
        margin-right: 8px;
        font-size: 16px;
        color: #e6a23c;
      }
    }
  }

  .delete-actions {
    display: flex;
    justify-content: flex-end;
    gap: 12px;
    padding: 20px 30px;
    background: #fafafa;
    border-top: 1px solid #ebeef5;
    border-radius: 0 0 8px 8px;
  }

  .delete-btn {
    background: linear-gradient(135deg, #f56c6c, #f78989);
    border: none;
    color: white;
    font-weight: 600;
    padding: 10px 20px;
    border-radius: 6px;
    transition: all 0.3s ease;
    
    &:hover {
      background: linear-gradient(135deg, #f78989, #f9a3a3);
      transform: translateY(-2px);
      box-shadow: 0 6px 20px rgba(245, 108, 108, 0.3);
    }
    
    &:active {
      transform: translateY(0);
    }
    
    .el-icon {
      margin-right: 6px;
    }
  }
}

// 删除图标脉冲动画
@keyframes deleteIconPulse {
  0%, 100% {
    transform: scale(1);
    box-shadow: 0 4px 12px rgba(245, 108, 108, 0.2);
  }
  50% {
    transform: scale(1.05);
    box-shadow: 0 6px 20px rgba(245, 108, 108, 0.4);
  }
}
</style>
