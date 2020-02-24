#include <stdio.h>
#include <string.h>
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

int phone_book_insert(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*name_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*contact_num_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char *type, char *name, char *contact_number);
int phone_book_delete(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*name_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*contact_num_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char *type, char *name);
int phone_book_update(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*name_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*contact_num_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char *type, char *name, char *contact_number);
int phone_book_sort(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*name_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*contact_num_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char *type);
int print_phone_books(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*name_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*contact_num_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char *type);
int get_phone_book_info(char *input, char *type, char *name, char *contact_number);
char *get_word(char *buf, char *word);
int find_id_by_name(char (*name_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char *name, char *type);
int get_phone_book_type(char *input, char *type);


int main() {
    char phone_books[PHONE_BOOKS_LEN][MAX_CONTACT][MAX_PHONE_NUMBER];
    char name_list[PHONE_BOOKS_LEN][MAX_CONTACT][MAX_PHONE_NUMBER];
    char contact_num_list[PHONE_BOOKS_LEN][MAX_CONTACT][MAX_PHONE_NUMBER];
    char buf[BUF_SIZE];
    char type[MAX_PHONE_NUMBER];
    char name[MAX_PHONE_NUMBER];
    char contact_number[MAX_PHONE_NUMBER];
    char cmd;

    /* init */
    for(int i=0;i<PHONE_BOOKS_LEN;i++){
        for(int j=0;j<MAX_CONTACT;j++)
            strcpy(phone_books[i][j], "-1");
    }
    for(int i=0;i<PHONE_BOOKS_LEN;i++){
        for(int j=0;j<MAX_CONTACT;j++)
            strcpy(name_list[i][j], "-1");
    }
    for(int i=0;i<PHONE_BOOKS_LEN;i++){
        for(int j=0;j<MAX_CONTACT;j++)
            strcpy(contact_num_list[i][j], "-1");
    }

    while(fgets(buf, BUF_SIZE, stdin)){
        cmd = buf[0];
        get_phone_book_type(buf, type);
        if(cmd == 'p' && (!strcmp(type, "school") || !strcmp(type, "family") || !strcmp(type, "company"))){
            print_phone_books(phone_books, name_list, contact_num_list, type);
            continue;
        }

        int ret = get_phone_book_info(buf, type, name, contact_number);

        /* To ensure the function of phone_book_delete and phone_book_sort */
        if(cmd == 'd' || cmd == 's')
            ret = 0;
        if(ret < 0) continue;

        switch(cmd){
            case 'i':
                phone_book_insert(phone_books, name_list, contact_num_list, type, name, contact_number);
                break;
            case 'd':
                phone_book_delete(phone_books, name_list, contact_num_list, type, name);
                break;
            case 'u':
                phone_book_update(phone_books, name_list, contact_num_list, type, name, contact_number);
                break;
            case 's':
                phone_book_sort(phone_books, name_list, contact_num_list, type);
                break;
            default:
                fprintf(stderr, "unknown command\n");
                break;
        }
    }
    return 0;
}

int phone_book_insert(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*name_list)[MAX_CONTACT][MAX_PHONE_NUMBER],char (*contact_num_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char *type, char *name, char *contact_number){
    int i;
    for(i=0;i<MAX_CONTACT;i++){
        if (!strcmp(type, "school") && !strcmp((*(name_list + SCHOOL_PHONE_BOOK))[i], "-1")){
            strcpy((*(name_list + SCHOOL_PHONE_BOOK))[i], name);
            strcpy((*(phone_books + SCHOOL_PHONE_BOOK))[i], type);
            strcpy((*(contact_num_list + SCHOOL_PHONE_BOOK))[i], contact_number);
            return 0;
        } else if(!strcmp(type, "family") && !strcmp((*(name_list + FAMILY_PHONE_BOOK))[i], "-1")){
            strcpy((*(name_list + FAMILY_PHONE_BOOK))[i], name);
            strcpy((*(phone_books + FAMILY_PHONE_BOOK))[i], type);
            strcpy((*(contact_num_list + FAMILY_PHONE_BOOK))[i], contact_number);
            return 0;
        } else if(!strcmp(type, "company") && !strcmp((*(name_list + COMPANY_PHONE_BOOK))[i], "-1")){
            strcpy((*(name_list + COMPANY_PHONE_BOOK))[i], name);
            strcpy((*(phone_books + COMPANY_PHONE_BOOK))[i], type);
            strcpy((*(contact_num_list + COMPANY_PHONE_BOOK))[i], contact_number);
            return 0;
        }
    }
    return -1;
}

int phone_book_delete(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*name_list)[MAX_CONTACT][MAX_PHONE_NUMBER],char (*contact_num_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char *type, char *name){
    int phone_book_id;
    if(!strcmp(type, "school")){
       phone_book_id = find_id_by_name(name_list, name, type);
       if(phone_book_id < 0) return -1;
       strcpy((*(name_list + SCHOOL_PHONE_BOOK))[phone_book_id], "-1");
       strcpy((*(phone_books + SCHOOL_PHONE_BOOK))[phone_book_id], "-1");
       strcpy((*(contact_num_list + SCHOOL_PHONE_BOOK))[phone_book_id], "-1");
    } else if(!strcmp(type, "family")){
        phone_book_id = find_id_by_name(name_list, name, type);
        if(phone_book_id < 0) return -1;
        strcpy((*(name_list + FAMILY_PHONE_BOOK))[phone_book_id], "-1");
        strcpy((*(phone_books + FAMILY_PHONE_BOOK))[phone_book_id], "-1");
        strcpy((*(contact_num_list + FAMILY_PHONE_BOOK))[phone_book_id], "-1");
    } else if(!strcmp(type, "company")){
        phone_book_id = find_id_by_name(name_list, name, type);
        if(phone_book_id < 0) return -1;
        strcpy((*(name_list + COMPANY_PHONE_BOOK))[phone_book_id], "-1");
        strcpy((*(phone_books + COMPANY_PHONE_BOOK))[phone_book_id], "-1");
        strcpy((*(contact_num_list + COMPANY_PHONE_BOOK))[phone_book_id], "-1");
    }
    return 0;
}

int phone_book_update(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*name_list)[MAX_CONTACT][MAX_PHONE_NUMBER],char (*contact_num_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char *type, char *name, char *contact_number){
    int phone_book_id;
    if(!strcmp(type, "school")){
        phone_book_id = find_id_by_name(name_list, name, type);
        if(phone_book_id < 0) return -1;
        strcpy((*(contact_num_list + SCHOOL_PHONE_BOOK))[phone_book_id], contact_number);
    } else if(!strcmp(type, "family")){
        phone_book_id = find_id_by_name(name_list, name, type);
        if(phone_book_id < 0) return -1;
        strcpy((*(contact_num_list + FAMILY_PHONE_BOOK))[phone_book_id], contact_number);
    } else if(!strcmp(type, "company")){
        phone_book_id = find_id_by_name(name_list, name, type);
        if(phone_book_id < 0) return -1;
        strcpy((*(contact_num_list + COMPANY_PHONE_BOOK))[phone_book_id], contact_number);
    }
    return 0;
}

int print_phone_books(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*name_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*contact_num_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char *type){
    int i, j;
    printf("Type\tContact Name\tContact Number\n");
    if(!strcmp(type, "school")){
        for(i=0;i<MAX_CONTACT;i++){
            if(strcmp((*(phone_books + SCHOOL_PHONE_BOOK))[i], "-1") > 0){
                printf("%s\t%s\t%s\n", (*(phone_books + SCHOOL_PHONE_BOOK))[i], (*(name_list + SCHOOL_PHONE_BOOK))[i], (*(contact_num_list + SCHOOL_PHONE_BOOK))[i]);
            }
        }
    } else if(!strcmp(type, "family")){
        for(i=0;i<MAX_CONTACT;i++){
            if(strcmp((*(phone_books + FAMILY_PHONE_BOOK))[i], "-1") > 0){
                printf("%s\t%s\t%s\n", (*(phone_books + FAMILY_PHONE_BOOK))[i], (*(name_list + FAMILY_PHONE_BOOK))[i], (*(contact_num_list + FAMILY_PHONE_BOOK))[i]);
            }
        }
    } else if(!strcmp(type, "company")){
        for(i=0;i<MAX_CONTACT;i++){
            if(strcmp((*(phone_books + COMPANY_PHONE_BOOK))[i], "-1") > 0){
                printf("%s\t%s\t%s\n", (*(phone_books + COMPANY_PHONE_BOOK))[i], (*(name_list + COMPANY_PHONE_BOOK))[i], (*(contact_num_list + COMPANY_PHONE_BOOK))[i]);
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

int find_id_by_name(char (*name_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char *name, char *type){
    int i;
    if(!strcmp(type, "school")){
        for(i=0;i<MAX_CONTACT;i++){
            if(!strcmp(name, (*(name_list+SCHOOL_PHONE_BOOK))[i]) && (*(name_list+SCHOOL_PHONE_BOOK))[i])
                return i;
        }
    }else if(!strcmp(type, "family")){
        for(i=0;i<MAX_CONTACT;i++){
            if(!strcmp(name, (*(name_list+FAMILY_PHONE_BOOK))[i]) && (*(name_list+FAMILY_PHONE_BOOK))[i])
                return i;
        }
    }else if(!strcmp(type, "company")){
        for(i=0;i<MAX_CONTACT;i++){
            if(!strcmp(name, (*(name_list+COMPANY_PHONE_BOOK))[i]) && (*(name_list+COMPANY_PHONE_BOOK))[i])
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

int phone_book_sort(char (*phone_books)[MAX_CONTACT][MAX_PHONE_NUMBER], char (*name_list)[MAX_CONTACT][MAX_PHONE_NUMBER],char (*contact_num_list)[MAX_CONTACT][MAX_PHONE_NUMBER], char *type){
    int i, j;
    char tmp[MAX_PHONE_NUMBER];
    if(!strcmp(type, "school")){
        for(i=0;i<MAX_CONTACT;i++){
            for(j=0;j<MAX_CONTACT-1-i;j++){
                if(strcmp((*(name_list + SCHOOL_PHONE_BOOK))[j], (*(name_list + SCHOOL_PHONE_BOOK))[j+1]) > 0 && strcmp((*(name_list + SCHOOL_PHONE_BOOK))[j], "-1") > 0 && strcmp((*(name_list + SCHOOL_PHONE_BOOK))[j+1], "-1") > 0){
                    /* sort the name list */
                    strcpy(tmp, (*(name_list + SCHOOL_PHONE_BOOK))[j+1]);
                    strcpy((*(name_list + SCHOOL_PHONE_BOOK))[j+1], (*(name_list + SCHOOL_PHONE_BOOK))[j]);
                    strcpy((*(name_list) + SCHOOL_PHONE_BOOK)[j], tmp);

                    /* sort the contact number list */
                    strcpy(tmp, (*(contact_num_list + SCHOOL_PHONE_BOOK))[j+1]);
                    strcpy((*(contact_num_list + SCHOOL_PHONE_BOOK))[j+1], (*(contact_num_list + SCHOOL_PHONE_BOOK))[j]);
                    strcpy((*(contact_num_list) + SCHOOL_PHONE_BOOK)[j], tmp);
                }
            }
        }
    } else if(!strcmp(type, "family")){
        for(i=0;i<MAX_CONTACT;i++){
            for(j=0;j<MAX_CONTACT-1-i;j++){
                if(strcmp((*(name_list + FAMILY_PHONE_BOOK))[j], (*(name_list + FAMILY_PHONE_BOOK))[j+1]) > 0 && strcmp((*(name_list + FAMILY_PHONE_BOOK))[j], "-1") > 0 && strcmp((*(name_list + FAMILY_PHONE_BOOK))[j+1], "-1") > 0){
                    /* sort the name list */
                    strcpy(tmp, (*(name_list + FAMILY_PHONE_BOOK))[j+1]);
                    strcpy((*(name_list + FAMILY_PHONE_BOOK))[j+1], (*(name_list + FAMILY_PHONE_BOOK))[j]);
                    strcpy((*(name_list + FAMILY_PHONE_BOOK))[j], tmp);

                    /* sort the contact number list */
                    strcpy(tmp, (*(contact_num_list + FAMILY_PHONE_BOOK))[j+1]);
                    strcpy((*(contact_num_list + FAMILY_PHONE_BOOK))[j+1], (*(contact_num_list + FAMILY_PHONE_BOOK))[j]);
                    strcpy((*(contact_num_list + FAMILY_PHONE_BOOK))[j], tmp);
                }
            }
        }
    } else if(!strcmp(type, "company")){
        for(i=0;i<MAX_CONTACT;i++){
            for(j=0;j<MAX_CONTACT-1-i;j++){
                if(strcmp((*(name_list + COMPANY_PHONE_BOOK))[j], (*(name_list + COMPANY_PHONE_BOOK))[j+1]) > 0 && strcmp((*(name_list + COMPANY_PHONE_BOOK))[j], "-1") > 0 && strcmp((*(name_list + COMPANY_PHONE_BOOK))[j+1], "-1") > 0){
                    /* sort the name list */
                    strcpy(tmp, (*(name_list + COMPANY_PHONE_BOOK))[j+1]);
                    strcpy((*(name_list + COMPANY_PHONE_BOOK))[j+1], (*(name_list + COMPANY_PHONE_BOOK))[j]);
                    strcpy((*(name_list + COMPANY_PHONE_BOOK))[j], tmp);

                    /* sort the contact number list */
                    strcpy(tmp, (*(contact_num_list + COMPANY_PHONE_BOOK))[j+1]);
                    strcpy((*(contact_num_list + COMPANY_PHONE_BOOK))[j+1], (*(contact_num_list + COMPANY_PHONE_BOOK))[j]);
                    strcpy((*(contact_num_list + COMPANY_PHONE_BOOK))[j], tmp);
                }
            }
        }
    }
    return 0;
}
