#pragma once
#ifndef _TMPTEST_DEF_H_
#define _TMPTEST_DEF_H_



#if defined(CONFIG_TEST_TMPTEST_RECEIVER)
#	define TEST_TMPTEST_NAME receiver
#elif defined(CONFIG_TEST_TMPTEST_MULTI_THREAD)
#	define TEST_TMPTEST_NAME multi_thread
#endif



#endif // _TMPTEST_DEF_H_
