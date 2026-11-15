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
        /* Format: ./perpustakaan <username> <password> */
        uname = argv[1];
        pass = argv[2];
    } else if (argc >= 5) {
        /* Format: ./perpustakaan -u <username> -p <password> */
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
            printf("[CLA LOGIN GAGAL] Username atau password dari argumen baris perintah salah!\n");
            return 0;
        }
    }

    return 0;
}

/* Helper internal: Registrasi akun anggota (mahasiswa) baru */
static void register_new_user(User users[], int *user_count, int max_users) {
    char uname[32];
    char pass[32];
    int i;

    if (*user_count >= max_users) {
        printf("[!] Kapasitas akun penuh, tidak dapat mendaftarkan akun baru.\n");
        return;
    }

    printf("\n--- REGISTRASI AKUN ANGGOTA BARU ---\n");
    read_string("Masukkan Username baru (tanpa spasi): ", uname, sizeof(uname));

    if (uname[0] == '\0' || strchr(uname, ' ') != NULL) {
        printf("[!] Username tidak boleh kosong atau mengandung spasi!\n");
        return;
    }

    /* Cek apakah username sudah terdaftar */
    for (i = 0; i < *user_count; i++) {
        if (strcmp(users[i].username, uname) == 0) {
            printf("[!] Username '%s' sudah digunakan! Silakan pilih username lain.\n", uname);
            return;
        }
    }

    read_string("Masukkan Password (tanpa spasi)     : ", pass, sizeof(pass));
    if (pass[0] == '\0' || strchr(pass, ' ') != NULL) {
        printf("[!] Password tidak boleh kosong atau mengandung spasi!\n");
        return;
    }

    /* Simpan ke array di memori dan langsung sinkronisasi ke user.txt */
    strcpy(users[*user_count].username, uname);
    strcpy(users[*user_count].password, pass);
    strcpy(users[*user_count].role, "user");
    (*user_count)++;

    if (save_users(users, *user_count)) {
        printf("[SUKSES] Akun '%s' berhasil didaftarkan sebagai 'user' dan disimpan ke %s!\n",
               uname, DB_USER_FILE);
    }
}

int interactive_auth_menu(User users[], int *user_count, int max_users, User *logged_in_user) {
    int choice;
    char uname[32];
    char pass[32];
    int idx;

    while (1) {
        printf("\n============================================================\n");
        printf("      SISTEM MANAJEMEN PERPUSTAKAAN - GERBANG AUTENTIKASI   \n");
        printf("============================================================\n");
        printf("  1. Login ke Sistem\n");
        printf("  2. Registrasi Akun Anggota (User Baru)\n");
        printf("  0. Keluar Aplikasi (Exit)\n");
        printf("------------------------------------------------------------\n");

        choice = read_int_range("Pilih menu [0-2]: ", 0, 2);

        if (choice == 0) {
            return 0; /* Pengguna memilih keluar dari aplikasi */
        } else if (choice == 2) {
            register_new_user(users, user_count, max_users);
        } else if (choice == 1) {
            printf("\n--- LOGIN PENGGUNA ---\n");
            read_string("Username : ", uname, sizeof(uname));
            read_string("Password : ", pass, sizeof(pass));

            idx = verify_credentials(users, *user_count, uname, pass);
            if (idx != -1) {
                *logged_in_user = users[idx];
                printf("\n[LOGIN BERHASIL] Selamat datang, %s! Anda masuk sebagai [%s].\n",
                       logged_in_user->username, logged_in_user->role);
                return 1;
            } else {
                printf("\n[LOGIN GAGAL] Username atau password tidak cocok! Silakan coba lagi.\n");
            }
        }
    }
}