# IEEE-Robotics-2026

Embedded robotics platform for autonomous navigation and task execution
using STM32 microcontrollers, computer vision, and sensor fusion.

## Overview

This project implements a modular robotics stack built around a custom
embedded OS running on STM32 hardware.\
It integrates motor control, inertial sensing, and
vision to enable autonomous behaviors in a competition robot.

Key goals: - Deterministic real-time control - Sensor fusion--based
localization - Vision-assisted navigation - Modular embedded
architecture

------------------------------------------------------------------------

## System Architecture

    Robotics System
    ├── Embedded OS (scheduler, tasks, IPC)
    ├── Motor Control (PWM + encoders)
    ├── Sensors
    │   ├── IMU (BNO055)
    │   ├── ToF (VL53L1X)
    │   └── Wheel encoders
    └── Vision (OpenMV cameras)

------------------------------------------------------------------------

## Hardware

-   MCU: STM32F303RE (Nucleo)
-   Motors: DC
-   Motor driver: L298N
-   Encoders: quadrature
-   IMU: Bosch BNO055
-   ToF sensor: VL53L1X
-   Cameras: OpenMV M7 
-   Power: LiPo battery

------------------------------------------------------------------------

## Software

-   Language: C (bare-metal / minimal HAL)
-   Toolchain: arm-none-eabi-gcc
-   Debug: OpenOCD + GDB
-   OS: Custom lightweight scheduler
-   Communication: I2C, UART

------------------------------------------------------------------------

## Features

-   Preemptive embedded scheduler
-   Interrupt-driven UART + I2C drivers
-   Motor speed and heading control
-   Encoder-based odometry
-   IMU orientation tracking
-   Vision-assisted object / AprilTag detection
-   Modular task-based robotics framework

------------------------------------------------------------------------
