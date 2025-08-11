from fastapi import APIRouter
from .core.record import record


sub_app_record = APIRouter()


@sub_app_record.post("/")
def record_start():

	if record.start_recording():
		print('正在录音...')
		return {'message': 'start recording...'}
	else:
		return {'message': 'has been recording...'}


@sub_app_record.put("/")
async def record_stop():

	if record.stop_recording():
		print('结束录音...')
		record.save_recording()
		return {'message': 'end recording'}
	else:
		return {'message': 'record has been ended'}


@sub_app_record.get("/")
async def record_status():
	return {'status': record.get_status()}
