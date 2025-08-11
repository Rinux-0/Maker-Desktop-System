from config import url_api_ai, api_key_ai
from fastapi.responses import StreamingResponse
from openai import OpenAI
from typing import AsyncGenerator, List, Dict
import asyncio
import json
import time


class AI:
	def __init__(self):
		self.client = OpenAI(api_key=api_key_ai, base_url=url_api_ai)

	async def response_stream(self,
		messages: List[Dict[str, str]],
		##### system: AI 的行为、角色、背景
		##### user: 用户输入的问题、请求
		##### assistant: （自动生成）
		temperature: float = 0.7,
		max_tokens: int = 2000
	) -> AsyncGenerator[str, None]:
		"""
		与DeepSeek进行流式对话
		:param messages: 对话消息列表
		:param temperature: 温度参数
		:param max_tokens: 最大token数
		:yield: 流式返回的AI回复内容
		"""

		try:
			response = self.client.chat.completions.create(
				model='deepseek-chat',
				messages=messages,
				max_tokens=max_tokens,
				temperature=temperature,
				stream=True
			)
			for chunk in response:
				content = chunk.choices[0].delta.content
				if content:
					# await asyncio.sleep(0.3)  # 模拟延时
					yield json.dumps({
						'content': content,
						'type': 'content'
					})

			yield json.dumps({'type': 'done'})  # 发送完成信号
		except Exception as e:
			yield json.dumps({
				'error': str(e),
				'type': 'error'
			})


ai = AI()


async def ai_load_model():
	# global ai
	# ai = None
	print('ai_model load success')


async def ai_chat_by_stream(msg: List[Dict[str, str]]):
	async def generate_stream():
		async for chunk in ai.response_stream(msg):
			yield f'data: {chunk}\n\n'

	return StreamingResponse(
		generate_stream(),
		media_type="text/event-stream",
		headers={
			'Cache-Control': 'no-cache',
			'Connection': 'keep-alive',
			'Access-Control-Allow-Origin': '*',
			'Access-Control-Allow-Headers': '*'
		}
	)


async def ai_chat_by_no_stream(msg: List[Dict[str, str]]):
	response = ai.client.chat.completions.create(
		model='deepseek-chat',
		messages=msg,
		stream=False,
		temperature=0.7,
		max_tokens=2000
	)

	# print(response.choices[0].message.content)

	return {
		'content': response.choices[0].message.content,
		'type': 'complete'
	}
