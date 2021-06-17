#include <stdio.h>
#include <cjson/cJSON.h>

int main()
{
    char *json_string = "{\"test\": [\"test1\", \"test2\"]}";
    cJSON *json, *json_value, *json_timestamp;
    json = cJSON_Parse(json_string);

    if (cJSON_GetObjectItemCaseSensitive(json, "haha") != NULL)
    {
        printf("no haha item\n");
    }
    if (cJSON_GetObjectItemCaseSensitive(json, "test") != NULL)
    {
        cJSON *test = NULL;
        cJSON_ArrayForEach(test, cJSON_GetObjectItemCaseSensitive(json, "test"))
        {
            cJSON_Delete(test);
            break;
        }
    }
    printf("current json: %s\n", cJSON_Print(json));
    cJSON_Delete(json);
    return 0;
}
