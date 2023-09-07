#pragma once

#include "png_structs.h"

/* высылает сообщение соответствующей ошибки в поток ошибок */
void panic(const char *err);

/* высылает сообщение ошибки, очищает структуру изображения и завершает выполнение программы */
void crash(const char *err, Image *img);