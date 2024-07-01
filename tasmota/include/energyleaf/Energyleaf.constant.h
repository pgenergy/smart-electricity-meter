#ifndef ENERGYLEAF_CONSTANT_HPP
#define ENERGYLEAF_CONSTANT_HPP

#ifdef USE_ENERGYLEAF

#ifndef ENERGYLEAF_USE_LED
#define ENERGYLEAF_USE_LED true
#endif

//Note: Only set and use the led if nothing else the led is already used!
#ifndef ENERGYLEAF_LED_PIN
#define ENERGYLEAF_LED_PIN 2
#endif

#ifndef ENERGYLEAF_ENDPOINT_HOST
#define ENERGYLEAF_ENDPOINT_HOST "admin.energyleaf.de"
#endif

#ifndef ENERGYLEAF_ENDPOINT_PORT
#define ENERGYLEAF_ENDPOINT_PORT 443
#endif

//Note: do not add a / at the beginn of the path, as its added inside of the code
#ifndef ENERGYLEAF_ENDPOINT_TOKEN
#define ENERGYLEAF_ENDPOINT_TOKEN "/api/v1/token"
#endif

//Note: do not add a / at the beginn of the path, as its added inside of the code
#ifndef ENERGYLEAF_ENDPOINT_DATA
#define ENERGYLEAF_ENDPOINT_DATA "/api/v1/sensor_input"
#endif

//Note: do not add a / at the beginn of the path, as its added inside of the code
#ifndef ENERGYLEAF_ENDPOINT_SCRIPT
#define ENERGYLEAF_ENDPOINT_SCRIPT "/api/v1/script_accepted"
#endif

#ifndef ENERGYLEAF_KEYWORD
#define ENERGYLEAF_KEYWORD "ENERGYLEAF_KWH"
#endif

#ifndef ENERGYLEAF_KEYWORD_SPLIT
#define ENERGYLEAF_KEYWORD_SPLIT "ENERGYLEAF_KWH_1"
#endif

#ifndef ENERGYLEAF_KEYWORD_VALUE_OUT
#define ENERGYLEAF_KEYWORD_VALUE_OUT "ENERGYLEAF_OUT"
#endif

#ifndef ENERGYLEAF_KEYWORD_VALUE_CURRENT
#define ENERGYLEAF_KEYWORD_VALUE_CURRENT "ENERGYLEAF_CURRENT"
#endif

#ifndef ENERGYLEAF_DRIVER_AUTO_RUN
#define ENERGYLEAF_DRIVER_AUTO_RUN true
#endif

#ifndef ENERGYLEAF_DRIVER_AUTO_FULL_RUN
#define ENERGYLEAF_DRIVER_AUTO_FULL_RUN true
#endif

#ifndef ENERGYLEAF_RETRY_AUTO_RESET
#define ENERGYLEAF_RETRY_AUTO_RESET 30
#endif

#ifndef ENERGYLEAF_SLEEP
#define ENERGYLEAF_SLEEP false
#endif

#ifndef ENERGYLEAF_SLEEP_SECONDS
#define ENERGYLEAF_SLEEP_SECONDS 4
#endif

#ifndef ENERGYLEAF_SLEEP_ITERATIONS
#define ENERGYLEAF_SLEEP_ITERATIONS 5
#endif

//Value that identifies the # times the sensor trys to get a correct (approvable) sensor reading.
#ifndef ENERGYLEAF_CNT_MAX
#define ENERGYLEAF_CNT_MAX 5
#endif

#endif

#endif