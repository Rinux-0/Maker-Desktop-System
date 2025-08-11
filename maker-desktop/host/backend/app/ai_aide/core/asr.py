import asyncio

from fastapi.responses import StreamingResponse
from funasr.utils.postprocess_utils import rich_transcription_postprocess
import json
import funasr
import soundfile
import os


class ASR:
	def __init__(self):
		self.model_stream = funasr.AutoModel(
			model="paraformer-zh-streaming",
			disable_update=True
		)
		self.model_punc = funasr.AutoModel(
			model="ct-punc",
			disable_update = True
		)
		self.model_no_stream = funasr.AutoModel(
			model="iic/SenseVoiceSmall",
			vad_model="fsmn-vad",
			vad_kwargs={"max_single_segment_time": 30000},
			device="cuda:0",
			disable_update=True
		)

		self.wav_file = os.path.join(os.getcwd(), 'file/audio/input.wav')

		self.chunk_size = [0, 10, 5]				  # [0, 10, 5] 600ms, [0, 8, 4] 480ms
		self.chunk_stride = self.chunk_size[1] * 960  # 600ms


	async def response_stream(self, speech: list, chunk_num: int):
		cache = {}

		for i in range(chunk_num):
			speech_chunk = speech[i * self.chunk_stride:(i + 1) * self.chunk_stride]
			data = self.model_stream.generate(
				input=speech_chunk, cache=cache,
				is_final=(i == chunk_num - 1),
				chunk_size=self.chunk_size,
				encoder_chunk_look_back=4,  # number of chunks to lookback for encoder self-attention
				decoder_chunk_look_back=1  # number of encoder chunks to lookback for decoder cross-attention
			)
			print(data[0]['text'])
			await asyncio.sleep(0.05)
			yield json.dumps({
				'content': data[0]['text'],
				'type': 'content'
			})


asr = ASR()


async def asr_load_model():
	# global asr
	# asr = ASR()ursor
	print('asr_model load success')


async def asr_by_stream():
	"""（流式）实时语音识别，分块处理"""

	speech, sample_rate = soundfile.read(asr.wav_file)
	chunk_num = int(len(speech - 1) / asr.chunk_stride + 1)

	async def generate_stream():
		async for chunk in asr.response_stream(speech, chunk_num):
			yield f'data: {chunk}\n\n'

	return StreamingResponse(
		generate_stream(),
		media_type='text/event-stream',
		headers={
			'Cache-Control': 'no-cache',
			'Connection': 'keep-alive',
			'Access-Control-Allow-Origin': '*',
			'Access-Control-Allow-Headers': '*'
		}
	)


def asr_by_no_stream():
	"""（非流式）非实时语音识别，整块处理"""

	res = asr.model_no_stream.generate(
		input=asr.wav_file,
		cache={},
		language='auto',  # "zn", "en", "yue", "ja", "ko", "nospeech"
		use_itn=True,
		batch_size_s=60,
		merge_vad=True,
		merge_length_s=15
	)

	return rich_transcription_postprocess(res[0]["text"])


async def asr_set_punc(str_input: str):
	text = asr.model_punc.generate(input=str_input)
	print(text[0]['text'])
	return {'content': text[0]['text']}
