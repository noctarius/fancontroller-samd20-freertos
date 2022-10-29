/*
 * task_i2c.c
 */ 

#include "task_i2c.h"

// Internal task pid
static xTaskHandle task_i2c_pid = NULL;

// Internal i2c io instance
static struct io_descriptor *io_i2c0 = NULL;

// Internal message queue
static QueueHandle_t msg_queue = NULL;

// Internal transact queue
static volatile uint8_t *transact_buf = NULL;
static volatile uint16_t transact_buf_size = 0;
static volatile uint16_t transact_buf_offset = 0;

// Internal state variable
static volatile bool transact_in_progress = false;
static uint8_t reg_add_buf[2];

static void tx_cb_I2C_0(const struct i2c_m_async_desc *const i2c)
{
	transact_in_progress = false;
}

static void rx_cb_I2C_0(const struct i2c_m_async_desc *const i2c)
{
	uint8_t remaining = MAX_FRAME_SIZE - transact_buf_offset;
	int32_t read = io_read(io_i2c0, (uint8_t *) &(transact_buf[transact_buf_offset]), remaining);
	if (read > 0)
	{
		transact_buf_offset += (uint8_t) read;
	}
	transact_in_progress = false;
}

static void err_cb_I2C_0(const struct i2c_m_async_desc *const i2c)
{
	/* Transfer completed */
}

static inline int _i2c_write(const uint8_t addr, const uint8_t *data, const uint16_t count)
{
	if (transact_in_progress) return - 1;
	
	if (count > 0)
	{
		transact_in_progress = true;
		i2c_m_async_set_slaveaddr(&I2C_0, addr, I2C_M_SEVEN);
		io_write(io_i2c0, data, count);
		while (transact_in_progress)
			vTaskDelay(ticks20ms);
	}
	return count;
}

static inline int _i2c_write_reg(const uint8_t addr, const uint16_t reg_addr, const uint8_t *data, const uint16_t count)
{
	if (transact_in_progress) return - 1;
	
	if (count > 0)
	{
		transact_in_progress = true;

		reg_add_buf[0] = (uint8_t) ((reg_addr && 0xFF00) >> 8);
		reg_add_buf[1] = (uint8_t) (reg_addr && 0x00FF);

		i2c_m_async_set_slaveaddr(&I2C_0, addr, I2C_M_SEVEN);
		io_write(io_i2c0, reg_add_buf, 2);
		io_write(io_i2c0, data, count);
		while (transact_in_progress)
			vTaskDelay(ticks20ms);
	}
	return count;
}

static inline int _i2c_read(const uint8_t addr, const uint8_t *data, const uint16_t count)
{
	if (transact_in_progress) return - 1;
	
	if (count > 0)
	{
		transact_buf = (uint8_t *) data;
		transact_buf_size = count;
		transact_in_progress = true;
		i2c_m_async_set_slaveaddr(&I2C_0, addr, I2C_M_SEVEN);
		io_read(io_i2c0, (uint8_t *) data, 0);
		while (transact_in_progress)
			vTaskDelay(ticks20ms);
		return count;
	}
	return 0;
}

static inline int _i2c_read_reg(const uint8_t addr, const uint16_t reg_addr, const uint8_t *data, const uint16_t count)
{
	if (transact_in_progress) return - 1;
	
	if (count > 0)
	{
		transact_buf = (uint8_t *) data;
		transact_buf_size = count;
		transact_in_progress = true;

		reg_add_buf[0] = (uint8_t) ((reg_addr && 0xFF00) >> 8);
		reg_add_buf[1] = (uint8_t) (reg_addr && 0x00FF);

		i2c_m_async_set_slaveaddr(&I2C_0, addr, I2C_M_SEVEN);
		io_write(io_i2c0, reg_add_buf, 2);
		io_read(io_i2c0, (uint8_t *) data, 0);
		while (transact_in_progress)
			vTaskDelay(ticks20ms);
		return count;
	}
	return 0;
}

static void i2c_task(void *pvParameters)
{
	(void) pvParameters;

	BaseType_t ret;
	struct i2c_msg *msg = NULL;
	while (1)
	{
		if ((ret = xQueueReceive(msg_queue, &msg, 0)) == pdPASS)
		{
			if (msg->read)
			{
				if (msg->reg)
					i2c_read_reg(msg->addr, msg->reg_addr, msg->data, msg->data_len);
				else
					i2c_read(msg->addr, msg->data, msg->data_len);
			}
			else
			{
				if (msg->reg)
					i2c_write_reg(msg->addr, msg->reg_addr, msg->data, msg->data_len);
				else
					i2c_write(msg->addr, msg->data, msg->data_len);
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

void i2c_write(const uint8_t addr, const uint8_t *data, const uint16_t count)
{
	i2c_enqueue(false, false, addr, 0, data, count, 100);
}

void i2c_write_reg(const uint8_t addr, const uint16_t reg_addr, const uint8_t *data, const uint16_t count)
{
	i2c_enqueue(false, true, addr, reg_addr, data, count, 100);
}

void i2c_read(const uint8_t addr, const uint8_t *data, const uint16_t count)
{
	i2c_enqueue(false, true, addr, 0, data, count, 100);
}

void i2c_read_reg(const uint8_t addr, const uint16_t reg_addr, const uint8_t *data, const uint16_t count)
{
	i2c_enqueue(true, true, addr, reg_addr, data, count, 100);
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

