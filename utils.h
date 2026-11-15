#ifndef UTILS_H
#define UTILS_H

/* Membersihkan buffer stdin hingga ketemu newline atau EOF */
void clear_input_buffer(void);

/* Membaca string satu baris dengan aman (menghapus \r dan \n di ujung) */
void read_string(const char *prompt, char *buffer, int max_len);

/* Membaca bilangan bulat dengan validasi rentang (mencegah infinite loop) */
int read_int_range(const char *prompt, int min_val, int max_val);

/* Menahan layar sampai pengguna menekan Enter */
void press_enter_to_continue(void);

/* Membersihkan layar terminal (lintas platform Windows/Linux) */
void clear_screen(void);

#endif /* UTILS_H */