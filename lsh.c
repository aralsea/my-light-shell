#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#define LSH_RL_BUFFER_SIZE 1024
#define LSH_TOK_BUFFFER_SIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
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

        buffer[position] = c;
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

char **lsh_split_line(char *line) {
    int buffer_size = LSH_TOK_BUFFFER_SIZE;
    char **tokens = malloc(sizeof(char *) * buffer_size);
    //メモリ確保に失敗した場合
    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    int position = 0;
    char *token;
    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buffer_size) {
            buffer_size += LSH_RL_BUFFER_SIZE;
            tokens = realloc(tokens, sizeof(char *) * buffer_size);
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
};
void print_parsed_line(char **args) {
    /*
    int position = 0;
    while (args[position] != NULL) {
        printf("%s\n", args[position]);
        position++;
    }*/

    while (*args != NULL) {
        printf("%s\n", *args);
        args++;
    }
}

int lsh_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid =
        fork();  //プロセスをフォークする，つまり今実行しているこのシェルのプロセスをコピーする

    if (pid == 0) {
        //自分は子プロセス

        // execした上でエラーがあれば出力
        if (execvp(args[0], args) == -1) {
            perror("an error occured in the child process");
        }
    } else if (pid < 0) {
        perror("an error occured while forking");
    } else {
        //自分は親プロセス（つまり元のシェル），pidには子プロセスのプロセスIDが入っている
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
            // WUNTRACEDオプションは，指定したプロセスが終了ではなく停止（Ctrl+Cとか？）したときにも復帰できるようにする
            // statusを参照で渡すことで，指定したプロセスがどうなったかの情報が返ってくる
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        //正常終了or停止までwaitpidを繰り返す
    }

    return 1;
}

void loop_lsh(void) {
    char *line;
    char **args;
    int status;

    do {
        printf(">");
        line = lsh_read_line();
        args = lsh_split_line(line);
        print_parsed_line(args);
        // status = lsh_execute(args);
    } while (0);
}
int main(int argc, char **argv) {
    loop_lsh();
    return EXIT_SUCCESS;
}