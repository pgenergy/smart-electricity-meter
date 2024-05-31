# Energyleaf - Smart Meter Sensor
This repository is based on [Tasmota](https://github.com/arendst/Tasmota) (GPL-3.0-only) and extends it by a special driver for energyleaf.

## Hardware

Currently only the so called Hichi-Sensors combined with an ESP01S as Controller are tested. But any other ESP8266 based ESP should work. 

## Functionality

The driver collects the data based on two keywords that can be used in the scripts to read sml and sends it to energyleaf.de or to other endpoints defined in the code that support the same endpoint behavior as energyleaf.de

## Available Commands

<ol>
	<li><code>driver159 rt</code> - Start the driver in an test mode</li>
	<li><code>driver159 rf</code> - Start the driver (full)</li>
	<li><code>driver159 rs</code> - Start the driver (half, without sending data directly)</li>
	<li><code>driver159 s</code> - Stop the driver</li>
	<li><code>driver159 e(0/1)</code> - Changing the sleep mode (currently unsupported)</li>
	<li><code>driver159 v</code> - In an test mode, send test data</li>
	<li><code>driver159 ps</code> - Print in console currently data stored in the sensor (software)</li>
    	<li><code>driver159 pd</code> - Print in console currently data stored in the driver</li>
    	<li><code>driver159 m</code> - Direct send currently data</li>
    	<li><code>driver159 f</code> - Force script update</li>
    	<li><code>driver159 a</code> - Reset the counter for retries</li>
    	<li><code>driver159 i</code> - Print some informations</li>
</ol>

## Available Build-Defines

<ol>
	<li><code>ENERGYLEAF_ENDPOINT_HOST</code> - Host</li>
	<li><code>ENERGYLEAF_ENDPOINT_PORT</code> - Port</li>
	<li><code>ENERGYLEAF_ENDPOINT_TOKEN</code> - Endpoint for tokens</li>
	<li><code>ENERGYLEAF_ENDPOINT_DATA</code> - Endpoint for the data</li>
	<li><code>ENERGYLEAF_ENDPOINT_SCRIPT</code> - Endpoint for the script</li>
	<li><code>ENERGYLEAF_KEYWORD</code> - Keyword 1</li>
	<li><code>ENERGYLEAF_KEYWORD_SPLIT</code> - Keyword 2</li>
    	<li><code>ENERGYLEAF_DRIVER_AUTO_RUN</code> - Run directly</li>
    	<li><code>ENERGYLEAF_DRIVER_AUTO_FULL_RUN</code> - Full direct run</li>
    	<li><code>ENERGYLEAF_TEST_INSTANCE</code> - Special testmode (need to be uncommented or set to be active)</li>
    	<li><code>ENERGYLEAF_TEST_INSTANCE_VALUE</code> - Value for above test mode</li>
    	<li><code>ENERGYLEAF_RETRY_AUTO_RESET</code> - Value of seconds after the retries are reset</li>
    	<li><code>ENERGYLEAF_SLEEP</code> - Using raw sleep (default=false)</li>
    	<li><code>ENERGYLEAF_SLEEP_SECONDS</code> - Seconds per Sleep- or Idle-iteration</li>
    	<li><code>ENERGYLEAF_SLEEP_ITERATIONS</code> - Sleep- or Idle-Iteration</li>
    	<li><code>ENERGYLEAF_CNT_MAX</code> - Max. Retries to get a good sensor reading</li>
</ol>


## Notes

- For changing the endpoint, the certificate maybe needs to be changed.
- Current code uses directly the led based on an ESP01s.
- For more information about the behaviour during the processing, use the logging functionality from tasmota in a higher level.
