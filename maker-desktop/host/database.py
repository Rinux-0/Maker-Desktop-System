import sqlite3
from datetime import datetime

class UserDatabase:
    def __init__(self, db_name='health_monitor.db'):
        self.conn = sqlite3.connect(db_name, check_same_thread=False)  # 启用线程安全模式
        self.create_tables()
    
    def create_tables(self):
        cursor = self.conn.cursor()
        # 用户表
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id TEXT UNIQUE,
                nfc_id TEXT,
                fingerprint_id TEXT,
                name TEXT,
                age INTEGER,
                gender TEXT,
                last_seen TIMESTAMP
            )
        ''')
        # 健康数据表
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS health_data (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id TEXT,
                timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                heart_rate REAL,
                breath_rate REAL,
                temperature REAL,
                distance REAL,
                FOREIGN KEY(user_id) REFERENCES users(user_id)
            )
        ''')
        # 检查并升级旧表（如果没有breath_rate字段）
        cursor.execute("PRAGMA table_info(health_data)")
        columns = [row[1] for row in cursor.fetchall()]
        if "breath_rate" not in columns:
            cursor.execute("ALTER TABLE health_data ADD COLUMN breath_rate REAL")
        self.conn.commit()
    
    def get_health_data(self, user_id, limit=1):
        cursor = self.conn.cursor()
        cursor.execute(
            '''
            SELECT 
                strftime('%Y-%m-%d %H:%M:%S', timestamp) as fmt_time,
                heart_rate, 
                breath_rate,
                temperature, 
                distance 
            FROM health_data 
            WHERE user_id = ? 
            ORDER BY timestamp DESC 
            LIMIT ?
            ''', 
            (user_id, limit))
        return cursor.fetchall() 
    
    def get_user_details(self, user_id):
        """获取用户的完整信息（包括年龄等）"""
        cursor = self.conn.cursor()
        cursor.execute('''
            SELECT name, age, gender 
            FROM users 
            WHERE user_id = ?
        ''', (user_id,))
        return cursor.fetchone()  
    
    def add_or_update_user(self, user_id, nfc_id, fingerprint_id):
        cursor = self.conn.cursor()
        cursor.execute('''
            INSERT OR REPLACE INTO users 
            (user_id, nfc_id, fingerprint_id, last_seen) 
            VALUES (?, ?, ?, ?)
        ''', (user_id, nfc_id, fingerprint_id, datetime.now()))
        self.conn.commit()
        return cursor.lastrowid
    
    def get_user(self, user_id):
        cursor = self.conn.cursor()
        cursor.execute('SELECT * FROM users WHERE user_id = ?', (user_id,))
        return cursor.fetchone()
    
    def get_all_users(self):
        cursor = self.conn.cursor()
        cursor.execute('SELECT user_id, name FROM users')
        return cursor.fetchall()
    
    def update_user_info(self, user_id, name=None, age=None, gender=None):
        cursor = self.conn.cursor()
        
        if name:
            cursor.execute('UPDATE users SET name = ? WHERE user_id = ?', (name, user_id))
        if age:
            cursor.execute('UPDATE users SET age = ? WHERE user_id = ?', (age, user_id))
        if gender:
            cursor.execute('UPDATE users SET gender = ? WHERE user_id = ?', (gender, user_id))
        
        self.conn.commit()
    
    def set_current_user(self, user_id):
        cursor = self.conn.cursor()
        cursor.execute('UPDATE users SET last_seen = ? WHERE user_id = ?', (datetime.now(), user_id))
        self.conn.commit()
    
    def save_health_data(self, user_id, heart_rate, breath_rate, temperature, distance):
        cursor = self.conn.cursor()
        cursor.execute('''
            INSERT INTO health_data (user_id, heart_rate, breath_rate, temperature, distance)
            VALUES (?, ?, ?, ?, ?)
        ''', (user_id, heart_rate, breath_rate, temperature, distance))
        self.conn.commit()
    
    def get_health_data(self, user_id, limit=50):
        cursor = self.conn.cursor()
        cursor.execute('''
            SELECT timestamp, heart_rate, breath_rate, temperature, distance 
            FROM health_data 
            WHERE user_id = ? 
            ORDER BY timestamp DESC 
            LIMIT ?
        ''', (user_id, limit))
        return cursor.fetchall()
    
    def close(self):
        self.conn.close()

# 单例数据库实例
user_db = UserDatabase()