# import uvicorn
from config import cfg_tortoise_orm
# from contextlib import asynccontextmanager
from fastapi import FastAPI, Request
# from pydantic import ValidationError
from tortoise.contrib.fastapi import register_tortoise
from app import (
	app_ai_aide,
	app_health,
	app_offstage,
	app_user,

	# ai_load_model,
	# asr_load_model,
	# record_load_model,
	# tts_load_model
)


# @asynccontextmanager
# async def lifespan(app: FastAPI):
# 	"""启停 附加处理"""

# 	'加载模型'
# 	await ai_load_model()
# 	await asr_load_model()
# 	await record_load_model()
# 	await tts_load_model()

# 	yield
# 	pass


app_backend = FastAPI() # (lifespan=lifespan)


register_tortoise(
	app_backend,
	config=cfg_tortoise_orm,
	generate_schemas=True
)


# @app_backend.middleware('http')
# async def http_middleware(request: Request, call_next):
# 	print(request.base_url)
# 	response = await call_next(request)
# 	return response


app_backend.include_router(app_user, prefix='/user', tags=['用户数据'])
app_backend.include_router(app_health, prefix='/health', tags=['健康数据'])
app_backend.include_router(app_ai_aide, prefix='/ai_aide', tags=['ai助手'])
app_backend.include_router(app_offstage, prefix='/offstage', tags=['后台日志'])


# if __name__ == '__main__':
# 	try:
# 		uvicorn.run(
# 			app='main:app_backend',
# 			host='0.0.0.0', port=8749,
# 			reload=True
# 		)
# 	except ValidationError as e:
# 		print(e.json())
