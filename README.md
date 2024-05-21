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
