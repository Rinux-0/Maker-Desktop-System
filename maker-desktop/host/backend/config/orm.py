CFG_SQLITE = {
	'engine': 'tortoise.backends.sqlite',
	'credentials': {
		'file_path': 'database/back.db'
	}
}

cfg_tortoise_orm = {
	'connections': {    # db_configs
		'sqlite': CFG_SQLITE
	},
	'apps': {
		'backend': {    # app name
			'models': ['model', 'aerich.models'],    # db model path
			'default_connection': 'sqlite'  # default db_config
		}
	},
	'use_tz': False,
	'timezone': 'Asia/Shanghai'
}