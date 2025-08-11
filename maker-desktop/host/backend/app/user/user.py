from fastapi import APIRouter
from model import User_DB#, User_IO


app_user = APIRouter()


now_user_id = -1


@app_user.get('/whoami')
def user_get_whoami():
	return {'data': now_user_id}


@app_user.get('/')
async def user_get_list():
	"""获取用户列表"""

	try:
		users = await User_DB.all().values('id', 'name')
	except Exception as e:
		return {'error': str(e)}

	return {'data': users}


@app_user.get('/id/{user_id}')
async def user_get_data_by_id(user_id: int):
	"""获取用户详细信息 (通过user_id)"""

	try:
		user = await User_DB.get_or_none(id=user_id)
	except Exception as e:
		return {'error': str(e)}

	if not user:
		return {'error': '用户不存在'}
	return {'data': user}


@app_user.post('/name/{user_name}')
async def user_get_data_by_name(user_name: str):
	"""获取用户详细信息 (通过user_name)"""

	try:
		user = await User_DB.get_or_none(name=user_name)
	except Exception as e:
		return {'error': str(e)}

	if not user:
		return {'error': '用户不存在'}
	return {'data': user}


@app_user.post('/')
async def user_add(name: str, age: int, height: int, weight: int,blood:str, phone:str, intro:str):#user_detail: User_IO):
	"""添加新用户"""

	try:
		# 检查用户是否已存在
		user_exist = await User_DB.get_or_none(name=name)
		if user_exist:
			return {'error': f'已存在用户[{user_exist.id}]:{user_exist.name}'}

		# 查找最小可用的ID
		ids_exist = await User_DB.all().values_list('id', flat=True)
		ids_exist.sort()

		user_id = 0
		for id_exist in ids_exist:
			if user_id < id_exist:
				break
			user_id = id_exist + 1

		# 创建新用户
		await User_DB.create(
			id=user_id,
			name=name,
			age=age,
			height=height,
			weight=weight,
			blood=blood,
			phone=phone,
			intro=intro
		)
	except Exception as e:
		return {'error': str(e)}

	return {'message': f'添加用户[{user_id}]:{name}'}


@app_user.put('/id/{user_id}')
async def user_modify(user_id: int, name: str, age: int, height: int, weight: int,blood:str, phone:str, intro:str):#user_detail: User_IO):
	"""修改用户信息"""

	try:
		# 检查用户是否存在
		user_exist = await User_DB.get_or_none(id=user_id)
		if not user_exist:
			return {'error': '用户不存在'}

		# 更新用户信息
		await User_DB.filter(id=user_id).update(
			name=name,
			age=age,
			height=height,
			weight=weight,
			blood=blood,
			phone=phone,
			intro=intro
		)
	except Exception as e:
		return {'error': str(e)}

	return {'message': f'更新用户[{user_id}]:{name}'}


@app_user.delete('/id/{user_id}')
async def user_delete(user_id: int):
	"""删除用户"""

	try:
		# 检查用户是否存在
		user_exist = await User_DB.get_or_none(id=user_id)
		if not user_exist:
			return {'error': '用户不存在'}

		# 删除用户
		await User_DB.filter(id=user_id).delete()
	except Exception as e:
		return {'error': str(e)}

	return {'message': f'删除用户[{user_id}]:{user_exist.name}'}


@app_user.post('/whoami')
async def user_set_current(user_id: int):
	"""设置当前用户"""

	try:
		# 检查用户是否存在
		user_exist = await User_DB.get_or_none(id=user_id)
		if not user_exist:
			return {'error': '用户不存在'}
	except Exception as e:
		return {'error': str(e)}

	global now_user_id
	now_user_id = user_id

	print(f'当前用户ID: {now_user_id}')

	return {
		'message': f'修改当前用户成功',
		'data': now_user_id
	}
