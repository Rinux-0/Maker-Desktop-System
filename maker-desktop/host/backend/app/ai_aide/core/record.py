from queue import Queue
from threading import Event
import numpy as np
import sounddevice as sd
import soundfile as sf
import traceback
import os


class Record:
	file_pos = './file/audio'
	file_mainname = 'input'
	file_extname = 'wav'

	is_recording = False

	record_stream = None
	audio_queue: Queue = None
	stop_event = Event()

	device_name: str
	num_channel: int
	bit_rate: int
	sample_rate: int
	sample_bit: int
	dtype: str = 'int16'


	@staticmethod
	def get_default_input_device():
		"""获取系统当前默认输入设备信息"""

		try:
			# 获取系统默认设备信息 [输入设备, 输出设备]
			default_device_id = sd.default.device
			device_index = default_device_id[0]

			devices = sd.query_devices()        # 获取所有设备信息

			return devices[device_index]
		except Exception as e:
			print(f"获取默认设备时出错: {e}")
			return None


	def _audio_callback(self, indata, frames, time, status):
		"""音频回调函数 (内部使用)"""
		if status:
			print(f"音频错误: {status}")
		self.audio_queue.put(indata.copy())


	def get_file_path(self):
		"""获取完整的文件路径"""
		return f"{self.file_pos}/{self.file_mainname}.{self.file_extname}"


	def print_cfg_info(self):
		print(f"\n===== cfg info =====")
		print(f"设备: {self.device_name}")
		print(f"声道数: {self.num_channel}")
		print(f"采样率: {self.sample_rate} Hz")
		print(f"采样位数: {self.sample_bit} bit")
		print(f"比特率: {self.bit_rate} kbps")
		print(f"保存路径: {self.get_file_path()}")
		print("===================")


	def start_recording(self):
		"""开始录音（使用系统当前默认输入设备）"""

		try:
			if self.is_recording:
				print("录音已在进行中...")
				return False

			# 重置停止事件
			self.stop_event.clear()
			self.audio_queue = Queue()

			# 获取系统当前默认输入设备
			device_info = self.get_default_input_device()
			if not device_info:
				print("错误: 未找到默认输入设备")
				return False

			self.device_name = device_info['name']

			# 设置声道数
			device_max_channel = device_info['max_input_channels']
			self.num_channel = min(2, device_max_channel)

			# 设置采样率
			self.sample_rate = 16000  # int(device_info['default_samplerate'])

			# 设置采样位
			if 'formats' in device_info:
				if device_info['formats'] & sd.paInt32:
					self.sample_bit = 32
					self.dtype = "int32"
				elif device_info['formats'] & sd.paInt24:
					self.sample_bit = 24
					self.dtype = "int24"
				else:
					self.sample_bit = 16
					self.dtype = "int16"
			else:
				print("警告: 无法获取设备支持格式信息，使用16位")
				self.sample_bit = 16
				self.dtype = "int16"

			# 设置比特率
			self.bit_rate = 256  # 320

			# 确保目录存在
			os.makedirs(self.file_pos, exist_ok=True)

			# 打印设备信息
			self.print_cfg_info()

			# 创建输入流
			try:
				self.record_stream = sd.InputStream(
					samplerate=self.sample_rate,
					channels=self.num_channel,
					callback=self._audio_callback,
					dtype=self.dtype
				)
			except Exception as e:
				print(f"无法打开音频流: {e}")
				return False

			# 开始录音
			self.record_stream.start()
			self.is_recording = True
			return True

		except Exception as e:
			print(f"启动录音时发生错误: {e}")
			traceback.print_exc()
			return False


	def stop_recording(self):
		"""停止录音"""

		if not self.is_recording:
			print("没有正在进行的录音")
			return True

		try:
			# 停止录音流
			self.record_stream.stop()
			self.record_stream.close()
			self.stop_event.set()
			self.is_recording = False

			print("录音已停止")
			return True

		except Exception as e:
			print(f"停止录音时发生错误: {e}")
			traceback.print_exc()
			return False


	def save_recording(self):
		# 从队列中获取音频数据
		audio_data = []
		while not self.audio_queue.empty():
			audio_data.append(self.audio_queue.get())

		if not audio_data:
			print("没有录到音频数据")
			return False

		# 合并音频数据
		audio_data = np.vstack(audio_data)
		file_path = self.get_file_path()

		# 设置输出参数
		format_params = {}
		if self.file_extname in ('wav', 'flac'):
			# 设置PCM格式的采样位数
			subtype_map = {
				16: 'PCM_16',
				24: 'PCM_24',
				32: 'PCM_32'
			}
			format_params['subtype'] = subtype_map.get(self.sample_bit, 'PCM_16')
		elif self.file_extname in ('mp3', 'ogg', 'opus'):
			# 设置比特率（转换为bps）
			format_params['bitrate'] = f'{self.bit_rate}k'

		# 保存文件
		sf.write(
			file=file_path,
			data=audio_data,
			samplerate=self.sample_rate,
			**format_params
		)

		# 获取文件大小
		file_size = os.path.getsize(file_path) / 1024  # KB

		print(f"\n===== 录音已保存 =====")
		print(f"文件路径: {file_path}")
		print(f"采样率: {self.sample_rate} Hz")
		print(f"采样位数: {self.sample_bit} bit")
		print(f"比特率: {self.bit_rate} kbps")
		print(f"文件大小: {file_size:.2f} KB")
		print(f"持续时间: {len(audio_data) / self.sample_rate:.2f} s")
		print("=====================")

		return True


	def get_status(self):
		return self.is_recording


record = Record()


async def record_load_model():
	# global record
	# record = Record()
	print('record_model load success')
