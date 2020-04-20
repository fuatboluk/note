// Author: Fuat Bölük <fuat@fuxproject.org>


#include "db.h"


#include <pwd.h>
#include <errno.h>
#include <ctype.h>
#include <libgen.h>
#include <getopt.h>


#define NOT  ".note"


extern char * read_db(char * db_name, char * db_key);
extern char * write_db(char * db_name, char * db_key, char * db_data);
extern char * delete_db(char * db_name, char * db_key);
extern char * list_db(char * db_name, char * condition);


static char usage()
{
    printf("Note - Write, Read, Delete, and List Notes by Group\n");
    printf("\n");
    printf("Options:\n");
    printf("\t-h | --help\tprogram usage\n");
    printf("\t-g | --group\tused with the all option\n");
    printf("\t-c | --content\tused only with the write option\n");
    printf("\n");
    printf("\t-r | --read\tnote read\t\t | name\n");
    printf("\t-w | --write\tnote write or update\t | name and content\n");
    printf("\t-d | --delete\tnote delete\t\t | name\n");
    printf("\t-l | --list\tlist notes in group\t | key or all\n");
    printf("\n");
    printf("Usage  :\n");
    printf("\tnote -g <group> -l all\n");
    printf("\tnote -g <group> -r <name>\n");
    printf("\tnote -g <group> -d <name>\n");
    printf("\tnote -g <group> -w <name> -c <content>\n");
}


int main(int argc, char * argv[])
{
    char database[256];
    char * home = getenv("HOME");

    int c;
    int option_index = 0;

    char * data_grp = NULL;
    char * data_cnt = NULL;
    char * data_rea = NULL;
    char * data_wrt = NULL;
    char * data_dlt = NULL;
    char * data_lst = NULL;

    static struct option long_options[] = {
        {"help",      no_argument,         0, 'h'},
        {"group",     required_argument,   0, 'g'},
        {"content",   required_argument,   0, 'c'},
        {"read",      required_argument,   0, 'r'},
        {"write",     required_argument,   0, 'w'},
        {"delete",    required_argument,   0, 'd'},
        {"list",      required_argument,   0, 'l'},
        {0,           0,                   0,   0}
    };

    char * opt_string = "hg:c:r:w:d:l:";

    while (1) {
      c = getopt_long(argc, argv, opt_string, long_options, &option_index);

      if (c == -1) { break; }

      switch(c) {
          case 'h':
              usage();
          break;

          case 'g':
              if (optarg != NULL) {
                  data_grp = optarg;
              }
          break;

          case 'c':
              if (optarg != NULL) {
                  data_cnt = optarg;
              }
          break;

          case 'r':
              if (optarg != NULL) {
                  data_rea = optarg;
              }
          break;

          case 'w':
              if (optarg != NULL) {
                  data_wrt = optarg;
              }
          break;

          case 'd':
              if (optarg != NULL) {
                  data_dlt = optarg;
              }
          break;

          case 'l':
              if (optarg != NULL) {
                  data_lst = optarg;
              }
          break;

          case '?':
              usage();
          break;
      }
    }

    if (data_grp == NULL) {
        usage();
    } else if (data_grp != NULL && data_wrt != NULL && data_cnt != NULL && 
               data_lst == NULL && data_dlt == NULL && data_rea == NULL) {
        sprintf(database, "%s/%s/%s", home, NOT, data_grp);
        write_db(database, data_wrt, data_cnt);
    } else if (data_grp != NULL && data_rea != NULL && 
               data_lst == NULL && data_dlt == NULL && data_wrt == NULL && data_cnt == NULL ) {
        sprintf(database, "%s/%s/%s", home, NOT, data_grp);
        read_db(database, data_rea);
    } else if (data_grp != NULL && data_dlt != NULL && 
               data_lst == NULL && data_wrt == NULL && data_cnt == NULL && data_rea == NULL) {
        sprintf(database, "%s/%s/%s", home, NOT, data_grp);
        delete_db(database, data_dlt);
    } else if (data_grp != NULL && data_lst != NULL && 
               data_dlt == NULL && data_wrt == NULL && data_cnt == NULL && data_rea == NULL) {
        sprintf(database, "%s/%s/%s", home, NOT, data_grp);
        list_db(database, data_lst);
    }

    return 0;
} 


// End