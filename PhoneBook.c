#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

enum {
    SCHOOL_PHONE_BOOK,
    FAMILY_PHONE_BOOK,
    COMPANY_PHONE_BOOK,

    PHONE_BOOKS_LEN
};

/* 最大聯絡人數量 */
#define MAX_CONTACT 256

/* 最大電話號碼長度 */
#define MAX_PHONE_NUMBER 32

#define BUF_SIZE 1024

#define PHONE_BOOK_TYPE_LEN 8

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

int phone_book_insert(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char **name_list, char *type, char *name, char *contact_number);
int phone_book_delete(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char **name_list, char *name);
int phone_book_update(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char **name_list, char *type, char *name, char *contact_number);
int phone_book_sort(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char *type);
int print_phone_books(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char *phone_book_type);
int get_phone_book_info(char *input, char *type, char *name, char *contact_number);
char *get_word(char *buf, char *word);
int find_id_by_name(char **name_list, char *name);
int get_phone_book_type(char *input, char *phone_book_type);


int main() {
    char phone_books[PHONE_BOOKS_LEN][MAX_CONTACT][MAX_PHONE_NUMBER];
    char buf[BUF_SIZE];
    char *name_list[MAX_CONTACT] = {NULL};
    char phone_book_type[PHONE_BOOK_TYPE_LEN];
    char cmd;
    while(fgets(buf, BUF_SIZE, stdin)){
        cmd = buf[0];
        get_phone_book_type(buf, phone_book_type);
        if(cmd == 'p' && (!strcmp(phone_book_type, "school") || !strcmp(phone_book_type, "family") || !strcmp(phone_book_type, "company"))){
            print_phone_books(phone_books, phone_book_type);
            continue;
        }

        char type[MAX_CONTACT];
        char name[MAX_CONTACT];
        char contact_number[MAX_CONTACT];
        int ret = get_phone_book_info(buf, type, name, contact_number);
        /* To ensure the function of phone_book_delete and phone_book_sort */
        if(cmd == 'd' || cmd == 's')
            ret = 0;
        if(ret < 0) continue;

        switch(cmd){
            case 'i':
                phone_book_insert(phone_books, name_list, type, name, contact_number);
                break;
            case 'd':
                phone_book_delete(phone_books, name_list, name);
                break;
            case 'u':
                phone_book_update(phone_books, name_list, type, name, contact_number);
                break;
            case 's':
                phone_book_sort(phone_books, type);
                break;
            default:
                fprintf(stderr, "unknown command\n");
                break;
        }
    }
    return 0;
}

int phone_book_insert(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char **name_list, char *type, char *name, char *contact_number){
    int i;
    for(i=0;i<MAX_CONTACT;i++){
        if(!name_list[i]){
            const size_t name_length = strlen(name);
            name_list[i] = malloc(name_length+1);
            memcpy(name_list[i], name, name_length+1);
            strcpy((*phone_books)[i], type);
            strcpy((*(phone_books+1))[i], name);
            strcpy((*(phone_books+2))[i], contact_number);
            return 0;
        }
    }
    return -1;
}

int phone_book_delete(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char **name_list, char *name){
    const int phone_book_id = find_id_by_name(name_list, name);
    if(phone_book_id < 0) return -1;
    free(name_list[phone_book_id]);
    name_list[phone_book_id] = NULL;
    strcpy((*phone_books)[phone_book_id], "\0");
    strcpy((*(phone_books+1))[phone_book_id], "\0");
    strcpy((*(phone_books+2))[phone_book_id], "\0");
    return 0;
}

int phone_book_update(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char **name_list, char *type, char *name, char *contact_number){
    const int phone_book_id = find_id_by_name(name_list, name);
    if(phone_book_id < 0) return -1;
    strcpy((*phone_books)[phone_book_id], type);
    strcpy((*(phone_books+1))[phone_book_id], name);
    strcpy((*(phone_books+2))[phone_book_id], contact_number);
    return 0;
}

int print_phone_books(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char *phone_book_type){
    int i, j;
    printf("Type\tContact Name\tContact Number\n");
    for(i=0;i<MAX_CONTACT;i++){
        if(!strcmp(*((*phone_books)+i), phone_book_type)){
            for(j=0;j<PHONE_BOOKS_LEN;j++){
                printf("%s", *(*(phone_books+j)+i));
                if(j != 2)
                    printf("\t");
            }
            printf("\n");
        }
    }
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

int find_id_by_name(char **name_list, char *name){
    for(int i=0;i<MAX_CONTACT;i++){
        if(name_list[i] && !(strcmp(name, name_list[i])))
            return i;
    }
    return ERR_CONTACT_NOT_FOUND;
}

int get_phone_book_type(char *input, char *phone_book_type){
    input = get_word(input, phone_book_type);

    /* get phone book type */
    input = get_word(input, phone_book_type);
    if(!input)
        return ERR_COMMAND_FORMAT;
    return 0;
}

int phone_book_sort(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char *type){
    int i, j;
    char tmp[MAX_PHONE_NUMBER];
    for(i=0;i<MAX_CONTACT;i++){
        if(!strcmp((*phone_books)[i], type)){
            for (j = i+1; j < MAX_CONTACT; j++){
                if(!strcmp((*phone_books)[j], type) && (strcmp((*(phone_books + 1))[i], (*(phone_books + 1))[j]) > 0)){
                    /* sort the contact name */
                    strcpy(tmp, (*(phone_books + 1))[j]);
                    strcpy((*(phone_books + 1))[j], (*(phone_books + 1))[i]);
                    strcpy((*(phone_books + 1))[i], tmp);
                    /* sort the contact number */
                    strcpy(tmp, (*(phone_books + 2))[j]);
                    strcpy((*(phone_books + 2))[j], (*(phone_books + 2))[i]);
                    strcpy((*(phone_books + 2))[i], tmp);
                }
            }
        }
    }
    return 0;
}
