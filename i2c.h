/**
 * \file        i2c.h
 * \date        04-Apr-2012
 * \author      Chris McHarg
 * \copyright   SmallHD
 * \brief       I2C interface
 */

#ifndef LIBSMALLHD_I2C_H
#define LIBSMALLHD_I2C_H

#include <linux/i2c.h>

#include "stdint.h"

struct i2c_handle {
	int fd;
	int addr;
	int flags;
};

struct i2c_handle *i2c_init(int adapter, int addr, int flags);
void i2c_uninit(struct i2c_handle *h);

int i2c_transfer(struct i2c_handle *h, struct i2c_msg *msgs, int num);
int i2c_master_send(struct i2c_handle *h, const char *buf, int count);
int i2c_master_recv(struct i2c_handle *h, char *buf , int count);

/* We also have versions to read/write multiple bytes from/to a remote register. */
int i2c_write_to_reg(
			struct i2c_handle *h,
			unsigned char reg_addr,
			const char *buf,
			int len);

int i2c_write_to_16reg(struct i2c_handle *h,
                       unsigned int reg_addr,
                       const void *buf,
                       int len);

int i2c_read_from_reg(
			struct i2c_handle *h,
			unsigned char reg_addr,
			char *buf,
			int len);


int i2c_read_from_16reg(struct i2c_handle *h,
                        unsigned int reg_addr,
                        void *buf,
                        int len);

int send_i2c16_seq(struct i2c_handle *i2c_h, int len, const uint32_t (*seq)[3]);

__s32 i2c_smbus_write_quick(struct i2c_handle *h, __u8 value);
__s32 i2c_smbus_read_byte(struct i2c_handle *h);
__s32 i2c_smbus_write_byte(struct i2c_handle *h, __u8 value);
__s32 i2c_smbus_read_byte_data(struct i2c_handle *h, __u8 command);
__s32 i2c_smbus_write_byte_data(struct i2c_handle *h, __u8 command, __u8 value);
__s32 i2c_smbus_read_word_data(struct i2c_handle *h, __u8 command);
__s32 i2c_smbus_write_word_data(struct i2c_handle *h, __u8 command, __u16 value);
__s32 i2c_smbus_process_call(struct i2c_handle *h, __u8 command, __u16 value);
__s32 i2c_smbus_read_block_data(struct i2c_handle *h, __u8 command,
        __u8 *values);
__s32 i2c_smbus_write_block_data(struct i2c_handle *h, __u8 command,
        __u8 length, const __u8 *values);
__s32 i2c_smbus_read_i2c_block_data(struct i2c_handle *h, __u8 command,
        __u8 length, __u8 *values);
__s32 i2c_smbus_write_i2c_block_data(struct i2c_handle *h, __u8 command,
        __u8 length, const __u8 *values);
__s32 i2c_smbus_block_process_call(struct i2c_handle *h, __u8 command,
        __u8 length, __u8 *values);


#endif

