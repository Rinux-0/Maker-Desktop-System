from fastapi import APIRouter
from .core.asr import asr_by_stream, asr_by_no_stream, asr_set_punc


sub_app_asr = APIRouter()


@sub_app_asr.post('/')
async def auto_speech_recognize(stream: bool = True):
	"""
	change speech from the user to text.
	:param stream: whether asr in streaming mode.
	"""

	if stream:
		return await asr_by_stream()
	else:
		return asr_by_no_stream()


@sub_app_asr.put('/')
async def asr_stream_setback_punc(str_input: str):
	return await asr_set_punc(str_input)