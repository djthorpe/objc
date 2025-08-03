# UC8151 E-Paper Display Wiring Diagram

## Raspberry Pi Pico to UC8151 E-Paper Display Connections

```
Raspberry Pi Pico                    UC8151 E-Paper Display
=====================================|======================================
                                     |
PIN 17 (GPIO 17) -------------------|--- CS    (Chip Select)
PIN 18 (GPIO 18) -------------------|--- CLK   (SPI Clock)
PIN 19 (GPIO 19) -------------------|--- MOSI  (SPI Data Out)
PIN 20 (GPIO 20) -------------------|--- DC    (Data/Command Select)
PIN 21 (GPIO 21) -------------------|--- RESET (Reset Pin)
PIN 26 (GPIO 26) -------------------|--- BUSY  (Busy Status)
3.3V OUT ---------------------------|--- VCC   (Power Supply)
GND ---------------------------------|--- GND   (Ground)
                                     |
```

## Pin Function Description

| Pin | Function | Direction | Description |
|-----|----------|-----------|-------------|
| CS  | Chip Select | Output | SPI slave select (active low) |
| CLK | Clock | Output | SPI clock signal |
| MOSI | Data Out | Output | SPI data from Pico to display |
| DC | Data/Command | Output | Command (low) or Data (high) select |
| RESET | Reset | Output | Hardware reset (active low) |
| BUSY | Busy Status | Input | Display busy indicator (low = busy) |
| VCC | Power | - | 3.3V power supply |
| GND | Ground | - | Common ground |

## Physical Layout (Top View of Pico)

```
                    Raspberry Pi Pico
                   ┌─────────────────┐
              GND ●│ 38         VBUS │● 
              GP2 ●│ 37          3V3 │● ← Connect to VCC
              GP3 ●│ 36       3V3_EN │● 
              GP4 ●│ 35         VSYS │● 
              GP5 ●│ 34          GND │● ← Connect to GND
              GND ●│ 33         VREF │● 
              GP6 ●│ 32         GP28 │● 
              GP7 ●│ 31         ADC2 │● 
              GP8 ●│ 30         ADC1 │● 
              GP9 ●│ 29         ADC0 │● 
             GP10 ●│ 28          RUN │● 
             GP11 ●│ 27         GP27 │● 
             GP12 ●│ 26         GP26 │● ← Connect to BUSY
             GP13 ●│ 25         GP25 │● 
              GND ●│ 24          GND │● 
             GP14 ●│ 23         GP24 │● 
             GP15 ●│ 22         GP23 │● 
             GP16 ●│ 21         GP22 │● 
             GP17 ●│ 20         GP21 │● ← Connect to RESET
             GP18 ●│ 19         GP20 │● ← Connect to DC
             GP19 ●│ 18         GP19 │● ← Connect to MOSI
              GND ●│ 17         GP18 │● ← Connect to CLK
                  └─────────────────┘
                       ↑
                  Connect to CS
```

## SPI Configuration

- **Interface**: SPI0
- **Mode**: Mode 0 (CPOL=0, CPHA=0)
- **Frequency**: 10 MHz maximum
- **Data Order**: MSB first
- **CS Active**: Low

## Power Requirements

- **Voltage**: 3.3V
- **Current**: ~20-40mA during updates, <1mA during standby
- **Peak Current**: Up to 100mA during display refresh

## Notes

1. **CS Pin**: The CS (Chip Select) pin must be configured as an output and is controlled by the SPI driver
2. **BUSY Pin**: This is an input pin that indicates when the display is updating (low = busy)
3. **DC Pin**: Data/Command select - low for commands, high for data
4. **RESET Pin**: Hardware reset pin - pulse low to reset the display
5. **Power**: Use the Pico's 3.3V output, not the 5V VBUS
6. **Ground**: Ensure solid ground connection for reliable SPI communication

## Troubleshooting

- **No display response**: Check power and ground connections first
- **Garbled display**: Verify SPI pin connections (CLK, MOSI, CS)
- **Incomplete updates**: Check BUSY pin connection
- **Random behavior**: Ensure RESET pin is properly connected
