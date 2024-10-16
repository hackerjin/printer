#ifndef _PRINTER_H
#define _PRINTER_H
#include <stdint.h>
#include "main.h"



//每行总点数
#define TPH_DOTS_PER_LINE 384
//每行字节长度 384/8
#define TPH_DI_LEN 48
//所有通道打印
#define ALL_STB_NUM 0xFF


void init_printer(void);

void set_heat_density(uint8_t density);

/**
 * @brief 数组打印
 *
 * @param data
 * @param length 数据长度必须是整行 48*n
 */
void start_printing(uint8_t *data, uint32_t len);

/**
 * @brief 可变队列打印
 * 
 */
 
 
void start_printing_by_queuebuf(void);

/**
 * @brief 单通道打印
 *
 * @param stbnum
 * @param data
 * @param len
 */
void start_printing_by_onestb(uint8_t stbnum, uint8_t *data, uint32_t len);




#endif

