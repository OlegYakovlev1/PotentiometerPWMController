# PWM Dual Controller

ESP32-S3 project using ESP-IDF where a single potentiometer controls:

- LED brightness using PWM
- DC motor speed using PWM

The LED and motor use separate PWM channels, allowing both devices to operate independently.

---

# Features

- ESP-IDF implementation
- PWM control using LEDC
- ADC reading using `adc_oneshot`
- Independent PWM channels for LED and motor
- Real-time brightness and speed control
- 12-bit ADC resolution
- 8-bit PWM resolution
- Non-blocking FreeRTOS loop

---

# Hardware Requirements

- ESP32-S3
- Potentiometer (~10 kΩ)
- LED
- 220 Ω resistor
- DC motor
- NPN transistor (ST2N)
- Base resistor (220)
- Flyback diode
- External motor power supply

---

# Pin Configuration

| Component | ESP32 Pin |
|---|---|
| Potentiometer OUT | GPIO1 |
| LED | GPIO16 |
| Motor PWM | GPIO17 |

---

# Wiring

## Potentiometer

| Potentiometer | ESP32 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| OUT | GPIO1 |

---

## LED

| ESP32 | Component |
|---|---|
| GPIO16 | 220 Ω resistor |
| Resistor | LED anode |
| LED cathode | GND |

---

## Motor Driver Circuit

| Component | Connection |
|---|---|
| GPIO17 | Base resistor |
| Base resistor | Transistor base |
| Transistor emitter | GND |
| Transistor collector | Motor negative |
| Motor positive | External power supply + |
| External power supply GND | ESP32 GND |

---

## Flyback Diode

| Diode Side | Connection |
|---|---|
| Cathode | Motor positive |
| Anode | Motor negative |

---

# PWM Configuration

| Parameter | Value |
|---|---|
| PWM Frequency | 1000 Hz |
| PWM Resolution | 8-bit |
| ADC Resolution | 12-bit |
| ADC Maximum Value | 4095 |
| PWM Maximum Duty | 255 |

---

# How It Works

1. The potentiometer value is read using the ESP-IDF ADC OneShot driver.
2. The ADC value is mapped from:
   - `0–4095`
   - to `0–255`
3. The mapped value is applied to:
   - LED PWM channel
   - Motor PWM channel
4. LED brightness and motor speed change simultaneously.

---

# Example Serial Output

```text
I (1234) PWM_CONTROLLER: ADC: 0 | PWM Duty: 0
I (1284) PWM_CONTROLLER: ADC: 1024 | PWM Duty: 63
I (1334) PWM_CONTROLLER: ADC: 2048 | PWM Duty: 127
I (1384) PWM_CONTROLLER: ADC: 3072 | PWM Duty: 191
I (1434) PWM_CONTROLLER: ADC: 4095 | PWM Duty: 255
```

---

# Important Notes

- The motor must NOT be connected directly to an ESP32 GPIO pin.
- A transistor or MOSFET is required for motor control.
- ESP32 GND and external power supply GND must be connected together.
- A flyback diode is required to protect the transistor and ESP32 from voltage spikes.

---

# Technologies Used

- ESP-IDF
- FreeRTOS
- LEDC PWM driver
- ADC OneShot driver

---

# Project Goal

The purpose of this project is to demonstrate:

- PWM generation on ESP32
- ADC input handling
- Independent PWM outputs
- Safe DC motor control using a transistor
- Embedded programming with ESP-IDF and FreeRTOS
