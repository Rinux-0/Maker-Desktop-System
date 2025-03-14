#ifndef _KEYPAD_H_
#define _KEYPAD_H_



#include "../util/util_def.h"

void keypad_init(void);
void keypad_past_update(void);
void keypad_now_read(void);
bool is_diff_keypad_status(void);
const u8* get_keypad_now(void);



#endif /* _KEYPAD_H_ */
