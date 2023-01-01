#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "cJSON/cJSON.h"

#define READ_BLOCK_SIZE (4096)

#define _macro_to_str(x) #x
#define _check_expr_print(err,line) fputs("Error["_macro_to_str(line)"]:"err"\n",stderr)
#define check_expr(expr,err) if(!(expr)) {\
    _check_expr_print(err,__LINE__);\
    exit(EXIT_FAILURE);\
}

enum
{
    MODE_PRINT,
    MODE_GET_TYPE,
    MODE_GET_LENGTH,
    MODE_GET_KEYS
};

int main(int argc, char const *argv[])
{
    char* json_str = NULL;
    int json_str_len = 0;
    bool json_str_is_allocated = false;
    char* path_str = NULL;      /** not allocated */
    int mode = MODE_PRINT;
    cJSON* obj = NULL;
    cJSON* key_array_obj = NULL;

    /** parse options */
    int opt;
    while((opt = getopt(argc, (char* const*)argv, "tklj:p:"))!=-1)
    {
        switch (opt)
        {
        case 't':{
            mode = MODE_GET_TYPE;
        } break;
        case 'k':{
            mode = MODE_GET_KEYS;
        } break;
        case 'l':{
            mode = MODE_GET_LENGTH;
        } break;
        case 'j':{
            json_str = optarg;
        } break;
        case 'p':{
            path_str = optarg;
        } break;
        default:
            break;
        }
    }

    /** if json_str is not provided, read from stdin */
    if(!json_str)
    {
        json_str_is_allocated = true;
        int allocated_size = 0;
        int read_offset = 0;
        int read_len = 0;
        do
        {
            read_offset += read_len;
            json_str_len += read_len;
            if(read_offset == allocated_size)
            {
                int old_size = allocated_size;
                allocated_size += READ_BLOCK_SIZE;
                char* new = realloc(json_str,allocated_size);
                check_expr(new, "OOM");
                json_str = new;
            }
        } while ((read_len = fread(json_str+read_offset,1,allocated_size-read_offset,stdin))>0);
    }
    else
    {
        json_str_len = strlen(json_str);
    }

    /** parse obj */
    obj = cJSON_ParseWithLength(json_str,json_str_len);
    /** walk throw path */
    cJSON* c_obj = obj;
    if(path_str)
    {
        char* parse_end = path_str;
        int path_str_len = strlen(path_str);
        while(c_obj && (parse_end != path_str+path_str_len))
        {
            key_array_obj = cJSON_ParseWithLengthOpts(parse_end,path_str_len-(parse_end-path_str),(const char**)&parse_end,false);
            if((!key_array_obj) && (parse_end==path_str+path_str_len))
                break;  /** key_str tail */
            check_expr(cJSON_IsArray(key_array_obj),"Invalid key");
            cJSON* key_obj = cJSON_GetArrayItem(key_array_obj,0);
            if(cJSON_IsString(key_obj)&&cJSON_IsObject(c_obj))
                c_obj = cJSON_GetObjectItemCaseSensitive(c_obj,cJSON_GetStringValue(key_obj));
            else if(cJSON_IsNumber(key_obj)&&cJSON_IsArray(c_obj))
                c_obj = cJSON_GetArrayItem(c_obj,cJSON_GetNumberValue(key_obj));
            else
                c_obj = NULL;
            cJSON_Delete(key_array_obj);
            key_array_obj = NULL;
        }
    }

    /** output */
    switch (mode)
    {
    case MODE_PRINT:{
        if(c_obj)
        {
            char* output = cJSON_PrintUnformatted(c_obj);
            check_expr(output,"OOM");
            puts(output);
            free(output);
        }
    } break;
    case MODE_GET_TYPE:{
        char* type = "undefined";
        if(cJSON_IsObject(c_obj))
            type = "object";
        else if(cJSON_IsArray(c_obj))
            type = "array";
        else if(cJSON_IsNull(c_obj))
            type = "null";  /** This is not standard in JavaScript */
        else if(cJSON_IsBool(c_obj))
            type = "boolean";
        else if(cJSON_IsNumber(c_obj))
            type = "number";
        else if(cJSON_IsString(c_obj))
            type = "string";
        puts(type);
    } break;
    case MODE_GET_LENGTH:{
        check_expr(cJSON_IsArray(c_obj),"Not an array");
        printf("%d\n",cJSON_GetArraySize(c_obj));
    } break;
    case MODE_GET_KEYS:{
        check_expr(cJSON_IsObject(c_obj),"Not an object");
        cJSON* keys = NULL;
        check_expr(keys = cJSON_CreateArray(),"OOM");
        cJSON* item = NULL;
        cJSON_ArrayForEach(item,c_obj)
        {
            cJSON* item_key_obj = cJSON_CreateString(item->string);
            check_expr(item_key_obj,"OOM");
            cJSON_AddItemToArray(keys,item_key_obj);
        }
        char* output = cJSON_PrintUnformatted(keys);
        check_expr(output,"OOM");
        puts(output);
        free(output);
        cJSON_Delete(keys);
    } break;
    default:
        break;
    }

    /** release resources */
    if(json_str_is_allocated && json_str)
        free(json_str);
    if(obj)
        cJSON_Delete(obj);
    if(key_array_obj)
        cJSON_Delete(key_array_obj);
    return 0;
}
