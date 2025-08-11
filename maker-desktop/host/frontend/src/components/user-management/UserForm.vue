<template>
  <el-form
    ref="formRef"
    :model="form"
    :rules="rules"
    label-width="80px"
    class="user-form animate fade-in-up"
  >
    <el-form-item label="姓名" prop="name" class="animate fade-in-left delay-100">
      <el-input v-model="form.name" placeholder="请输入姓名" class="input-focus" />
    </el-form-item>

    <el-form-item label="年龄" prop="age" class="animate fade-in-left delay-200">
      <el-input-number
        v-model="form.age"
        :min="1"
        :max="150"
        :precision="0"
        style="width: 100%"
        class="input-focus"
      />
    </el-form-item>

    <el-form-item label="身高" prop="height" class="animate fade-in-left delay-300">
      <el-input-number
        v-model="form.height"
        :min="50"
        :max="300"
        :precision="0"
        style="width: 100%"
        class="input-focus"
      />
      <span class="unit">cm</span>
    </el-form-item>

    <el-form-item label="体重" prop="weight" class="animate fade-in-left delay-400">
      <el-input-number
        v-model="form.weight"
        :min="10"
        :max="500"
        :precision="0"
        style="width: 100%"
        class="input-focus"
      />
      <span class="unit">kg</span>
    </el-form-item>

    <el-form-item label="血型" prop="blood" class="animate fade-in-left delay-500">
      <el-select v-model="form.blood" placeholder="请选择血型" style="width: 100%" class="input-focus">
        <el-option label="A型" value="A" />
        <el-option label="B型" value="B" />
        <el-option label="AB型" value="AB" />
        <el-option label="O型" value="O" />
      </el-select>
    </el-form-item>

    <el-form-item label="电话" prop="phone" class="animate fade-in-left delay-600">
      <el-input v-model="form.phone" placeholder="请输入电话号码" class="input-focus" />
    </el-form-item>

    <el-form-item label="简介" prop="intro" class="animate fade-in-left delay-700">
      <el-input
        v-model="form.intro"
        type="textarea"
        :rows="3"
        placeholder="请输入简介"
        :maxlength="48"
        show-word-limit
        class="input-focus"
      />
    </el-form-item>
  </el-form>
</template>

<script setup>
import { ref, reactive, watch } from 'vue'

const props = defineProps({
  userData: {
    type: Object,
    default: () => ({})
  },
  isEdit: {
    type: Boolean,
    default: false
  }
})

const emit = defineEmits(['update:userData'])

const formRef = ref(null)

// 表单数据
const form = reactive({
  name: '',
  age: 25,
  height: 170,
  weight: 60,
  blood: '',
  phone: '',
  intro: ''
})

// 表单验证规则
const rules = {
  name: [
    { required: true, message: '请输入姓名', trigger: 'blur' },
    { min: 1, max: 16, message: '姓名长度在1到16个字符', trigger: 'blur' }
  ],
  age: [
    { required: true, message: '请输入年龄', trigger: 'blur' },
    { type: 'number', min: 1, max: 150, message: '年龄必须在1到150之间', trigger: 'blur' }
  ],
  height: [
    { required: true, message: '请输入身高', trigger: 'blur' },
    { type: 'number', min: 50, max: 300, message: '身高必须在50到300之间', trigger: 'blur' }
  ],
  weight: [
    { required: true, message: '请输入体重', trigger: 'blur' },
    { type: 'number', min: 10, max: 500, message: '体重必须在10到500之间', trigger: 'blur' }
  ],
  blood: [
    { required: true, message: '请选择血型', trigger: 'change' }
  ],
  phone: [
    { required: true, message: '请输入电话号码', trigger: 'blur' },
    { pattern: /^1[3-9]\d{9}$/, message: '请输入正确的手机号码', trigger: 'blur' }
  ],
  intro: [
    { max: 16, message: '简介长度不能超过16个字符', trigger: 'blur' }
  ]
}

// 监听用户数据变化
watch(() => props.userData, (newData) => {
  // console.log('UserForm接收到新数据:', newData)
  if (newData && Object.keys(newData).length > 0) {
    // 确保数据类型正确
    Object.assign(form, {
      name: newData.name || '',
      age: Number(newData.age) || 25,
      height: Number(newData.height) || 170,
      weight: Number(newData.weight) || 60,
      blood: newData.blood || '',
      phone: newData.phone || '',
      intro: newData.intro || ''
    })
    // console.log('UserForm更新后的表单数据:', form)
  }
}, { immediate: true, deep: true })

// 监听表单变化
watch(form, (newForm) => {
  // console.log('UserForm表单数据变化:', newForm)
  emit('update:userData', { ...newForm })
}, { deep: true })

// 验证表单
const validate = async () => {
  if (!formRef.value) return false
  try {
    await formRef.value.validate()
    return true
  } catch (error) {
    return false
  }
}

// 重置表单
const resetForm = () => {
  if (formRef.value) {
    formRef.value.resetFields()
  }
  Object.assign(form, {
    name: '',
    age: 25,
    height: 170,
    weight: 60,
    blood: '',
    phone: '',
    intro: ''
  })
}

// 暴露方法
defineExpose({
  validate,
  resetForm,
  form
})
</script>

<style lang="scss" scoped>
.user-form {
  .unit {
    margin-left: 8px;
    color: rgba(255, 255, 255, 0.6);
  }
}
</style>
