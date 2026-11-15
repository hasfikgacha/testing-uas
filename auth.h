#ifndef AUTH_H
#define AUTH_H

#include "models.h"

/* Memeriksa kecocokan username & password di array users.
 * Mengembalikan indeks user jika cocok, atau -1 jika gagal.
 */
int verify_credentials(const User users[], int user_count, const char *username, const char *password);

/* Menangani login langsung dari Command Line Arguments (CLA).
 * Format yang didukung:
 *   1) ./perpustakaan <username> <password>
 *   2) ./perpustakaan -u <username> -p <password>
 */
int login_via_cla(int argc, char *argv[], const User users[], int user_count, User *logged_in_user);

/* Menampilkan menu login interaktif (Login / Registrasi User Baru / Keluar) */
int interactive_auth_menu(User users[], int *user_count, int max_users, User *logged_in_user);

#endif /* AUTH_H */