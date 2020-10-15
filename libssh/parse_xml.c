#define _GNU_SOURCE

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <cJSON.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#define DEFAULT_XML_FILE "manifest.xml"

typedef struct filenode
{
    char *name;
    char *type;
    char *path;
    char *version;
    char *checksum;
    struct filenode *next;
} FileNode;

typedef struct filelist
{
    FileNode *head;
    FileNode *curr;
} FileList;

FileList file_list = {.head = NULL, .curr = NULL};

int substr(char *s1, char *s2)
{
    if ((s1 == NULL) || (s2 == NULL))
        return -1;
    char *s3 = strcasestr(s1,s2);
    if(s3 == NULL)
        return -1;
    return strlen(s1)-strlen(s3);
}

static int parse_manifest(const char *file_name)
{
    assert(file_name);

    xmlDocPtr doc;   //xml整个文档的树形结构
    xmlNodePtr cur, manifest, builds, build, file;  //xml节点
    xmlChar *id;     //phone id

    //获取树形结构
    doc = xmlParseFile(file_name);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse xml file:%s\n", file_name);
        goto FAILED;
    }

    //获取根节点
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL) {
        fprintf(stderr, "Root is empty.\n");
        goto FAILED;
    }

    if ((xmlStrcmp(cur->name, (const xmlChar *)"xml"))) {
        fprintf(stderr, "The root is not xml.\n");
        goto FAILED;
    }

    //遍历处理根节点的每一个子节点
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"manifest"))) {
            manifest = cur;
            break;
        }
        cur = cur->next;
    }
    if ((xmlStrcmp(manifest->name, (const xmlChar *)"manifest"))) {
        fprintf(stderr, "The xml file %s is invalid, no manifest child node, is %s.\n", file_name, manifest->name);
        goto FAILED;
    }
    
    cur = manifest->xmlChildrenNode;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"builds"))) {
            builds = cur;
            break;
        }
        cur = cur->next;
    }
    
    cur = builds->xmlChildrenNode;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"build"))) {
            build = cur;
            break;
        }
        cur = cur->next;
    }
    
    cur = build->xmlChildrenNode;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"file"))) {
            file = cur;
            const char *file_name = xmlGetProp(file, "fileName");
            const char *file_version = xmlGetProp(file, "fileVersion");
            const char *file_path = xmlGetProp(file, "path");
            const char *file_checksum = xmlGetProp(file, "checksum");
            const char *file_type = NULL;
            if (substr((char *)file_name, ".BIN") != -1)
            {
                file_type = "boot";
            }
            else if (substr((char *)file_name, ".ub") != -1)
            {
                file_type = "kernel";
            }
            else if (substr((char *)file_name, ".bit") != -1)
            {
                file_type = "fpga";
            }
            else
            {
                file_type = "unknown";
            }

            FileNode *cur_file = malloc(sizeof(FileNode));
            cur_file->name = strdup(file_name);
            cur_file->version = strdup(file_version);
            cur_file->type = strdup(file_type);
            cur_file->path = strdup(file_path);
            cur_file->checksum = strdup(file_checksum);
            cur_file->next = NULL;
                
            if (file_list.head == NULL)
            {
                file_list.head = cur_file;
                file_list.curr = cur_file;
            }
            else
            {
                file_list.curr->next = cur_file;
                file_list.curr = file_list.curr->next;
            }
        }
        cur = cur->next;
    }

    xmlFreeDoc(doc);
    return 0;
FAILED:
    if (doc) {
        xmlFreeDoc(doc);
    }
    return -1;
}

int free_file_list()
{
    if (file_list.head == NULL)
        return 0;

    FileNode *curr = file_list.head;
    FileNode *next = file_list.head->next; 
    while (1)
    {
        printf("DEBUG: free %s\n", curr->name);
        free(curr->name);
        free(curr->version);
        free(curr->type);
        free(curr->path);
        free(curr->checksum);
        free(curr);

        if (next == NULL)
        {
            file_list.head = NULL;
            file_list.curr = NULL;
            break;
        }
        else
        {
            curr = next;
            next = next->next;
        }
    }
}
char *dest_dir = "/tmp/download";
int report_download_file()
{
    int rc = 0;
    cJSON *root =  cJSON_CreateObject();
    cJSON *files = cJSON_CreateArray();
    cJSON *file = NULL;
    char abs_file_path[64] = {0};

    cJSON_AddItemToObject(root, "files", files);

    for (FileNode *curr = file_list.head; curr != NULL; curr = curr->next)
    {
        file = cJSON_CreateObject();
        cJSON_AddItemToArray(files, file);
        cJSON_AddItemToObject(file, "file-name", cJSON_CreateString(curr->name));
        cJSON_AddItemToObject(file, "type", cJSON_CreateString(curr->type));
        sprintf(abs_file_path, "%s/%s", dest_dir, curr->path);
        cJSON_AddItemToObject(file, "path", cJSON_CreateString(abs_file_path));
        cJSON_AddItemToObject(file, "version", cJSON_CreateString(curr->version));
    }
    char *string = cJSON_Print(root);
    printf("%s\n", string);
}

int main(int argc, char*argv[])
{
    char *xml_file = DEFAULT_XML_FILE;
    int rc = 0;

    if (argc == 2) {
        xml_file = argv[1];
    }

    if ((rc = parse_manifest(xml_file)) != 0) {
        fprintf(stderr, "Failed to parse manifest.\n");
        goto cleanup;
    }

    for (FileNode *curr = file_list.head; curr != NULL; curr = curr->next)
    {
        printf("file %s: version(%s) type(%s) path(%s) checksum(%s)\n", curr->name, curr->version, curr->type, curr->path, curr->checksum);
    }
    report_download_file();

cleanup:
    free_file_list();
    return rc;
}
