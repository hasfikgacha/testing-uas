#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* Buang karakter sisa di buffer */
    }
}

void read_string(const char *prompt, char *buffer, int max_len) {
    if (prompt && prompt[0] != '\0') {
        printf("%s", prompt);
    }
    if (fgets(buffer, max_len, stdin) != NULL) {
        /* Hapus karakter newline (\n atau \r) di akhir string */
        buffer[strcspn(buffer, "\r\n")] = '\0';
    } else {
        buffer[0] = '\0';
        clear_input_buffer();
    }
}

int read_int_range(const char *prompt, int min_val, int max_val) {
    char line[128];
    int value;
    char extra;

    while (1) {
        read_string(prompt, line, sizeof(line));
        /* Pastikan input murni angka tanpa huruf tambahan di belakangnya */
        if (sscanf(line, "%d %c", &value, &extra) == 1) {
            if (value >= min_val && value <= max_val) {
                return value;
            }
        }
        printf("[!] Input tidak valid! Masukkan angka antara %d hingga %d.\n", min_val, max_val);
    }
}

void press_enter_to_continue(void) {
    char dummy[16];
    printf("\nTekan [ENTER] untuk melanjutkan...");
    fgets(dummy, sizeof(dummy), stdin);
}

void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}