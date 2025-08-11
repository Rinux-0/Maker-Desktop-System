import { createRouter, createWebHistory } from 'vue-router'
import Layout from '@/layout/index.vue'

const routes = [
	{
		path: '/',
		component: Layout,
		redirect: '/dashboard',
		children: [
			{
				path: 'dashboard',
				name: 'Dashboard',
				component: () => import('@/views/dashboard/index.vue'),
				meta: { title: '健康监测' }
			}
		]
	}
]

const router = createRouter({
	history: createWebHistory(),
	routes
})

export default router
