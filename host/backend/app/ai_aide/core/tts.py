import json

from openai import OpenAI
from config import api_key_tts, url_api_tts
from pydub.playback import play
from pydub import AudioSegment
import requests
import os


class TTS:
	file_pos = './file/audio'
	file_mainname = 'output'
	file_extname = 'wav'

	url = url_api_tts
	token = api_key_tts

	response = None

	headers = {
		'Content-Type': 'application/json',
		'Authorization': "Bearer " + token
	}
	request_data = {
		"model": "fnlp/MOSS-TTSD-v0.5",
		"stream": True,
		# "references": [{
		# 	"audio": "https://sf-maas-uat-prod.oss-cn-shanghai.aliyuncs.com/voice_template/fish_audio-Charles.mp3",
		# 	"text": "他又躺在那里，眼睛闭着，仍然沉浸在梦境的气氛里。那是个庞杂而亮堂的梦",
		# }],
		"max_tokens": 1600,
		'sample_rate': 16000,
		"response_format": "wav",
		"speed": 1,
		"gain": 1
	}


	def get_file_path(self):
		"""获取完整的文件路径"""
		return f"{self.file_pos}/{self.file_mainname}.{self.file_extname}"


	def text_to_speech(self,
		text: str,
		voice: str = "fnlp/MOSS-TTSD-v0.5:alex" # anna
	):
		self.request_data['input'] = text
		self.request_data['voice'] = voice

		try:
			self.response = requests.post(url=self.url, headers=self.headers, data=json.dumps(self.request_data))
			if self.response.status_code != 200:
				raise Exception(f"请求失败: {self.response.status_code}, {self.response.text}")

			os.makedirs(self.file_pos, exist_ok=True)
			with open(self.get_file_path(), "wb") as f:
				f.write(self.response.content)
		except Exception as e:
			print(f"error: {e}")
			return None

		return True


	def play_speech(self):
		"""播放生成的语音"""

		os.makedirs(self.file_pos, exist_ok=True)

		audio = AudioSegment.from_wav(self.get_file_path())
		play(audio)

		print("▶️ 播放完成")


tts = TTS()


async def tts_load_model():
	# global tts
	# tts = TTS()   // 在此处初始化 会有问题
	print('tts_model load success')
