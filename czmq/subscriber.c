#include <czmq.h>
#include <cjson/cJSON.h>

int main(int argc, char ** argv) {
    zsock_t *socket = zsock_new_sub("ipc://example.sock", "TOPIC");
    assert(socket);
    cJSON *json, *json_value, *json_timestamp;

    char *topic;
    char *frame;
    zmsg_t *msg;
    int rc = zsock_recv(socket, "sm", &topic, &msg);
    assert(rc == 0);

    zsys_info("Recv on %s", topic);
    while(frame = zmsg_popstr(msg)) {
        zsys_info("> %s", frame);
        json = cJSON_Parse(frame);
        if(NULL == json)
        {
            zsys_info("Error before: [%s]\n", cJSON_GetErrorPtr());
            return -1;
        }
        json_value = cJSON_GetObjectItem(json, "value");
        if(json_value->type == cJSON_Number)
        {
            zsys_info("value: %d\n", json_value->valueint);
        }

        json_timestamp = cJSON_GetObjectItem(json, "timestamp");
        if(json_timestamp->type == cJSON_String)
        {
            //char *op = cJSON_Print(json_timestamp);
            char *op = json_timestamp->valuestring;
            zsys_info("%s\n", op);
            //free(op);
        }

        cJSON_Delete(json);
        free(frame);
    }
    free(topic);
    zmsg_destroy(&msg);

    zsock_destroy(&socket);
    return 0;
}
