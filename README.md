[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/iq6XCcqA)
# Ka Yi Cheng
# ECS643/ECS714
# Lab5: Using ADC for automatic calibration

This lab project is working on the automatic calibration of voltage values from VR1 and VR2, with the RGB LED transitioning between green, red, and blue based on the calibration process, and the shield LED behavior controlled by potentiometer values, ensuring smooth operation or signaling an error state if the calibration fails.

- Start with the RGB LED showing green
  
- Calibration Process:
  - Press B1:
    - The voltages of VR1 and VR2 are measured and saved as minimum values.
    - The RGB LED changes to red.
  - Press B1 Again:
    - The voltages of VR1 and VR2 are measured again and saved as maximum values.
      
- Validation:
  - If the maximum values are greater than their corresponding minimum values:
    - The system operates normally, controlling the shield LED based on the potentiometer values.
    - The potentiometer controls how long the shield LED stays on and off.
  - If the maximum does not exceed the minimum values:
    - The system enters an error state, where the RGB LED flashes blue with a 1-second period (0.5s on, 0.5s off).
