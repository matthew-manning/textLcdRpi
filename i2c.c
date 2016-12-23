

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

//#include "common.h"
#include "i2c.h"


static int check_handle(struct i2c_handle *handle)
{
    if (handle == NULL || handle->fd < 1) {
        perror("i2c: invalid handle");
        return -1;
    }

    return 0;
}

int i2c_transfer(struct i2c_handle *h, struct i2c_msg *msgs, int num)
{
    struct i2c_rdwr_ioctl_data data = {
        .msgs = msgs,
        .nmsgs = num
    };
    int ret;

    if (check_handle(h) < 0)
        return -1;

    ret = ioctl(h->fd, I2C_RDWR, &data);
    if (ret < 0) {
        perror("i2c: RDWR failed");
        return -1;
    }

    return ret;
}

int i2c_master_send(struct i2c_handle *h, const char *buf, int count)
{
    int ret;
    struct i2c_msg msg;

    if (check_handle(h) < 0)
        return -1;

    msg.addr = h->addr;
    msg.flags = h->flags & I2C_M_TEN;
    msg.len = count;
    msg.buf = (unsigned char *)buf;

    ret = i2c_transfer(h, &msg, 1);

    return (ret == 1) ? count : -1;
}

int i2c_write_to_reg(
			struct i2c_handle *h,
			unsigned char reg_addr,
			const char *buf,
			int len)
{
    unsigned char *data;
    int count;
    int ret;

    data = malloc(len + 1);
    if (!data)
        return -1;

    data[0] = reg_addr;
    memcpy(data+1, buf, len);

    count = len + 1;

    ret = i2c_master_send(h, (char *)data, count);

    free(data);

    return (ret == count) ? 0 : -1;
}


/**
  * @brief Write to a 16bit I2C address a number of bytes
  *
  * This is in difference to most I2C devices which use a single byte
  * for the I2C register address. For such devices i2c_write_to_reg
  * should be used.
  *
  * The 16 bit address is sent out big endian
  *
  * @param h        handle obtained with i2c_init()
  * @param reg_addr 16bit register to access
  * @param buf      pointer to buffer containing data to send
  * @param len      number of bytes
  * @retval 0 if sucessful, -1 otherwise
  */
int i2c_write_to_16reg(struct i2c_handle *h,
                       unsigned int reg_addr,
                       const void *buf,
                       int len)
{
    unsigned char *data;
    int count;
    int ret;

    data = malloc(len + 2);
    if (!data)
        return -1;


    data[0] = (reg_addr>>8)&0xFF;
    data[1] =  reg_addr    &0xFF;
    memcpy(data+2, buf, len);

    count = len + 2;

    ret = i2c_master_send(h, (char *)data, count);

    free(data);

    return (ret == count) ? 0 : -1;
}


int i2c_master_recv(struct i2c_handle *h, char *buf , int count)
{
    struct i2c_msg msg;
    int ret;

    if (check_handle(h) < 0)
        return -1;

    msg.addr = h->addr;
    msg.flags = h->flags & I2C_M_TEN;
    msg.flags |= I2C_M_RD;
    msg.len = count;
    msg.buf = (unsigned char *)buf;

    ret = i2c_transfer(h, &msg, 1);

    return (ret == 1) ? count : -1;
}


int i2c_read_from_reg(
			struct i2c_handle *h,
			unsigned char reg_addr,
			char *buf,
			int len)
{
    struct i2c_msg msg[2];
    int ret;

    msg[0].addr = h->addr;
    msg[0].flags = h->flags & I2C_M_TEN;
    msg[0].len = 1;
    msg[0].buf = &reg_addr;

    msg[1].addr = h->addr;
    msg[1].flags = h->flags & I2C_M_TEN;
    msg[1].flags |= I2C_M_RD;
    msg[1].len = len;
    msg[1].buf = (unsigned char *)buf;

    ret = i2c_transfer(h, msg, 2);

    return (ret == 2) ? 0 : -1;
}


/**
  * @brief read from a 16bit I2C address a number of bytes
  *
  * This is in difference to most I2C devices which use a single byte
  * for the I2C register address. For such devices i2c_read_to_reg
  * should be used.
  *
  * The 16 bit address is sent out big endian
  *
  * @param h        handle obtained with i2c_init()
  * @param reg_addr 16bit register to access
  * @param buf      pointer to buffer containing data to send
  * @param len      number of bytes
  * @retval 0 if sucessful, -1 otherwise
  */
int i2c_read_from_16reg(struct i2c_handle *h,
                        unsigned int reg_addr,
                        void *buf,
                        int len)
{
    struct i2c_msg msg[2];
    int ret;
    reg_addr = ((reg_addr &0x00FF)<<8) |
               ((reg_addr &0xFF00)>>8);
    msg[0].addr  = h->addr;
    msg[0].flags = h->flags & I2C_M_TEN;
    msg[0].len   = 2;
    msg[0].buf   = (unsigned char*)&reg_addr;

    msg[1].addr   = h->addr;
    msg[1].flags  = h->flags & I2C_M_TEN;
    msg[1].flags |= I2C_M_RD;
    msg[1].len    = len;
    msg[1].buf    = (unsigned char *)buf;

    ret = i2c_transfer(h, msg, 2);

    return (ret == 2) ? 0 : -1;
}


/**
  * @brief Send a sequence of I2C operations utilising 16b registers
  *
  * Send a number of I2C commands to an i2c device that uses 16bit register
  * addresses. The list consists of an array of tupples. The first element of
  * each tupple is the data length (bits), next is register number, finally
  * the data (little endian - low byte sent first)
  *
  * @param i2c_h  handle for the I2C interface
  * @param len    Number of writes to perform
  * @param seq    pointer to an array describing the writes
  * @retval 0 if sucessful, -1 otherwise
  */
int send_i2c16_seq(struct i2c_handle *i2c_h, int len, const uint32_t (*seq)[3])
{
	int i;
	int retval=0;
	for(i=0; i<len; i++) {
		switch(seq[i][0]) {
			case  8:
			case 16:
			case 24:
			case 32:
				retval = i2c_write_to_16reg(i2c_h, seq[i][1], &(seq[i][2]), seq[i][0]/8);
				break;
			default: retval = -1;
		}
		if(retval!=0) return retval;
	}
	return 0;
}



#if 0
static __s32 i2c_smbus_access(struct i2c_handle *h, char read_write,
        __u8 command, int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data args;

    if (check_handle(h) < 0)
        return -1;

    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;

    return ioctl(h->fd, I2C_SMBUS, &args);
}

__s32 i2c_smbus_write_quick(struct i2c_handle *h, __u8 value)
{
    return i2c_smbus_access(h, value, 0, I2C_SMBUS_QUICK, NULL);
}

__s32 i2c_smbus_read_byte(struct i2c_handle *h)
{
    union i2c_smbus_data data;

    if (i2c_smbus_access(h, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data))
        return -1;
    else
        return 0x0FF & data.byte;
}

__s32 i2c_smbus_write_byte(struct i2c_handle *h, __u8 value)
{
    return i2c_smbus_access(h, I2C_SMBUS_WRITE, value, I2C_SMBUS_BYTE, NULL);
}

__s32 i2c_smbus_read_byte_data(struct i2c_handle *h, __u8 command)
{
    union i2c_smbus_data data;

    if (i2c_smbus_access(h, I2C_SMBUS_READ, command,
            I2C_SMBUS_BYTE_DATA, &data))
        return -1;
    else
        return 0x0FF & data.byte;
}

__s32 i2c_smbus_write_byte_data(struct i2c_handle *h, __u8 command, __u8 value)
{
    union i2c_smbus_data data;

    data.byte = value;

    return i2c_smbus_access(h, I2C_SMBUS_WRITE, command,
            I2C_SMBUS_BYTE_DATA, &data);
}

__s32 i2c_smbus_read_word_data(struct i2c_handle *h, __u8 command)
{
    union i2c_smbus_data data;

    if (i2c_smbus_access(h, I2C_SMBUS_READ, command,
            I2C_SMBUS_WORD_DATA, &data))
        return -1;
    else
        return 0x0FFFF & data.word;
}

__s32 i2c_smbus_write_word_data(struct i2c_handle *h, __u8 command, __u16 value)
{
    union i2c_smbus_data data;

    data.word = value;

    return i2c_smbus_access(h, I2C_SMBUS_WRITE, command,
            I2C_SMBUS_WORD_DATA, &data);
}

__s32 i2c_smbus_process_call(struct i2c_handle *h, __u8 command, __u16 value)
{
    union i2c_smbus_data data;

    data.word = value;

    if (i2c_smbus_access(h, I2C_SMBUS_WRITE, command,
            I2C_SMBUS_PROC_CALL, &data))
        return -1;
    else
        return 0x0FFFF & data.word;
}

__s32 i2c_smbus_read_block_data(struct i2c_handle *h, __u8 command,
        __u8 *values)
{
    union i2c_smbus_data data;
    int i;

    if (i2c_smbus_access(h, I2C_SMBUS_READ, command,
            I2C_SMBUS_BLOCK_DATA, &data))
        return -1;
    else {
        for (i = 1; i <= data.block[0]; i++)
            values[i-1] = data.block[i];
        return data.block[0];
    }
}

__s32 i2c_smbus_write_block_data(struct i2c_handle *h, __u8 command,
        __u8 length, const __u8 *values)
{
    union i2c_smbus_data data;
    int i;

    if (length > 32)
        length = 32;
    for (i = 1; i <= length; i++)
        data.block[i] = values[i-1];
    data.block[0] = length;

    return i2c_smbus_access(h, I2C_SMBUS_WRITE, command,
            I2C_SMBUS_BLOCK_DATA, &data);
}

__s32 i2c_smbus_read_i2c_block_data(struct i2c_handle *h, __u8 command,
        __u8 length, __u8 *values)
{
    union i2c_smbus_data data;
    int i;

    if (length > 32)
        length = 32;
    data.block[0] = length;
    if (i2c_smbus_access(h, I2C_SMBUS_READ, command,
                         length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN :
                          I2C_SMBUS_I2C_BLOCK_DATA, &data))
        return -1;
    else {
        for (i = 1; i <= data.block[0]; i++)
            values[i-1] = data.block[i];
        return data.block[0];
    }
}

__s32 i2c_smbus_write_i2c_block_data(struct i2c_handle *h, __u8 command,
        __u8 length, const __u8 *values)
{
    union i2c_smbus_data data;
    int i;

    if (length > 32)
        length = 32;
    for (i = 1; i <= length; i++)
        data.block[i] = values[i-1];
    data.block[0] = length;

    return i2c_smbus_access(h, I2C_SMBUS_WRITE, command,
                            I2C_SMBUS_I2C_BLOCK_BROKEN, &data);
}

__s32 i2c_smbus_block_process_call(struct i2c_handle *h, __u8 command,
        __u8 length, __u8 *values)
{
    union i2c_smbus_data data;
    int i;

    if (length > 32)
        length = 32;
    for (i = 1; i <= length; i++)
        data.block[i] = values[i-1];
    data.block[0] = length;

    if (i2c_smbus_access(h, I2C_SMBUS_WRITE, command,
                         I2C_SMBUS_BLOCK_PROC_CALL, &data))
        return -1;
    else {
        for (i = 1; i <= data.block[0]; i++)
            values[i-1] = data.block[i];
        return data.block[0];
    }
}
#endif

struct i2c_handle *i2c_init(int adapter, int addr, int flags)
{
    char dev_path[32];
    struct i2c_handle *handle;
    int fd;

    snprintf(dev_path, sizeof(dev_path)-1, "/dev/i2c-%d", adapter);
    fd = open(dev_path, O_RDWR);
    if (fd < 0) {
        perror("i2c: unable to open device");
        return NULL;
    }

    if (ioctl(fd, I2C_SLAVE, addr) < 0) {
        perror("i2c: slave selection failed");
        close(fd);
        return NULL;
    }

    handle = calloc(1, sizeof(struct i2c_handle));
    if (!handle) {
        perror("i2c: out of memory");
        close(fd);
        return NULL;
    }

    handle->fd = fd;
    handle->addr = addr;
    handle->flags = flags;

    return handle;
}

void i2c_uninit(struct i2c_handle *h)
{
    if (check_handle(h) < 0)
        return;

    close(h->fd);
    free(h);
}

