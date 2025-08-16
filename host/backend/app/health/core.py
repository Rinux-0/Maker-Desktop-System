from model import Breath_DB, Heart_DB, Temperature_DB, Distance_DB


async def health_get_data_core(user_id: int, data_num: int, breath: bool = True, heart: bool = True, temperature: bool = True, distance: bool = True):
	data = dict()

	try:
		if breath:
			data['breath'] = await Breath_DB.filter(user=user_id).order_by('id').limit(data_num).values_list('breath')
		if heart:
			data['heart'] = await Heart_DB.filter(user=user_id).order_by('id').limit(data_num).values_list('heart')
		if temperature:
			data['temperature'] = await Temperature_DB.filter(user=user_id).order_by('id').limit(data_num).values_list('temperature')
		if distance:
			data['distance'] = await Distance_DB.filter(user=user_id).order_by('id').limit(data_num).values_list('distance')
	except Exception as e:
		return {'error': str(e)}

	return {
		'user_id': user_id,
		'data_num': data_num,
		'data': data
	}
