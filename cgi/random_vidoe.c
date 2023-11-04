#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <locale.h>
#include <sqlite3.h>
#include <time.h>
#include "cgic.h"

struct view
{
    /* data */
    DIR *dp;
    struct dirent *dirp;
    char *path;
    int fd;
    sqlite3 *db;
}g_view;
int cgiMain() {
    int sqlite3_open_result;
    setlocale(LC_ALL, "zh_CN");
    memset(&g_view, 0, sizeof(struct view));
    g_view.dp = opendir("/www/ln_dir");
    if (g_view.dp) {
        cgiHeaderContentType("text/html");
        fprintf(cgiOut, "<HTML><HEAD>\n");
        fprintf(cgiOut, "<meta charset=\"Unicode\">\n");
        fprintf(cgiOut, "<TITLE>random</TITLE>\n");
        fprintf(cgiOut, "</HEAD>\n");
        fprintf(cgiOut, "<BODY>");
        sqlite3_open_result = sqlite3_open("video.db", &g_view.db);
        if (sqlite3_open_result == SQLITE_OK) {
            char sql_select_buf[] = "SELECT * FROM video;";
            char **result = 0;
            int nrow = 0;
            int ncolumn = 0;
            char *errmsg;
            int rc = sqlite3_get_table(g_view.db, sql_select_buf, &result, &nrow, &ncolumn, &errmsg);
            if (rc == SQLITE_OK) {
                if (nrow) {
                    char access_buf[1024] = {0};
                    srand((unsigned)time(NULL));
                    int rand_value;
                    int f_exit = -1;
                    do {
                        //getpid 获取当前进程ID，使不同网页同时刷新得到的rand_value不相同
                        rand_value = (rand() + getpid()) % nrow + 1;
                        sprintf(access_buf, "/www%s", result[rand_value]);
                        f_exit = access(access_buf, F_OK);
                        if (f_exit) {
                            //DELETE FROM video WHERE path = "aaa";
                            sprintf(access_buf, "DELETE FROM video WHERE path = \"%s\";", result[rand_value]);
                            if (SQLITE_OK != sqlite3_exec(g_view.db, access_buf, 0, 0, &errmsg)) {
                                sqlite3_free(errmsg);
                            }
                        }
                    } while (f_exit);
                    //fprintf(cgiOut, "rand_value %d nrow %d ncolumn %d %s", rand_value, nrow, ncolumn, result[nrow]);
                    //fprintf(cgiOut, "getpid %d", getpid());
                    fprintf(cgiOut, "<video width=\"900\" height=\"600\" controls>");
                    fprintf(cgiOut, "<source src=\"%s\" type=\"video/mp4\">", result[rand_value]);
                    fprintf(cgiOut, "</video>");
                }
                sqlite3_free_table(result);
            } else {
                sqlite3_free(errmsg);
            }
            
        }
        //fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"20;url=view.cgi\">");
        fprintf(cgiOut, "</BODY>");
        fprintf(cgiOut, "</HTML>\n");
    } else {
        //open dir failed
        cgiHeaderContentType("text/html");
        fprintf(cgiOut, "<HTML>\n");
        fprintf(cgiOut, "<HTML><HEAD>\n");
        fprintf(cgiOut, "<TITLE>logining</TITLE></HEAD>\n");
        fprintf(cgiOut, "<BODY>");
        fprintf(cgiOut, "<H1>%s<Hi>","not found file");		
        fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"2;url=../index.html\">");
    }
    closedir(g_view.dp);
    sqlite3_close(g_view.db);
    return 0;
}
