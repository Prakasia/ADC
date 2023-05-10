/************		SAMPLE CODE FOR RPI PICO ADC		***********
 * 					-----------------------------
 * **********			SOIL MOISTURE SENSOR			***********
 * Editor : Shalu Prakasia
 * Connect the sensor to the ADC0 (GPIO 26) 
 * 
 * Sensor value
 * 0 	~	300	 - dry soil
 * 300 	~	700  - humid soil
 * 700	~	1023 - water
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

// soil moisture sensor connected to channel 0 (GPIO 26)
static const struct adc_dt_spec adc_chan0 = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);

void main(void)
{
	int err;
	int16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		.buffer_size = sizeof(buf),
	};

	// check if the sensor device is ready
	if (!device_is_ready(adc_chan0.dev)) {
		printk("ADC controller device not ready\n");
		return;
	}

	/* Configure channel0 according to the device tree, before reading. */
	err = adc_channel_setup_dt(&adc_chan0);
	if (err < 0) {
		printk("Could not setup channel\n");
		return;
	}
	printk("Soil Moisture sensor on %s channel %d: \n", adc_chan0.dev->name, adc_chan0.channel_id);
	printk("ADC reading:\n");

	while (1) {
		(void)adc_sequence_init_dt(&adc_chan0, &sequence);

		err = adc_read(adc_chan0.dev, &sequence);
		if (err < 0) {
			printk("Could not read (%d)\n", err);
			continue;
		} 
		else {
			printk("%"PRId16, buf);
			if(buf <= 300) {
				printk("Dry soil!! Watering needed :(\n");
			}
			else if(buf > 300 && buf <= 700){
				printk("Humid soil.. Just right for the plants :) \n");
			}
			else if(buf > 700 ){
				printk("High moisture content!! ");
			}
		}
		k_sleep(K_MSEC(3000));
	}
}




