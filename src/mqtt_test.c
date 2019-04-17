#include "msp430.h"

#include "uart_hub.h"

#include "mqtt.h"
#include "mqtt_pal.h"

#include "modem.h"

/*
void on_mqtt_publish(void** state, struct mqtt_response_publish *publish) {
    __no_operation();
}

int main () {
    enum MQTTErrors mqtt_error;
    volatile char mqtt_error_message[128];
    const uint8_t payload[5] = {'A', 'L', 'I', 'V', 'E'};

    uint8_t sendbuf[256];
    uint8_t recvbuf[256];

    WDTCTL = WDTPW | WDTHOLD;

    __bis_SR_register(GIE);

    uart_hub_init();
    uart_hub_select(1);


    if (modem_init()) {
        __no_operation();
    }

    if (modem_connect("2.37.198.126", "1883")) {
        __no_operation();
    }

    struct mqtt_client client;

    if ((mqtt_error = mqtt_init(&client, 0, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), on_mqtt_publish)) != MQTT_OK) {
        strcpy((const char *) mqtt_error_message, mqtt_error_str(mqtt_error));
        __no_operation();
    }

    if ((mqtt_error = mqtt_connect(&client, "pmcu-1", NULL, NULL, 0, NULL, NULL, 0, 400)) != MQTT_OK) {
        strcpy((const char *) mqtt_error_message, mqtt_error_str(mqtt_error));
        __no_operation();
    }
    mqtt_sync(&client);


    if (mqtt_publish(&client, "pmcu/1", (void *)payload, sizeof(payload), MQTT_PUBLISH_QOS_0) != MQTT_OK) {
        strcpy((const char *) mqtt_error_message, mqtt_error_str(mqtt_error));
        __no_operation();
    }
    mqtt_sync(&client);

    modem_disconnect();

    __bis_SR_register(LPM0_bits);
    __no_operation();
}
*/

