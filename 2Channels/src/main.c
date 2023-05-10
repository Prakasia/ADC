/************		SAMPLE CODE FOR RPI PICO ADC		***********
 * 					-----------------------------
 * **********	SOIL MOISTURE SENSOR AND AIR QUALITY SENSOR		***********
 * 
 * Editor : Shalu Prakasia
 * 
 * Connect the soil moisture sensor to ADC0 (GPIO 26) 
 * Sensor value Range
 * 0 	~	300	 - dry soil
 * 300 	~	700  - humid soil
 * 700	~	1023 - water
 * 
 * Connect the air quality sensor to ADC1 (GPIO 27)
 * 0	~	300	- Fresh air 
 * 300	~	700	- Low pollution
 * > 700		- High pollution
 * 
*/

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <stdio.h>

#define STACK_SIZE  500 

static const struct adc_dt_spec soil_moisture_chan0 = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);
static const struct adc_dt_spec air_quality_chan1 = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 1);

int16_t soilbuf; 
struct adc_sequence soil_sequence = {
	.buffer = &soilbuf,
	.buffer_size = sizeof(soilbuf),
};

int16_t airbuf;
struct adc_sequence air_sequence = {
	.buffer = &airbuf,
	.buffer_size = sizeof(airbuf),
};

// Thread task to read the adc channel
void adc_task(struct adc_dt_spec* chan, struct adc_sequence* seq)
{
	int err;
	// check if the sensor device is ready
	if (!device_is_ready(chan->dev)) {
		printk("ADC controller device not ready\n");
		return;
	}
	/* Configure channel0 according to the device tree, before reading. */
	err = adc_channel_setup_dt(chan);
	if (err < 0) {
		printk("Could not setup channel\n");
		return;
	}
	while (1) {
		(void)adc_sequence_init_dt(chan, seq);
		err = adc_read(chan->dev, seq);
		if (err < 0) {
			printk("Could not read (%d)\n", err);
			continue;
		} 
		else {
			printk("\nChannel %d: ",chan->channel_id);
		}
		k_sleep(K_MSEC(3000));
	}
}

// Display task
void display_task()
{
	while(1){
		printk("\nSoil moisture Channel %d value = %"PRId16, soil_moisture_chan0.channel_id, soilbuf);
		printk("\nAir quality Channel %d value = %"PRId16, air_quality_chan1.channel_id, airbuf);
		k_sleep(K_MSEC(5000));
	}
}

// Thread define
K_THREAD_DEFINE(thread1_id, STACK_SIZE, adc_task, &soil_moisture_chan0, &soil_sequence, NULL, 5, 0, 0);
K_THREAD_DEFINE(thread2_id, STACK_SIZE, adc_task, &air_quality_chan1, &air_sequence, NULL, 5, 0, 0);
K_THREAD_DEFINE(thread3_id, STACK_SIZE, display_task, NULL, NULL, NULL, 5, 0, 0);




