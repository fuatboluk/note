// Author: Fuat Bölük <mektup@fuatboluk.com.tr>


#ifndef DB_H__
#define DB_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <db.h>
#include <errno.h>


char * bin2hex(const unsigned char * bin, size_t len)
{
    char *out;
    size_t  i;

    if (bin == NULL || len == 0)
        return NULL;

    out = malloc(len*2+1);
    for (i=0; i<len; i++) {
        out[i*2]   = "0123456789ABCDEF"[bin[i] >> 4];
        out[i*2+1] = "0123456789ABCDEF"[bin[i] & 0x0F];
    }
    out[len*2] = '\0';

    return out;
}


int hexdump(const char hex, char * out)
{
    if (out == NULL)
        return 0;

    if (hex >= '0' && hex <= '9') {
        *out = hex - '0';
    } else if (hex >= 'A' && hex <= 'F') {
        *out = hex - 'A' + 10;
    } else if (hex >= 'a' && hex <= 'f') {
        *out = hex - 'a' + 10;
    } else {
        return 0;
    }

    return 1;
}


size_t hex2bin(const char * hex, unsigned char ** out)
{
    size_t len;
    char   b1;
    char   b2;
    size_t i;

    if (hex == NULL || *hex == '\0' || out == NULL)
        return 0;

    len = strlen(hex);
    if (len % 2 != 0)
        return 0;
    len /= 2;

    *out = malloc(len);
    memset(*out, 'A', len);
    for (i=0; i<len; i++) {
        if (!hexdump(hex[i*2], &b1) || !hexdump(hex[i*2+1], &b2)) {
            return 0;
        }
        (*out)[i] = (b1 << 4) | b2;
    }
    return len;
}


char * read_db(char * db_name, char * db_key)
{
    int ret;

    DB * db;
    DBT key, data;

    char db_data[999999];

    ret = db_create(&db, NULL, 0);

    if (ret != 0) {
        fprintf(stderr, "Failed to initialize the database handle: %s\n", db_strerror(ret));
        exit(EXIT_SUCCESS);
    }

    ret = db->open(db, NULL, db_name, NULL, DB_BTREE, DB_CREATE, 0);

    if (ret != 0) {
        fprintf(stderr, "Failed to open database file %s: %s\n", db_name, db_strerror(ret));
        exit(EXIT_SUCCESS);
    }

    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    key.data  = bin2hex((unsigned char *)db_key, strlen(db_key));
    key.size  = strlen(key.data) + 1;

    data.data = db_data;
    data.ulen = sizeof(db_data);

    data.flags = DB_DBT_USERMEM;

    ret = db->get(db, NULL, &key, &data, 0);

    if (ret != 0) {
        printf("Key not exists!\n");
    } else {
        size_t binlen;
        unsigned char * bin;
        binlen = hex2bin(db_data, &bin);
        printf("%.*s\n", (int)binlen, (char *)bin);
    }

    db->close(db, 0);
}


char * write_db(char * db_name, char * db_key, char * db_data)
{
    int ret;

    DB * db;
    DBT key, data;

    ret = db_create(&db, NULL, 0);

    if (ret != 0) {
        fprintf(stderr, "Failed to initialize the database handle: %s\n", db_strerror(ret));
        exit(EXIT_SUCCESS);
    }

    ret = db->open(db, NULL, db_name, NULL, DB_BTREE, DB_CREATE, 0);

    if (ret != 0) {
        fprintf(stderr, "Failed to open database file %s: %s\n", db_name, db_strerror(ret));
        exit(EXIT_SUCCESS);
    }

    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    key.data  = bin2hex((unsigned char *)db_key, strlen(db_key));
    key.size  = strlen(key.data) + 1;

    data.data = bin2hex((unsigned char *)db_data, strlen(db_data));
    data.size = strlen(data.data) + 1;

    ret = db->put(db, NULL, &key, &data, 0);

    if (ret != 0) {
        printf("Key exists!\n");
    }

    db->close(db, 0);
}


char * delete_db(char * db_name, char * db_key)
{
    int ret;

    DB * db;
    DBT key, data;

    ret = db_create(&db, NULL, 0);

    if (ret != 0) {
        fprintf(stderr, "Failed to initialize the database handle: %s\n", db_strerror(ret));
        exit(EXIT_SUCCESS);
    }

    ret = db->open(db, NULL, db_name, NULL, DB_BTREE, DB_CREATE, 0);

    if (ret != 0) {
        fprintf(stderr, "Failed to open database file %s: %s\n", db_name, db_strerror(ret));
        exit(EXIT_SUCCESS);
    }

    memset(&key, 0, sizeof(DBT));

    key.data  = bin2hex((unsigned char *)db_key, strlen(db_key));
    key.size  = strlen(key.data) + 1;

    ret = db->del(db, NULL, &key, 0);

    if (ret != 0) {
        printf("Key not exists!\n");
    } else {
        printf("%s, deleted!\n", db_key);
    }

    db->close(db, 0);
}


char * list_db(char * db_name, char * condition)
{
    int ret;

    DB * db;
    DBC * cursor;
    DBT key, data;

    char db_key[256];
    char db_data[999999];

    ret = db_create(&db, NULL, 0);

    if (ret != 0) {
        fprintf(stderr, "Failed to initialize the database handle: %s\n", db_strerror(ret));
        exit(EXIT_SUCCESS);
    }

    ret = db->open(db, NULL, db_name, NULL, DB_BTREE, DB_CREATE, 0);

    if (ret != 0) {
        fprintf(stderr, "Failed to open database file %s: %s\n", db_name, db_strerror(ret));
        exit(EXIT_SUCCESS);
    }

    ret = db->cursor(db, NULL, &cursor, 0);

    if (ret != 0) {
        fprintf(stderr, "Failed to initialize the database cursor: %s\n", db_strerror(ret));
        exit(EXIT_SUCCESS);
    }

    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    if (!strcmp(condition, "all")) {
        while (!(ret = cursor->c_get(cursor, &key, &data, DB_NEXT))) {
            size_t keylen;
            unsigned char * keyhex;
            keylen = hex2bin(key.data, &keyhex);
            sprintf(db_key, "%.*s", (int)keylen, (char *)keyhex);

            size_t datalen;
            unsigned char * datahex;
            datalen = hex2bin(data.data, &datahex);
            sprintf(db_data, "%.*s", (int)datalen, (char *)datahex);

            printf("%s \x1b[0;33m-->\x1b[0;97m %s\n", &db_key, &db_data);
        }
    } else if (!strcmp(condition, "key")) {
        while (!(ret = cursor->c_get(cursor, &key, &data, DB_NEXT))) {
            size_t keylen;
            unsigned char * keyhex;
            keylen = hex2bin(key.data, &keyhex);
            sprintf(db_key, "%.*s", (int)keylen, (char *)keyhex);

            printf("%s\n", &db_key);
        }
    } else {
        fprintf(stderr, "Listing condition unknown: %s\n", condition);
        exit(EXIT_SUCCESS);
    }

    cursor->c_close(cursor);
    db->close(db, 0);
}


#endif /* DB_H__ */


// End