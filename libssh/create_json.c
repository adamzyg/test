#define _GNU_SOURCE

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <cJSON.h>

int main()
{
    int rc = 0;
    cJSON *root =  cJSON_CreateObject();
    cJSON *files = cJSON_CreateArray();
    cJSON *file = NULL;

    
    cJSON_AddItemToObject(root, "slot", cJSON_CreateString("primary"));
    cJSON_AddItemToObject(root, "action", cJSON_CreateString("install"));
    cJSON_AddItemToObject(root, "files", files);

    file = cJSON_CreateString("image.ub");
    cJSON_AddItemToArray(files, file);
    file = cJSON_CreateString("BOOT.BIN");
    cJSON_AddItemToArray(files, file);
    file = cJSON_CreateString("system.bit");
    cJSON_AddItemToArray(files, file);

    char *string = cJSON_Print(root);
    printf("%s\n", string);
    cJSON_Delete(root);
    return 0;
}

