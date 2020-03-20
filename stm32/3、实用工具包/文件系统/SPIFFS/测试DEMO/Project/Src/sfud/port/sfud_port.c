/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>
#include <stm32f1xx_hal.h>
#include <sfud_cfg.h>
#include <string.h>


static char log_buf[256];

typedef struct
{
	SPI_HandleTypeDef hspi;
}spi_device, *spi_device_t;

typedef struct {
    spi_device_t spix;
    GPIO_TypeDef *cs_gpiox;
    uint16_t cs_gpio_pin;
} spi_user_data, *spi_user_data_t;

void sfud_log_debug(const char *file, const long line, const char *format, ...);

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, 
					uint8_t *read_buf, size_t read_size) 
{
    sfud_err result = SFUD_SUCCESS;
//    uint8_t send_data, read_data;
    spi_user_data_t spi_dev = (spi_user_data_t) spi->user_data;

    if (write_size) {
        SFUD_ASSERT(write_buf);
    }
    if (read_size) {
        SFUD_ASSERT(read_buf);
    }

	HAL_GPIO_WritePin(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin, GPIO_PIN_RESET);
  
	if (write_size > 0) {
		/* send data */
		if (HAL_SPI_Transmit(&spi_dev->spix->hspi, (uint8_t *)write_buf, write_size, 100) != HAL_OK)
		{
			result = SFUD_ERR_WRITE;
			goto exit;
		}
	}
	
	if (read_size > 0) {
		if (HAL_SPI_Receive(&spi_dev->spix->hspi,  (uint8_t *)read_buf, read_size, 100) != HAL_OK)
		{
			result = SFUD_ERR_READ;
			goto exit;
		}
	}

exit:	
	HAL_GPIO_WritePin(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin, GPIO_PIN_SET);
	
    return result;
}


static void rcc_configuration(spi_user_data_t spi) {
	SPI_TypeDef *spi_type; 
	
	spi_type = spi->spix->hspi.Instance;
    if (spi_type == SPI1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_SPI1_CLK_ENABLE();
    } else if (spi_type == SPI2) {
        /* you can add SPI2 code here */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_SPI2_CLK_ENABLE();
    }
}

static void gpio_configuration(spi_user_data_t spi) {
    GPIO_InitTypeDef GPIO_InitStruct;
	SPI_TypeDef *spi_type; 
	
	spi_type = spi->spix->hspi.Instance;
    if (spi_type == SPI1) {
        /* SCK:PA5  MISO:PA6  MOSI:PA7 */
        GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        /* CS: PC4 */
        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
		
    } else if (spi_type == SPI2) {
			/**SPI2 GPIO Configuration    
			PB12     ------> SPI2_NSS
			PB13     ------> SPI2_SCK
			PB14     ------> SPI2_MISO
			PB15     ------> SPI2_MOSI 
			*/
			GPIO_InitStruct.Pin = GPIO_PIN_12;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
			  
			GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

			GPIO_InitStruct.Pin = GPIO_PIN_14;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

static void spi_configuration(spi_user_data_t spi) {	
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)&spi->spix->hspi;
	SPI_TypeDef *spi_type;
	
	spi_type = spi->spix->hspi.Instance;
	if (spi_type == SPI1) {
		
		hspi->Instance = SPI1;
		hspi->Init.Mode = SPI_MODE_MASTER;
		hspi->Init.Direction = SPI_DIRECTION_2LINES;
		hspi->Init.DataSize = SPI_DATASIZE_8BIT;
		hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
		hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
		hspi->Init.NSS = SPI_NSS_SOFT;
		hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
		hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
		hspi->Init.TIMode = SPI_TIMODE_DISABLE;
		hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
		hspi->Init.CRCPolynomial = 10;
		hspi->State = HAL_SPI_STATE_RESET;
		
	} else if (spi_type == SPI2) {
		
		hspi->Instance = SPI2;
		hspi->Init.Mode = SPI_MODE_MASTER;
		hspi->Init.Direction = SPI_DIRECTION_2LINES;
		hspi->Init.DataSize = SPI_DATASIZE_8BIT;
		hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
		hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
		hspi->Init.NSS = SPI_NSS_SOFT;
		hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
		hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
		hspi->Init.TIMode = SPI_TIMODE_DISABLE;
		hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
		hspi->Init.CRCPolynomial = 10;
		hspi->State = HAL_SPI_STATE_RESET;
		
	}
	HAL_SPI_Init(hspi);
}


#ifdef SFUD_USING_QSPI
/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
        uint8_t *read_buf, size_t read_size) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your qspi read flash data code
     */

    return result;
}
#endif /* SFUD_USING_QSPI */

static void spi_enter_critical(const struct __sfud_spi *spi)
{
	__disable_irq();
}

static void spi_exit_critical(const struct __sfud_spi *spi)
{
	__enable_irq();
}

/* about 100 microsecond delay */
static void retry_delay_100us(void) {
    uint32_t delay = 120;
    while(delay--);
}

static spi_device    spi2_device = { .hspi = { .Instance = SPI2 }, };
static spi_user_data spi2 = { .spix = &spi2_device, .cs_gpiox = GPIOB, .cs_gpio_pin = GPIO_PIN_12, };

sfud_err sfud_spi_port_init(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your port spi bus and device object initialize code like this:
     * 1. rcc initialize
     * 2. gpio initialize
     * 3. spi device initialize
     * 4. flash->spi and flash->retry item initialize
     *    flash->spi.wr = spi_write_read; //Required
     *    flash->spi.qspi_read = qspi_read; //Required when QSPI mode enable
     *    flash->spi.lock = spi_lock;
     *    flash->spi.unlock = spi_unlock;
     *    flash->spi.user_data = &spix;
     *    flash->retry.delay = null;
     *    flash->retry.times = 10000; //Required
     */
    //各管脚的初始化配置	
    /* Peripheral clock enable */	
	switch (flash->index)
	{
		case SFUD_MX25L256_DEVICE_INDEX: 
		{			
			rcc_configuration(&spi2);
			gpio_configuration(&spi2);
			spi_configuration(&spi2);
			
			flash->spi.wr = spi_write_read;
			flash->spi.lock = spi_enter_critical;
			flash->spi.unlock = spi_exit_critical;
			flash->spi.user_data = &spi2;
			/* about 100 microsecond delay */
			flash->retry.delay = retry_delay_100us;
			/* adout 60 seconds timeout */
			flash->retry.times = 60 * 10000;	
			flash->spi.name = "spi2";
		}
		default : break;
	}
	
    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}
