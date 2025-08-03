#include <drivers/drivers.h>
#include <math.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// REGISTER DEFINITIONS

// UC8151 Register addresses
#define UC8151_PSR 0x00      // Panel Setting Register
#define UC8151_PWR 0x01      // Power Setting
#define UC8151_POF 0x02      // Power Off
#define UC8151_PFS 0x03      // Power Off Sequence Setting
#define UC8151_PON 0x04      // Power On
#define UC8151_PMES 0x05     // Power On Measure
#define UC8151_BTST 0x06     // Booster Soft Start
#define UC8151_DSLP 0x07     // Deep Sleep
#define UC8151_DTM1 0x10     // Data Transmission Mode 1
#define UC8151_DSP 0x11      // Data Stop
#define UC8151_DRF 0x12      // Display Refresh
#define UC8151_DTM2 0x13     // Data Transmission Mode 2
#define UC8151_LUT_VCOM 0x20 // VCOM LUT
#define UC8151_LUT_WW 0x21   // White-to-White LUT
#define UC8151_LUT_BW 0x22   // Black-to-White LUT
#define UC8151_LUT_WB 0x23   // White-to-Black LUT
#define UC8151_LUT_BB 0x24   // Black-to-Black LUT
#define UC8151_PLL 0x30      // PLL Control
#define UC8151_TSC 0x40      // Temperature Sensor Command
#define UC8151_TSE 0x41      // Temperature Sensor Enable
#define UC8151_TSR 0x43      // Temperature Sensor Read
#define UC8151_TSW 0x42      // Temperature Sensor Write
#define UC8151_CDI 0x50      // VCOM and Data Interval Setting
#define UC8151_LPD 0x51      // Low Power Detection
#define UC8151_TCON 0x60     // TCON Setting
#define UC8151_TRES 0x61     // Resolution Setting
#define UC8151_REV 0x70      // Revision
#define UC8151_FLG 0x71      // Get Status
#define UC8151_AMV 0x80      // Auto Measure VCOM
#define UC8151_VV 0x81       // VCOM Value
#define UC8151_VDCS 0x82     // VDC Setting
#define UC8151_PTL 0x90      // Partial Window
#define UC8151_PTIN 0x91     // Partial In
#define UC8151_PTOU 0x92     // Partial Out
#define UC8151_PGM 0xa0      // Program Mode
#define UC8151_APG 0xa1      // Active Program
#define UC8151_ROTP 0xa2     // Read OTP Data
#define UC8151_CCSET 0xe0    // Cascade Setting
#define UC8151_PWS 0xe3      // Power Saving
#define UC8151_TSSET 0xe5    // Temperature Sensor Setting

// PSR (Panel Setting Register) bits
#define UC8151_PSR_RES_96x230 0b00000000
#define UC8151_PSR_RES_96x252 0b01000000
#define UC8151_PSR_RES_128x296 0b10000000
#define UC8151_PSR_RES_160x296 0b11000000
#define UC8151_PSR_LUT_OTP 0b00000000
#define UC8151_PSR_LUT_REG 0b00100000
#define UC8151_PSR_FORMAT_BWR 0b00000000
#define UC8151_PSR_FORMAT_BW 0b00010000
#define UC8151_PSR_SCAN_DOWN 0b00000000
#define UC8151_PSR_SCAN_UP 0b00001000
#define UC8151_PSR_SHIFT_LEFT 0b00000000
#define UC8151_PSR_SHIFT_RIGHT 0b00000100
#define UC8151_PSR_BOOSTER_OFF 0b00000000
#define UC8151_PSR_BOOSTER_ON 0b00000010
#define UC8151_PSR_RESET_SOFT 0b00000000
#define UC8151_PSR_RESET_NONE 0b00000001

// PWR (Power Setting) bits
#define UC8151_PWR_VDS_EXTERNAL 0b00000000
#define UC8151_PWR_VDS_INTERNAL 0b00000010
#define UC8151_PWR_VDG_EXTERNAL 0b00000000
#define UC8151_PWR_VDG_INTERNAL 0b00000001
#define UC8151_PWR_VCOM_VD 0b00000000
#define UC8151_PWR_VCOM_VG 0b00000100
#define UC8151_PWR_VGHL_16V 0b00000000
#define UC8151_PWR_VGHL_15V 0b00000001
#define UC8151_PWR_VGHL_14V 0b00000010
#define UC8151_PWR_VGHL_13V 0b00000011

// BTST (Booster Soft Start) bits
#define UC8151_BTST_START_10MS 0b00000000
#define UC8151_BTST_START_20MS 0b01000000
#define UC8151_BTST_START_30MS 0b10000000
#define UC8151_BTST_START_40MS 0b11000000
#define UC8151_BTST_STRENGTH_1 0b00000000
#define UC8151_BTST_STRENGTH_2 0b00001000
#define UC8151_BTST_STRENGTH_3 0b00010000
#define UC8151_BTST_STRENGTH_4 0b00011000
#define UC8151_BTST_STRENGTH_5 0b00100000
#define UC8151_BTST_STRENGTH_6 0b00101000
#define UC8151_BTST_STRENGTH_7 0b00110000
#define UC8151_BTST_STRENGTH_8 0b00111000
#define UC8151_BTST_OFF_6_58US 0b00000111

// PFS (Power Off Sequence Setting) bits
#define UC8151_PFS_FRAMES_1 0b00000000

// TSE (Temperature Sensor Enable) bits
#define UC8151_TSE_TEMP_INTERNAL 0b00000000
#define UC8151_TSE_OFFSET_0 0b00000000

// PLL bits
#define UC8151_PLL_HZ_100 0b00111010
#define UC8151_PLL_HZ_200 0b00111001

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

/**
 * @brief Send a command to the UC8151 with optional data.
 */
static bool _driver_uc8151_send_command(driver_uc8151_t *uc8151, uint8_t cmd,
                                        const uint8_t *data, size_t data_len) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));

  // Set DC low for command
  hw_gpio_set(&uc8151->dc, false);

  // Send command byte (let SPI hardware handle CS)
  size_t bytes_sent = hw_spi_xfr(uc8151->spi, &cmd, 1, 0);
  if (bytes_sent != 1) {
    return false;
  }

  // Send data if provided
  if (data && data_len > 0) {
    // Set DC high for data
    hw_gpio_set(&uc8151->dc, true);
    bytes_sent = hw_spi_xfr(uc8151->spi, (void *)data, data_len, 0);
    if (bytes_sent != data_len) {
      return false;
    }
  }

  return true;
}

/**
 * @brief Send data to the UC8151 display.
 */
static bool _driver_uc8151_data(driver_uc8151_t *uc8151, const uint8_t *data,
                                size_t data_len) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));
  sys_assert(data && data_len > 0);

  // Set DC high for data
  hw_gpio_set(&uc8151->dc, true);

  // Send all data at once (let SPI hardware handle CS)
  size_t bytes_sent = hw_spi_xfr(uc8151->spi, (void *)data, data_len, 0);

  // Set DC back to low for subsequent commands
  hw_gpio_set(&uc8151->dc, false);

  return (bytes_sent == data_len);
}

/**
 * @brief Load default lookup tables for the UC8151.
 */
static void _driver_uc8151_default_luts(driver_uc8151_t *uc8151) {
  const uint8_t lut_vcom[] = {
      0x00, 0x64, 0x64, 0x37, 0x00, 0x01, 0x00, 0x8c, 0x8c, 0x00, 0x00,
      0x04, 0x00, 0x64, 0x64, 0x37, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_ww[] = {
      0x54, 0x64, 0x64, 0x37, 0x00, 0x01, 0x60, 0x8c, 0x8c, 0x00, 0x00,
      0x04, 0xa8, 0x64, 0x64, 0x37, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_bw[] = {
      0x54, 0x64, 0x64, 0x37, 0x00, 0x01, 0x60, 0x8c, 0x8c, 0x00, 0x00,
      0x04, 0xa8, 0x64, 0x64, 0x37, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_wb[] = {
      0xa8, 0x64, 0x64, 0x37, 0x00, 0x01, 0x60, 0x8c, 0x8c, 0x00, 0x00,
      0x04, 0x54, 0x64, 0x64, 0x37, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_bb[] = {
      0xa8, 0x64, 0x64, 0x37, 0x00, 0x01, 0x60, 0x8c, 0x8c, 0x00, 0x00,
      0x04, 0x54, 0x64, 0x64, 0x37, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  _driver_uc8151_send_command(uc8151, UC8151_LUT_VCOM, lut_vcom,
                              sizeof(lut_vcom));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_WW, lut_ww, sizeof(lut_ww));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_BW, lut_bw, sizeof(lut_bw));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_WB, lut_wb, sizeof(lut_wb));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_BB, lut_bb, sizeof(lut_bb));

  driver_uc8151_busy_wait(uc8151);
}

/**
 * @brief Load medium speed lookup tables.
 */
static void _driver_uc8151_medium_luts(driver_uc8151_t *uc8151) {
  const uint8_t lut_vcom[] = {
      0x00, 0x16, 0x16, 0x0d, 0x00, 0x01, 0x00, 0x23, 0x23, 0x00, 0x00,
      0x02, 0x00, 0x16, 0x16, 0x0d, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_ww[] = {
      0x54, 0x16, 0x16, 0x0d, 0x00, 0x01, 0x60, 0x23, 0x23, 0x00, 0x00,
      0x02, 0xa8, 0x16, 0x16, 0x0d, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_bw[] = {
      0x54, 0x16, 0x16, 0x0d, 0x00, 0x01, 0x60, 0x23, 0x23, 0x00, 0x00,
      0x02, 0xa8, 0x16, 0x16, 0x0d, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_wb[] = {
      0xa8, 0x16, 0x16, 0x0d, 0x00, 0x01, 0x60, 0x23, 0x23, 0x00, 0x00,
      0x02, 0x54, 0x16, 0x16, 0x0d, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_bb[] = {
      0xa8, 0x16, 0x16, 0x0d, 0x00, 0x01, 0x60, 0x23, 0x23, 0x00, 0x00,
      0x02, 0x54, 0x16, 0x16, 0x0d, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  _driver_uc8151_send_command(uc8151, UC8151_LUT_VCOM, lut_vcom,
                              sizeof(lut_vcom));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_WW, lut_ww, sizeof(lut_ww));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_BW, lut_bw, sizeof(lut_bw));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_WB, lut_wb, sizeof(lut_wb));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_BB, lut_bb, sizeof(lut_bb));

  driver_uc8151_busy_wait(uc8151);
}

/**
 * @brief Load fast speed lookup tables.
 */
static void _driver_uc8151_fast_luts(driver_uc8151_t *uc8151) {
  const uint8_t lut_vcom[] = {
      0x00, 0x04, 0x04, 0x07, 0x00, 0x01, 0x00, 0x0c, 0x0c, 0x00, 0x00,
      0x02, 0x00, 0x04, 0x04, 0x07, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_ww[] = {
      0x54, 0x04, 0x04, 0x07, 0x00, 0x01, 0x60, 0x0c, 0x0c, 0x00, 0x00,
      0x02, 0xa8, 0x04, 0x04, 0x07, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_bw[] = {
      0x54, 0x04, 0x04, 0x07, 0x00, 0x01, 0x60, 0x0c, 0x0c, 0x00, 0x00,
      0x02, 0xa8, 0x04, 0x04, 0x07, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_wb[] = {
      0xa8, 0x04, 0x04, 0x07, 0x00, 0x01, 0x60, 0x0c, 0x0c, 0x00, 0x00,
      0x02, 0x54, 0x04, 0x04, 0x07, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_bb[] = {
      0xa8, 0x04, 0x04, 0x07, 0x00, 0x01, 0x60, 0x0c, 0x0c, 0x00, 0x00,
      0x02, 0x54, 0x04, 0x04, 0x07, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  _driver_uc8151_send_command(uc8151, UC8151_LUT_VCOM, lut_vcom,
                              sizeof(lut_vcom));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_WW, lut_ww, sizeof(lut_ww));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_BW, lut_bw, sizeof(lut_bw));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_WB, lut_wb, sizeof(lut_wb));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_BB, lut_bb, sizeof(lut_bb));

  const uint8_t pll_setting = UC8151_PLL_HZ_200;
  _driver_uc8151_send_command(uc8151, UC8151_PLL, &pll_setting, 1);

  driver_uc8151_busy_wait(uc8151);
}

/**
 * @brief Load turbo speed lookup tables.
 */
static void _driver_uc8151_turbo_luts(driver_uc8151_t *uc8151) {
  const uint8_t lut_vcom[] = {
      0x00, 0x01, 0x01, 0x02, 0x00, 0x01, 0x00, 0x02, 0x02, 0x00, 0x00,
      0x02, 0x00, 0x02, 0x02, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_ww[] = {
      0x54, 0x01, 0x01, 0x02, 0x00, 0x01, 0x60, 0x02, 0x02, 0x00, 0x00,
      0x02, 0xa8, 0x02, 0x02, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_bw[] = {
      0x54, 0x01, 0x01, 0x02, 0x00, 0x01, 0x60, 0x02, 0x02, 0x00, 0x00,
      0x02, 0xa8, 0x02, 0x02, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_wb[] = {
      0xa8, 0x01, 0x01, 0x02, 0x00, 0x01, 0x60, 0x02, 0x02, 0x00, 0x00,
      0x02, 0x54, 0x02, 0x02, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const uint8_t lut_bb[] = {
      0xa8, 0x01, 0x01, 0x02, 0x00, 0x01, 0x60, 0x02, 0x02, 0x00, 0x00,
      0x02, 0x54, 0x02, 0x02, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  _driver_uc8151_send_command(uc8151, UC8151_LUT_VCOM, lut_vcom,
                              sizeof(lut_vcom));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_WW, lut_ww, sizeof(lut_ww));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_BW, lut_bw, sizeof(lut_bw));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_WB, lut_wb, sizeof(lut_wb));
  _driver_uc8151_send_command(uc8151, UC8151_LUT_BB, lut_bb, sizeof(lut_bb));

  const uint8_t pll_setting = UC8151_PLL_HZ_200;
  _driver_uc8151_send_command(uc8151, UC8151_PLL, &pll_setting, 1);

  driver_uc8151_busy_wait(uc8151);
}

/**
 * @brief Setup and configure the UC8151 display to exactly match original.
 */
static bool _driver_uc8151_setup(driver_uc8151_t *uc8151) {
  driver_uc8151_reset(uc8151);

  // PSR: RES_128x296 means 128 rows x 296 columns = 296 wide x 128 high display
  uint8_t psr_setting = UC8151_PSR_RES_128x296 | UC8151_PSR_FORMAT_BW |
                        UC8151_PSR_BOOSTER_ON | UC8151_PSR_RESET_NONE;

  psr_setting |= (uc8151->update_speed == DRIVER_UC8151_UPDATE_SPEED_DEFAULT)
                     ? UC8151_PSR_LUT_OTP
                     : UC8151_PSR_LUT_REG;

  psr_setting |= (uc8151->rotation == DRIVER_UC8151_ROTATION_180)
                     ? (UC8151_PSR_SHIFT_LEFT | UC8151_PSR_SCAN_UP)
                     : (UC8151_PSR_SHIFT_RIGHT | UC8151_PSR_SCAN_DOWN);

  if (!_driver_uc8151_send_command(uc8151, UC8151_PSR, &psr_setting, 1)) {
    return false;
  }

  // Load appropriate LUTs based on update speed
  switch (uc8151->update_speed) {
  case DRIVER_UC8151_UPDATE_SPEED_TURBO:
    _driver_uc8151_turbo_luts(uc8151);
    break;
  case DRIVER_UC8151_UPDATE_SPEED_FAST:
    _driver_uc8151_fast_luts(uc8151);
    break;
  case DRIVER_UC8151_UPDATE_SPEED_MEDIUM:
    _driver_uc8151_medium_luts(uc8151);
    break;
  case DRIVER_UC8151_UPDATE_SPEED_DEFAULT:
  default:
    _driver_uc8151_default_luts(uc8151);
    break;
  }

  const uint8_t pwr_data[] = {UC8151_PWR_VDS_INTERNAL | UC8151_PWR_VDG_INTERNAL,
                              UC8151_PWR_VCOM_VD | UC8151_PWR_VGHL_16V,
                              0b101011, 0b101011, 0b101011};
  if (!_driver_uc8151_send_command(uc8151, UC8151_PWR, pwr_data,
                                   sizeof(pwr_data))) {
    return false;
  }

  if (!_driver_uc8151_send_command(uc8151, UC8151_PON, NULL, 0)) {
    return false;
  }
  driver_uc8151_busy_wait(uc8151);

  const uint8_t btst_data[] = {
      UC8151_BTST_START_10MS | UC8151_BTST_STRENGTH_3 | UC8151_BTST_OFF_6_58US,
      UC8151_BTST_START_10MS | UC8151_BTST_STRENGTH_3 | UC8151_BTST_OFF_6_58US,
      UC8151_BTST_START_10MS | UC8151_BTST_STRENGTH_3 | UC8151_BTST_OFF_6_58US};
  if (!_driver_uc8151_send_command(uc8151, UC8151_BTST, btst_data,
                                   sizeof(btst_data))) {
    return false;
  }

  const uint8_t pfs_data = UC8151_PFS_FRAMES_1;
  if (!_driver_uc8151_send_command(uc8151, UC8151_PFS, &pfs_data, 1)) {
    return false;
  }

  const uint8_t tse_data = UC8151_TSE_TEMP_INTERNAL | UC8151_TSE_OFFSET_0;
  if (!_driver_uc8151_send_command(uc8151, UC8151_TSE, &tse_data, 1)) {
    return false;
  }

  const uint8_t tcon_data = 0x22;
  if (!_driver_uc8151_send_command(uc8151, UC8151_TCON, &tcon_data, 1)) {
    return false;
  }

  const uint8_t cdi_data = uc8151->inverted ? 0b10011100 : 0b01001100;
  if (!_driver_uc8151_send_command(uc8151, UC8151_CDI, &cdi_data, 1)) {
    return false;
  }

  const uint8_t pll_data = UC8151_PLL_HZ_100;
  if (!_driver_uc8151_send_command(uc8151, UC8151_PLL, &pll_data, 1)) {
    return false;
  }

  if (!_driver_uc8151_send_command(uc8151, UC8151_POF, NULL, 0)) {
    return false;
  }
  driver_uc8151_busy_wait(uc8151);

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a UC8151 display driver using the SPI interface.
 */
driver_uc8151_t driver_uc8151_init(hw_spi_t *spi, uint8_t dc_pin,
                                   uint8_t reset_pin, uint8_t busy_pin,
                                   uint16_t width, uint16_t height) {
  sys_assert(spi);
  sys_assert(hw_spi_valid(spi));
  sys_assert(width > 0 && height > 0);

  // Create the UC8151 driver structure
  driver_uc8151_t uc8151 = {0};

  // Initialize GPIO pins
  uc8151.dc = hw_gpio_init(dc_pin, HW_GPIO_OUTPUT);
  uc8151.reset = hw_gpio_init(reset_pin, HW_GPIO_OUTPUT);
  uc8151.busy = hw_gpio_init(busy_pin, HW_GPIO_INPUT);

  // Check if GPIO initialization was successful
  if (!hw_gpio_valid(&uc8151.dc)) {
    return uc8151; // Return invalid structure
  }

  if (!hw_gpio_valid(&uc8151.reset)) {
    return uc8151; // Return invalid structure
  }

  if (!hw_gpio_valid(&uc8151.busy)) {
    return uc8151; // Return invalid structure
  }

  // Set default GPIO states
  hw_gpio_set(&uc8151.reset, true); // Reset high (inactive)
  hw_gpio_set(&uc8151.dc, false);   // DC low initially

  // Initialize structure
  uc8151.spi = spi;
  uc8151.width = width;
  uc8151.height = height;
  uc8151.update_speed = DRIVER_UC8151_UPDATE_SPEED_DEFAULT;
  uc8151.rotation = DRIVER_UC8151_ROTATION_0;
  uc8151.inverted = false;
  uc8151.blocking = true;

  // Setup the display
  if (!_driver_uc8151_setup(&uc8151)) {
    // Clear the structure to indicate failure
    sys_memset(&uc8151, 0, sizeof(driver_uc8151_t));
  }

  return uc8151;
}

/**
 * @brief Finalize and release a UC8151 display driver.
 */
void driver_uc8151_finalize(driver_uc8151_t *uc8151) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));

  // Power off the display
  driver_uc8151_power_off(uc8151);

  // Finalize GPIO pins
  if (hw_gpio_valid(&uc8151->dc)) {
    hw_gpio_finalize(&uc8151->dc);
  }
  if (hw_gpio_valid(&uc8151->reset)) {
    hw_gpio_finalize(&uc8151->reset);
  }
  if (hw_gpio_valid(&uc8151->busy)) {
    hw_gpio_finalize(&uc8151->busy);
  }

  // Clear the driver structure
  sys_memset(uc8151, 0, sizeof(driver_uc8151_t));
}

///////////////////////////////////////////////////////////////////////////////
// CONFIGURATION

/**
 * @brief Set the update speed of the UC8151 display.
 */
bool driver_uc8151_set_update_speed(driver_uc8151_t *uc8151,
                                    driver_uc8151_update_speed_t speed) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));

  uc8151->update_speed = speed;
  return _driver_uc8151_setup(uc8151);
}

/**
 * @brief Set the rotation of the UC8151 display.
 */
bool driver_uc8151_set_rotation(driver_uc8151_t *uc8151,
                                driver_uc8151_rotation_t rotation) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));

  uc8151->rotation = rotation;
  return _driver_uc8151_setup(uc8151);
}

/**
 * @brief Set the color inversion of the UC8151 display.
 */
bool driver_uc8151_set_inverted(driver_uc8151_t *uc8151, bool inverted) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));

  uc8151->inverted = inverted;
  return _driver_uc8151_setup(uc8151);
}

/**
 * @brief Set the blocking mode of the UC8151 display.
 */
bool driver_uc8151_set_blocking(driver_uc8151_t *uc8151, bool blocking) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));

  uc8151->blocking = blocking;
  return true;
}

/**
 * @brief Get the estimated update time for the current speed setting.
 */
uint32_t driver_uc8151_get_update_time(driver_uc8151_t *uc8151) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));

  switch (uc8151->update_speed) {
  case DRIVER_UC8151_UPDATE_SPEED_DEFAULT:
    return 4500;
  case DRIVER_UC8151_UPDATE_SPEED_MEDIUM:
    return 2000;
  case DRIVER_UC8151_UPDATE_SPEED_FAST:
    return 800;
  case DRIVER_UC8151_UPDATE_SPEED_TURBO:
    return 250;
  default:
    return 4500;
  }
}

///////////////////////////////////////////////////////////////////////////////
// DISPLAY OPERATIONS

/**
 * @brief Check if the UC8151 display is busy.
 */
bool driver_uc8151_is_busy(driver_uc8151_t *uc8151) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));

  if (!hw_gpio_valid(&uc8151->busy)) {
    return false;
  }
  bool busy_state = !hw_gpio_get(&uc8151->busy);
  return busy_state;
}

/**
 * @brief Wait for the UC8151 display to become ready.
 */
void driver_uc8151_busy_wait(driver_uc8151_t *uc8151) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));

  int wait_count = 0;
  while (driver_uc8151_is_busy(uc8151)) {
    sys_sleep(100); // Match original 100ms delay
    wait_count++;
    if (wait_count > 200) { // 20 second timeout (more generous)
      break;
    }
  }
}

/**
 * @brief Reset the UC8151 display.
 */
void driver_uc8151_reset(driver_uc8151_t *uc8151) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));

  if (!hw_gpio_valid(&uc8151->reset)) {
    return;
  }

  hw_gpio_set(&uc8151->reset, false);
  sys_sleep(200); // Match original 200ms
  hw_gpio_set(&uc8151->reset, true);
  sys_sleep(200); // Match original 200ms
  // Note: Original doesn't call WaitUntilIdle after reset
}

/**
 * @brief Turn off the UC8151 display.
 */
void driver_uc8151_power_off(driver_uc8151_t *uc8151) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));

  _driver_uc8151_send_command(uc8151, UC8151_POF, NULL, 0);
  driver_uc8151_busy_wait(uc8151);
}

/**
 * @brief Update the entire UC8151 display with new framebuffer data.
 */
bool driver_uc8151_update(driver_uc8151_t *uc8151, const void *data,
                          size_t size) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));
  sys_assert(data);

  size_t expected_size = (uc8151->width * uc8151->height) / 8;
  if (size != expected_size) {
    return false;
  }

  if (uc8151->blocking) {
    driver_uc8151_busy_wait(uc8151);
  }

  // Power on the display
  if (!_driver_uc8151_send_command(uc8151, UC8151_PON, NULL, 0)) {
    return false;
  }

  // Disable partial mode (important!)
  if (!_driver_uc8151_send_command(uc8151, UC8151_PTOU, NULL, 0)) {
    return false;
  }

  // Send DTM2 command and framebuffer data (like C++ version)
  if (!_driver_uc8151_send_command(uc8151, UC8151_DTM2, NULL, 0)) {
    return false;
  }
  if (!_driver_uc8151_data(uc8151, data, size)) {
    return false;
  }

  // Data stop
  if (!_driver_uc8151_send_command(uc8151, UC8151_DSP, NULL, 0)) {
    return false;
  }

  // Send display refresh command
  if (!_driver_uc8151_send_command(uc8151, UC8151_DRF, NULL, 0)) {
    return false;
  }

  // Wait until idle
  if (uc8151->blocking) {
    driver_uc8151_busy_wait(uc8151);
  }

  return true;
}

/**
 * @brief Update a partial region of the UC8151 display.
 */
bool driver_uc8151_partial_update(driver_uc8151_t *uc8151, const uint8_t *data,
                                  size_t size, driver_uc8151_rect_t rect) {
  sys_assert(uc8151 && driver_uc8151_valid(uc8151));
  sys_assert(data);

  // Validate framebuffer size
  size_t expected_size = (uc8151->width * uc8151->height) / 8;
  if (size != expected_size) {
    return false;
  }

  // Validate region bounds
  if (rect.x >= uc8151->width || rect.y >= uc8151->height ||
      rect.x + rect.width > uc8151->width ||
      rect.y + rect.height > uc8151->height) {
    return false;
  }

  // Ensure height is aligned to byte boundaries (multiple of 8 pixels)
  if (rect.height % 8 != 0) {
    return false; // For simplicity, require 8-pixel aligned heights
  }

  int cols = rect.height / 8;
  int y1 = rect.y / 8;
  int rows = rect.width;
  int x1 = rect.x;

  // Wait until idle before starting partial update
  if (uc8151->blocking) {
    driver_uc8151_busy_wait(uc8151);
  }

  // Turn on
  if (!_driver_uc8151_send_command(uc8151, UC8151_PON, NULL, 0)) {
    return false;
  }

  // Enable partial mode
  if (!_driver_uc8151_send_command(uc8151, UC8151_PTIN, NULL, 0)) {
    return false;
  }

  // Set partial window
  uint8_t partial_window[7] = {
      (uint8_t)(rect.y),
      (uint8_t)(rect.y + rect.height - 1),
      (uint8_t)(rect.x >> 8),
      (uint8_t)(rect.x & 0xff),
      (uint8_t)((rect.x + rect.width - 1) >> 8),
      (uint8_t)((rect.x + rect.width - 1) & 0xff),
      0b00000001 // PT_SCAN
  };
  if (!_driver_uc8151_send_command(uc8151, UC8151_PTL, partial_window,
                                   sizeof(partial_window))) {
    return false;
  }

  // Start data transmission
  if (!_driver_uc8151_send_command(uc8151, UC8151_DTM2, NULL, 0)) {
    return false;
  }

  // Send partial framebuffer data using improved pointer arithmetic
  // UC8151 framebuffer layout: each column is (height/8) bytes, columns are
  // consecutive
  const uint16_t bytes_per_column = uc8151->height / 8;
  const uint16_t start_byte_in_column = y1;

  // Send data column by column for the partial region
  for (int dx = 0; dx < rows; dx++) {
    int col = dx + x1;

    // Calculate pointer to the start of data for this column within the partial
    // region
    const uint8_t *col_data =
        data + (col * bytes_per_column) + start_byte_in_column;

    // Send the partial column data
    if (!_driver_uc8151_data(uc8151, col_data, cols)) {
      return false;
    }
  }

  // Data stop
  if (!_driver_uc8151_send_command(uc8151, UC8151_DSP, NULL, 0)) {
    return false;
  }

  // Start display refresh
  if (!_driver_uc8151_send_command(uc8151, UC8151_DRF, NULL, 0)) {
    return false;
  }

  if (uc8151->blocking) {
    driver_uc8151_busy_wait(uc8151);
  }

  return true;
}
