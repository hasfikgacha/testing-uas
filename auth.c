#include <stdio.h>
#include <string.h>
#include "auth.h"
#include "file_io.h"
#include "utils.h"

int verify_credentials(const User users[], int user_count, const char *username, const char *password) {
    int i;
    for (i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0) {
            return i;
        }
    }
    return -1;
}

int login_via_cla(int argc, char *argv[], const User users[], int user_count, User *logged_in_user) {
    const char *uname = NULL;
    const char *pass = NULL;
    int idx;
    int i;

    if (argc == 3) {
        uname = argv[1];
        pass = argv[2];
    } else if (argc >= 5) {
        for (i = 1; i < argc - 1; i++) {
            if (strcmp(argv[i], "-u") == 0) uname = argv[i + 1];
            if (strcmp(argv[i], "-p") == 0) pass = argv[i + 1];
        }
    }

    if (uname != NULL && pass != NULL) {
        idx = verify_credentials(users, user_count, uname, pass);
        if (idx != -1) {
            *logged_in_user = users[idx];
            printf("[CLA LOGIN SUKSES] Selamat datang, %s (Role: %s)!\n",
                   logged_in_user->username, logged_in_user->role);
            return 1;
        } else {
            printf("[CLA LOGIN GAGAL] Kredensial dari argumen baris perintah salah!\n");
            return 0;
        }
    }
    return 0;
}

static void register_new_user(User users[], int *user_count, int max_users) {
    char uname[32];
    char pass[32];
    int i;

    if (*user_count >= max_users) {
        printf("[!] Kapasitas akun penuh.\n");
        return;
    }

    printf("\n--- REGISTRASI AKUN ANGGOTA BARU ---\n");
    read_string("Masukkan Username baru (tanpa spasi): ", uname, sizeof(uname));

    if (uname[0] == '\0' || strchr(uname, ' ') != NULL) {
        printf("[!] Username tidak boleh kosong atau mengandung spasi!\n");
        return;
    }

    for (i = 0; i < *user_count; i++) {
        if (strcmp(users[i].username, uname) == 0) {
            printf("[!] Username '%s' sudah terdaftar!\n", uname);
            return;
        }
    }

    read_string("Masukkan Password (tanpa spasi)     : ", pass, sizeof(pass));
    if (pass[0] == '\0' || strchr(pass, ' ') != NULL) {
        printf("[!] Password tidak boleh kosong atau mengandung spasi!\n");
        return;
    }

    strcpy(users[*user_count].username, uname);
    strcpy(users[*user_count].password, pass);
    strcpy(users[*user_count].role, "user");
    (*user_count)++;

    if (save_users(users, *user_count)) {
        printf("[SUKSES] Akun mahasiswa '%s' berhasil didaftarkan ke %s!\n", uname, DB_USER_FILE);
    }
}

int interactive_auth_menu(User users[], int *user_count, int max_users, User *logged_in_user) {
    int choice;
    char uname[32];
    char pass[32];
    int idx;
    const char *expected_role = NULL;

    while (1) {
        printf("\n============================================================\n");
        printf("      SISTEM MANAJEMEN PERPUSTAKAAN - GERBANG AUTENTIKASI   \n");
        printf("============================================================\n");
        printf("  1. Login sebagai Admin (Pustakawan)\n");
        printf("  2. Login sebagai Mahasiswa (Anggota)\n");
        printf("  3. Registrasi Akun Anggota Baru\n");
        printf("  0. Keluar Aplikasi (Exit)\n");
        printf("------------------------------------------------------------\n");

        choice = read_int_range("Pilih menu [0-3]: ", 0, 3);

        if (choice == 0) {
            return 0;
        } else if (choice == 3) {
            register_new_user(users, user_count, max_users);
        } else if (choice == 1 || choice == 2) {
            expected_role = (choice == 1) ? "admin" : "user";
            printf("\n--- LOGIN %s ---\n", (choice == 1) ? "ADMIN (PUSTAKAWAN)" : "MAHASISWA (ANGGOTA)");
            read_string("Username : ", uname, sizeof(uname));
            read_string("Password : ", pass, sizeof(pass));

            idx = verify_credentials(users, *user_count, uname, pass);
            if (idx != -1) {
                /* Validasi kecocokan role akun dengan menu pilihan */
                if (strcmp(users[idx].role, expected_role) != 0) {
                    printf("\n[LOGIN GAGAL] Akun '%s' bukan berhak akses '%s'! Periksa kembali peran akun Anda.\n",
                           uname, expected_role);
                } else {
                    *logged_in_user = users[idx];
                    printf("\n[LOGIN BERHASIL] Selamat datang, %s! Sesi aktif sebagai [%s].\n",
                           logged_in_user->username, logged_in_user->role);
                    return 1;
                }
            } else {
                printf("\n[LOGIN GAGAL] Username atau password salah!\n");
            }
        }
    }
}