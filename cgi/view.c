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
    int table = 0;
    if (g_view.dp) {
        cgiHeaderContentType("text/html");
        fprintf(cgiOut, "<HTML>\n");
        fprintf(cgiOut, "<HTML><HEAD>\n");
        fprintf(cgiOut, "<meta charset=\"Unicode\">\n");
        fprintf(cgiOut, "<TITLE>video</TITLE></HEAD>\n");
        fprintf(cgiOut, "<BODY>");
        fprintf(cgiOut, "<table><tbody>");
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
        g_view.dirp = readdir(g_view.dp);
        g_view.dirp = readdir(g_view.dp);
        while ((g_view.dirp = readdir(g_view.dp))){
            g_view.path = (char*)malloc(strlen("/www/ln_dir/\0") + strlen(g_view.dirp->d_name) + 1);
            strcpy(g_view.path, "/www/ln_dir/\0");
            strcat(g_view.path, g_view.dirp->d_name);
            DIR *dp = opendir(g_view.path);
            struct dirent *dirp = NULL;
            char img_name[256] = {0};
            char video_name[256] = {0};
            dirp = readdir(dp);
            dirp = readdir(dp);
            chdir(g_view.path);
            while ((dirp = readdir(dp))) {
                char *ret = strrchr(dirp->d_name, '.');
                if (ret == NULL) {
                    continue;
                }
                if ((strrchr(ret, '.'))) {
                    ret = strrchr(ret, '.');
                    if ((strrchr(ret, '.'))) {
                        ret = strrchr(ret, '.');
                        if ((strrchr(ret, '.'))) {
                            ret = strrchr(ret, '.');
                        }
                    }
                }
                if (!strcmp(ret, ".mp4\0") && strcmp(dirp->d_name, "video.mp4\0")) {                  
                    //fprintf(cgiOut, "<a href=\"%s/%s/%s\" target=\"_blank\" charset=\"Unicode\">%s</a><br>", "/ln_dir", g_view.dirp->d_name, dirp->d_name, dirp->d_name);
                    //fprintf(cgiOut, "<source src=\"%s/%s/%s\" type=\"video/mp4\"><br>", "/ln_dir", g_view.dirp->d_name, dirp->d_name);
                    char *path = (char*)malloc(strlen(dirp->d_name) + 1);
                    char *new_path = (char*)malloc(strlen("/video.mp4\0") + 1);
                    strcpy(path, dirp->d_name);
                    strcpy(new_path, "video.mp4\0");
                    memset(video_name, 0, sizeof(video_name));
                    strcpy(video_name, dirp->d_name);
                    symlink(path, new_path);
                    free(path);
                    free(new_path);
                } else if (!strcmp(ret, ".png\0") || !strcmp(ret, ".jpg\0") || !strcmp(ret, ".gif\0")) {
                    //fprintf(cgiOut, "<a href=\"%s/%s/%s\" target=\"_blank\">%s</a><br>", "/ln_dir", g_view.dirp->d_name, dirp->d_name, dirp->d_name);
                    memset(img_name, 0, sizeof(img_name));
                    strcpy(img_name, dirp->d_name);
                }                
            };
            if (/*strlen(img_name) != 0 && */strlen(video_name) != 0) {

                if (table & 0x01) {
                } else {
                    fprintf(cgiOut, "<tr>");
                }
                fprintf(cgiOut, "<td>");
                fprintf(cgiOut, "<a href=\"%s/%s/%s\" target=\"_blank\">", "/ln_dir", g_view.dirp->d_name, "video.mp4\0");
                fprintf(cgiOut, "<img src=\"%s/%s/%s\" width=\"200\" height=\"120\" /><br>%s<br></a><br>\n\n", "/ln_dir", g_view.dirp->d_name, img_name, video_name);
                fprintf(cgiOut, "</td>");
                if (table & 0x01) {
                    fprintf(cgiOut, "</tr>");
                } else {
                }
                if (sqlite3_open_result == SQLITE_OK) {
                    char *errmsg;
                    char sql_exec_buf[1024] = {0};
                    sprintf(sql_exec_buf, "INSERT INTO video VALUES(\"%s/%s/%s\");", "/ln_dir", g_view.dirp->d_name, "video.mp4\0");
                    if (SQLITE_OK != sqlite3_exec(g_view.db, sql_exec_buf, 0, 0, &errmsg)) {
                        sqlite3_free(errmsg);
                    }
                }
                table++;
            }
            closedir(dp);
            free(g_view.path);
        };
        if (table & 0x01) {
            fprintf(cgiOut, "</tr>");
        } else {
        }
        fprintf(cgiOut, "</tbody></table>");
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
