
#ifdef USE_ENERGYLEAF
#define XDRV_159 159

#include "include/energyleaf/Energyleaf.constant.h"
#include <include/tasmota.h>
#include <cstdint>
#include <WiFiClientSecureLightBearSSL.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include "Energyleaf.pb.h"
#include "include/energyleaf/Energyleaf.error.h"
#include "include/energyleaf/Energyleaf.cert.h"


struct ENERGYLEAF_STATE {
    bool running = ENERGYLEAF_DRIVER_AUTO_RUN;
    bool full_running = ENERGYLEAF_DRIVER_AUTO_FULL_RUN;
    bool sleep = ENERGYLEAF_SLEEP;
    bool debug = false;
    //Current token of the sensor
    char accessToken[45]; 
    //Lifetime counter of the token (decreases)
    uint16_t expiresIn = 0;
    uint8_t timer = ENERGYLEAF_SLEEP_SECONDS * ENERGYLEAF_SLEEP_ITERATIONS;
    //State if the sensor got min. one token already 
    bool active = false;
    //State if certificates for secured connection are loaded
    bool certLoaded = false;   
    //State if the script is needed to be loaded (forced load)
    bool needScript = false;   
    //The type of this sensor
    const energyleaf_SensorType type = energyleaf_SensorType::energyleaf_SensorType_DIGITAL_ELECTRICITY;  
    //Retry counter
    uint8_t retryCounter = 0;
    //Identifies if there was min. one sml update
    bool smlUpdate = false;
    //Identifies that currently a send request is processing
    bool lock = false;
    //Identifies the counter for the auto reset of retries.
    uint8_t counterAutoResetRetry = ENERGYLEAF_RETRY_AUTO_RESET;
    uint8_t retCnt = 0;
    bool dataRdy = false;
    bool wifiEnable = false;
};

struct ENERGYLEAF_MEM {
    // kWh total */
    double value = 0.f;
    // W currently */
    double value_current = 0.f;
    // kWh total out (e.g. pv) */
    double value_out = 0.f;
};

BearSSL::WiFiClientSecure_light *energyleafClient = nullptr;
HTTPClient *energyleafHttpsClient;
ENERGYLEAF_MEM *energyleaf_mem;
ENERGYLEAF_STATE *energyleaf;

void energyleafInit(void);
ENERGYLEAF_ERROR energyleafSendData(void);
ENERGYLEAF_ERROR energyleafSendDataIntern(void);
ENERGYLEAF_ERROR energyleafRequestTokenIntern(void);
bool XDRV_159_cmd(void);

void energyleafInit(void) {
    energyleaf_mem = new ENERGYLEAF_MEM;
    energyleaf = new ENERGYLEAF_STATE;
    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER: INIT 1/2"));
    pinMode(2, OUTPUT);
    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER: MAC:[%s]"),WiFi.macAddress().c_str());
    if(!energyleafClient) {
        AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER: INIT CLIENT"));
        energyleafClient = new BearSSL::WiFiClientSecure_light(1024,1024);
        energyleafHttpsClient = new HTTPClient();
        energyleafHttpsClient->setReuse(true);

        if(!energyleaf->certLoaded){
            //Load certificates for secured connection
            AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER: LOADING CERT"));
            energyleafClient->setTrustAnchor(&TAs,TA_SIZE);
            energyleaf->certLoaded = true;
        }

        if(strstr_P(glob_script_mem.script_ram,PSTR(">D\nscript error must start with >D"))) {
            //Check whether the sensor is in the initial state by checking whether the current script is the default script
            AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER: DEFAULT SCRIPT IS LOADED - NEED TO LOAD SCRIPT FOR THIS SENSOR"));
            //Currently not directly used. Maybe used in the future
            energyleaf->active = false;
            energyleaf->needScript = true;
        } else if (bitRead(Settings->rule_enabled,0) == 0) { //if script not enable, activate it
            energyleaf->active = false;
            bitWrite(Settings->rule_enabled, 0, 1);
        }
    }
    
    AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER: INIT 2/2"));
    //Try to get the token here already. The script is only sent if it has to be (the endpoint normally knows this). If no network connection is available at this time, a new attempt is made in the next second.
    
    if(energyleaf->running) {
        energyleaf->active = energyleafRequestTokenIntern() == ENERGYLEAF_ERROR::NO_ERROR ? true : false;
    } else {
        energyleaf->active = false;
    }

    if(!energyleaf->active) {
        AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER: SENSOR NOT ACTIVE - AUTO RETRY SOON"));
    } else {
        AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER: SENSOR ACTIVE"));
    }
}

ENERGYLEAF_ERROR energyleafSendData(void) {
    if(!energyleaf->running && !energyleaf->debug) {
        return ENERGYLEAF_ERROR::ERROR;
    }
    if((!energyleaf->active && energyleaf->retryCounter == 5 && (energyleaf->running || energyleaf->debug)) || (!energyleaf->active && (energyleaf->running || energyleaf->debug))){
        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER: RETRY COUNTER LIMIT REACHED, CHECK FOR PROBLEMS AND RESTART SENSOR IF FIXED [COUNTER:%d]"),energyleaf->retryCounter);
        return ENERGYLEAF_ERROR::ERROR;
    }
    if(energyleaf_mem->value == 0.f) {
        char output[20];
        dtostrf(energyleaf_mem->value,sizeof(output) - 1,4,output);
        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER: Sensor wanted to send value [%s]"),output);
        return ENERGYLEAF_ERROR::RET;
    }
    //call energyleafSendDataIntern
    ENERGYLEAF_ERROR state = energyleafSendDataIntern();
    if(state != ENERGYLEAF_ERROR::NO_ERROR) {
        if(!energyleaf->active || (energyleaf->active &&  energyleaf->expiresIn <= 0) || state == ENERGYLEAF_ERROR::TOKEN_EXPIRED) {
            //token expired, renew and retry
            AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER: NOT ACTIVE OR COUNTER EXPIRED [ACTIVE:%S;COUNTER:%d]"),energyleaf->active ? "true":"false",energyleaf->expiresIn);
            energyleaf->active = energyleafRequestTokenIntern() == ENERGYLEAF_ERROR::NO_ERROR ? true : false;
            if(energyleaf->active && energyleaf->expiresIn > 0) {
                //We ignore the status because if it is wrong again (the transfer has failed), we cannot fix it by automatically retrying and possibly spamming the endpoint host.
                state = energyleafSendDataIntern();
                return ENERGYLEAF_ERROR::IGNORE;
            } else {    
                AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER: COULD NOT GET A NEW TOKEN - NO RETRY"));
                return ENERGYLEAF_ERROR::IGNORE;
            }
        } else if(state == ENERGYLEAF_ERROR::RET) {
            return state;
        } else {
            AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER: ERROR IN THE TRANSMISSION OF DATA - NO RETRY"));
            return ENERGYLEAF_ERROR::IGNORE;
        }
    } else {
        AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER: SUCCESSFULLY TRANSMITTED DATA"));
        return ENERGYLEAF_ERROR::NO_ERROR;
    }
}

ENERGYLEAF_ERROR energyleafSendDataIntern(void) {
    if(!energyleaf->running && !energyleaf->debug) {
        return ENERGYLEAF_ERROR::RET;
    }
    if(energyleafClient && energyleafHttpsClient) {
        ESP.wdtFeed();
        yield();
        if(energyleaf->wifiEnable) {
            ESP.wdtFeed();
            yield();
            #if ENERGYLEAF_USE_LED == true
            digitalWrite(ENERGYLEAF_LED_PIN,HIGH);
            delay(500);
            digitalWrite(ENERGYLEAF_LED_PIN,LOW);
            delay(500);
            digitalWrite(ENERGYLEAF_LED_PIN,HIGH);
            delay(500);
            digitalWrite(ENERGYLEAF_LED_PIN,LOW);
            #endif
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: WIFI IS AVAILABLE"));
            bool state = false;
            {
                //Prepare SensorDataRequest
                uint8_t *bufferSensorDataRequest = new uint8_t[energyleaf_SensorDataRequestV2_size];
                int byteswritten = 0;
                {
                    energyleaf_SensorDataRequestV2 *sensorDataRequest = new energyleaf_SensorDataRequestV2();
                    memcpy(sensorDataRequest->access_token, energyleaf->accessToken, sizeof(energyleaf->accessToken));
                    AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: USED TOKEN [%s]"),sensorDataRequest->access_token);
                    sensorDataRequest->type = energyleaf->type;

                    sensorDataRequest->value = energyleaf_mem->value;
                    char output[20];
                    dtostrf(energyleaf_mem->value,sizeof(output) - 1,4,output);
                    AddLog(LOG_LEVEL_NONE, PSTR("ENERGYLEAF_DRIVER: Sending value [%s]"),output);

                    if(energyleaf_mem->value_current != 0.f) {
                        sensorDataRequest->value_current = energyleaf_mem->value_current;
                        sensorDataRequest->has_value_current = true;
                    } else {
                        sensorDataRequest->has_value_current = false;
                    }

                    if(energyleaf_mem->value_out != 0.f) {
                        sensorDataRequest->value_out = energyleaf_mem->value_out;
                        sensorDataRequest->has_value_out = true;
                    } else {
                        sensorDataRequest->has_value_out = false;
                    }

                    pb_ostream_t *streamSensorDataRequestOut = new pb_ostream_t(pb_ostream_from_buffer(bufferSensorDataRequest, energyleaf_SensorDataRequestV2_size));

                    state = pb_encode(streamSensorDataRequestOut,energyleaf_SensorDataRequestV2_fields, sensorDataRequest);
                    byteswritten = streamSensorDataRequestOut->bytes_written;
                    if(streamSensorDataRequestOut != nullptr) {
                        delete streamSensorDataRequestOut;
                        streamSensorDataRequestOut = nullptr;
                    }
                    if(sensorDataRequest != nullptr) {
                        delete sensorDataRequest;
                        sensorDataRequest = nullptr;
                    }
                }

                if(!state) {
                    AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: UNSUCCESSFUL - COULD NOT CREATE PACKAGE"));
                    if(bufferSensorDataRequest != nullptr) {
                        delete[] bufferSensorDataRequest;
                        bufferSensorDataRequest = nullptr;
                    }
                    return ENERGYLEAF_ERROR::ERROR;
                }
                
                AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: pb_encode successful"));
                ESP.wdtFeed();
                yield();
                //Send SensorDataRequest and process received header
                {
                    state = energyleafHttpsClient->begin(*energyleafClient,ENERGYLEAF_ENDPOINT_HOST,ENERGYLEAF_ENDPOINT_PORT,ENERGYLEAF_ENDPOINT_DATA,true);
                    if(!state) {
                        energyleafHttpsClient->end(); 
                        AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: UNSUCCESSFUL - COULD NOT CONNECT TO SERVICE"));
                        if(bufferSensorDataRequest != nullptr) {
                            delete[] bufferSensorDataRequest;
                            bufferSensorDataRequest = nullptr;
                        }
                        return ENERGYLEAF_ERROR::ERROR;
                    }

                    energyleafHttpsClient->setUserAgent("Energyleaf-Sensor-Digital");
                    energyleafHttpsClient->addHeader("Content-Type", "application/x-protobuf");
                    energyleafHttpsClient->addHeader("Content-Length", String(byteswritten));

                    int httpCode = energyleafHttpsClient->POST(bufferSensorDataRequest, byteswritten);

                    if(bufferSensorDataRequest != nullptr) {
                        delete[] bufferSensorDataRequest;
                        bufferSensorDataRequest = nullptr;
                    }

                    if(energyleafHttpsClient->connected() && httpCode >= 200 && httpCode <= 299 && energyleafHttpsClient->header("Transfer-Encoding").equals("chunked")) {
                        AddLog(LOG_LEVEL_DEBUG,PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: GOT A %d STATUS"),httpCode);
                    } else {
                        AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: UNSUCCESSFUL - GOT A %d STATUS"),httpCode);
                        if(energyleafHttpsClient->connected()){
                            energyleafHttpsClient->end(); 
                        }
                        if(httpCode == ENERGYLEAF_TOKEN_EXPIRED_CODE) {
                            return ENERGYLEAF_ERROR::TOKEN_EXPIRED;
                        } else {
                            return ENERGYLEAF_ERROR::ERROR;
                        }
                    }                    
                }

                if(!state) {
                    energyleafHttpsClient->end();
                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: UNSUCCESSFUL - GOT WRONG CONTENT-TYPE FROM SERVICE"));
                    return ENERGYLEAF_ERROR::ERROR;
                }
            }

            ESP.wdtFeed();
            yield();

            {
                //Process received body and generate SensorDataResponse from it
                energyleaf_SensorDataResponse *sensorDataResponse = new energyleaf_SensorDataResponse();
                {
                    uint8_t *bufferSensorDataResponse = new uint8_t[energyleaf_SensorDataResponse_size];
                    int currentSize = 0;
                    {
                        while(energyleafHttpsClient->connected()) {
                            char chunkSize[16];
                            int l = energyleafHttpsClient->getStream().readBytesUntil('\n',chunkSize,sizeof(chunkSize));
                            if(l<=0) {
                                break;
                            }

                            chunkSize[l-1] = 0;
                            int chunkSizeI = strtol(chunkSize,NULL,16);

                            if(chunkSizeI == 0) {
                                break;
                            }

                            char chunkData[chunkSizeI];
                            l = energyleafHttpsClient->getStream().readBytes(chunkData,chunkSizeI);
                            Serial.println(chunkData);
                            if(l<=0) {
                                break;
                            }

                            if(currentSize + l <= energyleaf_SensorDataResponse_size) {
                                memcpy(bufferSensorDataResponse + currentSize, chunkData, l);
                                currentSize += l;
                            } else {
                                AddLog(LOG_LEVEL_ERROR,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - NOT ENOUGH SPACE FOR DATA"));
                                energyleafHttpsClient->end(); 
                                if(sensorDataResponse != nullptr) {
                                    delete sensorDataResponse;
                                    sensorDataResponse = nullptr;
                                }
                                if(bufferSensorDataResponse != nullptr) {
                                    delete[] bufferSensorDataResponse;
                                    bufferSensorDataResponse = nullptr;
                                }
                                return ENERGYLEAF_ERROR::ERROR;
                            }
                            ESP.wdtFeed();
                            yield();
                        }
                        state = currentSize > 0;
                    }
                    energyleafHttpsClient->end();

                    if(!state) {
                        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: UNSUCCESSFUL - GOT PACKAGE WITH WRONG SIZE [SIZE:%d]"),currentSize);
                        if(sensorDataResponse != nullptr) {
                            delete sensorDataResponse;
                            sensorDataResponse = nullptr;
                        }
                        if(bufferSensorDataResponse != nullptr) {
                            delete[] bufferSensorDataResponse;
                            bufferSensorDataResponse = nullptr;
                        }
                        return ENERGYLEAF_ERROR::ERROR;
                    }

                    AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: [SIZE:%d]"),currentSize);

                    {
                        pb_istream_t *streamSensorDataResponseIn = new pb_istream_t(pb_istream_from_buffer(bufferSensorDataResponse,currentSize));
                        state = pb_decode(streamSensorDataResponseIn,energyleaf_SensorDataResponse_fields, sensorDataResponse);
                        if(streamSensorDataResponseIn != nullptr) {
                            delete streamSensorDataResponseIn;
                            streamSensorDataResponseIn = nullptr;
                        }
                        if(bufferSensorDataResponse != nullptr) {
                            delete[] bufferSensorDataResponse;
                            bufferSensorDataResponse = nullptr;
                        }
                    }

                    

                    if(!state) {
                        energyleafHttpsClient->end(); 
                        AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: UNSUCCESSFUL - GOT WRONG CONTENT-TYPE FROM SERVICE"));
                        if(sensorDataResponse != nullptr) {
                            delete sensorDataResponse;
                            sensorDataResponse = nullptr;
                        }
                        return ENERGYLEAF_ERROR::ERROR;
                    }
                }

                state = sensorDataResponse->status >= 200 && sensorDataResponse->status <= 299;
                if(!state) {
                    energyleafHttpsClient->end(); 
                    if(sensorDataResponse->has_status_message) {
                        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: UNSUCCESSFUL - ERROR WITH %d STATUS [%s]"),sensorDataResponse->status, sensorDataResponse->status_message);
                    } else {
                        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: UNSUCCESSFUL - ERROR WITH %d STATUS"),sensorDataResponse->status);
                    }

                    ENERGYLEAF_ERROR ret;

                    if(sensorDataResponse->status == ENERGYLEAF_TOKEN_EXPIRED_CODE) {
                        ret = ENERGYLEAF_ERROR::TOKEN_EXPIRED;
                    } else {
                        if(sensorDataResponse->status == ENERGYLEAF_ENDPOINT_DATA_RETRY && sensorDataResponse->has_status_message && sensorDataResponse->status_message == ENERGYLEAF_ENDPOINT_DATA_RETRY_MSG) {
                            ret = ENERGYLEAF_ERROR::RET;
                        } else {
                            ret = ENERGYLEAF_ERROR::ERROR;
                        }
                    }

                    if(sensorDataResponse != nullptr) {
                        delete sensorDataResponse;
                        sensorDataResponse = nullptr;
                    }

                    return ret;
                }

                if(sensorDataResponse != nullptr) {
                    delete sensorDataResponse;
                    sensorDataResponse = nullptr;      
                }  
            }
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: SUCCESSFUL - DATA WAS TRASMITTED TO THE SERVICE"));
            return ENERGYLEAF_ERROR::NO_ERROR;
        } else {
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: NO WIFI TO CREATE A REQUEST - CHECK IF YOU HAVE A NETWORK CONNECTION"));
            return ENERGYLEAF_ERROR::ERROR;
        }
    } else {
        AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_DATA_REQUEST: NO CLIENT IS AVAILABLE - RESTART SENSOR OR CONTACT SUPPORT"));
        return ENERGYLEAF_ERROR::ERROR; 
    }
}

ENERGYLEAF_ERROR energyleafRequestTokenIntern(void) {
    if(!energyleaf->running && !energyleaf->debug) {
        return ENERGYLEAF_ERROR::RET;
    }
    if(energyleafClient && energyleafHttpsClient) {
        ESP.wdtFeed();
        yield();
        //fresh start and the default script is loaded, not the script of the sensor.
        AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: PREPARING REQUEST FOR TOKEN%s"),energyleaf->needScript ? " AND FORCING SCRIPT" : "");
        if(energyleaf->wifiEnable) {
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: WIFI IS AVAILABLE"));

            bool state = false;
            int byteswritten = 0;
            {  
                //Prepare TokenRequest
                uint8_t *bufferTokenRequest = new uint8_t[energyleaf_TokenRequest_size];
                {
                    energyleaf_TokenRequest *tokenRequest = new energyleaf_TokenRequest();
                    //collect the MAC of this sensor
                    memcpy(tokenRequest->client_id, WiFi.macAddress().c_str(), sizeof(tokenRequest->client_id));
                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: USED MAC [%s]"),tokenRequest->client_id);
                    //set the type of this sensor
                    tokenRequest->type = energyleaf->type;
                    //if true it forces the server to (re-)send the script in the response
                    tokenRequest->need_script = energyleaf->needScript;
                    tokenRequest->has_need_script = energyleaf->needScript;
                    pb_ostream_t *streamTokenRequestOut = new pb_ostream_t(pb_ostream_from_buffer(bufferTokenRequest, energyleaf_TokenRequest_size));

                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: NEED SCRIPT [%s]"),tokenRequest->need_script ? "true" : "false");

                
                    state = pb_encode(streamTokenRequestOut,energyleaf_TokenRequest_fields, tokenRequest);
                    byteswritten = streamTokenRequestOut->bytes_written;
                    if(streamTokenRequestOut != nullptr) {
                        delete streamTokenRequestOut;
                        streamTokenRequestOut = nullptr;
                    }
                    if(tokenRequest != nullptr) {
                        delete tokenRequest;
                        tokenRequest = nullptr;
                    }
                }

                if(!state) {
                    AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - COULD NOT CREATE PACKAGE"));
                    if(bufferTokenRequest != nullptr) {
                        delete[] bufferTokenRequest;
                        bufferTokenRequest = nullptr;
                    }
                    return ENERGYLEAF_ERROR::ERROR;
                }

                AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: pb_encode successful"));
                ESP.wdtFeed();
                yield();
                //Send TokenRequest and process received header
                {
                    AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: [%d][%d]"),ESP.getFreeContStack(),ESP.getFreeHeap());

                    state = energyleafHttpsClient->begin(*energyleafClient,ENERGYLEAF_ENDPOINT_HOST,ENERGYLEAF_ENDPOINT_PORT,ENERGYLEAF_ENDPOINT_TOKEN,true);
                    if(!state) {
                        energyleafHttpsClient->end(); 
                        AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - COULD NOT CONNECT TO SERVICE"));
                        if(bufferTokenRequest != nullptr) {
                            delete[] bufferTokenRequest;
                            bufferTokenRequest = nullptr;
                        }
                        return ENERGYLEAF_ERROR::ERROR;
                    }

                    energyleafHttpsClient->setUserAgent("Energyleaf-Sensor-Digital");
                    energyleafHttpsClient->addHeader("Content-Type", "application/x-protobuf");
                    energyleafHttpsClient->addHeader("Content-Length", String(byteswritten));

                    int httpCode = energyleafHttpsClient->POST(bufferTokenRequest, byteswritten);

                    if(bufferTokenRequest != nullptr) {
                        delete[] bufferTokenRequest;
                        bufferTokenRequest = nullptr;
                    }

                    if(energyleafHttpsClient->connected() && httpCode >= 200 && httpCode <= 299 && energyleafHttpsClient->header("Transfer-Encoding").equals("chunked")) {
                        AddLog(LOG_LEVEL_DEBUG,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: GOT A %d STATUS"),httpCode);
                    } else {
                        AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT A %d STATUS"),httpCode);
                        energyleafHttpsClient->end(); 
                        if(httpCode == ENERGYLEAF_TOKEN_EXPIRED_CODE) {
                            return ENERGYLEAF_ERROR::TOKEN_EXPIRED;
                        } else {
                            return ENERGYLEAF_ERROR::ERROR;
                        }
                    }
                }

                if(!state) {
                    energyleafHttpsClient->end(); 
                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT WRONG CONTENT-TYPE FROM SERVICE"));
                    return ENERGYLEAF_ERROR::ERROR;
                }
            }

            uint8_t* bufferScriptAcceptedRequest = new uint8_t[energyleaf_ScriptAcceptedRequest_size];
            ESP.wdtFeed();
            yield();

            {
                //Process received body and generate TokenResponse from it
                energyleaf_TokenResponse *tokenResponse = new energyleaf_TokenResponse();;
                {
                    uint8_t *bufferTokenResponse = new uint8_t[energyleaf_TokenResponse_size];
                    int currentSize = 0;
                    {
                        while(energyleafHttpsClient->connected()) {
                            char chunkSize[16];
                            int l = energyleafHttpsClient->getStream().readBytesUntil('\n',chunkSize,sizeof(chunkSize));
                            if(l<=0) {
                                break;
                            }

                            chunkSize[l-1] = 0;
                            int chunkSizeI = strtol(chunkSize,NULL,16);

                            if(chunkSizeI == 0) {
                                break;
                            }

                            char chunkData[chunkSizeI];
                            l = energyleafHttpsClient->getStream().readBytes(chunkData,chunkSizeI);
                            Serial.println(chunkData);
                            if(l<=0) {
                                break;
                            }

                            if(currentSize + l <= energyleaf_TokenResponse_size) {
                                memcpy(bufferTokenResponse + currentSize, chunkData, l);
                                currentSize += l;
                            } else {
                                AddLog(LOG_LEVEL_ERROR,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - NOT ENOUGH SPACE FOR DATA"));
                                energyleafHttpsClient->end(); 
                                if(bufferScriptAcceptedRequest != nullptr) {
                                    delete[] bufferScriptAcceptedRequest;
                                    bufferScriptAcceptedRequest = nullptr;
                                }
                                if(tokenResponse != nullptr) {
                                    delete tokenResponse;
                                    tokenResponse = nullptr;
                                }
                                if(bufferTokenResponse != nullptr) {
                                    delete[] bufferTokenResponse;
                                    bufferTokenResponse = nullptr;
                                }
                                return ENERGYLEAF_ERROR::ERROR;
                            }
                            ESP.wdtFeed();
                            yield();
                        }
                        state = currentSize > 0;
                    }

                    AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: CONNECTED TO ENERGYLEAF SERIVCE"));
                    energyleafHttpsClient->end(); 

                    if(!state) {
                        energyleafHttpsClient->end(); 
                        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT PACKAGE WITH WRONG SIZE [SIZE:%d]"),currentSize);
                        if(bufferScriptAcceptedRequest != nullptr) {
                            delete[] bufferScriptAcceptedRequest;
                            bufferScriptAcceptedRequest = nullptr;
                        }
                        if(tokenResponse != nullptr) {
                            delete tokenResponse;
                            tokenResponse = nullptr;
                        }
                        if(bufferTokenResponse != nullptr) {
                            delete[] bufferTokenResponse;
                            bufferTokenResponse = nullptr;
                        }
                        return ENERGYLEAF_ERROR::ERROR;
                    }

                    {
                        pb_istream_t *streamTokenResponseIn = new pb_istream_t(pb_istream_from_buffer(bufferTokenResponse,currentSize));
                        state = pb_decode(streamTokenResponseIn,energyleaf_TokenResponse_fields, tokenResponse);
                        if(bufferTokenResponse != nullptr) {
                            delete bufferTokenResponse;
                            bufferTokenResponse = nullptr;
                        }
                    }

                    if(bufferTokenResponse != nullptr) {
                        delete[] bufferTokenResponse;
                        bufferTokenResponse = nullptr;
                    }

                    if(!state) {
                        energyleafHttpsClient->end(); 
                        AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT WRONG CONTENT-TYPE FROM SERVICE"));
                        if(bufferScriptAcceptedRequest != nullptr) {
                            delete[] bufferScriptAcceptedRequest;
                            bufferScriptAcceptedRequest = nullptr;
                        }
                        if(tokenResponse != nullptr) {
                            delete tokenResponse;
                            tokenResponse = nullptr;
                        }
                        return ENERGYLEAF_ERROR::ERROR;
                    }

                }

                ESP.wdtFeed();
                yield();

                state = tokenResponse->status >= 200 && tokenResponse->status <= 299 && tokenResponse->has_access_token;
                if(!state) {
                    energyleafHttpsClient->end(); 
                    if(tokenResponse->has_status_message) {
                        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - ERROR WITH %d STATUS [%s]"),tokenResponse->status, tokenResponse->status_message);
                    } else {
                        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - ERROR WITH %d STATUS"),tokenResponse->status);
                    }

                    ENERGYLEAF_ERROR ret;
                    if(tokenResponse->status == ENERGYLEAF_TOKEN_EXPIRED_CODE) {
                        ret = ENERGYLEAF_ERROR::TOKEN_EXPIRED;
                    } else {
                        ret = ENERGYLEAF_ERROR::ERROR;
                    }

                    if(bufferScriptAcceptedRequest != nullptr) {
                        delete[] bufferScriptAcceptedRequest;
                        bufferScriptAcceptedRequest = nullptr;
                    }
                    if(tokenResponse != nullptr) {
                        delete tokenResponse;
                        tokenResponse = nullptr;
                    }

                    return ret;
                }

                //Store Script and the Parameter from the TokenResponse
                {
                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: SUCCESSFUL"));
                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: [TOKEN:%s]"),tokenResponse->access_token);
                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: [STATUS:%d]"),tokenResponse->status);
                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: [SCRIPT:%s]"),tokenResponse->has_script ? "true" : "false");
                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: [SCRIPT:%s]"),tokenResponse->script);
          
                    strcpy(energyleaf->accessToken, tokenResponse->access_token);
                    energyleaf->expiresIn = tokenResponse->expires_in;

                    if(tokenResponse->has_script) {
                        state = sizeof(tokenResponse->script) < glob_script_mem.script_size;
                        if(!state) {
                            AddLog(LOG_LEVEL_ERROR,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - SCRIPT IS TO LARGE"));
                            energyleafHttpsClient->end(); 
                            if(tokenResponse != nullptr) {
                                delete tokenResponse;
                                tokenResponse = nullptr;
                            }
                            return ENERGYLEAF_ERROR::ERROR;
                        }

                        uint8_t *script_ex_ptr = (uint8_t*)glob_script_mem.script_ram;

                        //uint8_t sc_state = bitRead(Settings->rule_enabled,0);
                        bitWrite(Settings->rule_enabled,0,0);

                        memcpy(script_ex_ptr, tokenResponse->script, sizeof(tokenResponse->script));

                        script_ex_ptr = nullptr;

                        bitWrite(Settings->rule_enabled, 0, 1);
                        SaveScript();
                        SaveScriptEnd();

                        energyleaf->needScript = false;
                    } else {
                        if(bufferScriptAcceptedRequest != nullptr) {
                        delete[] bufferScriptAcceptedRequest;
                        bufferScriptAcceptedRequest = nullptr;
                        }
                        if(tokenResponse != nullptr) {
                            delete tokenResponse;
                            tokenResponse = nullptr;
                        }
                        if(energyleaf->needScript) {
                            AddLog(LOG_LEVEL_ERROR,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - SCRIPT REQUEST BUT NOT RECEIVED "));
                            energyleafHttpsClient->end(); 
                            return ENERGYLEAF_ERROR::ERROR;
                        } else {
                            //Only a new Token was requested and given
                            AddLog(LOG_LEVEL_ERROR,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: SUCCESSFUL - NEW TOKEN RECEIVED "));
                            energyleafHttpsClient->end(); 
                            return ENERGYLEAF_ERROR::NO_ERROR;
                        }
                    }
                }

                if(tokenResponse != nullptr) {
                    delete tokenResponse;
                    tokenResponse = nullptr;
                }
                energyleafHttpsClient->end(); 

                //Prepare ScriptAcceptedRequest
                {
                    energyleaf_ScriptAcceptedRequest *scriptAcceptedRequest = new energyleaf_ScriptAcceptedRequest();

                    memcpy(scriptAcceptedRequest->access_token, energyleaf->accessToken, sizeof(energyleaf->accessToken));

                    pb_ostream_t *streamScriptAcceptedRequestOut = new pb_ostream_t(pb_ostream_from_buffer(bufferScriptAcceptedRequest, energyleaf_ScriptAcceptedRequest_size));

                    AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: Creating SAR-Package"));
                
                    state = pb_encode(streamScriptAcceptedRequestOut,energyleaf_ScriptAcceptedRequest_fields, scriptAcceptedRequest);
                    byteswritten = streamScriptAcceptedRequestOut->bytes_written;
                    if(streamScriptAcceptedRequestOut != nullptr) {
                        delete streamScriptAcceptedRequestOut;
                        streamScriptAcceptedRequestOut = nullptr;
                    }
                    if(scriptAcceptedRequest != nullptr) {
                        delete scriptAcceptedRequest;
                        scriptAcceptedRequest = nullptr;
                    }
                }

                if(!state) {
                    AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - COULD NOT CREATE SAR-PACKAGE"));
                    if(bufferScriptAcceptedRequest != nullptr) {
                        delete[] bufferScriptAcceptedRequest;
                        bufferScriptAcceptedRequest = nullptr;
                    }
                    return ENERGYLEAF_ERROR::ERROR;
                }

                AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: pb_encode successful"));

                state = energyleafHttpsClient->begin(*energyleafClient,ENERGYLEAF_ENDPOINT_HOST,ENERGYLEAF_ENDPOINT_PORT,ENERGYLEAF_ENDPOINT_SCRIPT,true);
                if(!state) {
                    energyleafHttpsClient->end(); 
                    AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - COULD NOT CONNECT TO SERVICE"));
                    if(bufferScriptAcceptedRequest != nullptr) {
                        delete[] bufferScriptAcceptedRequest;
                        bufferScriptAcceptedRequest = nullptr;
                    }
                    return ENERGYLEAF_ERROR::ERROR;
                }

                //Send ScriptAcceptedRequest
                {
                    energyleafHttpsClient->setUserAgent("Energyleaf-Sensor-Digital");
                    energyleafHttpsClient->addHeader("Content-Type", "application/x-protobuf");
                    energyleafHttpsClient->addHeader("Content-Length", String(byteswritten));
                    
                    AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: CONNECTED TO ENERGYLEAF SERIVCE"));
                    int httpCode = energyleafHttpsClient->POST(bufferScriptAcceptedRequest, byteswritten);

                    if(bufferScriptAcceptedRequest != nullptr) {
                        delete[] bufferScriptAcceptedRequest;
                        bufferScriptAcceptedRequest = nullptr;
                    }

                    if(energyleafHttpsClient->connected() && httpCode >= 200 && httpCode <= 299) {
                        AddLog(LOG_LEVEL_DEBUG,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: GOT A %d STATUS"),httpCode);
                    } else {
                        AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT A %d STATUS"),httpCode);
                        if(energyleafHttpsClient->connected()){
                            energyleafHttpsClient->end(); 
                        }
                        return ENERGYLEAF_ERROR::ERROR;
                    }
                }
            }         
            energyleafHttpsClient->end(); 

            return ENERGYLEAF_ERROR::NO_ERROR;
        } else {
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: NO WIFI TO CREATE A REQUEST - AUTO-RETRY IN SOME TIME"));
            return ENERGYLEAF_ERROR::ERROR;
        }
    } else {
        AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: NO CLIENT IS AVAILABLE - RESTART SENSOR OR CONTACT SUPPORT"));
        return ENERGYLEAF_ERROR::ERROR; 
    }
}

void energyleafEverySecond(void) {
    ESP.wdtFeed();
    yield();
    if(!energyleaf->running && !energyleaf->debug) {
        return;
    }

    if(!energyleaf->active) {
        //driver not active (no token)

        //auto reset of retries
        if(energyleaf->counterAutoResetRetry > 0) {
            --energyleaf->counterAutoResetRetry;
        } else {
            energyleaf->counterAutoResetRetry = ENERGYLEAF_RETRY_AUTO_RESET;
            energyleaf->retryCounter = 0;
        }

        //check that only a specific amount of retries is done
        if(energyleaf->retryCounter == 5) {
            AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER: RETRY COUNTER LIMIT REACHED, CHECK FOR PROBLEMS AND RESTART SENSOR IF FIXED [COUNTER:%d]"),energyleaf->retryCounter);
            return;
        } else {
            if(energyleaf->retryCounter==0) {
                energyleaf->active = energyleafRequestTokenIntern() == ENERGYLEAF_ERROR::NO_ERROR ? true : false;
            }
            ++energyleaf->retryCounter;
            return;
        }
    } else {
        //driver is active (have token)

        //verify if a new token is needed (expired)
        if(energyleaf->expiresIn <= 0) {
            energyleaf->active = energyleafRequestTokenIntern() == ENERGYLEAF_ERROR::NO_ERROR ? true : false;
            if(!energyleaf->active) {
                return;
            }
        } else {
            --energyleaf->expiresIn;
        }

        //request sensor to send new data (driver is running, script is enable, no lock is set and wifi is connected)
        if(energyleaf->running && energyleaf->full_running && bitRead(Settings->rule_enabled,0) && !energyleaf->lock) {
            if(!energyleaf->sleep){
                if(energyleaf->timer <= 0) {
                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_SENSOR: TIMER EXPIRED - FORWARD"));
                    energyleaf->timer = ENERGYLEAF_SLEEP_SECONDS * ENERGYLEAF_SLEEP_ITERATIONS;
                } else {
                    --energyleaf->timer;
                    return;
                }
            }
            energyleaf->lock = true;
            #if ENERGYLEAF_USE_LED == true
            digitalWrite(ENERGYLEAF_LED_PIN,HIGH);
            delay(500);
            digitalWrite(ENERGYLEAF_LED_PIN,LOW);
            #endif
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_SENSOR: SML-Update [%s] - REQUEST SENDING"),energyleaf->smlUpdate ? "true" : "false");
            XsnsXdrvCall(FUNC_ENERGYLEAF_SEND);
            if(energyleaf->lock) energyleaf->lock = false;
        } else {
            if(energyleaf->running && energyleaf->full_running) {
                //force new script;
                energyleaf->needScript = true;
                AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_SENSOR: SML-Update [%s]"),energyleaf->smlUpdate ? "true" : "false");
                energyleaf->active = energyleafRequestTokenIntern() == ENERGYLEAF_ERROR::NO_ERROR ? true : false;
                return;
            }
        }
    }
}

void energyleafDevicePower(void) {
    //Currently not used
}

bool XDRV_159_cmd(void) {
    bool ret = true;
    if(XdrvMailbox.data_len > 0) {
        char *cp = XdrvMailbox.data;
        if(*cp == 'r') {
            //RUN
            ++cp;
            if(*cp == 's') {
                //SENSOR
                if(!energyleaf->running && !energyleaf->debug) {
                    energyleaf->running = true;
                    ResponseTime_P(PSTR(",\"ENERGYLEAF\":{\"CMD\":\"START - SENSOR\"}}"));
                }
            } else if(*cp == 't') {
                //DEBUG / TEST
                if(!energyleaf->running && !energyleaf->debug) {
                    energyleaf->debug = true;
                    ResponseTime_P(PSTR(",\"ENERGYLEAF\":{\"CMD\":\"START - DEBUG\"}}"));
                }
            } else if(*cp == 'f') {
                if(energyleaf->running) {
                    energyleaf->full_running = true;
                }
            }
        } else if(*cp == 's') {
            //STOP
            energyleaf->running = false;
            energyleaf->debug = false;
            energyleaf->full_running = false;
            ResponseTime_P(PSTR(",\"ENERGYLEAF\":{\"CMD\":\"STOP\"}}"));
        } else if(*cp == 'e') {
            //swap sleep mode
            ++cp;
            if(*cp == '0'){
                energyleaf->sleep = false;
                ResponseTime_P(PSTR(",\"ENERGYLEAF\":{\"CMD\":\"SLEEP - OFF\"}}"));
            } else if (*cp == '1') {
                energyleaf->sleep = true;
                ResponseTime_P(PSTR(",\"ENERGYLEAF\":{\"CMD\":\"SLEEP - ON\"}}"));
            }
        } else if(*cp == 'v') {
            //VERIFY / TEST
            energyleaf_mem->value += 0.15;
            energyleafSendData();
            ResponseTime_P(PSTR(",\"ENERGYLEAF\":{\"CMD\":\"VERIFY / TEST\"}}"));
        } else if(*cp == 'p') {
            //PRINT
            ++cp;
            if(*cp == 's') {
                //SENSOR
                XsnsXdrvCall(FUNC_ENERGYLEAF_PRINT);
                ResponseTime_P(PSTR(",\"ENERGYLEAF\":{\"CMD\":\"PRINT - SENSOR\"}}"));
            } else if(*cp == 'd') {
                //DRIVER
                char output[20];
                dtostrf(energyleaf_mem->value,sizeof(output) - 1,8,output);
                AddLog(LOG_LEVEL_NONE, PSTR("ENERGYLEAF_DRIVER: [VALUE:%s]"),output);
                ResponseTime_P(PSTR(",\"ENERGYLEAF\":{\"CMD\":\"PRINT - DRIVER\"}}"));
            } 
        } else if(*cp == 'm') {
            //MANUAL
            ++cp;
             if(*cp == 's') {
                //SEND
                energyleaf->smlUpdate = true;
                XsnsXdrvCall(FUNC_ENERGYLEAF_SEND);
                ResponseTime_P(PSTR(",\"ENERGYLEAF\":{\"CMD\":\"MANUAL - SEND\"}}"));
             }
        } else if(*cp == 'f') {
            energyleaf->needScript = true;
            energyleaf->active = energyleafRequestTokenIntern() == ENERGYLEAF_ERROR::NO_ERROR ? true : false;
            ResponseTime_P(PSTR(",\"ENERGYLEAF\":{\"CMD\":\"FORCE SCRIPT\"}}"));
        } else if(*cp == 'a') {
            //ADJUST / RESET
            energyleaf->retryCounter = 0;
            ResponseTime_P(PSTR(",\"ENERGYLEAF\":{\"CMD\":\"RESET RETRYCOUNTER\"}}"));
        } else if(*cp == 'i') {
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER: run [%s]"),energyleaf->running ? "true" : "false");
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER: token [%s]"),energyleaf->accessToken);
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER: expires in [%d] seconds"),energyleaf->expiresIn);
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER: active [%s]"),energyleaf->active ? "true" : "false");
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER: debug [%s]"),energyleaf->debug   ? "true" : "false");
            ResponseTime_P(PSTR(",\"ENERGYLEAF\":{\"CMD\":\"PRINTING INFORMATION OF DRIVER\"}}"));
        }
    } else {
        ret = false;
    }
    return ret;
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xdrv159(uint32_t function) {
    bool result = true;

    if(FUNC_INIT == function) {
        energyleafInit();
    } else {
        switch(function) {
            case FUNC_EVERY_SECOND:
                if(energyleaf->wifiEnable) {
                    //Without wifi a run trough the code will waste instructions.
                    energyleafEverySecond();
                }
            break;
            case FUNC_NETWORK_UP:
                if(WiFi.isConnected()) {
                    energyleaf->wifiEnable = true;
                }
            break;
            case FUNC_NETWORK_DOWN:
                    energyleaf->wifiEnable = false;
            break;
            case FUNC_SET_DEVICE_POWER: 
                energyleafDevicePower();
            break;
            case FUNC_COMMAND_DRIVER:
            if(XdrvMailbox.index == XDRV_159) {
                result = XDRV_159_cmd();
            }
            break;
        }
    }
    return result;
}

#endif
