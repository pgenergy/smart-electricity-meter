/*
  user_config_override.h - user configuration overrides my_user_config.h for Tasmota

  Copyright (C) 2021  Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _USER_CONFIG_OVERRIDE_H_
#define _USER_CONFIG_OVERRIDE_H_

/*****************************************************************************************************\
 * USAGE:
 *   To modify the stock configuration without changing the my_user_config.h file:
 *   (1) copy this file to "user_config_override.h" (It will be ignored by Git)
 *   (2) define your own settings below
 *
 ******************************************************************************************************
 * ATTENTION:
 *   - Changes to SECTION1 PARAMETER defines will only override flash settings if you change define CFG_HOLDER.
 *   - Expect compiler warnings when no ifdef/undef/endif sequence is used.
 *   - You still need to update my_user_config.h for major define USE_MQTT_TLS.
 *   - All parameters can be persistent changed online using commands via MQTT, WebConsole or Serial.
\*****************************************************************************************************/

/*
Examples :

// -- Master parameter control --------------------
#undef  CFG_HOLDER
#define CFG_HOLDER        4617                   // [Reset 1] Change this value to load SECTION1 configuration parameters to flash

// -- Setup your own Wifi settings  ---------------
#undef  STA_SSID1
#define STA_SSID1         "YourSSID"             // [Ssid1] Wifi SSID

#undef  STA_PASS1
#define STA_PASS1         "YourWifiPassword"     // [Password1] Wifi password

// -- Setup your own MQTT settings  ---------------
#undef  MQTT_HOST
#define MQTT_HOST         "your-mqtt-server.com" // [MqttHost]

#undef  MQTT_PORT
#define MQTT_PORT         1883                   // [MqttPort] MQTT port (10123 on CloudMQTT)

#undef  MQTT_USER
#define MQTT_USER         "YourMqttUser"         // [MqttUser] Optional user

#undef  MQTT_PASS
#define MQTT_PASS         "YourMqttPass"         // [MqttPassword] Optional password

// You might even pass some parameters from the command line ----------------------------
// Ie:  export PLATFORMIO_BUILD_FLAGS='-DUSE_CONFIG_OVERRIDE -DMY_IP="192.168.1.99" -DMY_GW="192.168.1.1" -DMY_DNS="192.168.1.1"'

#ifdef MY_IP
#undef  WIFI_IP_ADDRESS
#define WIFI_IP_ADDRESS     MY_IP                // Set to 0.0.0.0 for using DHCP or enter a static IP address
#endif

#ifdef MY_GW
#undef  WIFI_GATEWAY
#define WIFI_GATEWAY        MY_GW                // if not using DHCP set Gateway IP address
#endif

#ifdef MY_DNS
#undef  WIFI_DNS
#define WIFI_DNS            MY_DNS               // If not using DHCP set DNS IP address (might be equal to WIFI_GATEWAY)
#endif

#ifdef MY_DNS2
#undef  WIFI_DNS2
#define WIFI_DNS2           MY_DNS2              // If not using DHCP set DNS IP address (might be equal to WIFI_GATEWAY)
#endif

// !!! Remember that your changes GOES AT THE BOTTOM OF THIS FILE right before the last #endif !!!
*/

#ifndef USE_SCRIPT
#define USE_SCRIPT
#endif
#ifndef USE_SML_M
#define USE_SML_M
#endif
#ifdef USE_RULES
#undef USE_RULES
#endif
#ifndef USE_ENERGYLEAF
#define USE_ENERGYLEAF
#endif
#ifdef PROJECT
#undef PROJECT
#define PROJECT                "ENERGYLEAF SENSOR" 
#else
#define PROJECT                "ENERGYLEAF SENSOR" 
#endif
#ifdef FRIENDLY_NAME
#undef FRIENDLY_NAME
#define FRIENDLY_NAME                "ENERGYLEAF_SENSOR" 
#else
#define FRIENDLY_NAME                "ENERGYLEAF_SENSOR" 
#endif
#ifdef OTA_URL
#undef OTA_URL
#define OTA_URL "https://ota.energyleaf.de/release/dem/tasmota.bin.gz"
#else
#define OTA_URL "https://ota.energyleaf.de/release/dem/tasmota.bin.gz"
#endif
#ifdef MODULE
#undef MODULE
#define MODULE                 WEMOS
#else
#define MODULE                 WEMOS
#endif
#ifdef SERIAL_LOG_LEVEL
#undef SERIAL_LOG_LEVEL
#define SERIAL_LOG_LEVEL       LOG_LEVEL_NONE
#else
#define SERIAL_LOG_LEVEL       LOG_LEVEL_NONE
#endif

#ifdef WEB_LOG_LEVEL
#undef WEB_LOG_LEVEL
#define WEB_LOG_LEVEL       LOG_LEVEL_NONE
#else
#define WEB_LOG_LEVEL       LOG_LEVEL_NONE
#endif

//Uncomment and modify values to use Syslog
/*#ifdef SYS_LOG_HOST
#undef SYS_LOG_HOST
#define SYS_LOG_HOST       "192.168.1.5"
#else
#define SYS_LOG_HOST       "192.168.1.5"
#endif
#ifdef SYS_LOG_LEVEL
#undef SYS_LOG_LEVEL
#define SYS_LOG_LEVEL       LOG_LEVEL_NONE
#else
#define SYS_LOG_LEVEL       LOG_LEVEL_NONE
#endif*/

#ifdef MQTT_USE
#undef MQTT_USE
#define MQTT_USE       false
#else
#define MQTT_USE       false
#endif

#ifdef USE_DOMOTICZ  
#undef USE_DOMOTICZ
#endif

#ifdef USE_TASMOTA_DISCOVERY
#undef USE_TASMOTA_DISCOVERY
#endif

#ifdef USE_KNX_WEB_MENU
#undef USE_KNX_WEB_MENU
#endif

#ifndef DISABLE_REFERER_CHK   
#define DISABLE_REFERER_CHK   
#endif
#ifdef USE_EMULATION_HUE
#undef USE_EMULATION_HUE
#endif
#ifdef USE_EMULATION_WEMO
#undef USE_EMULATION_WEMO
#endif
#ifdef WEBSERVER_ADVERTISE
#undef WEBSERVER_ADVERTISE
#endif

#ifdef USE_TIMERS
#undef USE_TIMERS
#endif
#ifdef USE_TIMERS_WEB
#undef USE_TIMERS_WEB
#endif
#ifdef USE_SUNRISE
#undef USE_SUNRISE
#endif

#ifdef SUPPORT_MQTT_EVENT
#undef SUPPORT_MQTT_EVENT
#endif
#ifdef USE_SONOFF_RF
#undef USE_SONOFF_RF
#endif
#ifdef USE_RF_FLASH
#undef USE_RF_FLASH
#endif
#ifdef USE_SONOFF_SC
#undef USE_SONOFF_SC
#endif

#ifdef ROTARY_V1
#undef ROTARY_V1
#endif
#ifdef USE_ARMTRONIX_DIMMERS
#undef USE_ARMTRONIX_DIMMERS
#endif
#ifdef USE_PS_16_DZ
#undef USE_PS_16_DZ
#endif
#ifdef USE_SONOFF_IFAN
#undef USE_SONOFF_IFAN
#endif
#ifdef USE_BUZZER
#undef USE_BUZZER
#endif
#ifdef USE_ARILUX_RF
#undef USE_ARILUX_RF
#endif
#ifdef USE_SHUTTER
#undef USE_SHUTTER
#endif
#ifdef USE_EXS_DIMMER
#undef USE_EXS_DIMMER
#endif
#ifdef USE_DEVICE_GROUPS
#undef USE_DEVICE_GROUPS
#endif
#ifdef USE_DEVICE_GROUPS_SEND
#undef USE_DEVICE_GROUPS_SEND
#endif
#ifdef USE_PWM_DIMMER
#undef USE_PWM_DIMMER
#endif
#ifdef USE_PWM_DIMMER_REMOTE
#undef USE_PWM_DIMMER_REMOTE
#endif
#ifdef USE_SONOFF_D1
#undef USE_SONOFF_D1
#endif
#ifdef USE_PWM_DIMMER
#undef USE_PWM_DIMMER
#endif
#ifdef USE_SHELLY_DIMMER
#undef USE_SHELLY_DIMMER
#endif
#ifdef SHELLY_CMDS
#undef SHELLY_CMDS
#endif
#ifdef SHELLY_FW_UPGRADE
#undef SHELLY_FW_UPGRADE
#endif

#ifdef USE_IMPROV
#undef USE_IMPROV
#endif
#ifdef USE_ENHANCED_GUI_WIFI_SCAN
#undef USE_ENHANCED_GUI_WIFI_SCAN
#endif
#ifdef USE_EXPRESSION
#undef USE_EXPRESSION
#endif
#ifdef SUPPORT_IF_STATEMENT
#undef SUPPORT_IF_STATEMENT
#endif
#ifdef USE_TUYA_MCU
#undef USE_TUYA_MCU
#endif
#ifdef USE_TUYA_TIME
#undef USE_TUYA_TIME
#endif
#ifdef USE_LIGHT
#undef USE_LIGHT
#endif
#ifdef USE_WS2812
#undef USE_WS2812
#endif
#ifdef USE_MY92X1
#undef USE_MY92X1
#endif
#ifdef USE_SM16716
#undef USE_SM16716
#endif
#ifdef USE_SM2135
#undef USE_SM2135
#endif
#ifdef USE_SM2335
#undef USE_SM2335
#endif
#ifdef USE_BP1658CJ
#undef USE_BP1658CJ
#endif
#ifdef USE_BP5758D
#undef USE_BP5758D
#endif
#ifdef USE_SONOFF_L1
#undef USE_SONOFF_L1
#endif
#ifdef USE_ELECTRIQ_MOODL
#undef USE_ELECTRIQ_MOODL
#endif
#ifdef USE_LIGHT_PALETTE
#undef USE_LIGHT_PALETTE
#endif
#ifdef USE_LIGHT_VIRTUAL_CT
#undef USE_LIGHT_VIRTUAL_CT
#endif
#ifdef USE_DGR_LIGHT_SEQUENCE
#undef USE_DGR_LIGHT_SEQUENCE
#endif
#ifdef USE_COUNTER
#undef USE_COUNTER
#endif

#ifdef USE_DS18x20
#undef USE_DS18x20
#endif
#ifdef USE_I2C
#undef USE_I2C
#endif
#ifdef USE_SPI
#undef USE_SPI
#endif
#ifdef USE_ENERGY_SENSOR
#undef USE_ENERGY_SENSOR
#endif
#ifdef USE_ENERGY_MARGIN_DETECTION
#undef USE_ENERGY_MARGIN_DETECTION
#endif
#ifdef USE_ENERGY_POWER_LIMIT
#undef USE_ENERGY_POWER_LIMIT
#endif
#ifdef USE_ENERGY_DUMMY
#undef USE_ENERGY_DUMMY
#endif
#ifdef USE_HLW8012
#undef USE_HLW8012
#endif
#ifdef USE_CSE7766
#undef USE_CSE7766
#endif
#ifdef USE_PZEM004T
#undef USE_PZEM004T
#endif
#ifdef USE_CSE7766
#undef USE_CSE7766
#endif
#ifdef USE_PZEM_AC
#undef USE_PZEM_AC
#endif
#ifdef USE_PZEM_DC
#undef USE_PZEM_DC
#endif
#ifdef USE_MCP39F501
#undef USE_MCP39F501
#endif
#ifdef USE_BL09XX
#undef USE_BL09XX
#endif
#ifdef USE_DHT
#undef USE_DHT
#endif
#ifdef USE_IR_REMOTE
#undef USE_IR_REMOTE
#endif
#ifdef USE_IR_SEND_NEC
#undef USE_IR_SEND_NEC
#endif
#ifdef USE_IR_SEND_RC5
#undef USE_IR_SEND_RC5
#endif
#ifdef USE_IR_SEND_RC6
#undef USE_IR_SEND_RC6
#endif
#ifdef USE_IR_RECEIVE
#undef USE_IR_RECEIVE
#endif
#ifdef USE_ZIGBEE_ZNP
#undef USE_ZIGBEE_ZNP
#endif
#ifdef USE_SENDMAIL
#undef USE_SENDMAIL
#endif
#endif  // _USER_CONFIG_OVERRIDE_H_
