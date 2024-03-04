#ifdef USE_ENERGYLEAF
#define XDRV_159 159

#include <include/tasmota.h>
#include <cstdint>
#include <WiFiClientSecureLightBearSSL.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include <include/energyleaf/Energyleaf.pb.h>

const unsigned char TA0_DN[] PROGMEM = {
        0x30, 0x4F, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
        0x02, 0x55, 0x53, 0x31, 0x29, 0x30, 0x27, 0x06, 0x03, 0x55, 0x04, 0x0A,
        0x13, 0x20, 0x49, 0x6E, 0x74, 0x65, 0x72, 0x6E, 0x65, 0x74, 0x20, 0x53,
        0x65, 0x63, 0x75, 0x72, 0x69, 0x74, 0x79, 0x20, 0x52, 0x65, 0x73, 0x65,
        0x61, 0x72, 0x63, 0x68, 0x20, 0x47, 0x72, 0x6F, 0x75, 0x70, 0x31, 0x15,
        0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x0C, 0x49, 0x53, 0x52,
        0x47, 0x20, 0x52, 0x6F, 0x6F, 0x74, 0x20, 0x58, 0x31
    };

const unsigned char TA0_RSA_N[] PROGMEM = {
        0xAD, 0xE8, 0x24, 0x73, 0xF4, 0x14, 0x37, 0xF3, 0x9B, 0x9E, 0x2B, 0x57,
        0x28, 0x1C, 0x87, 0xBE, 0xDC, 0xB7, 0xDF, 0x38, 0x90, 0x8C, 0x6E, 0x3C,
        0xE6, 0x57, 0xA0, 0x78, 0xF7, 0x75, 0xC2, 0xA2, 0xFE, 0xF5, 0x6A, 0x6E,
        0xF6, 0x00, 0x4F, 0x28, 0xDB, 0xDE, 0x68, 0x86, 0x6C, 0x44, 0x93, 0xB6,
        0xB1, 0x63, 0xFD, 0x14, 0x12, 0x6B, 0xBF, 0x1F, 0xD2, 0xEA, 0x31, 0x9B,
        0x21, 0x7E, 0xD1, 0x33, 0x3C, 0xBA, 0x48, 0xF5, 0xDD, 0x79, 0xDF, 0xB3,
        0xB8, 0xFF, 0x12, 0xF1, 0x21, 0x9A, 0x4B, 0xC1, 0x8A, 0x86, 0x71, 0x69,
        0x4A, 0x66, 0x66, 0x6C, 0x8F, 0x7E, 0x3C, 0x70, 0xBF, 0xAD, 0x29, 0x22,
        0x06, 0xF3, 0xE4, 0xC0, 0xE6, 0x80, 0xAE, 0xE2, 0x4B, 0x8F, 0xB7, 0x99,
        0x7E, 0x94, 0x03, 0x9F, 0xD3, 0x47, 0x97, 0x7C, 0x99, 0x48, 0x23, 0x53,
        0xE8, 0x38, 0xAE, 0x4F, 0x0A, 0x6F, 0x83, 0x2E, 0xD1, 0x49, 0x57, 0x8C,
        0x80, 0x74, 0xB6, 0xDA, 0x2F, 0xD0, 0x38, 0x8D, 0x7B, 0x03, 0x70, 0x21,
        0x1B, 0x75, 0xF2, 0x30, 0x3C, 0xFA, 0x8F, 0xAE, 0xDD, 0xDA, 0x63, 0xAB,
        0xEB, 0x16, 0x4F, 0xC2, 0x8E, 0x11, 0x4B, 0x7E, 0xCF, 0x0B, 0xE8, 0xFF,
        0xB5, 0x77, 0x2E, 0xF4, 0xB2, 0x7B, 0x4A, 0xE0, 0x4C, 0x12, 0x25, 0x0C,
        0x70, 0x8D, 0x03, 0x29, 0xA0, 0xE1, 0x53, 0x24, 0xEC, 0x13, 0xD9, 0xEE,
        0x19, 0xBF, 0x10, 0xB3, 0x4A, 0x8C, 0x3F, 0x89, 0xA3, 0x61, 0x51, 0xDE,
        0xAC, 0x87, 0x07, 0x94, 0xF4, 0x63, 0x71, 0xEC, 0x2E, 0xE2, 0x6F, 0x5B,
        0x98, 0x81, 0xE1, 0x89, 0x5C, 0x34, 0x79, 0x6C, 0x76, 0xEF, 0x3B, 0x90,
        0x62, 0x79, 0xE6, 0xDB, 0xA4, 0x9A, 0x2F, 0x26, 0xC5, 0xD0, 0x10, 0xE1,
        0x0E, 0xDE, 0xD9, 0x10, 0x8E, 0x16, 0xFB, 0xB7, 0xF7, 0xA8, 0xF7, 0xC7,
        0xE5, 0x02, 0x07, 0x98, 0x8F, 0x36, 0x08, 0x95, 0xE7, 0xE2, 0x37, 0x96,
        0x0D, 0x36, 0x75, 0x9E, 0xFB, 0x0E, 0x72, 0xB1, 0x1D, 0x9B, 0xBC, 0x03,
        0xF9, 0x49, 0x05, 0xD8, 0x81, 0xDD, 0x05, 0xB4, 0x2A, 0xD6, 0x41, 0xE9,
        0xAC, 0x01, 0x76, 0x95, 0x0A, 0x0F, 0xD8, 0xDF, 0xD5, 0xBD, 0x12, 0x1F,
        0x35, 0x2F, 0x28, 0x17, 0x6C, 0xD2, 0x98, 0xC1, 0xA8, 0x09, 0x64, 0x77,
        0x6E, 0x47, 0x37, 0xBA, 0xCE, 0xAC, 0x59, 0x5E, 0x68, 0x9D, 0x7F, 0x72,
        0xD6, 0x89, 0xC5, 0x06, 0x41, 0x29, 0x3E, 0x59, 0x3E, 0xDD, 0x26, 0xF5,
        0x24, 0xC9, 0x11, 0xA7, 0x5A, 0xA3, 0x4C, 0x40, 0x1F, 0x46, 0xA1, 0x99,
        0xB5, 0xA7, 0x3A, 0x51, 0x6E, 0x86, 0x3B, 0x9E, 0x7D, 0x72, 0xA7, 0x12,
        0x05, 0x78, 0x59, 0xED, 0x3E, 0x51, 0x78, 0x15, 0x0B, 0x03, 0x8F, 0x8D,
        0xD0, 0x2F, 0x05, 0xB2, 0x3E, 0x7B, 0x4A, 0x1C, 0x4B, 0x73, 0x05, 0x12,
        0xFC, 0xC6, 0xEA, 0xE0, 0x50, 0x13, 0x7C, 0x43, 0x93, 0x74, 0xB3, 0xCA,
        0x74, 0xE7, 0x8E, 0x1F, 0x01, 0x08, 0xD0, 0x30, 0xD4, 0x5B, 0x71, 0x36,
        0xB4, 0x07, 0xBA, 0xC1, 0x30, 0x30, 0x5C, 0x48, 0xB7, 0x82, 0x3B, 0x98,
        0xA6, 0x7D, 0x60, 0x8A, 0xA2, 0xA3, 0x29, 0x82, 0xCC, 0xBA, 0xBD, 0x83,
        0x04, 0x1B, 0xA2, 0x83, 0x03, 0x41, 0xA1, 0xD6, 0x05, 0xF1, 0x1B, 0xC2,
        0xB6, 0xF0, 0xA8, 0x7C, 0x86, 0x3B, 0x46, 0xA8, 0x48, 0x2A, 0x88, 0xDC,
        0x76, 0x9A, 0x76, 0xBF, 0x1F, 0x6A, 0xA5, 0x3D, 0x19, 0x8F, 0xEB, 0x38,
        0xF3, 0x64, 0xDE, 0xC8, 0x2B, 0x0D, 0x0A, 0x28, 0xFF, 0xF7, 0xDB, 0xE2,
        0x15, 0x42, 0xD4, 0x22, 0xD0, 0x27, 0x5D, 0xE1, 0x79, 0xFE, 0x18, 0xE7,
        0x70, 0x88, 0xAD, 0x4E, 0xE6, 0xD9, 0x8B, 0x3A, 0xC6, 0xDD, 0x27, 0x51,
        0x6E, 0xFF, 0xBC, 0x64, 0xF5, 0x33, 0x43, 0x4F
    };

const unsigned char TA0_RSA_E[] PROGMEM  = {
        0x01, 0x00, 0x01
    };

const br_x509_trust_anchor TAs PROGMEM  = {
                { (unsigned char *)TA0_DN, sizeof TA0_DN },
                BR_X509_TA_CA,
                {
                        BR_KEYTYPE_RSA,
                        { .rsa = {
                                (unsigned char *)TA0_RSA_N, sizeof TA0_RSA_N,
                                (unsigned char *)TA0_RSA_E, sizeof TA0_RSA_E,
                        } }
                }
    };
const uint8_t TA_SIZE PROGMEM = 1;

const char ENERGYLEAF_DATANAME[] PROGMEM = "ENERGYLEAF_KWH";

const char ENERGYLEAF_HOST[] = "admin.energyleaf.de";
const uint16_t ENERGYLEAF_PORT = 443;
const char ENERGYLEAF_POST_DATA[] PROGMEM = "POST /api/v1/sensor_input HTTP/1.1\r\n"
                                     "Host: %s\r\n"
                                     "Content-Type: application/x-protobuf\r\n"
                                     "Content-Length: %d\r\n\r\n";

const char ENERGYLEAF_POST_AUTH[] PROGMEM = "POST /api/v1/token HTTP/1.1\r\n"
                                     "Host: %s\r\n"
                                     "Content-Type: application/x-protobuf\r\n"
                                     "Content-Length: %d\r\n\r\n";  


struct ENERGYLEAF_STATE {
    bool active = false;    
    bool init = false;
    char access_token[45];       
    uint32_t expires_in = -1;  
    bool certLoaded = false;       
    bool needScript = false;   
    const SensorType type = SensorType_DIGITAL_ELECTRICITY;                   
} energyleaf;

struct ENERGYLEAF_MEM {
    float value = 0;
} energyleaf_mem;

BearSSL::WiFiClientSecure_light *energyleafClient = nullptr;

void energyleafInit(void) {
    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER: INIT 0/2"));
    if(!energyleafClient) {
        AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER: INIT CLIENT"));
        energyleafClient = new BearSSL::WiFiClientSecure_light(1024,1024);
        if(!energyleaf.certLoaded){
            AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER: LOADING CERT"));
            energyleafClient->setTrustAnchor(&TAs,TA_SIZE);
            energyleaf.certLoaded = true;
        }
        if(strstr_P(glob_script_mem.script_ram,PSTR(">D\nscript error must start with >D"))) {
            AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER: DEFAULT SCRIPT IS LOADED - NEED TO LOAD SCRIPT FOR THIS SENSOR"));
            energyleaf.needScript = true;
        }
    }
    
    AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER: INIT 1/2"));
    //energyleafRequestToken(energyleaf.needScript);
}

void energyleafSendData() {
    //call energyleafSendDataIntern
}

bool energyleafSendDataIntern() {

}

bool energyleafRequestTokenIntern(bool script) {
    /**
     * Function flows:
     * [script = true && initial = true & expired = UNKNOWN] Prepare TokenRequest -> Send TokenRequest -> Receive Header & Body -> Generate TokenResponse -> Verify -> Save Script -> Prepare ScriptAcceptedRequest -> Send ScriptAcceptedRequest -> Receive Header & Body -> Generate ScriptAcceptedResponse -> Verify (what should happen?)
     * [script = true && initial = false & expired = REGARDLESS] Prepare TokenRequest -> Send TokenRequest -> Receive Header & Body -> Generate TokenResponse -> Verify -> Save Script -> Prepare ScriptAcceptedRequest -> Send ScriptAcceptedRequest -> Receive Header & Body -> Generate ScriptAcceptedResponse -> Verify (what should happen?) (did this need a ScriptAccepted?)
     * [script = false && initial = true & expired = UNKNOWN] Prepare TokenRequest -> Send TokenRequest -> Receive Header & Body -> Generate TokenResponse -> Verify -> (if Script available) Save Script -> Prepare ScriptAcceptedRequest -> Send ScriptAcceptedRequest -> Receive Header & Body -> Generate ScriptAcceptedResponse -> Verify (what should happen?) (did this need a ScriptAccepted?)
     * [script = false && initial = false & expired = REGARDLESS] Prepare TokenRequest -> Send TokenRequest -> Receive Header & Body -> Generate TokenResponse -> Verify -> (if Script available) Save Script -> Prepare ScriptAcceptedRequest -> Send ScriptAcceptedRequest -> Receive Header & Body -> Generate ScriptAcceptedResponse -> Verify (what should happen?) (did this need a ScriptAccepted?)
     */
    if(energyleafClient) {
        //fresh start and the default script is loaded, not the script of the sensor.
        AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: PREPARING REQUEST FOR TOKEN%s"),script ? " AND SCRIPT" : "");
        if(WiFi.status() == WL_CONNECTED) {
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: WIFI IS AVAILABLE"));

            bool state = false;
            bool chunked = false;
            uint16_t bodySize = 0;
            {  
                //Prepare TokenRequest
                uint8_t bufferTokenRequest[TokenRequest_size];
                pb_ostream_t streamTokenRequestOut;
                {
                    TokenRequest tokenRequest = TokenRequest_init_default;
                    //collect the MAC of this sensor
                    memcpy(tokenRequest.client_id, WiFi.macAddress().c_str(), sizeof(tokenRequest.client_id));
                    AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: USED MAC [%s]"),tokenRequest.client_id);
                    //set the type of this sensor
                    tokenRequest.type = energyleaf.type;
                    //if true it forces the server to (re-)send the script in the response
                    tokenRequest.need_script = script;

                    streamTokenRequestOut = pb_ostream_from_buffer(bufferTokenRequest, sizeof(bufferTokenRequest));

                    AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: NEED SCRIPT [%s]"),tokenRequest.need_script ? "true" : "false");

                
                    state = pb_encode(&streamTokenRequestOut,TokenRequest_fields, &tokenRequest);
                }

                if(!state) {
                    AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - COULD NOT CREATE PACKAGE"));
                    return false;
                }

                AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: pb_encode successful"));

                //Send TokenRequest and process received header
                {
                    state = energyleaf.certLoaded && energyleafClient->connect(ENERGYLEAF_HOST,ENERGYLEAF_PORT);
                    if(!state) {
                        if(energyleafClient->connected()){
                            energyleafClient->stop(); 
                        }
                        AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - COULD NOT CONNECT TO SERVICE"));
                        return false;
                    }

                    AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: CONNECTED TO ENERGYLEAF SERIVCE"));

                    energyleafClient->write_P(PSTR("POST /api/v1/token HTTP/1.1\r\n"));
                    energyleafClient->write_P(PSTR("Host: "));
                    energyleafClient->write(ENERGYLEAF_HOST);
                    energyleafClient->write_P(PSTR("\r\n"));
                    energyleafClient->write_P(PSTR("Content-Type: application/x-protobuf\r\n"));
                    energyleafClient->write_P(PSTR("Content-Length: "));
                    energyleafClient->print(streamTokenRequestOut.bytes_written);
                    energyleafClient->write_P(PSTR("\r\n\r\n"));

                    energyleafClient->write(bufferTokenRequest, streamTokenRequestOut.bytes_written);

                    char header[128];
                    char headerStatus[4];
                    state = false;

                    while(true) {
                        int l = energyleafClient->readBytesUntil('\n',header,sizeof(header));
                        if(l<=1) {
                            break;
                        }

                        header[l-1] = 0;

                        AddLog(LOG_LEVEL_DEBUG,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST_Header: %s"),header);

                        if(strstr_P(header,PSTR("HTTP/1.1"))) {
                            strncpy(headerStatus,&header[9],3);
                            headerStatus[3] = '\0';
                            uint16_t headerStatusCode = atoi(headerStatus);
                            if(headerStatusCode >= 200 && headerStatusCode <= 299) {
                                AddLog(LOG_LEVEL_DEBUG,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: GOT A %d STATUS"),headerStatusCode);
                            } else {
                                AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT A %d STATUS"),headerStatusCode);
                                //which code indicates no connection or did we need that?
                            }
                            continue;
                        }

                        if(strstr_P(header,PSTR("Content-Length:"))){
                            char contentLength[4];
                            strncpy(contentLength,&header[16],3);
                            contentLength[3] = '\0';
                            bodySize = atoi(contentLength);
                            chunked = false;
                            continue;
                        }

                        if(strstr_P(header,PSTR("Transfer-Encoding: chunked"))) {
                            chunked = true;
                            continue;
                        }

                        if(strstr_P(header,PSTR("Content-Type: application/x-protobuf"))) {
                            state = true;
                            continue;
                        }
                    }
                }

                if(!state) {
                    energyleafClient->stop(); 
                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT WRONG CONTENT-TYPE FROM SERVICE"));
                    return false;
                }
            }

            uint8_t bufferScriptAcceptedRequest[ScriptAcceptedRequest_size];
            pb_ostream_t streamScriptAcceptedRequestOut;

            {
                //Process received body and generate TokenResponse from it
                TokenResponse tokenResponse = TokenResponse_init_default;
                {
                    uint8_t bufferTokenResponse[TokenResponse_size];
                    int currentSize = 0;
                    {
                        if(chunked) {
                            while(true) {
                                char chunkSize[16];
                                int l = energyleafClient->readBytesUntil('\n',chunkSize,sizeof(chunkSize));
                                if(l<=0) {
                                    break;
                                }

                                chunkSize[l-1] = 0;
                                int chunkSizeI = strtol(chunkSize,NULL,16);

                                if(chunkSizeI == 0) {
                                    break;
                                }

                                char chunkData[chunkSizeI];
                                l = energyleafClient->readBytes(chunkData,chunkSizeI);
                                if(l<=0) {
                                    break;
                                }

                                if(currentSize + l <= TokenResponse_size) {
                                    memcpy(bufferTokenResponse + currentSize, chunkData, l);
                                    currentSize += l;
                                } else {
                                    AddLog(LOG_LEVEL_ERROR,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - NOT ENOUGH SPACE FOR DATA"));
                                    energyleafClient->stop(); 
                                    return false;
                                }
                            }
                        } else {
                            //currently not tested, therefore better use chunked data
                            currentSize = bodySize;
                            while(true) {
                                int l = energyleafClient->readBytesUntil('\n',bufferTokenResponse,currentSize);
                                if(l==1) {
                                    break;
                                }
                            }
                        }

                        state = currentSize > 0;
                    }

                    if(!state) {
                        energyleafClient->stop(); 
                        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT PACKAGE WITH WRONG SIZE [SIZE:%d]"),currentSize);
                        return false;
                    }
                    AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: [SIZE:%d]"),currentSize);

                    {
                        pb_istream_t streamTokenResponseIn = pb_istream_from_buffer(bufferTokenResponse,currentSize);
                        state = pb_decode(&streamTokenResponseIn,TokenResponse_fields, &tokenResponse);
                    }

                    if(!state) {
                        energyleafClient->stop(); 
                        AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT WRONG CONTENT-TYPE FROM SERVICE"));
                        return false;
                    }
                }

                state = tokenResponse.status >= 200 && tokenResponse.status <= 299 && tokenResponse.has_access_token;
                if(!state) {
                    energyleafClient->stop(); 
                    if(tokenResponse.has_status_message) {
                        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - ERROR WITH %d STATUS [%s]"),tokenResponse.status, tokenResponse.status_message);
                    } else {
                        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - ERROR WITH %d STATUS"),tokenResponse.status);
                    }
                    return false;
                }

                //Store Script and the Parameter from the TokenResponse
                {
                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: SUCCESSFUL"));
                    AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: [TOKEN:%s]"),tokenResponse.access_token);
                    AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: [STATUS:%d]"),tokenResponse.status);
                    AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: [SCRIPT:%s]"),tokenResponse.has_script ? "true" : "false");
                    AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: [SCRIPT:%s]"),tokenResponse.script);

                    
                    strcpy(energyleaf.access_token, tokenResponse.access_token);
                    energyleaf.expires_in = tokenResponse.expires_in;

                    if(tokenResponse.has_script) {
                        state = sizeof(tokenResponse.script) < glob_script_mem.script_size;
                        if(!state) {
                            AddLog(LOG_LEVEL_ERROR,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - SCRIPT IS TO LARGE"));
                            energyleafClient->stop(); 
                            return false;
                        }

                        uint8_t *script_ex_ptr = (uint8_t*)glob_script_mem.script_ram;

                        uint8_t sc_state = bitRead(Settings->rule_enabled,0);
                        bitWrite(Settings->rule_enabled,0,0);

                        memcpy(script_ex_ptr, tokenResponse.script, sizeof(tokenResponse.script));

                        script_ex_ptr = nullptr;
                        bitWrite(Settings->rule_enabled, 0, sc_state);
                        SaveScript();
                        SaveScriptEnd();
                    } else {
                        if(script) {
                            AddLog(LOG_LEVEL_ERROR,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - SCRIPT REQUEST BUT NOT RECEIVED "));
                            energyleafClient->stop(); 
                            return false;
                        }
                    }
                }

                //Prepare ScriptAcceptedRequest
                {
                    ScriptAcceptedRequest scriptAcceptedRequest = ScriptAcceptedRequest_init_default;

                    memcpy(scriptAcceptedRequest.access_token, energyleaf.access_token, sizeof(energyleaf.access_token));

                    streamScriptAcceptedRequestOut = pb_ostream_from_buffer(bufferScriptAcceptedRequest, sizeof(bufferScriptAcceptedRequest));

                    AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: Creating SAR-Package"));
                
                    state = pb_encode(&streamScriptAcceptedRequestOut,ScriptAcceptedRequest_fields, &scriptAcceptedRequest);
                }

                if(!state) {
                    AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - COULD NOT CREATE SAR-PACKAGE"));
                    return false;
                }

                AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: pb_encode successful"));

                //Send ScriptAcceptedRequest and process received header
                {
                    state = energyleaf.certLoaded && energyleafClient->connected();
                    if(!state) {
                        if(energyleafClient->connected()){
                            energyleafClient->stop(); 
                        }
                        AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - COULD NOT HOLD CONNECTION TO SERVICE"));
                        return false;
                    }

                    energyleafClient->write_P(PSTR("POST /api/v1/script_accepted HTTP/1.1\r\n"));
                    energyleafClient->write_P(PSTR("Host: "));
                    energyleafClient->write(ENERGYLEAF_HOST);
                    energyleafClient->write_P(PSTR("\r\n"));
                    energyleafClient->write_P(PSTR("Content-Type: application/x-protobuf\r\n"));
                    energyleafClient->write_P(PSTR("Content-Length: "));
                    energyleafClient->print(streamScriptAcceptedRequestOut.bytes_written);
                    energyleafClient->write_P(PSTR("\r\n\r\n"));

                    energyleafClient->write(bufferScriptAcceptedRequest, streamScriptAcceptedRequestOut.bytes_written);

                    char header[128];
                    char headerStatus[4];
                    state = false;

                    while(true) {
                        int l = energyleafClient->readBytesUntil('\n',header,sizeof(header));
                        if(l<=1) {
                            break;
                        }

                        header[l-1] = 0;

                        AddLog(LOG_LEVEL_DEBUG,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST_Header: %s"),header);

                        if(strstr_P(header,PSTR("HTTP/1.1"))) {
                            strncpy(headerStatus,&header[9],3);
                            headerStatus[3] = '\0';
                            uint16_t headerStatusCode = atoi(headerStatus);
                            if(headerStatusCode >= 200 && headerStatusCode <= 299) {
                                AddLog(LOG_LEVEL_DEBUG,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: GOT A %d STATUS"),headerStatusCode);
                            } else {
                                AddLog(LOG_LEVEL_INFO,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT A %d STATUS"),headerStatusCode);
                                //which code indicates no connection or did we need that?
                            }
                            continue;
                        }

                        if(strstr_P(header,PSTR("Content-Length:"))){
                            char contentLength[4];
                            strncpy(contentLength,&header[16],3);
                            contentLength[3] = '\0';
                            bodySize = atoi(contentLength);
                            chunked = false;
                            continue;
                        }

                        if(strstr_P(header,PSTR("Transfer-Encoding: chunked"))) {
                            chunked = true;
                            continue;
                        }

                        if(strstr_P(header,PSTR("Content-Type: application/x-protobuf"))) {
                            state = true;
                            continue;
                        }
                    }
                }

                if(!state) {
                    energyleafClient->stop(); 
                    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT WRONG CONTENT-TYPE FROM SERVICE"));
                    return false;
                }
            }

            {
                //Process received body and generate ScriptAcceptedResponse from it
                ScriptAcceptedResponse scriptAcceptedResponse = ScriptAcceptedResponse_init_default;
                {
                    uint8_t bufferScriptAcceptedResponse[ScriptAcceptedResponse_size];
                    int currentSize = 0;
                    {
                        if(chunked) {
                            while(true) {
                                char chunkSize[16];
                                int l = energyleafClient->readBytesUntil('\n',chunkSize,sizeof(chunkSize));
                                if(l<=0) {
                                    break;
                                }

                                chunkSize[l-1] = 0;
                                int chunkSizeI = strtol(chunkSize,NULL,16);

                                if(chunkSizeI == 0) {
                                    break;
                                }

                                char chunkData[chunkSizeI];
                                l = energyleafClient->readBytes(chunkData,chunkSizeI);
                                if(l<=0) {
                                    break;
                                }

                                if(currentSize + l <= ScriptAcceptedResponse_size) {
                                    memcpy(bufferScriptAcceptedRequest + currentSize, chunkData, l);
                                    currentSize += l;
                                } else {
                                    AddLog(LOG_LEVEL_ERROR,PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - NOT ENOUGH SPACE FOR DATA"));
                                    energyleafClient->stop(); 
                                    return false;
                                }
                            }
                        } else {
                            //currently not tested, therefore better use chunked data
                            currentSize = bodySize;
                            while(true) {
                                int l = energyleafClient->readBytesUntil('\n',bufferScriptAcceptedRequest,currentSize);
                                if(l==1) {
                                    break;
                                }
                            }
                        }

                        state = currentSize > 0;
                    }

                    if(!state) {
                        energyleafClient->stop(); 
                        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT PACKAGE WITH WRONG SIZE [SIZE:%d]"),currentSize);
                        return false;
                    }

                    AddLog(LOG_LEVEL_DEBUG, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: [SIZE:%d]"),currentSize);

                    {
                        pb_istream_t streamScriptAcceptedResponseIn = pb_istream_from_buffer(bufferScriptAcceptedResponse,currentSize);
                        state = pb_decode(&streamScriptAcceptedResponseIn,ScriptAcceptedResponse_fields, &scriptAcceptedResponse);
                    }

                    if(!state) {
                        energyleafClient->stop(); 
                        AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - GOT WRONG CONTENT-TYPE FROM SERVICE"));
                        return false;
                    }
                }

                state = scriptAcceptedResponse.status >= 200 && scriptAcceptedResponse.status <= 299;
                if(!state) {
                    energyleafClient->stop(); 
                    if(scriptAcceptedResponse.has_status_message) {
                        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - ERROR WITH %d STATUS [%s]"),scriptAcceptedResponse.status, scriptAcceptedResponse.status_message);
                    } else {
                        AddLog(LOG_LEVEL_ERROR, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: UNSUCCESSFUL - ERROR WITH %d STATUS"),scriptAcceptedResponse.status);
                    }
                    return false;
                }
                
            }            
            energyleafClient->stop(); 
            return true;
        } else {
            AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: NO WIFI TO CREATE A REQUEST - AUTO-RETRY IN SOME TIME"));
            return false;
        }
    } else {
        AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DRIVER_TOKEN_REQUEST: NO CLIENT IS AVAILABLE - RESTART SENSOR OR CONTACT SUPPORT"));
        return false; 
    }
}

bool tryT = false;
void energyleafEverySecond(void) {
    if(!tryT) {
        AddLog(LOG_LEVEL_INFO, PSTR("EV SEC"));
        tryT = energyleafRequestTokenIntern(energyleaf.needScript);
    }
}

void energyleafDevicePower(void) {
    AddLog(LOG_LEVEL_INFO, PSTR("ENERGYLEAF_DEVICE_POWER: START"));
}








/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xdrv159(uint32_t function) {
    bool result = true; //If we need later to return an specific value do it with result.

    if(FUNC_INIT == function) {
        energyleafInit();
    } else {
        switch(function) {
            case FUNC_EVERY_SECOND:
                energyleafEverySecond();
            break;
            case FUNC_SET_DEVICE_POWER: 
                energyleafDevicePower();
            break;
        }
    }

    return result;
}

#endif