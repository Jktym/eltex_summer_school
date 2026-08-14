#include "copy_module.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("Слишком мало аргументов\n");
        return 1;
    } 
    
    int use_named_pipe = 0;
    int start_idx = 1; 
    const char *pipe_name = NULL;
    
    if (strcmp(argv[1], "-p") == 0) {
        if (argc < 4) {
            printf("Ошибка: после флага -p необходимо указать имя канала и хотя бы один файл.\n");
            return 1;
        }
        use_named_pipe = 1;
        pipe_name = argv[2]; 
        start_idx = 3;
    } else if (argv[1][0] == '-') {
        printf("Неизвестный флаг: %s\n", argv[1]); 
        return 1;
    }
    
    for (int i = start_idx; i < argc; i++) {
        int result = copy_file(argv[i], use_named_pipe, pipe_name);
        if(result) return 1;
    }

    return 0;
}