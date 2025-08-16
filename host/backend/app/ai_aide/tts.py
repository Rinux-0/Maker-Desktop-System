from fastapi import APIRouter, UploadFile
from .core.tts import tts


sub_app_tts = APIRouter()


@sub_app_tts.post('/')
async def text_to_speech_by_str(string: str):
	"""
	read text for the user.
	:param string: the text string to be processed.
	"""

	if tts.text_to_speech(string):
		tts.play_speech()

	return {
		'message': 'tts-ing',
		'str': string
	}
