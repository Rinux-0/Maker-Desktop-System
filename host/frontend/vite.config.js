import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import AutoImport from 'unplugin-auto-import/vite'
import Components from 'unplugin-vue-components/vite'
import { ElementPlusResolver } from 'unplugin-vue-components/resolvers'
import { resolve } from 'path'

export default defineConfig({
	plugins: [
		vue(),
		AutoImport({
			resolvers: [ElementPlusResolver()],
			imports: ['vue', 'vue-router', 'pinia'],
			dts: true
		}),
		Components({
			resolvers: [ElementPlusResolver()],
			dts: true
		})
	],
	resolve: {
		alias: {
			'@': resolve(__dirname, 'src')
		}
	},
	server: {
		port: 3000,
		proxy: {
			'/api': {
				target: 'http://192.168.888.888:8749',	// eg: 'http://192.168.888.888:8749'
				changeOrigin: true,
				rewrite: (path) => path.replace(/^\/api/, ''),
				configure: (proxy, options) => {
					proxy.on('error', (err, req, res) => {
						// console.log('proxy error', err)
					})
					proxy.on('proxyReq', (proxyReq, req, res) => {
						// console.log('Sending Request to the Target:', req.method, req.url)
					})
					proxy.on('proxyRes', (proxyRes, req, res) => {
						// console.log('Received Response from the Target:', proxyRes.statusCode, req.url)
					})
				}
			}
		}
	}
})
