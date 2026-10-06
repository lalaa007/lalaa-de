#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

int check_pram(int argc, const char *srcfile, int pronum);
int block_cur(const char *srcfile, int pronum, off_t *total_size);
int process_create(const char *srcfile, const char *destfile, int pronum, int blocksize, off_t total_size);
void process_wait(void);

int main(int argc, char *argv[])
{
    int pronum = 3;
    off_t total_size = 0;

    if (argc >= 4) {
        pronum = atoi(argv[3]);
    }

    if (check_pram(argc, argv[1], pronum) != 0) {
        return 1;
    }

    int blocksize = block_cur(argv[1], pronum, &total_size);
    printf("文件总大小: %ld 字节, 进程数: %d, 单块大小: %d 字节\n", 
           (long)total_size, pronum, blocksize);

    process_create(argv[1], argv[2], pronum, blocksize, total_size);

    process_wait();

    printf("所有子进程回收完毕，拷贝完成！\n");
    return 0;
}

int check_pram(int argc, const char *srcfile, int pronum)
{
    if (argc < 3) {
        fprintf(stderr, "用法: ./process_copy 源文件 目标文件 [进程数]\n");
        fprintf(stderr, "进程数可选，默认3\n");
        return -1;
    }

    if (pronum <= 0) {
        fprintf(stderr, "错误：进程数必须是正整数\n");
        return -1;
    }

    if (access(srcfile, R_OK) != 0) {
        perror("源文件不可访问");
        return -1;
    }

    return 0;
}

int block_cur(const char *srcfile, int pronum, off_t *total_size)
{
    struct stat st;
    if (stat(srcfile, &st) != 0) {
        perror("获取文件大小失败");
        exit(1);
    }

    *total_size = st.st_size;
    return (int)(st.st_size / pronum);
}

int process_create(const char *srcfile, const char *destfile, int pronum, int blocksize, off_t total_size)
{
    for (int i = 0; i < pronum; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork创建子进程失败");
            continue;
        }

        if (pid == 0) {
            off_t offset = i * blocksize;
            size_t len;

            if (i == pronum - 1) {
                len = total_size - offset;
            } else {
                len = blocksize;
            }

            char offset_str[32];
            char len_str[32];
            snprintf(offset_str, sizeof(offset_str), "%ld", (long)offset);
            snprintf(len_str, sizeof(len_str), "%ld", (long)len);

            execl("./block_copy", "block_copy", srcfile, destfile, offset_str, len_str, NULL);

            perror("execl执行失败");
            exit(1);
        }
    }

    return 0;
}

void process_wait(void)
{
    pid_t zpid;
    int status;

    while (1) {
        zpid = waitpid(-1, &status, WNOHANG);

        if (zpid == -1) {
            break;
        } else if (zpid == 0) {
            printf("父进程%ld运行中，等待子进程退出...\n", (long)getpid());
            sleep(1);
        } else {
            if (WIFEXITED(status)) {
                printf("回收子进程%d，正常退出，退出码: %d\n", zpid, WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("回收子进程%d，被信号杀死，信号号: %d\n", zpid, WTERMSIG(status));
            }
        }
    }
}

