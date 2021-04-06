/*
 * You should be very sure of what you are doing before making use of this feature.
 *
 * Compile with:
 *   gcc -I<path to mosquitto-repo/include> -fPIC -shared mosquitto_connection_events.c -o mosquitto_connection_events.so
 *
 * Use in config with:
 *
 *   plugin /path/to/connection_events.so
 *
 * Note that this only works on Mosquitto 2.0 or later.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mosquitto_broker.h"
#include "mosquitto_plugin.h"
#include "mosquitto.h"
#include "mqtt_protocol.h"

// When building on Windows, the strncasecmp function is unavailable. So we use the _strnicmp and _stricmp instead.
#ifdef _MSC_VER 
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

#define UNUSED(A) (void)(A)
#define AUTH_OPT_TARGET_TOPIC "target_topic"
#define DEFAULT_TARGET_TOPIC "$SYS/disconnections"

static char* target_topic = NULL;
static mosquitto_plugin_id_t *mosq_pid = NULL;

int publish_disconnect_event(const char* device_uuid, const char* username, const char* device_address, int reason) {
	size_t len;
	char buf[512];

	const char* MESSAGE_FORMAT = "{\"event\": \"disconnect\", \"data\":{\"client_id\":\"%s\",\"username\":\"%s\",\"address\":\"%s\", \"reason\":%d}}";
	len = (uint32_t)snprintf(buf, 512, MESSAGE_FORMAT, device_uuid, username, device_address, reason);

	return mosquitto_broker_publish_copy(NULL, target_topic, (int)len, buf, 0, 0, NULL);
}

static int callback_disconnect(int event, void* event_data, void* userdata)
{
	struct mosquitto_evt_disconnect* ed = event_data;
	const char* client_id = mosquitto_client_id(ed->client);
	const char* username = mosquitto_client_username(ed->client);
	const char* addr = mosquitto_client_address(ed->client);
	publish_disconnect_event(client_id, username, addr, ed->reason);
}

int mosquitto_plugin_version(int supported_version_count, const int *supported_versions)
{
	int i;

	for(i=0; i<supported_version_count; i++){
		if(supported_versions[i] == 5){
			return 5;
		}
	}
	return -1;
}

int mosquitto_plugin_init(mosquitto_plugin_id_t* identifier, void** user_data, struct mosquitto_opt* options, int option_count)
{
	int i;

	UNUSED(user_data);
	mosq_pid = identifier;
	
	// Parse Options
	for (i = 0; i < option_count; i++) {
		if (!strcasecmp(options[i].key, AUTH_OPT_TARGET_TOPIC)) {
			target_topic = mosquitto_strdup(options[i].value);
			if (target_topic == NULL) {
				return MOSQ_ERR_NOMEM;
			}
			break;
		}
	}
	if (target_topic == NULL) {
		mosquitto_log_printf(MOSQ_LOG_WARNING, "Warning: Disconnections plugin has no target_topic defined. Default %s will be used.", DEFAULT_TARGET_TOPIC);
		target_topic = mosquitto_strdup(DEFAULT_TARGET_TOPIC);
	}

	return mosquitto_callback_register(mosq_pid, MOSQ_EVT_DISCONNECT, callback_disconnect, NULL, NULL);
}

int mosquitto_plugin_cleanup(void* user_data, struct mosquitto_opt* opts, int opt_count)
{
	mosquitto_free(target_topic);
	target_topic = NULL;
	return mosquitto_callback_unregister(mosq_pid, MOSQ_EVT_DISCONNECT, callback_disconnect, NULL);
}
