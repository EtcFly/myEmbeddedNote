
#ifndef _MQTT_API_H
#define _MQTT_API_H














#ifdef _cplusplus
extern "C" {
#endif
	
	
int8_t mqtt_connect(uint8_t desp_ip[4], uint16_t keepAlive, uint8_t socketN, MQTTMessage *msg);	
int8_t mqtt_publish(int8_t qos, uint8_t dup, uint8_t retain, uint16_t id, uint8_t *topicName, uint8_t *poly);	
int8_t mqtt_subscribe(const char * topicFilter, int8_t qos, messageHandler handler);	
int8_t mqtt_unSubscribe(const char* topicFilter);	
int8_t mqtt_diconnect(void);

	
#ifdef _cplusplus	
}
#endif

#endif


