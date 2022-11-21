/*
 * task_i2c.c
 */ 

#include "task_i2c.h"
#include <string.h>

// Internal task pid
static xTaskHandle task_i2c_pid = NULL;

// Internal i2c io instance
static struct io_descriptor *io_i2c0 = NULL;

// Internal message queue
static QueueHandle_t msg_queue = NULL;

// Internal transaction variables
/*static volatile struct ongoing_operation
{
	uint8_t *buf;
	uint16_t remaining;
	uint16_t offet;
	uint16_t addr;
	bool in_progress;
	bool write_before_read;
} ongoing_operation; */
static volatile bool transact_in_progress = false;
static volatile bool page_in_progress = false;
static volatile bool write_before_read_in_progress = false;

static uint16_t transact_buf_remaining = 0;
static uint16_t transact_buf_offset = 0;

static uint8_t page[MAX_FRAME_SIZE + 2]; // +2 => reg addr

static inline uint16_t write_page(const struct i2c_m_async_desc *const i2c, const uint16_t addr, uint16_t reg_addr, uint8_t *buf, uint16_t count)
{
	page_in_progress = true;
	
	uint8_t remaining = min(MAX_FRAME_SIZE, count);
	int32_t written = 0;
	if (remaining > 0)
	{
		page[0] = (uint8_t) ((transact_buf_offset & 0xFF00) >> 8);
		page[1] = (uint8_t) (transact_buf_offset & 0x00FF);
		written += 2;
		
		memcpy((uint8_t *) &page[2], buf, remaining);
		written += remaining;
	}

	return io_write(io_i2c0, page, written) - (remaining > 0 ? 2 : 0);
}

static inline uint16_t read_page(const struct i2c_m_async_desc *const i2c, const uint16_t addr, uint16_t reg_addr, uint8_t *buf, uint16_t count)
{
	if (page_in_progress) return -1;

	if (count > 0)
	{
		transact_in_progress = true;

		write_before_read_in_progress = true;		
		page[0] = (uint8_t) ((reg_addr & 0xFF00) >> 8);
		page[1] = (uint8_t) (reg_addr & 0x00FF);
		
		i2c_m_async_set_slaveaddr(&I2C_0, addr, I2C_M_SEVEN);
		io_write(io_i2c0, page, 2);
		while (write_before_read_in_progress)
		{
			vTaskDelay(ticks20ms);
		}

		page_in_progress = true;
		return io_read(io_i2c0, buf, min(MAX_FRAME_SIZE, count));
	}
	
	return 0;
}

static void tx_cb_I2C_0(const struct i2c_m_async_desc *const i2c)
{
	if (write_before_read_in_progress)
		write_before_read_in_progress = false;
	else if (page_in_progress)
		page_in_progress = false;
	else
		transact_in_progress = false;
}

static void rx_cb_I2C_0(const struct i2c_m_async_desc *const i2c)
{
	page_in_progress = false;
}

static void err_cb_I2C_0(const struct i2c_m_async_desc *const i2c)
{
	/* Transfer completed */
	transact_in_progress = false;
	page_in_progress = false;
	write_before_read_in_progress = false;
}

static inline int _i2c_write(const uint8_t addr, const uint8_t *data, const uint16_t count)
{
	if (transact_in_progress)
		return - 1;
	
	if (count <= 0)
		return 0;
	
	if (count > MAX_FRAME_SIZE)
		return -3;
	
	transact_in_progress = true;
		
	i2c_m_async_set_slaveaddr(&I2C_0, addr, I2C_M_SEVEN);
	int32_t written = io_write(io_i2c0, data, count);
	if (written > 0)
	{
		transact_buf_remaining -= (uint16_t) (written - 2);
		transact_buf_offset += (uint16_t) (written - 2);
	}

	while (transact_in_progress)
	{
		vTaskDelay(ticks20ms);
	}
	return count;
}

static inline int _i2c_write_reg(const uint8_t addr, const uint16_t reg_addr, const uint8_t *data, const uint16_t count)
{
	if (transact_in_progress)
		return - 1;
	
	if (count <= 0)
		return 0;
	
	transact_buf_offset = 0;
	transact_buf_remaining = count;
	while (transact_buf_remaining > 0)
	{
		int32_t written = write_page(&I2C_0, addr, reg_addr + transact_buf_offset, (uint8_t *) (data + transact_buf_offset), transact_buf_remaining);
		if (written < 0)
		{
			transact_in_progress = false;
			return -2;
		}
		
		while (page_in_progress)
		{
			vTaskDelay(ticks20ms);
		}
		
		transact_buf_remaining -= (uint16_t) written;
		transact_buf_offset += (uint16_t) written;
	}
	transact_in_progress = false;
	return transact_buf_offset;
}

static inline int _i2c_read(const uint8_t addr, const uint8_t *data, const uint16_t count)
{
	if (transact_in_progress)
		return - 1;
		
	if (count <= 0)
		return 0;
		
	if (count > MAX_FRAME_SIZE)
		return -3;
	
	transact_in_progress = true;

	uint8_t remaining = min(MAX_FRAME_SIZE, count);

	i2c_m_async_set_slaveaddr(&I2C_0, addr, I2C_M_SEVEN);
	int32_t read = io_read(io_i2c0, (uint8_t *) data, remaining);
	if (read < 0)
	{
		transact_in_progress = false;
		return -2;
	}

	while (transact_in_progress)
	{
		vTaskDelay(ticks20ms);
	}
	transact_in_progress = false;
	return read;
}

static inline int _i2c_read_reg(const uint8_t addr, const uint16_t reg_addr, const uint8_t *data, const uint16_t count)
{
	if (transact_in_progress)
		return - 1;
	
	if (count <= 0)
		return 0;
	
	transact_buf_offset = 0;
	transact_buf_remaining = count;
	while (transact_buf_remaining > 0)
	{
		int32_t read = read_page(&I2C_0, addr, reg_addr + transact_buf_offset, (uint8_t *) (data + transact_buf_offset), transact_buf_remaining);
		if (read < 0)
		{
			transact_in_progress = false;
			return -2;
		}
		
		while (page_in_progress)
		{
			vTaskDelay(ticks20ms);
		}
		
		transact_buf_remaining -= (uint16_t) read;
		transact_buf_offset += (uint16_t) read;
	}
	transact_in_progress = false;
	return transact_buf_offset;
}

static void i2c_task(void *pvParameters)
{
	(void) pvParameters;

	BaseType_t ret;
	struct i2c_msg *msg;
	while (1)
	{
		if ((ret = xQueueReceive(msg_queue, &msg, 0)) == pdPASS)
		{
			if (msg->read)
			{
				if (msg->reg)
					_i2c_read_reg(msg->addr, msg->reg_addr, msg->data, msg->data_len);
				else
					_i2c_read(msg->addr, msg->data, msg->data_len);
			}
			else
			{
				if (msg->reg)
					_i2c_write_reg(msg->addr, msg->reg_addr, msg->data, msg->data_len);
				else
					_i2c_write(msg->addr, msg->data, msg->data_len);
			}

			xTaskNotifyGive(msg->initiator);
		}

		vTaskDelay(ticks100ms);
	}
}

static int16_t i2c_enqueue(const bool read, const bool reg, const uint8_t addr, const uint16_t reg_addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis)
{
	// Store for notification
	xTaskHandle initiator = xTaskGetCurrentTaskHandle();
	
	struct i2c_msg *msg = &(struct i2c_msg)
	{
		.addr = addr,
		.reg_addr = reg_addr,
		.read = read,
		.reg = reg,
		.data = (uint8_t *) data,
		.data_len = count,
		.initiator = initiator
	};
	
	if (xQueueSend(msg_queue, &msg, ticks100ms) != pdPASS)
	{
		// Queue full
		return ERR_I2C_QUEUE_FULL;
	}

	// Wait for full response received
	uint32_t notificationValue = ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(timeout_millis - 100));
	if (notificationValue == 1)
	{
		return count;
	}
	
	// Timeout
	return ERR_TIMEOUT;
}

void i2c_write(const uint8_t addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis)
{
	i2c_enqueue(false, false, addr, 0, data, count, timeout_millis);
}

void i2c_write_reg(const uint8_t addr, const uint16_t reg_addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis)
{
	i2c_enqueue(false, true, addr, reg_addr, data, count, timeout_millis);
}

void i2c_read(const uint8_t addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis)
{
	i2c_enqueue(false, true, addr, 0, data, count, timeout_millis);
}

void i2c_read_reg(const uint8_t addr, const uint16_t reg_addr, const uint8_t *data, const uint16_t count, const uint16_t timeout_millis)
{
	i2c_enqueue(true, true, addr, reg_addr, data, count, timeout_millis);
}

BaseType_t create_i2c_task()
{
	if (task_i2c_pid != NULL) return pdFAIL;
	return xTaskCreate(i2c_task, "I2C", TASK_I2C_STACK_SIZE, NULL, 2, task_i2c_pid);
}

void init_i2c_task()
{
	msg_queue = xQueueCreate(2, sizeof(struct i2c_msg *));
	i2c_m_async_register_callback(&I2C_0, I2C_M_ASYNC_TX_COMPLETE, (FUNC_PTR) tx_cb_I2C_0);
	i2c_m_async_register_callback(&I2C_0, I2C_M_ASYNC_RX_COMPLETE, (FUNC_PTR) rx_cb_I2C_0);
	i2c_m_async_register_callback(&I2C_0, I2C_M_ASYNC_ERROR, (FUNC_PTR) err_cb_I2C_0);
	i2c_m_async_get_io_descriptor(&I2C_0, &io_i2c0);
	i2c_m_async_enable(&I2C_0);
}

void kill_i2c_task()
{
	if (task_i2c_pid == NULL) return;
	vTaskDelete(task_i2c_pid);
	task_i2c_pid = NULL;
}

