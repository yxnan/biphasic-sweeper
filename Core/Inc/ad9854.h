/**
  ******************************************************************************
  * @file    ad9854.h
  * @author  yxnan <yxnan@pm.me>
  * @date    2021-06-04
  * @brief   driver for ad9854
  ******************************************************************************
  */

#ifndef __AD9854_H
#define __AD9854_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "gpio_wrapper.h"
#include "spi.h"
#include "utils.h"

#define ad9854_dev         hspi3
#define ad9854_spi_timeout 100000

/* keep this synchronized with definitions in main.h */
DEF_GPIO(ad9854_pin_rst,  AD9854_RST_GPIO_Port,  AD9854_RST_Pin);
DEF_GPIO(ad9854_pin_cs,   AD9854_CS_GPIO_Port,   AD9854_CS_Pin);
DEF_GPIO(ad9854_pin_sync, AD9854_SYNC_GPIO_Port, AD9854_SYNC_Pin);
DEF_GPIO(ad9854_pin_drdy, AD9854_DRDY_GPIO_Port, AD9854_DRDY_Pin);
#undef DEF_GPIO


typedef struct
{
  /* the physical serial address in the chip */
  const uint8_t addr;
  /* the current value stored locally for transmission */
  uint64_t value;
  /* number of bytes of the register */
  const uint8_t size;
} ad9854_register;

typedef struct
{
  ad9854_register* reg;
  uint8_t bits;
  uint8_t offset;
} ad9854_register_bit;

typedef struct
{
  ad9854_register par1;           // Phase Adjust Register 1
  ad9854_register par2;           // Phase Adjust Register 1
  ad9854_register ftw1;           // Frequency Tuning Word 1
  ad9854_register ftw2;           // Frequency Tuning Word 2
  ad9854_register dfw;            // Delta frequency word
  ad9854_register update_clk;     // Update clock
  ad9854_register ramp_rate_clk;  // Ramp rate clock
  ad9854_register cr;             // Control Register
  ad9854_register osk_i_mult;     // Output shaped keying I multiplier
  ad9854_register osk_q_mult;     // Output shaped keying Q multiplier
  ad9854_register osk_ramp_rate;  // Output shaped keying ramp rate
  ad9854_register qdac;           // QDAC
} ad9854_registers;


/* global struct with shadow registers of the real values which are
 * currently in the ADC */
extern ad9854_registers ad9854_regs;

#define DEF_REG_BIT(_name, _reg, _bits, _offset)                                \
  static const ad9854_register_bit ad9854_##_name = { .reg = &ad9854_regs._reg, \
                                                      .bits = _bits,            \
                                                      .offset = _offset }

DEF_REG_BIT(comp_pwd,      cr, 1, 28);
DEF_REG_BIT(qdac_pwd,      cr, 1, 26);
DEF_REG_BIT(dac_pwd,       cr, 1, 25);
DEF_REG_BIT(dig_pwd,       cr, 1, 24);
DEF_REG_BIT(pll_range,     cr, 1, 22);
DEF_REG_BIT(pll_bypass,    cr, 1, 21);
DEF_REG_BIT(pll_mult,      cr, 5, 16);
DEF_REG_BIT(clr_acc1,      cr, 1, 15);
DEF_REG_BIT(clr_acc2,      cr, 1, 14);
DEF_REG_BIT(triangle,      cr, 1, 13);
DEF_REG_BIT(src_qdac,      cr, 1, 12);
DEF_REG_BIT(mode,          cr, 3, 9);
DEF_REG_BIT(updclk,        cr, 1, 8);
DEF_REG_BIT(invsinc_byp,   cr, 1, 6);
DEF_REG_BIT(osk_en,        cr, 1, 5);
DEF_REG_BIT(osk_int,       cr, 1, 4);
DEF_REG_BIT(lsb_first,     cr, 1, 1);
DEF_REG_BIT(sdo_cr,        cr, 1, 0);

#undef DEF_REG_BIT

typedef enum {
  ad9854_cmd_nop     = 0x00,
  ad9854_cmd_wakeup  = 0x02,
  ad9854_cmd_pwrdwn  = 0x04,
  ad9854_cmd_reset   = 0x06,
  ad9854_cmd_start   = 0x08,
  ad9854_cmd_stop    = 0x0a,
  ad9854_cmd_syocal  = 0x16,
  ad9854_cmd_sygcal  = 0x17,
  ad9854_cmd_sfocal  = 0x19,
  ad9854_cmd_rdata   = 0x12,
  ad9854_cmd_rreg    = 0x20,
  ad9854_cmd_wreg    = 0x40,
} ad9854_cmd_t;

typedef enum {
  ad9854_chan_ain0   = 0x00,
  ad9854_chan_ain1   = 0x01,
  ad9854_chan_ain2   = 0x02,
  ad9854_chan_ain3   = 0x03,
  ad9854_chan_ain4   = 0x04,
  ad9854_chan_ain5   = 0x05,
  ad9854_chan_aincom = 0x0c,
  /* ADS124S08 only channels */
  ad9854_chan_ain6   = 0x06,
  ad9854_chan_ain7   = 0x07,
  ad9854_chan_ain8   = 0x08,
  ad9854_chan_ain9   = 0x09,
  ad9854_chan_ain10  = 0x0a,
  ad9854_chan_ain11  = 0x0b,
  /* For IDAC only */
  ad9854_idac_off    = 0x0f,
} ad9854_chan_t;

typedef enum {
  ad9854_conv_delay_x14    = 0x00,   // n * tMOD
  ad9854_conv_delay_x25    = 0x01,
  ad9854_conv_delay_x64    = 0x02,
  ad9854_conv_delay_x256   = 0x03,
  ad9854_conv_delay_x1024  = 0x04,
  ad9854_conv_delay_x2048  = 0x05,
  ad9854_conv_delay_x4096  = 0x06,
  ad9854_conv_delay_x1     = 0x07,
} ad9854_conv_delay_t;

typedef enum {
  ad9854_pga_gain_x1   = 0x00,
  ad9854_pga_gain_x2   = 0x01,
  ad9854_pga_gain_x4   = 0x02,
  ad9854_pga_gain_x8   = 0x03,
  ad9854_pga_gain_x16  = 0x04,
  ad9854_pga_gain_x32  = 0x05,
  ad9854_pga_gain_x64  = 0x06,
  ad9854_pga_gain_x128 = 0x07,
} ad9854_pga_gain_t;

typedef enum {
  ad9854_datarate_x2_5   = 0x00,
  ad9854_datarate_x5     = 0x01,
  ad9854_datarate_x10    = 0x02,
  ad9854_datarate_x16_6  = 0x03,
  ad9854_datarate_x20    = 0x04,
  ad9854_datarate_x50    = 0x05,
  ad9854_datarate_x60    = 0x06,
  ad9854_datarate_x100   = 0x07,
  ad9854_datarate_x200   = 0x08,
  ad9854_datarate_x400   = 0x09,
  ad9854_datarate_x800   = 0x0a,
  ad9854_datarate_x1000  = 0x0b,
  ad9854_datarate_x2000  = 0x0c,
  ad9854_datarate_x4000  = 0x0d,
} ad9854_datarate_t;

typedef enum {
  ad9854_refmon_disabled = 0x00,
  ad9854_refmon_l0       = 0x01,
  ad9854_refmon_l0_l1    = 0x02,
  ad9854_refmon_l0_10m   = 0x03,
} ad9854_refmon_t;

typedef enum {
  ad9854_refsel_p0n0     = 0x00,
  ad9854_refsel_p1n1     = 0x01,
  ad9854_refsel_internal = 0x02,
} ad9854_refsel_t;

typedef enum {
  ad9854_refcon_off        = 0x00,
  ad9854_refcon_pwrdwn     = 0x01,
  ad9854_refcon_always_on  = 0x02,
} ad9854_refcon_t;

typedef enum {
  ad9854_psw_open  = 0,
  ad9854_psw_close = 1,
} ad9854_psw_t;

typedef enum {
  ad9854_imag_off    = 0x00,
  ad9854_imag_10mu   = 0x01,
  ad9854_imag_50mu   = 0x02,
  ad9854_imag_100mu  = 0x03,
  ad9854_imag_250mu  = 0x04,
  ad9854_imag_500mu  = 0x05,
  ad9854_imag_750mu  = 0x06,
  ad9854_imag_1000mu = 0x07,
  ad9854_imag_1500mu = 0x08,
  ad9854_imag_2000mu = 0x09,
} ad9854_imag_t;

typedef enum {
  ad9854_vbias_div_2 = 0,
  ad9854_vbias_div_12 = 1,
} ad9854_vbias_t;

typedef enum {
  ad9854_sysmon_disabled       = 0x00,
  ad9854_sysmon_pga_short      = 0x01,
  ad9854_sysmon_temp_sensor    = 0x02,
  ad9854_sysmon_avdd_avss_4    = 0x03,
  ad9854_sysmon_dvdd_4         = 0x04,
  ad9854_sysmon_burnout_0_2mu  = 0x05,
  ad9854_sysmon_burnout_1mu    = 0x06,
  ad9854_sysmon_burnout_10mu   = 0x07,
} ad9854_sysmon_t;

typedef enum {
  ad9854_cal_sample_x1   = 0x00,
  ad9854_cal_sample_x4   = 0x01,
  ad9854_cal_sample_x8   = 0x02,
  ad9854_cal_sample_x16  = 0x03,
} ad9854_cal_sample_t;

typedef enum {
  ad9854_gpio_dir_out  = 0,
  ad9854_gpio_dir_in   = 1,
} ad9854_gpio_dir_t;

typedef enum {
  ad9854_gpio_conf_ain   = 0,
  ad9854_gpio_conf_gpio  = 1,
} ad9854_gpio_conf_t;


static INLINE void      ad9854_select();
static INLINE void      ad9854_unselect();
static INLINE void      ad9854_set_value(ad9854_register_bit field, uint8_t value);
static INLINE uint8_t   ad9854_get_value(ad9854_register_bit field);
static INLINE void      ad9854_read_reg(ad9854_register* reg);
static INLINE void      ad9854_write_reg(ad9854_register* reg, uint8_t byte);
static INLINE void      ad9854_send_cmd(uint8_t cmd);
static INLINE void      ad9854_update_reg(ad9854_register* reg);
static INLINE void      ad9854_update_matching_reg(ad9854_register_bit field);
static INLINE void      ad9854_performSystemOffsetCalibration();
static INLINE void      ad9854_performSystemGainCalibration();
static INLINE void      ad9854_performSelfOffsetCalibration();

void ad9854_init();
void ad9854_test();
void ad9854_reset();
void ad9854_read_conv_data(uint32_t *conv_data);

/**
 * reads multiple registers from device.
 *
 * @param reg   -- the first register to read
 * @param num   -- number of registers to read
 * @param data  -- buffer for received bytes,
                   should at least have a length of num
 *
 * @note: global struct ad9854_regs WILL be updated
 */
void ad9854_read_regs(ad9854_register* reg, uint8_t num);

void ad9854_write_regs(ad9854_register* reg, uint8_t num, uint8_t* data);


/** implementation starts here */
static INLINE void
ad9854_select()
{
  gpio_set_low(ad9854_pin_cs);
}

static INLINE void
ad9854_unselect()
{
  gpio_set_high(ad9854_pin_cs);
}

static INLINE void
ad9854_set_value(ad9854_register_bit field, uint8_t value)
{
  /* convert the numbers of bits in a mask with matching length */
  const uint8_t mask = ((uint8_t)1 << field.bits) - 1;
  /* clear affected bits */
  field.reg->value &= ~(mask << field.offset);
  /* set affected bits */
  field.reg->value |= ((value & mask) << field.offset);
}

static INLINE uint8_t
ad9854_get_value(ad9854_register_bit field)
{
  /* convert the numbers of bits in a mask with matching length */
  const uint8_t mask = ((uint8_t)1 << field.bits) - 1;
  return (field.reg->value >> field.offset) & mask;
}

static INLINE void
ad9854_read_reg(ad9854_register* reg)
{
  ad9854_read_regs(reg, 1);
}

static INLINE void
ad9854_write_reg(ad9854_register* reg, uint8_t byte)
{
  ad9854_write_regs(reg, 1, lit2addr(byte));
}

static INLINE void
ad9854_send_cmd(uint8_t cmd)
{
  HAL_SPI_Transmit(&ad9854_dev, lit2addr(cmd), 1, ad9854_spi_timeout);
}

static INLINE void
ad9854_update_reg(ad9854_register* reg)
{
  ad9854_write_reg(reg, reg->value);
}

static INLINE void
ad9854_update_matching_reg(ad9854_register_bit field)
{
  ad9854_update_reg(field.reg);
}

static INLINE void
ad9854_performSystemOffsetCalibration()
{
  ad9854_send_cmd(ad9854_cmd_syocal);
}

static INLINE void
ad9854_performSystemGainCalibration()
{
  ad9854_send_cmd(ad9854_cmd_sygcal);
}

static INLINE void
ad9854_performSelfOffsetCalibration()
{
  ad9854_send_cmd(ad9854_cmd_sfocal);
}

#ifdef  __cplusplus
}
#endif
#endif /* __AD9854_H */