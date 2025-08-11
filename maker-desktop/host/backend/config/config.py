import os

# 文件存储路径
path_pwd = os.getcwd()
path_upload = path_pwd + '/' + 'file/upload'
path_audio =  path_pwd + '/' + 'file/audio'

# AI服务配置
url_api_deepseek = 'https://api.deepseek.com/'
url_api_ai = url_api_deepseek

# TTS服务配置
url_api_siliconflow = 'https://api.siliconflow.cn/v1/audio/speech'
url_api_tts = url_api_siliconflow

# ASR服务配置
url_api_funasr = 'http://localhost:10095'
url_api_asr = url_api_funasr
