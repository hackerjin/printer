#ifndef _PRINTER_H
#define _PRINTER_H
#include <stdint.h>
#include "main.h"



//ÿ���ܵ���
#define TPH_DOTS_PER_LINE 384
//ÿ���ֽڳ��� 384/8
#define TPH_DI_LEN 48
//����ͨ����ӡ
#define ALL_STB_NUM 0xFF


void init_printer(void);

void set_heat_density(uint8_t density);

/**
 * @brief �����ӡ
 *
 * @param data
 * @param length ���ݳ��ȱ��������� 48*n
 */
void start_printing(uint8_t *data, uint32_t len);

/**
 * @brief �ɱ���д�ӡ
 * 
 */
 
 
void start_printing_by_queuebuf(void);

/**
 * @brief ��ͨ����ӡ
 *
 * @param stbnum
 * @param data
 * @param len
 */
void start_printing_by_onestb(uint8_t stbnum, uint8_t *data, uint32_t len);




#endif

