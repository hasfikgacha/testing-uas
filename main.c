#include <stdio.h>
#include <string.h>
#include "models.h"
#include "file_io.h"
#include "utils.h"
#include "auth.h"
#include "sort.h"

/* Array statis di Data Segment agar aman menampung >10.000 struct */
static Book catalog[MAX_BOOKS];
static User users[MAX_USERS];
static Loan loans[MAX_LOANS];

/* Menu Admin */
static void run_admin_session(const User *current_admin, int *book_count, int *loan_count) {
    int choice;
    while (1) {
        printf("\n============================================================\n");
        printf("   MENU ADMIN (PUSTAKAWAN) | Login sebagai: %-15s\n", current_admin->username);
        printf("   Total Katalog: %d buku | Total Transaksi: %d\n", *book_count, *loan_count);
        printf("============================================================\n");
        printf("  1. [Create] Tambah Data Buku Baru\n");
        printf("  2. [Read]   Tampilkan & Urutkan Katalog Buku (Sorting)\n");
        printf("  3. [Update] Perbarui Data / Stok Buku\n");
        printf("  4. [Delete] Hapus Buku dari Katalog\n");
        printf("  5. [Report] Lihat Seluruh Riwayat Transaksi Peminjaman\n");
        printf("  0. Logout (Kembali ke Menu Login)\n");
        printf("------------------------------------------------------------\n");

        choice = read_int_range("Pilih menu Admin [0-5]: ", 0, 5);
        if (choice == 0) {
            printf("\n[INFO] Logout berhasil dari sesi Admin.\n");
            break;
        } else if (choice == 2) {
            /* Admin dapat melihat seluruh buku termasuk yang stoknya 0 (only_available = 0) */
            interactive_sort_and_view_catalog(catalog, *book_count, 0);
        } else {
            printf("\n[INFO] Fitur Admin menu %d akan diimplementasikan pada Tahap 4.\n", choice);
            press_enter_to_continue();
        }
    }
}

/* Menu User */
static void run_user_session(const User *current_user, int *book_count, int *loan_count) {
    int choice;
    while (1) {
        printf("\n============================================================\n");
        printf("   MENU ANGGOTA PERPUSTAKAAN | Login sebagai: %-13s\n", current_user->username);
        printf("============================================================\n");
        printf("  1. Lihat & Urutkan Katalog Buku Tersedia (Sorting)\n");
        printf("  2. Cari Buku (Berdasarkan ID / Judul)\n");
        printf("  3. Pinjam Buku\n");
        printf("  4. Kembalikan Buku\n");
        printf("  5. Lihat Daftar Peminjaman Aktif Saya\n");
        printf("  0. Logout (Kembali ke Menu Login)\n");
        printf("------------------------------------------------------------\n");

        choice = read_int_range("Pilih menu User [0-5]: ", 0, 5);
        if (choice == 0) {
            printf("\n[INFO] Logout berhasil dari sesi User.\n");
            break;
        } else if (choice == 1) {
            /* User hanya melihat buku yang sedang tersedia / stok > 0 (only_available = 1) */
            interactive_sort_and_view_catalog(catalog, *book_count, 1);
        } else {
            printf("\n[INFO] Fitur User menu %d akan diimplementasikan pada Tahap 5.\n", choice);
            press_enter_to_continue();
        }
    }
}

int main(int argc, char *argv[]) {
    int book_count = 0;
    int user_count = 0;
    int loan_count = 0;
    User current_user;
    int cla_logged_in = 0;

    /* 1. Inisialisasi dan muat seluruh berkas database ke memori */
    book_count = init_and_load_books(catalog, MAX_BOOKS);
    if (book_count < 0) {
        return 1;
    }
    user_count = load_users(users, MAX_USERS);
    loan_count = load_loans(loans, MAX_LOANS);

    /* 2. Cek apakah pengguna melakukan login langsung melalui Command Line Arguments (CLA) */
    if (argc > 1) {
        cla_logged_in = login_via_cla(argc, argv, users, user_count, &current_user);
    }

    /* 3. Loop utama aplikasi */
    while (1) {
        if (!cla_logged_in) {
            if (!interactive_auth_menu(users, &user_count, MAX_USERS, &current_user)) {
                printf("\nTerima kasih telah menggunakan Sistem Manajemen Perpustakaan!\n");
                break;
            }
        }

        if (strcmp(current_user.role, "admin") == 0) {
            run_admin_session(&current_user, &book_count, &loan_count);
        } else {
            run_user_session(&current_user, &book_count, &loan_count);
        }

        cla_logged_in = 0;
    }

    return 0;
}