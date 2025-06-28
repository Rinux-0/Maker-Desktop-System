#pragma once
#ifndef _TEST_DEF_H_
#define _TEST_DEF_H_



#if defined(CONFIG_TEST_KEYTEST)
#	define TEST_NAME keytest
#elif defined(CONFIG_TEST_TMPTEST)
#	define TEST_NAME tmptest
#endif



#endif // _TEST_DEF_H_
