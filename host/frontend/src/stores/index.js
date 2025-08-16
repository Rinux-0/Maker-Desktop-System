import { defineStore } from 'pinia'
import { userAPI } from '@/utils/api'
import { healthAPI } from '@/utils/api'
import { aiAPI } from '@/utils/api' // Added aiAPI import

// 用户状态管理
export const useUserStore = defineStore('user', {
	state: () => ({
		currentUser: null,
		users: [],
		loading: false,
		error: null,
		// 轮询相关状态
		pollingInterval: null,
		lastUserId: -1,
		// 通知相关状态
		notification: null
	}),

	getters: {
		// 获取当前用户ID
		currentUserId: (state) => state.currentUser?.id ?? -1,

		// 获取用户列表（用于选择）
		userOptions: (state) => state.users.map(user => ({
			label: user.name,
			value: user.id
		}))
	},

	actions: {
		// 设置当前用户
		setCurrentUser(user) {
			this.currentUser = user
		},

		// 设置用户列表
		setUsers(users) {
			this.users = users
		},

		// 获取用户列表
		async fetchUsers() {
			this.loading = true
			this.error = null
			try {
				const response = await userAPI.getUsers()
				if (response.data) {
					this.users = response.data
				} else {
					this.error = response.error || '获取用户列表失败'
				}
			} catch (error) {
				console.error('获取用户列表错误:', error)
				this.error = error.response?.data?.error || error.message || '获取用户列表失败'
			} finally {
				this.loading = false
			}
		},

		// 获取用户详细信息
		async fetchUserById(id) {
			this.loading = true
			this.error = null
			try {
				const response = await userAPI.getUserById(id)
				if (response.data) {
					this.currentUser = response.data
					return response.data
				} else {
					this.error = response.error || '获取用户信息失败'
					return null
				}
			} catch (error) {
				console.error('获取用户信息错误:', error)
				this.error = error.response?.data?.error || error.message || '获取用户信息失败'
				return null
			} finally {
				this.loading = false
			}
		},

		// 切换用户
		async switchUser(userId) {
			this.loading = true
			this.error = null
			try {
				const response = await userAPI.setCurrentUser(userId)
				if (response.message) {
					// 获取用户详细信息
					const userData = await this.fetchUserById(userId)
					if (userData) {
						this.currentUser = userData
						// 更新轮询状态中的lastUserId
						this.lastUserId = userId
						return true
					} else {
						this.error = '获取用户详细信息失败'
					}
				} else {
					this.error = response.error || '切换用户失败'
				}
			} catch (error) {
				console.error('切换用户错误:', error)
				this.error = error.response?.data?.error || error.message || '切换用户失败'
			} finally {
				this.loading = false
			}
			return false
		},

		// 创建用户
		async createUser(userData) {
			this.loading = true
			this.error = null
			try {
				const response = await userAPI.createUser(userData)
				if (response.message) {
					// 重新获取用户列表
					await this.fetchUsers()
					return true
				} else {
					this.error = response.error || '创建用户失败'
				}
			} catch (error) {
				console.error('创建用户错误:', error)
				this.error = error.response?.data?.error || error.message || '创建用户失败'
			} finally {
				this.loading = false
			}
			return false
		},

		// 更新用户
		async updateUser(id, userData) {
			this.loading = true
			this.error = null
			try {
				const response = await userAPI.updateUser(id, userData)
				if (response.message) {
					// 如果是当前用户，更新当前用户信息
					if (this.currentUser && this.currentUser.id === id) {
						this.currentUser = { ...this.currentUser, ...userData }
					}
					// 重新获取用户列表
					await this.fetchUsers()
					return true
				} else {
					this.error = response.error || '更新用户失败'
				}
			} catch (error) {
				console.error('更新用户错误:', error)
				this.error = error.response?.data?.error || error.message || '更新用户失败'
			} finally {
				this.loading = false
			}
			return false
		},

		// 删除用户
		async deleteUser(id) {
			this.loading = true
			this.error = null
			try {
				const response = await userAPI.deleteUser(id)
				if (response.message) {
					// 如果删除的是当前用户，清空当前用户和轮询状态
					if (this.currentUser && this.currentUser.id === id) {
						this.currentUser = null
						this.lastUserId = -1
					}
					// 重新获取用户列表
					await this.fetchUsers()
					return true
				} else {
					this.error = response.error || '删除用户失败'
				}
			} catch (error) {
				console.error('删除用户错误:', error)
				this.error = error.response?.data?.error || error.message || '删除用户失败'
			} finally {
				this.loading = false
			}
			return false
		},

		// 初始化用户数据
		async initUserData() {
			// 获取用户列表
			await this.fetchUsers()

			// 获取当前用户ID
			try {
				const response = await userAPI.getCurrentUser()
				if (response.data !== -1) {
					const userData = await this.fetchUserById(response.data)
					if (userData) {
						this.currentUser = userData
						this.lastUserId = userData.id
					}
				} else {
					// 如果没有当前用户，设置lastUserId为-1
					this.lastUserId = -1
					this.currentUser = null
				}
			} catch (error) {
				console.error('获取当前用户失败:', error)
				// 出错时设置默认值
				this.lastUserId = -1
				this.currentUser = null
			}
		},

		// 开始轮询当前用户ID
		startPolling() {
			// 立即执行一次检查
			this.checkCurrentUserId()

			// 设置定时器，每隔1秒检查一次
			this.pollingInterval = setInterval(() => {
				this.checkCurrentUserId()
			}, 1000)
		},

		// 检查当前用户ID是否有变化
		async checkCurrentUserId() {
			try {
				const response = await userAPI.getCurrentUser()
				const currentUserId = response.data

				// 如果用户ID发生变化
				if (currentUserId !== this.lastUserId) {
					// 更新lastUserId
					this.lastUserId = currentUserId

					// 如果新的用户ID有效，获取用户信息
					if (currentUserId !== -1) {
						const userData = await this.fetchUserById(currentUserId)
						if (userData) {
							this.currentUser = userData
						}
					} else {
						// 如果用户ID为-1，清空当前用户
						this.currentUser = null
					}
				}
			} catch (error) {
				console.error('检查当前用户ID失败:', error)
			}
		},

		// 清除通知
		clearNotification() {
			this.notification = null
		}
	}
})

// 健康数据状态管理
export const useHealthStore = defineStore('health', {
	state: () => ({
		healthData: {
			heart: [],
			breath: [],
			temperature: [],
			distance: []
		},
		usageTime: '00:00:00',
		isPaused: false,
		loading: false,
		error: null
	}),

	actions: {
		// 设置健康数据
		setHealthData(data) {
			this.healthData = data
		},

		// 获取健康数据
		async fetchHealthData(userId, limit = 30) {
			this.loading = true
			this.error = null
			try {
				const response = await healthAPI.getHealthData(userId, limit)
				if (response.data) {
					// 处理数据结构：将元组列表转换为普通数组
					const processData = (dataList) => {
						if (!Array.isArray(dataList)) return []
						// 如果数据是元组列表（每个元组包含一个值），提取第一个元素
						return dataList.map(item => {
							if (Array.isArray(item) && item.length > 0) {
								// 如果是元组（数组），提取第一个元素
								return item[0]
							} else if (typeof item === 'object' && item !== null) {
								// 如果是对象，尝试提取值
								const values = Object.values(item)
								return values.length > 0 ? values[0] : item
							}
							// 如果是普通值，直接返回
							return item
						})
					}

					// 确保数据格式正确
					this.healthData = {
						heart: processData(response.data.heart || []),
						breath: processData(response.data.breath || []),
						temperature: processData(response.data.temperature || []),
						distance: processData(response.data.distance || [])
					}
					return true
				} else {
					this.error = response.error || '获取健康数据失败'
					return false
				}
			} catch (error) {
				console.error('获取健康数据错误:', error)
				this.error = error.response?.data?.error || error.message || '获取健康数据失败'
				return false
			} finally {
				this.loading = false
			}
		},

		// 清空健康数据
		async clearHealthData(userId) {
			this.loading = true
			this.error = null
			try {
				const response = await healthAPI.clearHealthData(userId)
				if (response.message) {
					// 清空本地数据
					this.healthData = {
						heart: [],
						breath: [],
						temperature: [],
						distance: []
					}
					return true
				} else {
					this.error = response.error || '清空健康数据失败'
					return false
				}
			} catch (error) {
				console.error('清空健康数据错误:', error)
				this.error = error.response?.data?.error || error.message || '清空健康数据失败'
				return false
			} finally {
				this.loading = false
			}
		},

		// 添加健康数据
		async addHealthData(userId, dataType, data) {
			this.loading = true
			this.error = null
			try {
				const response = await healthAPI.addHealthData(userId, dataType, data)
				if (response.message) {
					// 重新获取数据
					await this.fetchHealthData(userId)
					return true
				} else {
					this.error = response.error || '添加健康数据失败'
					return false
				}
			} catch (error) {
				console.error('添加健康数据错误:', error)
				this.error = error.response?.data?.error || error.message || '添加健康数据失败'
				return false
			} finally {
				this.loading = false
			}
		}
	}
})



// AI状态管理
export const useAIStore = defineStore('ai_aide', {
	state: () => ({
		messages: [],
		isRecording: false,
		ttsEnabled: false,
		loading: false,
		error: null,
		isStreaming: false
	}),

	actions: {
		// 添加消息
		addMessage(message) {
			this.messages.push(message)
		},

		// 清空消息
		clearMessages() {
			this.messages = []
		},

		// 切换录音状态
		toggleRecording() {
			this.isRecording = !this.isRecording
		},

		// 发送消息到AI
		async sendMessage(message) {
			this.loading = true
			this.error = null
			try {
				const response = await aiAPI.sendMessage(message)
				if (response.content) {
					this.addMessage({
						type: 'ai',
						content: response.content,
						timestamp: new Date()
					})
					return response.content
				} else {
					this.error = response.error || 'AI回复失败'
					return null
				}
			} catch (error) {
				console.error('发送消息错误:', error)
				this.error = error.response?.data?.error || error.message || '发送消息失败'
				return null
			} finally {
				this.loading = false
			}
		},

		// 流式发送消息到AI
		async sendMessageStream(message, onChunk) {
			this.loading = true
			this.isStreaming = true
			this.error = null

			try {
				const params = new URLSearchParams()
				params.append('msg_usr', message)
				params.append('stream', 'true')

				const response = await fetch(`/api/ai_aide/ai/chat?${params.toString()}`, {
					method: 'POST',
					headers: {
						'Content-Type': 'application/json',
					}
				})

				if (!response.ok) {
					throw new Error(`HTTP error! status: ${response.status}`)
				}

				const reader = response.body.getReader()
				const decoder = new TextDecoder()
				let buffer = ''

				while (true) {
					const { done, value } = await reader.read()
					if (done) break

					buffer += decoder.decode(value, { stream: true })
					const lines = buffer.split('\n')
					buffer = lines.pop() || ''

					for (const line of lines) {
						if (line.startsWith('data: ')) {
							const data = line.slice(6)
							if (data.trim()) {
								try {
									const parsed = JSON.parse(data)
									if (parsed.type === 'content' && parsed.content) {
										onChunk(parsed.content)
									} else if (parsed.type === 'done') {
										return
									} else if (parsed.type === 'error') {
										throw new Error(parsed.error)
									}
								} catch (e) {
									console.error('解析流数据失败:', e)
								}
							}
						}
					}
				}
			} catch (error) {
				console.error('流式发送消息错误:', error)
				this.error = error.message || '发送消息失败'
			} finally {
				this.loading = false
				this.isStreaming = false
			}
		},

		// 分析健康数据
		async analyzeHealth(userId) {
			this.loading = true
			this.error = null
			try {
				const response = await aiAPI.analyzeHealth(userId)
				if (response.content) {
					this.addMessage({
						type: 'ai',
						content: response.content,
						timestamp: new Date(),
						isAnalysis: true
					})
					return response.content
				} else {
					this.error = response.error || '健康数据分析失败'
					return null
				}
			} catch (error) {
				console.error('分析健康数据错误:', error)
				this.error = error.response?.data?.error || error.message || '分析健康数据失败'
				return null
			} finally {
				this.loading = false
			}
		},

		// 流式分析健康数据
		async analyzeHealthStream(userId, onChunk) {
			this.loading = true
			this.isStreaming = true
			this.error = null

			try {
				const params = new URLSearchParams()
				params.append('user_id', userId)
				params.append('stream', 'true')

				const response = await fetch(`/api/ai_aide/ai/analyse?${params.toString()}`, {
					method: 'POST',
					headers: {
						'Content-Type': 'application/json',
					}
				})

				if (!response.ok) {
					throw new Error(`HTTP error! status: ${response.status}`)
				}

				const reader = response.body.getReader()
				const decoder = new TextDecoder()
				let buffer = ''

				while (true) {
					const { done, value } = await reader.read()
					if (done) break

					buffer += decoder.decode(value, { stream: true })
					const lines = buffer.split('\n')
					buffer = lines.pop() || ''

					for (const line of lines) {
						if (line.startsWith('data: ')) {
							const data = line.slice(6)
							if (data.trim()) {
								try {
									const parsed = JSON.parse(data)
									if (parsed.type === 'content' && parsed.content) {
										onChunk(parsed.content)
									} else if (parsed.type === 'done') {
										return
									} else if (parsed.type === 'error') {
										throw new Error(parsed.error)
									}
								} catch (e) {
									console.error('解析流数据失败:', e)
								}
							}
						}
					}
				}
			} catch (error) {
				console.error('流式分析健康数据错误:', error)
				this.error = error.message || '分析健康数据失败'
			} finally {
				this.loading = false
				this.isStreaming = false
			}
		},

		// 语音识别
		async speechToText() {
			this.loading = true
			this.error = null
			try {
				const response = await aiAPI.speechToText()
				if (response.text) {
					return response.text
				} else {
					this.error = response.error || '语音识别失败'
					return null
				}
			} catch (error) {
				console.error('语音识别错误:', error)
				this.error = error.response?.data?.error || error.message || '语音识别失败'
				return null
			} finally {
				this.loading = false
			}
		},

		// 流式语音识别
		async speechToTextStream(onChunk) {
			this.loading = true
			this.error = null
			try {
				const response = await fetch('/api/ai_aide/asr/?stream=true', {
					method: 'POST',
					headers: {
						'Content-Type': 'application/json'
					}
				})

				if (!response.ok) {
					throw new Error(`HTTP error! status: ${response.status}`)
				}

				const reader = response.body.getReader()
				const decoder = new TextDecoder()
				let buffer = ''
				let finalText = ''

				while (true) {
					const { done, value } = await reader.read()
					if (done) break

					buffer += decoder.decode(value, { stream: true })
					const lines = buffer.split('\n')
					buffer = lines.pop() || ''

					for (const line of lines) {
						if (line.startsWith('data: ')) {
							const data = line.slice(6)
							if (data.trim()) {
								try {
									const parsed = JSON.parse(data)
									if (parsed.type === 'content' && parsed.content) {
										onChunk(parsed.content)
										finalText += parsed.content
									}
								} catch (e) {
									console.error('解析ASR流数据失败:', e)
								}
							}
						}
					}
				}

				return finalText
			} catch (error) {
				console.error('流式语音识别错误:', error)
				this.error = error.message || '语音识别失败'
				return null
			} finally {
				this.loading = false
			}
		},

		// 文本转语音
		async textToSpeech(text) {
			if (!this.ttsEnabled) return

			// 文本预处理
			let processedText = text
				// 1. 只保留中英文、数字、标点、ASCII
				.replace(/[^\u4e00-\u9fa5a-zA-Z0-9\s!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~]/g, '')
				// 2. 删除非数据的单独"-"（保留数字之间的连字符）
				.replace(/(?<!\d)-|-(?!\d)/g, '')
				// 3. 删除非数据的单独"."（保留数字之间的小数点）
				.replace(/(?<!\d)\.|\.(?!\d)/g, '')
				// 4. 清理首尾的空格
				.trim()

			try {
				const response = await aiAPI.textToSpeech(processedText)
				if (response.message) {
					return true
				} else {
					this.error = response.error || '语音播报失败'
					return false
				}
			} catch (error) {
				console.error('语音播报错误:', error)
				this.error = error.response?.data?.error || error.message || '语音播报失败'
				return false
			}
		}
	}
})
