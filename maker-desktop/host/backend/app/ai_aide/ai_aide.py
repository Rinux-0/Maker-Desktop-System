from fastapi import APIRouter
from .ai import sub_app_ai
from .asr import sub_app_asr
from .tts import sub_app_tts
from .record import sub_app_record


app_ai_aide = APIRouter()


app_ai_aide.include_router(sub_app_ai, prefix='/ai')
app_ai_aide.include_router(sub_app_asr, prefix='/asr')
app_ai_aide.include_router(sub_app_tts, prefix='/tts')
app_ai_aide.include_router(sub_app_record, prefix='/record')
