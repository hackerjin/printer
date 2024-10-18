#include "main.h"
#include "device.h"

extern ADC_HandleTypeDef hadc1;
extern device_state g_device_state;

int freq = 2000;    //设置频率2000kHz
int channel = 0;    //通道号，取值0 ~ 15
int resolution = 8; //分辨率，取值0~20，占空比duty最大取值为2^resolution-1

uint32_t ADC_Value[2];


#define EPISON 1e-7

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin & GPIO_PIN_8)
	{
       
        set_device_paper_status(PAPER_STATUS_LACK);
	}
}




static long map(long x, long in_min, long in_max, long out_min, long out_max) {
    const long dividend = out_max - out_min;
    const long divisor = in_max - in_min;
    const long delta = x - in_min;
    if(divisor == 0){
        return -1; //AVR returns -1, SAM returns 0
    }
    return (delta * dividend + (divisor / 2)) / divisor + out_min;
}

static uint32_t adc_alg_handle(uint32_t *adc, int size) {
    uint32_t sum = 0;
    uint32_t min_val = adc[0];
    uint32_t max_val = adc[0];

    for (int i = 0; i < size; i++) {
        if (adc[i] < min_val) {
            min_val = adc[i];
        }
        else if (adc[i] > max_val) {
            max_val = adc[i];
        }
        sum += adc[i];
    }
    sum = sum - (min_val + max_val);
    uint32_t avg_val = sum / (size - 2);
    return avg_val;
}

static void get_sensors_data()
{
  
  uint32_t adc1[10];
  uint32_t adc2[10];

  for (int sample_ptr = 0; sample_ptr < 10; sample_ptr++)
  {
		HAL_ADC_Start(&hadc1);
		if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
		{
			 adc1[sample_ptr] = HAL_ADC_GetValue(&hadc1);
		}
		HAL_ADC_Start(&hadc1);
		if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
		{
			 adc2[sample_ptr] = HAL_ADC_GetValue(&hadc1);
		}
		HAL_ADC_Stop(&hadc1);
  }
  
  
  ADC_Value[0] = adc_alg_handle(adc1, 10);
  ADC_Value[1] = adc_alg_handle(adc2, 10);
	
  
}

static uint32_t get_bat_data()
{
    uint32_t data = 0;
    data = ADC_Value[0] * 3.3f / 4096;
    return data;
    
}


static void read_bat()
{
	g_device_state.battery = map(get_bat_data()*2,3300,4200,0,100);
    
    if(g_device_state.battery > 100)
        g_device_state.battery = 100;
    
}

static float calculate_temp(float Rt)
{
    float Rp = 30000; // 30k
    float T2 = 273.15 + 25;
    float Bx = 3950; // B值
    float Ka = 273.15;
    float temp = 0.0f;

    temp = 1 / (log(Rt / Rp) / Bx + 1 / T2) - Ka + 0.5;
    return temp;
}


static float get_temp_data()
{
    float temp = 0.0f;
    float Rt=0;
    float vol=0;
    //ADC转换为电压 vol=AD/4096*VCC
    vol=(float)ADC_Value[1]*3.3f/4096;
    //printf("ADC temperatrue analog value = %f\n",vol);
    //电压转换为阻值 原理图为10k 1%精度  vol/VCC=Rt/(R+Rt)  vol/3.3=Rt/(10000+Rt)
    Rt=(vol*10000)/(3.3-vol);
    //printf("ADC temperatrue Rt = %f\n",Rt);
    temp = calculate_temp(Rt);
    return temp;
}



static void read_temp()
{
    float temperatrue = get_temp_data();
   
    if(temperatrue >= EPISON){
        g_device_state.temperature = temperatrue;
    }else{
        g_device_state.temperature = 0;
    }
}




void read_sensors()
{
    get_sensors_data();
    read_bat();
    read_temp;
    
    
}
