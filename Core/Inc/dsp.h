/**
 * @file dsp.h
 * @brief DSP Exercise.
 * @author Philipp Schilk, 2023
 */
#ifndef DSP_H_
#define DSP_H_

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include "arm_math.h"


void task(void);

void software_measure(float32_t* data, int len);
void arm_dsp_measure(float32_t* data, int len);

float32_t dB_value(float32_t rms);

void dump_waveform(int32_t *buf, size_t len);
void dump_fft_mag(float *buf, size_t len, uint32_t max_idx, uint32_t fs);




#endif /* DSP_H_ */
