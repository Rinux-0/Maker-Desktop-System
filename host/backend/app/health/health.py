from fastapi import APIRouter
from model import User_DB, Breath_DB, Heart_DB, Temperature_DB, Distance_DB #, Health_IO
from typing import Union, Optional
from .core import health_get_data_core
from ..user import user_get_whoami


app_health = APIRouter()


@app_health.get('/id/{user_id}')
async def health_get_data_by_id(
		user_id: Optional[int] = None,
		data_num: int = 1,
		breath: bool = True,
		heart: bool = True,
		temperature: bool = True,
		distance: bool = True
):
	"""获取指定用户的最新指定健康数据"""

	if user_id is None:
		user_id = user_get_whoami()['data']

	if not await User_DB.filter(id=user_id):
		return {'error': '用户不存在'}

	return await health_get_data_core(
		user_id, data_num, breath, heart, temperature, distance
	)


@app_health.post('/name/{user_name}')
async def health_get_data_by_name(
		user_name: str,
		data_num: int = 1,
		breath: bool = True,
		heart: bool = True,
		temperature: bool = True,
		distance: bool = True
):
	"""获取指定用户的最新指定健康数据"""

	user = await User_DB.get_or_none(name=user_name)

	if not user:
		return {'error': '用户不存在'}

	return await health_get_data_core(
		user.id, data_num, breath, heart, temperature, distance
	)


@app_health.post('/')
async def health_add_data(data_type: str, data: Union[int, float], user_id: Optional[int] = None):
	"""向指定用户添加指定数据"""

	if user_id is None:
		user_id = user_get_whoami()['data']

	if not await User_DB.filter(id=user_id):
		return {'error': '用户不存在'}

	data_id_base = 100 * user_id

	if data_type == 'breath':
		# data_id_base = 100
		ref_db_model = Breath_DB
	elif data_type == 'heart':
		# data_id_base = 200
		ref_db_model = Heart_DB
	elif data_type == 'temperature':
		# data_id_base = 300
		ref_db_model = Temperature_DB
	elif data_type == 'distance':
		# data_id_base = 400
		ref_db_model = Distance_DB
	else:
		return {'error': f'不支持的数据类型: {data_type}'}

	try:
		# 获取所有data记录 id升序
		records_exit = await ref_db_model.filter(user=user_id).order_by('id').values('id', data_type, 'user_id')

		# 删除所有历史
		await ref_db_model.filter(user=user_id).delete()

		# 创建新数据 id=0
		data = {data_type: data}
		await ref_db_model.create(id=data_id_base, user_id=user_id, **data)

		# 最新的49条旧数据 id+1
		for record_exit in records_exit:
			record_exit['id'] += 1
			data = {data_type: record_exit[data_type]}
			await ref_db_model.create(id=record_exit['id'], user_id=record_exit['user_id'], **data)
			if record_exit['id'] == data_id_base + 49:
				break
	except Exception as e:
		return {'error': str(e)}

	return {'message': f'添加{data_type}数据: {data}'}


@app_health.delete('/')
async def health_delete_all(user_id: Optional[int] = None):
	"""删除指定用户的所有健康数据"""

	if user_id is None:
		user_id = user_get_whoami()['data']

	if not await User_DB.filter(id=user_id):
		return {'error': '用户不存在'}

	try:
		await Breath_DB.filter(user=user_id).delete()
		await Heart_DB.filter(user=user_id).delete()
		await Temperature_DB.filter(user=user_id).delete()
		await Distance_DB.filter(user=user_id).delete()
	except Exception as e:
		return {'error': str(e)}

	return {
		'message': f'删除用户[{user_id}]的所有健康数据'
	}
