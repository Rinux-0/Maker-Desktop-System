import axios from 'axios'

// 创建axios实例
const api = axios.create({
	baseURL: '/api',
	timeout: 10000,
	headers: {
		'Content-Type': 'application/json'
	}
})

// 请求拦截器
api.interceptors.request.use(
	config => {
		// 添加请求日志
		// console.log('API Request:', config.method?.toUpperCase(), config.url, config.data || config.params)
		return config
	},
	error => {
		console.error('Request Error:', error)
		return Promise.reject(error)
	}
)

// 响应拦截器
api.interceptors.response.use(
	response => {
		// 添加响应日志
		// console.log('API Response:', response.config.url, response.data)
		return response.data
	},
	error => {
		console.error('API Error:', error.response?.status, error.response?.data, error.message)
		return Promise.reject(error)
	}
)

// 用户相关API
export const userAPI = {
	// 获取当前用户ID
	getCurrentUser: () => api.get('/user/whoami'),

	// 获取用户列表
	getUsers: () => api.get('/user/'),

	// 获取用户信息（通过ID）
	getUserById: (id) => api.get(`/user/id/${id}`),

	// 获取用户信息（通过姓名）
	getUserByName: (name) => api.post(`/user/name/${name}`),

	// 创建用户
	createUser: (userData) => {
		const params = new URLSearchParams()
		params.append('name', userData.name)
		params.append('age', userData.age)
		params.append('height', userData.height)
		params.append('weight', userData.weight)
		params.append('blood', userData.blood)
		params.append('phone', userData.phone)
		params.append('intro', userData.intro)

		return api.post(`/user/?${params.toString()}`)
	},

	// 更新用户
	updateUser: (id, userData) => {
		const params = new URLSearchParams()
		params.append('name', userData.name)
		params.append('age', userData.age)
		params.append('height', userData.height)
		params.append('weight', userData.weight)
		params.append('blood', userData.blood)
		params.append('phone', userData.phone)
		params.append('intro', userData.intro)

		return api.put(`/user/id/${id}?${params.toString()}`)
	},

	// 删除用户
	deleteUser: (id) => api.delete(`/user/id/${id}`),

	// 设置当前用户 - 修复：使用查询参数
	setCurrentUser: (id) => {
		return api.post(`/user/whoami?user_id=${id}`)
	}
}

// 健康数据相关API
export const healthAPI = {
	// 获取健康数据
	getHealthData: (userId, limit = 30) => api.get(`/health/id/${userId}?data_num=${limit}`),

	// 清空健康数据
	clearHealthData: (userId) => api.delete(`/health/?user_id=${userId}`),

	// 添加健康数据
	addHealthData: (userId, dataType, data) => {
		const params = new URLSearchParams()
		params.append('data_type', dataType)
		params.append('data', data)
		params.append('user_id', userId)

		return api.post('/health/', params, {
			headers: {
				'Content-Type': 'application/x-www-form-urlencoded'
			}
		})
	},

	// 获取使用时间
	getUsageTime: (userId) => api.get(`/health/${userId}/usage-time`),

	// 更新使用时间
	updateUsageTime: (userId, data) => api.put(`/health/${userId}/usage-time`, data)
}



// AI相关API
export const aiAPI = {
	// 发送消息到AI
	sendMessage: (message) => {
		const params = new URLSearchParams()
		params.append('msg_usr', message)
		params.append('stream', 'false')
		return api.post(`/ai_aide/ai/chat?${params.toString()}`)
	},

	// 分析健康数据
	analyzeHealth: (userId) => {
		const params = new URLSearchParams()
		params.append('user_id', userId)
		params.append('stream', 'true')
		return api.post(`/ai_aide/ai/analyse?${params.toString()}`)
	},

	// 语音识别
	speechToText: (stream = false) => {
		if (stream) {
			// 流式模式：调用后端录音并流式返回识别结果
			return fetch('/api/ai_aide/asr/?stream=true', {
				method: 'POST',
				headers: {
					'Content-Type': 'application/json'
				}
			})
		} else {
			// 非流式模式：调用后端录音并返回识别结果
			return api.post('/ai_aide/asr/?stream=false')
		}
	},

	// 文本转语音
	textToSpeech: (text) => {
		const params = new URLSearchParams()
		params.append('string', text)
		return api.post(`/ai_aide/tts/?${params.toString()}`)
	}
}

export default api
