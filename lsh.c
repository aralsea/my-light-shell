#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#define LSH_RL_BUFFER_SIZE 1024

char *lsh_read_line(void) {
    //文字列を読み込む関数，ただし何文字入力されるかわからないので「はじめにある程度のメモリ確保」→「溢れたらメモリをさらに確保」という手順を踏む

    //はじめにメモリを確保
    int buffer_size = LSH_RL_BUFFER_SIZE;
    char *buffer =
        malloc(sizeof(char) *
               buffer_size);  //失敗したらNULLという値（=0 or (void*)0になる）

    //メモリ確保に失敗した場合
    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    //以下メモリ確保に成功した場合
    int position = 0;
    int c;
    while (1) {
        c = getchar();  // getchar()の返り値はintで，EOF以外はcharの範囲に収まる

        //文字列の終了または改行をもって入力を終了
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }

        position++;

        //確保したメモリ範囲を超えた場合，さらにメモリを追加
        if (position >= buffer_size) {
            buffer_size += LSH_RL_BUFFER_SIZE;
            buffer = realloc(buffer, sizeof(char) * buffer_size);
            if (!buffer) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    return buffer;
}

void loop_lsh(void) {
    char *line;
    char **args;
    int status;

    do {
        printf(">");
        line = lsh_read_line();
        // args = lsh_split_line(line);
        // status = lsh_execute(args);
    } while (0);
}
int main(int argc, char **argv) {
    loop_lsh();
    return EXIT_SUCCESS;
}