
#include "stm32f1xx.h"
#include "mqtt_interface.h"
#include "MQTTClient.h"
#include "mqtt_api.h"






#define TCP_PORT_MQTT                   (1883u)
#define MAX_MQTT_SEND_BUFF               500U


MQTTClient s_client;
Network s_netInterface;
uint8_t s_sendBuf[MAX_MQTT_SEND_BUFF];
uint8_t s_recvBuff[MAX_MQTT_SEND_BUFF];

#ifdef MQTT_USING_CONNECT_DATA
MQTTPacket_connectData s_connectdata = 
{

};
#endif

/**
 *
 *
 */
int8_t mqtt_connect(uint8_t desp_ip[4], uint16_t keepAlive, uint8_t socketN, MQTTMessage *msg)
{	
	NewNetwork(&s_netInterface, socketN);
	ConnectNetwork(&s_netInterface, desp_ip, TCP_PORT_MQTT);
	MQTTClientInit(&s_client, &s_netInterface, 1000, s_sendBuf, sizeof(s_sendBuf), s_recvBuff, sizeof(s_recvBuff));

if ( 
#ifdef MQTT_USING_CONNECT_DATA
	MQTTConnect(&s_client, &s_connectdata);
#else
	 MQTTConnect(&s_client, (void *)0)
#endif
	 == FAILURE)
	{
		return -1;
	}	
	
	return 0;
}

int8_t mqtt_disconnect(void)
{
	int8_t ret;
	
	return ret;
}

#include "string.h"
int8_t mqtt_publish(int8_t qos, uint8_t dup, uint8_t retain, uint16_t id, uint8_t *topicName, uint8_t *poly)
{
	int8_t ret = -1;
	MQTTMessage msg;
	
	msg.dup = dup;
	msg.id = id;
	msg.payload = poly;
	msg.payloadlen = strlen((char *)poly);
	msg.qos = (enum QoS)qos;
	msg.retained = retain;
	
	if (MQTTPublish(&s_client, (const char *)topicName, &msg) == SUCCESSS)
	{
		ret = 0;
	}
	
	return ret;
}

int8_t mqtt_subscribe(const char * topicFilter, int8_t qos, messageHandler handler)
{
	int8_t ret;

    ret = MQTTSubscribe(&s_client, topicFilter, (enum QoS)qos, handler);
	
	return ((ret == SUCCESSS) ? 0 : -1);
}

int8_t mqtt_unSubscribe(const char* topicFilter)
{
	int8_t ret;
	
	ret = MQTTUnsubscribe(&s_client, topicFilter);
	
	return ((ret == SUCCESSS) ? 0 : -1);
}

int8_t mqtt_diconnect(void)
{
	int8_t ret;
	
	ret = MQTTDisconnect(&s_client);
	
	return ((ret == SUCCESSS) ? 0 : -1);
}
