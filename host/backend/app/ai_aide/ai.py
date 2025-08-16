from fastapi import APIRouter, HTTPException
from typing import List, Dict, Optional
from .core import ai
from ..health import health_get_data_by_id #, health_get_data_by_name
from ..user import user_get_whoami


sub_app_ai = APIRouter()


@sub_app_ai.post("/chat")
async def ai_chat(
		msg_usr: Optional[str] = '',
		msg_sys: Optional[str] = None,
		stream: bool = True
):
	"""
	AI对话接口
	支持流式逐词返回
	"""

	if msg_sys is None:
		msg_sys = (
			'角色：你是一个通用聊天模型，旨在提供自然流畅的对话体验。'
			# '行为：适当展现幽默感（例如：结合玩笑梗、夸张说法），切忌突兀不连贯。'
			# '行为：可适当运用emoji表情（不是字符堆叠），增强情绪体验。'
			'注意：保持信息准确、回答简明；尽量纯中文、数字，可以有常见的英文。'
			'注意：输出纯文本，不需要markdown等的渲染。'
			'注意：若询问问题或存在信息未知，则自动上网搜索。'
		)

	msg_usr = '你好' if len(msg_usr.strip()) == 0 else msg_usr

	msg: List[Dict[str, str]] = [
		{'role': 'system', 'content': msg_sys},
		{'role':   'user', 'content': msg_usr}
	]

	try:
		if stream:  # 流式响应
			return await ai.ai_chat_by_stream(msg)
		else:       # 非流式响应
			return await ai.ai_chat_by_no_stream(msg)
	except Exception as e:
		raise HTTPException(status_code=500, detail=f"AI对话失败: {str(e)}")


@sub_app_ai.post("/analyse")
async def ai_analyse(
		msg_usr: Optional[str] = None,
		stream: bool = True
):
	"""
	AI分析接口
	支持流式逐词返回
	"""

	user_id = user_get_whoami()['data']
	if user_id < 0:
		raise HTTPException(status_code=500, detail=f"当前用户为游客用户，无相关数据，请切换用户")

	health = await health_get_data_by_id(user_id, 30)

	msg_usr = str(msg_usr) + (
		'注意：若distance<30(cm)，提醒我保持适当距离以保护视力'
		'\n以下是我的健康数据，请为我分析，并提出相关建议' + str(health['data'])
	)

	msg_sys = (
		'背景：用户坐在电脑桌前，现在获取到了呼吸、心率、体温和屏幕距离；'
		'角色：你是一个专业的医疗健康助理，擅长分析健康监测数据并提供实用建议；'
		'行为：分析数据、提供建议；'
		'表达：口语化、逻辑清晰、通俗易懂、简单明了；尽量纯中文、数字，可以有常见的英文'
		'注意：输出纯文本，不需要markdown等的渲染。'
	)

	msg: List[Dict[str, str]] = [
		{'role': 'system', 'content': msg_sys},
		{'role':   'user', 'content': msg_usr}
	]

	try:
		if stream:  # 流式响应
			return await ai.ai_chat_by_stream(msg)
		else:       # 非流式响应
			return await ai.ai_chat_by_no_stream(msg)
	except Exception as e:
		raise HTTPException(status_code=500, detail=f"AI对话失败: {str(e)}")
