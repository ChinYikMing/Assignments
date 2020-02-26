#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* 最大聯絡人數量 */
#define MAX_CONTACT 256

/* 最大電話號碼長度 */
#define MAX_PHONE_NUMBER 32

#define BUF_SIZE 1024

/*
 * like hw8:
 * i <PHONE_BOOK_NAME> <CONTACT_NAME> <PHONE_NUMBER>
 * u <PHONE_BOOK_NAME> <CONTACT_NAME> <PHONE_NUMBER>
 * d <PHONE_BOOK_NAME> <CONTACT_NAME>
 * p <PHONE_BOOK_NAME>
 *
 * =================================================
 *
 * additional operation:
 * s <PHONE_BOOK_NAME> (根據使用者名稱排序)
 */

/* ERROR_RET_VAL */
#define ERR_COMMAND_FORMAT -1
#define ERR_UNKNOWN_TYPE -2
#define ERR_CONTACT_NOT_FOUND -3

typedef struct phone_book {
    char type[MAX_PHONE_NUMBER];
    char contact_name[MAX_PHONE_NUMBER];
    char contact_number[MAX_PHONE_NUMBER];
} pb;

int phone_book_insert(pb *pb_school, pb *pb_family, pb *pb_company, char *type, char *name, char *contact_number);
int phone_book_delete(pb *pb_school, pb *pb_family, pb *pb_company, char *type, char *name);
int phone_book_update(pb *pb_school, pb *pb_family, pb *pb_company, char *type, char *name, char *contact_number);
int phone_book_sort(pb *pb_school, pb *pb_family, pb *pb_company, char *type);
int print_phone_books(pb *pb_school, pb *pb_family, pb *pb_company, char *type);
int get_phone_book_info(char *input, char *type, char *name, char *contact_number);
char *get_word(char *buf, char *word);
int find_id_by_name(pb *pb_school, pb *pb_family, pb *pb_company, char *name, char *type);
int get_phone_book_type(char *input, char *type);

int main() {
    pb pb_school[MAX_CONTACT], pb_family[MAX_CONTACT], pb_company[MAX_CONTACT];
    char buf[BUF_SIZE];
    char type[MAX_PHONE_NUMBER];
    char name[MAX_PHONE_NUMBER];
    char contact_number[MAX_PHONE_NUMBER];
    char cmd;
    int i;

    /* init */
    for(i=0;i<MAX_CONTACT;i++){
        memcpy(pb_school[i].type, "school", 7);
        memcpy(pb_school[i].contact_name, "-1", 3);
        memcpy(pb_school[i].contact_number, "-1", 3);
    }
    for(i=0;i<MAX_CONTACT;i++){
        memcpy(pb_family[i].type, "family", 7);
        memcpy(pb_family[i].contact_name, "-1", 3);
        memcpy(pb_family[i].contact_number, "-1", 3);
    }
    for(i=0;i<MAX_CONTACT;i++){
        memcpy(pb_company[i].type, "company", 8);
        memcpy(pb_company[i].contact_name, "-1", 3);
        memcpy(pb_company[i].contact_number, "-1", 3);
    }

    while(fgets(buf, BUF_SIZE, stdin)){
        cmd = buf[0];
        get_phone_book_type(buf, type);
        if(cmd == 'p' && (!strcmp(type, "school") || !strcmp(type, "family") || !strcmp(type, "company"))){
            print_phone_books(pb_school, pb_family, pb_company, type);
            continue;
        }

        int ret = get_phone_book_info(buf, type, name, contact_number);

        /* To ensure the function of phone_book_delete and phone_book_sort */
        if(cmd == 'd' || cmd == 's')
            ret = 0;

        if(ret < 0) continue;

        switch(cmd){
            case 'i':
                phone_book_insert(pb_school, pb_family, pb_company, type, name, contact_number);
                break;
            case 'd':
                phone_book_delete(pb_school, pb_family, pb_company, type, name);
                break;
            case 'u':
                phone_book_update(pb_school, pb_family, pb_company, type, name, contact_number);
                break;
            case 's':
                phone_book_sort(pb_school, pb_family, pb_company, type);
                break;
            default:
                fprintf(stderr, "unknown command\n");
                break;
        }
    }
    return 0;
}

int phone_book_insert(pb *pb_school, pb *pb_family, pb *pb_company, char *type, char *name, char *contact_number){
    int i;
    for(i=0;i<MAX_CONTACT;i++){
        if(!strcmp(type, "school") && !strcmp((*(pb_school + i)).contact_name, "-1") && !strcmp((*(pb_school + i)).contact_number, "-1")){
           strcpy((*(pb_school + i)).contact_name, name);
           strcpy((*(pb_school + i)).contact_number, contact_number);
           return 0;
        } else if(!strcmp(type, "family") && !strcmp((*(pb_family + i)).contact_name, "-1") && !strcmp((*(pb_family + i)).contact_number, "-1")){
            strcpy((*(pb_family + i)).contact_name, name);
            strcpy((*(pb_family + i)).contact_number, contact_number);
            return 0;
        } else if(!strcmp(type, "company") && !strcmp((*(pb_company + i)).contact_name, "-1") && !strcmp((*(pb_company + i)).contact_number, "-1")){
            strcpy((*(pb_company + i)).contact_name, name);
            strcpy((*(pb_company + i)).contact_number, contact_number);
            return 0;
        }
    }
    return -1;
}

int phone_book_delete(pb *pb_school, pb *pb_family, pb *pb_company, char *type, char *name){
    int phone_book_id;
    if(!strcmp(type, "school")){
        phone_book_id = find_id_by_name(pb_school, pb_family, pb_company, name, type);
        if(phone_book_id < 0) return -1;
        strcpy((*(pb_school + phone_book_id)).contact_name, "-1");
        strcpy((*(pb_school + phone_book_id)).contact_number, "-1");
    } else if(!strcmp(type, "family")){
        phone_book_id = find_id_by_name(pb_school, pb_family, pb_company, name, type);
        if(phone_book_id < 0) return -1;
        strcpy((*(pb_family + phone_book_id)).contact_name, "-1");
        strcpy((*(pb_family + phone_book_id)).contact_number, "-1");
    } else if(!strcmp(type, "company")){
        phone_book_id = find_id_by_name(pb_school, pb_family, pb_company, name, type);
        if(phone_book_id < 0) return -1;
        strcpy((*(pb_company + phone_book_id)).contact_name, "-1");
        strcpy((*(pb_company + phone_book_id)).contact_number, "-1");
    }
    return 0;
}

int phone_book_update(pb *pb_school, pb *pb_family, pb *pb_company,char *type, char *name, char *contact_number){
    int phone_book_id;
    if(!strcmp(type, "school")){
        phone_book_id = find_id_by_name(pb_school, pb_family, pb_company, name, type);
        if(phone_book_id < 0) return -1;
        strcpy((*(pb_school + phone_book_id)).contact_number, contact_number);
    } else if(!strcmp(type, "family")){
        phone_book_id = find_id_by_name(pb_school, pb_family, pb_company, name, type);
        if(phone_book_id < 0) return -1;
        strcpy((*(pb_family + phone_book_id)).contact_number, contact_number);
    } else if(!strcmp(type, "company")){
        phone_book_id = find_id_by_name(pb_school, pb_family, pb_company, name, type);
        if(phone_book_id < 0) return -1;
        strcpy((*(pb_company + phone_book_id)).contact_number, contact_number);
    }
    return 0;
}

int print_phone_books(pb *pb_school, pb *pb_family, pb *pb_company, char *type){
    int i;
    printf("Type\tContact Name\tContact Number\n");
    if(!strcmp(type, "school")){
        for(i=0;i<MAX_CONTACT;i++){
            if(strcmp((*(pb_school + i)).contact_name, "-1") > 0){
                printf("%s\t%s\t%s\n", (*(pb_school + i)).type, (*(pb_school + i)).contact_name, (*(pb_school + i)).contact_number);
            }
        }
    } else if(!strcmp(type, "family")){
        for(i=0;i<MAX_CONTACT;i++){
            if(strcmp((*(pb_family + i)).contact_name, "-1") > 0){
                printf("%s\t%s\t%s\n", (*(pb_family + i)).type, (*(pb_family + i)).contact_name, (*(pb_family + i)).contact_number);
            }
        }
    } else if(!strcmp(type, "company")){
        for(i=0;i<MAX_CONTACT;i++){
           if(strcmp((*(pb_company + i)).contact_name, "-1") > 0){
                printf("%s\t%s\t%s\n", (*(pb_company + i)).type, (*(pb_company + i)).contact_name, (*(pb_company + i)).contact_number);
            }
        }
    }
    return 0;
}

int get_phone_book_info(char *input, char *type, char *name, char *contact_number){
    char buf[BUF_SIZE];
    size_t buf_length;
    input = get_word(input, buf);
    if(!input) return ERR_COMMAND_FORMAT;

    input = get_word(input, buf); /* type */
    if(strcmp(buf, "school") == 0 || strcmp(buf, "family") == 0 || strcmp(buf, "company") == 0)
        strcpy(type, buf);
    else
        return ERR_UNKNOWN_TYPE;

    input = get_word(input, buf); /* contact_name */
    buf_length = strlen(buf);
    if(buf_length)
        memcpy(name, buf, buf_length+1);
    else
        return ERR_COMMAND_FORMAT;

    input = get_word(input, buf); /* contact_number */
    if(isdigit(*buf))
        strcpy(contact_number, buf);
    else
        return ERR_COMMAND_FORMAT;

    return 0;
}

char *get_word(char *buf, char *word){
    char *src = buf, *dest = word, *end = buf + strlen(buf);
    word[0] = '\0';
    while(isspace(*src)) src++;
    while(!isspace(*src) && src < end)
        *dest++ = *src++;
    *dest = '\0';
    if(word[0] == '\0') return NULL;
    return src;
}

int find_id_by_name(pb *pb_school, pb *pb_family, pb *pb_company, char *name, char *type){
    int i;
    if(!strcmp(type, "school")){
        for(i=0;i<MAX_CONTACT;i++){
            if(!strcmp(name, (*(pb_school + i)).contact_name) && strcmp((*(pb_school + i)).contact_name, "-1") > 0)
                return i;
        }
    }else if(!strcmp(type, "family")){
        for(i=0;i<MAX_CONTACT;i++){
            if(!strcmp(name, (*(pb_family + i)).contact_name) && strcmp((*(pb_family + i)).contact_name, "-1") > 0)
                return i;
        }
    }else if(!strcmp(type, "company")){
        for(i=0;i<MAX_CONTACT;i++){
            if(!strcmp(name, (*(pb_company + i)).contact_name) && strcmp((*(pb_company + i)).contact_name, "-1") > 0)
                return i;
        }
    }
    return ERR_CONTACT_NOT_FOUND;
}

int get_phone_book_type(char *input, char *type){
    input = get_word(input, type);

    /* get phone book type */
    input = get_word(input, type);
    if(!input)
        return ERR_COMMAND_FORMAT;
    return 0;
}

int phone_book_sort(pb *pb_school, pb *pb_family, pb *pb_company, char *type){
    int i, j;
    char tmp[MAX_PHONE_NUMBER];
    if(!strcmp(type, "school")){
        for(i=0;i<MAX_CONTACT;i++){
            for(j=0;j<MAX_CONTACT-1-i;j++){
                if(strcmp((*(pb_school + j)).contact_name, "-1") > 0 && strcmp((*(pb_school + j + 1)).contact_name, "-1") > 0 && strcmp((*(pb_school + j)).contact_name, (*(pb_school + j + 1)).contact_name) > 0){
                    /* sort the contact name */
                    strcpy(tmp, (*(pb_school + j + 1)).contact_name);
                    strcpy((*(pb_school + j + 1)).contact_name, (*(pb_school + j)).contact_name);
                    strcpy((*(pb_school + j)).contact_name, tmp);

                    /* sort the contact number */
                    strcpy(tmp, (*(pb_school + j + 1)).contact_number);
                    strcpy((*(pb_school + j + 1)).contact_number, (*(pb_school + j)).contact_number);
                    strcpy((*(pb_school + j)).contact_number, tmp);
                }
            }
        }
    } else if(!strcmp(type, "family")){
        for(i=0;i<MAX_CONTACT;i++){
            for(j=0;j<MAX_CONTACT-1-i;j++){
                if(strcmp((*(pb_family + j)).contact_name, "-1") > 0 && strcmp((*(pb_family + j + 1)).contact_name, "-1") > 0 && strcmp((*(pb_family + j)).contact_name, (*(pb_family + j + 1)).contact_name) > 0){
                    /* sort the contact name */
                    strcpy(tmp, (*(pb_family + j + 1)).contact_name);
                    strcpy((*(pb_family + j + 1)).contact_name, (*(pb_family + j)).contact_name);
                    strcpy((*(pb_family + j)).contact_name, tmp);

                    /* sort the contact number */
                    strcpy(tmp, (*(pb_family + j + 1)).contact_number);
                    strcpy((*(pb_family + j + 1)).contact_number, (*(pb_family + j)).contact_number);
                    strcpy((*(pb_family + j)).contact_number, tmp);
                }
            }
        }
    } else if(!strcmp(type, "company")){
        for(i=0;i<MAX_CONTACT;i++){
            for(j=0;j<MAX_CONTACT-1-i;j++){
                if(strcmp((*(pb_company + j)).contact_name, "-1") > 0 && strcmp((*(pb_company + j + 1)).contact_name, "-1") > 0 && strcmp((*(pb_company + j)).contact_name, (*(pb_company + j + 1)).contact_name) > 0){
                    /* sort the contact name */
                    strcpy(tmp, (*(pb_company + j + 1)).contact_name);
                    strcpy((*(pb_company + j + 1)).contact_name, (*(pb_company + j)).contact_name);
                    strcpy((*(pb_company + j)).contact_name, tmp);

                    /* sort the contact number */
                    strcpy(tmp, (*(pb_company + j + 1)).contact_number);
                    strcpy((*(pb_company + j + 1)).contact_number, (*(pb_company + j)).contact_number);
                    strcpy((*(pb_company + j)).contact_number, tmp);
                }
            }
        }
    }
    return 0;
}
