#include "dsp.h"
#include "arm_math.h"
#include "main.h"
#include <inttypes.h>
#include <stdio.h>

#define INPUT_SIZE 8150
#define FS         16300
#define CLOCK_FREQUENCY_MHZ 80  // Assume 80 MHz clock frequency

#define RMS_REF 310000 
#define THREASHOLD 40

// Calculated considering 9.000.000 as 30 dB using a rough calibration. ref = 9.000.000 / (10 ^ (30/20))

int32_t mic_buffer[INPUT_SIZE] = {0};

void task(void) {

    printf("\r\n\r\n\r\n\r\n");

    // ==== Acquire microphone data: ===========================================

    mic_dma_finished_flag = 0;
    if (HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1_filter0, mic_buffer, INPUT_SIZE) != HAL_OK) {
      printf("Failed to start DFSDM!\r\n");
      Error_Handler();
    }
    while (!mic_dma_finished_flag) {
    }
    if (HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter0) != HAL_OK) {
      printf("Failed to stop ?? DFSDM!\r\n");
      Error_Handler();
    }

    // dump_waveform(mic_buffer, INPUT_SIZE);

    // === Find dominant frequency using CMSIS DSP: ============================

    float IN_Buffer[INPUT_SIZE] = {0};

    for (int i = 0; i < INPUT_SIZE; i++) {
      IN_Buffer[i] = (float)mic_buffer[i];
    }

    software_rms_measure(IN_Buffer, INPUT_SIZE);
    arm_rms_measure(IN_Buffer, INPUT_SIZE);

    printf("\n\rMeasuring finished \n\r\n\r");

}

float32_t dB_value(float32_t rms) {
  return 20*log10f(rms / RMS_REF);
}

void software_rms_measure(float32_t* data, int len) {

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    float32_t rms_value;
    float32_t sum = 0;

    uint32_t start_cyc = DWT->CYCCNT;

    for(int i = 0; i < len; i++) sum += (float)(data[i]*data[i]);
    rms_value = sqrt(sum / len);
    float32_t dB_level = dB_value(rms_value);

    uint32_t stop_cyc = DWT->CYCCNT;
    uint32_t cyc_count = stop_cyc - start_cyc;

    printf("Software noise calculation value: %f = %f dB \n\r", rms_value, dB_level);
    printf("Software noise calculation cycles: %10" PRIu32 " cycles \n\r", cyc_count);
    printf("Software noise calculation energy consumption: %10" PRIu32 " cycles \n\r", cyc_count);

    if (dB_level > THREASHOLD) HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET); else HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET);
}



void arm_rms_measure(float32_t* data, int len) {

  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  float32_t rms_value;

  uint32_t start_cyc = DWT->CYCCNT;
  arm_rms_f32(data, len, &rms_value);

  float32_t dB_level = dB_value(rms_value);

  uint32_t stop_cyc = DWT->CYCCNT;
  uint32_t cyc_count = stop_cyc - start_cyc;

  printf("ARM DSP noise calculation value: %f  = %f dB\n\r", rms_value, dB_level);
  printf("ARM DSP noise calculation cycles: %10" PRIu32 " cycles\n\r", cyc_count);

  if (dB_level > THREASHOLD) HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, SET); else HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);

}



void dump_waveform(int32_t *buf, size_t len) {
  printf("\r\nWAVEFORM:");
  fflush(stdout);
  for (size_t i = 0; i < len; i++) {
    printf("%s%" PRIi32, i == 0 ? "" : ",", buf[i]);
    fflush(stdout);
  }
  printf("\r\n");
}

void dump_autocorr(float *buf, size_t len) {
  printf("\r\nAUTOCORRELATION:");
  fflush(stdout);
  for (size_t i = 0; i < len; i++) {
    printf("%s%f", i == 0 ? "" : ",", buf[i]);
    fflush(stdout);
  }
  printf("\r\n");
}
