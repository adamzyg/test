#include <stdio.h>
#include <cjson/cJSON.h>

int main()
{
    char *json_string = "{\"test\": \"hello\"}";
    cJSON *json, *json_value, *json_timestamp;
    json = cJSON_Parse(json_string);

    if (cJSON_GetObjectItemCaseSensitive(json, "haha") != NULL)
    {
        printf("no haha item\n");
    }
    if (cJSON_GetObjectItemCaseSensitive(json, "test") != NULL)
    {
        printf("test item: %s\n", cJSON_GetObjectItemCaseSensitive(json, "test")->valuestring);
    }
    cJSON_Delete(json);
    return 0;
}
