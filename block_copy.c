#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[])
{
    if (argc != 5) {
        fprintf(stderr, "用法: %s 源文件 目标文件 偏移量 拷贝长度\n", argv[0]);
        exit(1);
    }

    const char *src_file = argv[1];
    const char *dst_file = argv[2];
    off_t offset = atoi(argv[3]);
    size_t total_len = atoi(argv[4]);

    int fd_src = open(src_file, O_RDONLY);
    if (fd_src == -1) {
        perror("打开源文件失败");
        exit(1);
    }

    int fd_dst = open(dst_file, O_WRONLY | O_CREAT, 0644);
    if (fd_dst == -1) {
        perror("打开目标文件失败");
        close(fd_src);
        exit(1);
    }

    if (lseek(fd_src, offset, SEEK_SET) == (off_t)-1) {
        perror("源文件定位失败");
        close(fd_src);
        close(fd_dst);
        exit(1);
    }
    if (lseek(fd_dst, offset, SEEK_SET) == (off_t)-1) {
        perror("目标文件定位失败");
        close(fd_src);
        close(fd_dst);
        exit(1);
    }

    char buf[BUF_SIZE];
    size_t remain = total_len;

    while (remain > 0) {
        size_t read_size = remain < BUF_SIZE ? remain : BUF_SIZE;
        
        ssize_t real_read = read(fd_src, buf, read_size);
        if (real_read == -1) {
            perror("读取文件失败");
            break;
        }
        if (real_read == 0) break;

        ssize_t real_write = write(fd_dst, buf, real_read);
        if (real_write == -1) {
            perror("写入文件失败");
            break;
        }

        remain -= real_write;
    }

    close(fd_src);
    close(fd_dst);

    printf("子进程%d完成拷贝: 偏移%ld, 长度%ld\n", getpid(), (long)offset, (long)total_len);
    exit(0);
}

