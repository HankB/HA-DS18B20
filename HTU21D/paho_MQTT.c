#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"
#include "paho_MQTT.h"

// gcc -Wall -o paho_MQTT -lpaho-mqtt3c paho_MQTT.c

#define ADDRESS     "tcp://oak:1883"
#define CLIENTID    "ExampleClientPub"
//#define TOPIC       "home_automation/niwot/familyroom/msg"
//#define PAYLOAD     "Hello World!"
//#define QOS         1
#define TIMEOUT     10000L

static MQTTClient client;
static MQTTClient_connectOptions conn_opts;
static MQTTClient_message pubmsg;
static MQTTClient_deliveryToken token;

/** @file routines to publish to an MQTT server
 * It assumes that only one connection will be established
 * at any given time and will return an error (-1 = MQTTCLIENT_FAILURE)
 * if a second connection is attempted before cleaning up the first.
 */

/** @brief
 * Connect to an MQTT server
 * @param address is of the form "//tcp://<host>:port" (e.g "tcp://oak:1883")
 * @param client_id to use for the session
 * @retval 0 on success or one of the errors listed in MQTTClient.h or
 *  -1 tro indicate that a previous session exists.
 */
int init_MQTT(const char* address, const char* client_id)
{
    conn_opts = MQTTClient_connectOptions_initializer;
    pubmsg = MQTTClient_message_initializer;
    int rc;

    if(client != 0)
    {
        printf("Existing connection not cleaned up\n");
        return MQTTCLIENT_FAILURE;
    }

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        return rc;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    /*(
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    */

    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s for client with ClientID: %s\n",
            (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}