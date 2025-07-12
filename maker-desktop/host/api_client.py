# api_client.py

from openai import OpenAI  # 使用 DeepSeek API 的 openai 兼容接口
from config import DEEPSEEK_API_KEY  # 假设你有一个 DeepSeek 的 API Key

client = OpenAI(api_key=DEEPSEEK_API_KEY, base_url="https://api.deepseek.com")
def query_deepseek(raw_data, is_conversation=False, previous_messages=None):
    """
    查询DeepSeek的聊天模型，支持对话和一次性请求分析。
    :param raw_data: 接收到的原始数据
    :param is_conversation: 如果是对话，传入True，否则为一次性分析请求
    :param previous_messages: 对话历史消息（如果是对话模式）
    :return: DeepSeek的回复内容
    """
    if is_conversation:
        # 对话模式
        prompt = f"{raw_data}"
        messages = previous_messages + [
            {"role": "user", "content": prompt}
        ]
    else:
        # 一次性请求模式 - 添加医疗助理上下文
        prompt = (
            "你是一个专业的医疗健康助理。用户提供健康监测数据，"
            "包括心率、体温、血氧和与屏幕的距离。请分析数据并提供专业建议，"
            "特别注意：如果距离小于30厘米，提醒用户保持适当距离保护视力。"
            f"\n\n健康数据：{raw_data}"
        )
        messages = [
            {"role": "system", "content": "你是一个专业的医疗健康助理，擅长分析健康监测数据并提供实用建议。"},
            {"role": "user", "content": prompt}
        ]

    # 调用 DeepSeek API
    response = client.chat.completions.create(
        model="deepseek-chat",
        messages=messages,
        stream=False
    )
    
    return response.choices[0].message.content.strip()


   