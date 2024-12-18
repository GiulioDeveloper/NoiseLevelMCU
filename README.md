# Embedded Systems Project @ ETHZ

## Project Description
The goal of the project is to implement a noise level analyzer using an STM32 MCU development board and its sensors. The system signals the user if a noise threshold has been exceeded using LEDs.

## Data Acquisition and Peripherals
- The system enters active mode and starts acquiring data from the onboard microphone.
- Data is sampled at **16.3 kHz** for a duration of **0.5 seconds** (8150 samples).
- The sampled data is converted and filtered by dedicated hardware, then transferred to memory via the DMA controller.
- After processing:
  - Two LEDs (one for each algorithm) indicate whether the threshold has been exceeded by turning on or off.
  - A timer starts counting and throws an interrupt after two seconds, putting the system into sleep mode.
  - When the timer finishes, the system wakes up and restarts the process.

## Processing, Performance, and Power
To estimate the noise level, the **root mean squared (RMS)** value of the signal is calculated, which is a measure of its intensity. The calculation compares two methods:
1. **Plain C function:** No further optimizations applied.
2. **ARM CMSIS-DSP `arm_rms_f32()`:** Optimized function provided by ARM.

### System Activity Summary
| Function/State | Cycles  | Frequency | Power/MHz | Execution Time | Power   | Energy  |
|----------------|---------|-----------|-----------|----------------|---------|---------|
| **C function** | ~51,000 | 80 MHz    | 1 mW      | ~640 ns        | 80 mW   | 51 µJ   |
| **ARM-DSP**    | ~43,000 | 80 MHz    | 1 mW      | ~540 ns        | 80 mW   | 43 µJ   |
| **Sleep**      | -       | -         | 10 µW     | 2 seconds      | -       | 20 µJ   |

### Observations
- The ARM-DSP function uses approximately **15% less energy** than the C function by leveraging architectural optimizations and SIMD instructions.
- Despite the time spent in sleep state being **1.7 million times longer**, the processing part consumes **4.7x more energy**.

## Conclusion
- Hardware-aware software and effective system state management are crucial for performance and energy efficiency.
- Further optimizations, such as using fixed-point data and algorithms, could reduce computation time further. However, this comes at the expense of design complexity and precision.

---

