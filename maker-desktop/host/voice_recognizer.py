from PyQt5.QtCore import QThread, pyqtSignal
import os
import json
from time import sleep
import pyaudio
from vosk import Model, KaldiRecognizer

class VoiceRecognitionThread(QThread):
    """
    语音识别线程
    负责录音、音频数据收集、Vosk模型识别、资源管理。
    """
    recognition_result = pyqtSignal(str)  # 识别结果信号
    recognition_error = pyqtSignal(str)   # 错误信号
    recognition_finished = pyqtSignal()   # 完成信号
    recording_status = pyqtSignal(bool)   # 录音状态信号

    def __init__(self):
        super().__init__()
        self.is_running = False
        self.is_recording = False
        self.audio_stream = None
        self.pyaudio_instance = None
        self.recognizer = None
        self.audio_frames = []  # 存储录音数据
        self.vosk_model = None

    def start_recording(self):
        """开始录音"""
        if self.is_recording:
            return
        self.is_recording = True
        self.recording_status.emit(True)
        self.audio_frames = []
        # 初始化 Vosk 模型（如果尚未初始化）
        if self.vosk_model is None:
            try:
                current_dir = os.path.dirname(os.path.abspath(__file__))
                model_path = os.path.join(current_dir, "vosk-model-small-cn-0.22")
                if not os.path.exists(model_path):
                    raise Exception(f"模型路径不存在: {model_path}")
                self.vosk_model = Model(model_path)
            except Exception as e:
                self.recognition_error.emit(f"⚠️ 初始化 Vosk 模型失败: {str(e)}")
                self.is_recording = False
                self.recording_status.emit(False)
                return
        # 初始化音频流
        try:
            self.pyaudio_instance = pyaudio.PyAudio()
            self.audio_stream = self.pyaudio_instance.open(
                format=pyaudio.paInt16,
                channels=1,
                rate=16000,
                input=True,
                frames_per_buffer=4096
            )
            self.recognizer = KaldiRecognizer(self.vosk_model, 16000)
        except Exception as e:
            self.recognition_error.emit(f"⚠️ 音频设备初始化失败: {str(e)}")
            self.is_recording = False
            self.recording_status.emit(False)
            return

    def stop_recording(self):
        """停止录音并识别"""
        if not self.is_recording:
            return
        self.is_recording = False
        self.recording_status.emit(False)
        try:
            if self.audio_stream and self.recognizer:
                self.audio_stream.stop_stream()
                self.audio_stream.close()
                if self.audio_frames:
                    audio_data = b''.join(self.audio_frames)
                    chunk_size = 4096
                    for i in range(0, len(audio_data), chunk_size):
                        chunk = audio_data[i:i + chunk_size]
                        if len(chunk) == chunk_size:
                            self.recognizer.AcceptWaveform(chunk)
                    result = self.recognizer.Result()
                    result_dict = json.loads(result)
                    if 'text' in result_dict:
                        query = result_dict['text'].strip()
                        if query:
                            self.recognition_result.emit(query)
                        else:
                            self.recognition_error.emit("⚠️ 未识别到语音内容，请重试")
                    else:
                        self.recognition_error.emit("⚠️ 语音识别失败，请重试")
                else:
                    self.recognition_error.emit("⚠️ 没有录制到音频数据，请重试")
        except Exception as e:
            self.recognition_error.emit(f"⚠️ 语音识别失败: {str(e)}")
        finally:
            if self.audio_stream:
                try:
                    self.audio_stream.stop_stream()
                    self.audio_stream.close()
                except:
                    pass
            if self.pyaudio_instance:
                try:
                    self.pyaudio_instance.terminate()
                except:
                    pass
            self.audio_stream = None
            self.pyaudio_instance = None
            self.recognizer = None
            self.audio_frames = []
            self.recognition_finished.emit()

    def run(self):
        """线程运行 - 持续录音"""
        self.is_running = True
        while self.is_running:
            if self.is_recording and self.audio_stream:
                try:
                    data = self.audio_stream.read(4096, exception_on_overflow=False)
                    if data:
                        self.audio_frames.append(data)
                except Exception as e:
                    self.recognition_error.emit(f"⚠️ 录音过程中出错: {str(e)}")
                    break
            else:
                sleep(0.01)

    def stop(self):
        """停止线程"""
        self.is_running = False
        self.stop_recording() 