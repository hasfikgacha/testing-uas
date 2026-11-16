#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "user.h"
#include "admin.h"
#include "file_io.h"
#include "sort.h"
#include "utils.h"

/* Helper internal: Mengecek apakah 'needle' ada di dalam 'haystack' tanpa membedakan huruf besar/kecil */
static int contains_substring_ci(const char *haystack, const char *needle) {
    int h_len = (int)strlen(haystack);
    int n_len = (int)strlen(needle);
    int i, j;

    if (n_len == 0) return 1;
    if (n_len > h_len) return 0;

    for (i = 0; i <= h_len - n_len; i++) {
        for (j = 0; j < n_len; j++) {
            if (tolower((unsigned char)haystack[i + j]) !=
                tolower((unsigned char)needle[j])) {
                break;
            }
        }
        if (j == n_len) {
            return 1; /* Cocok */
        }
    }
    return 0;
}

/* Helper internal: Mengambil tanggal sistem saat ini dengan format DD-MM-YYYY */
static void get_current_date_str(char *out_buf, int max_len) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t != NULL) {
        snprintf(out_buf, max_len, "%02d-%02d-%04d",
                 t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
    } else {
        strncpy(out_buf, "16-11-2026", max_len - 1);
        out_buf[max_len - 1] = '\0';
    }
}

/* 2. PENCARIAN BUKU: Berdasarkan ID (ocaid) atau Judul Buku (title) */
void user_search_books(const Book catalog[], int book_count) {
    int search_by, i, found = 0;
    char keyword[128];

    printf("\n============================================================\n");
    printf("          PENCARIAN BUKU (BERDASARKAN ID / JUDUL)           \n");
    printf("============================================================\n");
    printf("  1. Cari berdasarkan ID Buku (ocaid)\n");
    printf("  2. Cari berdasarkan Judul Buku (title)\n");
    printf("  0. Kembali\n");
    search_by = read_int_range("Pilih metode pencarian [0-2]: ", 0, 2);

    if (search_by == 0) return;

    read_string("Masukkan kata kunci pencarian: ", keyword, sizeof(keyword));
    if (keyword[0] == '\0') {
        printf("[!] Kata kunci tidak boleh kosong.\n");
        press_enter_to_continue();
        return;
    }

    printf("\n========================================================================================================\n");
    printf("                                      HASIL PENCARIAN: \"%s\"\n", keyword);
    printf("========================================================================================================\n");
    printf("%-4s | %-26s | %-34s | %-20s | %-5s | %-4s | %-4s\n",
           "NO", "ID BUKU (OCAID)", "JUDUL BUKU", "PENGARANG", "TAHUN", "HAL", "STOK");
    printf("--------------------------------------------------------------------------------------------------------\n");

    for (i = 0; i < book_count; i++) {
        int match = 0;
        if (search_by == 1) {
            match = contains_substring_ci(catalog[i].ocaid, keyword);
        } else {
            match = contains_substring_ci(catalog[i].title, keyword);
        }

        if (match) {
            found++;
            if (found <= 50) { /* Batasi tampilan maksimal 50 hasil pertama agar terminal tetap rapi */
                printf("%-4d | %-26.26s | %-34.34s | %-20.20s | %-5d | %-4d | %-4d\n",
                       found,
                       catalog[i].ocaid,
                       catalog[i].title,
                       catalog[i].author,
                       catalog[i].year,
                       catalog[i].page_count,
                       catalog[i].stock);
            }
        }
    }

    printf("========================================================================================================\n");
    if (found == 0) {
        printf("[!] Tidak ditemukan buku yang cocok dengan kata kunci \"%s\".\n", keyword);
    } else if (found > 50) {
        printf("Ditemukan total %d buku (Menampilkan 50 hasil pertama. Gunakan kata kunci lebih spesifik).\n", found);
    } else {
        printf("Ditemukan total %d buku yang cocok.\n", found);
    }

    press_enter_to_continue();
}

/* 3. PEMINJAMAN BUKU: Stok berkurang 1 & transaksi tercatat sebagai DIPINJAM */
void user_borrow_book(const User *current_user, Book catalog[], int book_count, Loan loans[], int *loan_count, int max_loans) {
    char target_id[MAX_ID_LEN];
    char date_input[20];
    char auto_date[20];
    int book_idx, i;
    unsigned int next_id = 1;
    Loan new_loan;

    if (*loan_count >= max_loans) {
        printf("\n[!] Kapasitas rekaman transaksi peminjaman sudah penuh!\n");
        press_enter_to_continue();
        return;
    }

    printf("\n============================================================\n");
    printf("                    FORM PEMINJAMAN BUKU                    \n");
    printf("============================================================\n");
    read_string("Masukkan ID Buku (ocaid) yang ingin dipinjam: ", target_id, sizeof(target_id));

    if (target_id[0] == '\0') {
        printf("[!] Batal: ID Buku tidak boleh kosong.\n");
        press_enter_to_continue();
        return;
    }

    book_idx = find_book_index_by_id(catalog, book_count, target_id);
    if (book_idx == -1) {
        printf("[!] Buku dengan ID '%s' tidak ditemukan di dalam katalog!\n", target_id);
        press_enter_to_continue();
        return;
    }

    /* Cek apakah stok buku > 0 */
    if (catalog[book_idx].stock <= 0) {
        printf("\n[GAGAL] Maaf, stok buku \"%s\" (ID: %s) saat ini sedang habis (Stok: 0).\n",
               catalog[book_idx].title, catalog[book_idx].ocaid);
        press_enter_to_continue();
        return;
    }

    /* Cek apakah user yang sama sedang meminjam buku ini dan belum mengembalikannya */
    for (i = 0; i < *loan_count; i++) {
        if (strcmp(loans[i].username, current_user->username) == 0 &&
            strcmp(loans[i].ocaid, target_id) == 0 &&
            strcmp(loans[i].status, "DIPINJAM") == 0) {
            printf("\n[DITOLAK] Anda sedang meminjam buku ini (ID Transaksi: %u) dan belum mengembalikannya!\n",
                   loans[i].id_peminjaman);
            press_enter_to_continue();
            return;
        }
    }

    printf("\n--- RINCIAN BUKU YANG AKAN DIPINJAM ---\n");
    printf("  ID Buku      : %s\n", catalog[book_idx].ocaid);
    printf("  Judul Buku   : %s\n", catalog[book_idx].title);
    printf("  Pengarang    : %s\n", catalog[book_idx].author);
    printf("  Stok Tersedia: %d eksemplar\n", catalog[book_idx].stock);

    get_current_date_str(auto_date, sizeof(auto_date));
    printf("\nMasukkan Tanggal Pinjam format DD-MM-YYYY (Tekan ENTER untuk otomatis [%s]): ", auto_date);
    read_string("", date_input, sizeof(date_input));

    if (date_input[0] == '\0' || strchr(date_input, ' ') != NULL) {
        strcpy(new_loan.tanggal_pinjam, auto_date);
    } else {
        strncpy(new_loan.tanggal_pinjam, date_input, sizeof(new_loan.tanggal_pinjam) - 1);
        new_loan.tanggal_pinjam[sizeof(new_loan.tanggal_pinjam) - 1] = '\0';
    }

    /* Hitung id_peminjaman otomatis (auto-increment dari ID terbesar + 1) */
    for (i = 0; i < *loan_count; i++) {
        if (loans[i].id_peminjaman >= next_id) {
            next_id = loans[i].id_peminjaman + 1;
        }
    }

    new_loan.id_peminjaman = next_id;
    strcpy(new_loan.username, current_user->username);
    strcpy(new_loan.ocaid, catalog[book_idx].ocaid);
    strcpy(new_loan.status, "DIPINJAM");

    /* 1. Kurangi stok buku di memori sebanyak 1 */
    catalog[book_idx].stock--;

    /* 2. Tambahkan transaksi ke array loans */
    loans[*loan_count] = new_loan;
    (*loan_count)++;

    /* 3. Sinkronisasi langsung ke berkas books_db.txt dan peminjaman.txt */
    if (save_books_to_db(catalog, book_count) && save_loans(loans, *loan_count)) {
        printf("\n[PEMINJAMAN BERHASIL!]\n");
        printf("  ID Transaksi : %u\n", new_loan.id_peminjaman);
        printf("  Judul Buku   : %s\n", catalog[book_idx].title);
        printf("  Tgl Pinjam   : %s\n", new_loan.tanggal_pinjam);
        printf("  Sisa Stok    : %d eksemplar\n", catalog[book_idx].stock);
    }

    press_enter_to_continue();
}

/* 4. PENGEMBALIAN BUKU: Stok bertambah 1 & status diperbarui menjadi KEMBALI */
void user_return_book(const User *current_user, Book catalog[], int book_count, Loan loans[], int loan_count) {
    int i, book_idx, active_found = 0, target_loan_idx = -1;
    char input_str[64];
    unsigned int input_id = 0;
    const char *book_title;

    printf("\n========================================================================================================\n");
    printf("                                  DAFTAR BUKU YANG SEDANG ANDA PINJAM                                   \n");
    printf("========================================================================================================\n");
    printf("%-8s | %-26s | %-38s | %-12s | %-10s\n",
           "ID TRX", "ID BUKU (OCAID)", "JUDUL BUKU", "TGL PINJAM", "STATUS");
    printf("--------------------------------------------------------------------------------------------------------\n");

    for (i = 0; i < loan_count; i++) {
        if (strcmp(loans[i].username, current_user->username) == 0 &&
            strcmp(loans[i].status, "DIPINJAM") == 0) {
            book_idx = find_book_index_by_id(catalog, book_count, loans[i].ocaid);
            book_title = (book_idx != -1) ? catalog[book_idx].title : "[Buku Tidak Ditemukan]";

            printf("%-8u | %-26.26s | %-38.38s | %-12s | %-10s\n",
                   loans[i].id_peminjaman,
                   loans[i].ocaid,
                   book_title,
                   loans[i].tanggal_pinjam,
                   loans[i].status);
            active_found++;
        }
    }
    printf("========================================================================================================\n");

    if (active_found == 0) {
        printf("[INFO] Anda tidak memiliki pinjaman buku yang sedang aktif saat ini.\n");
        press_enter_to_continue();
        return;
    }

    read_string("\nMasukkan ID Transaksi (ID TRX) atau ID Buku (ocaid) yang ingin dikembalikan (0 utk batal): ",
                input_str, sizeof(input_str));

    if (input_str[0] == '\0' || strcmp(input_str, "0") == 0) {
        printf("[INFO] Pengembalian buku dibatalkan.\n");
        press_enter_to_continue();
        return;
    }

    /* Cek apakah user memasukkan angka ID Transaksi atau string ocaid */
    sscanf(input_str, "%u", &input_id);

    for (i = 0; i < loan_count; i++) {
        if (strcmp(loans[i].username, current_user->username) == 0 &&
            strcmp(loans[i].status, "DIPINJAM") == 0) {
            if ((input_id > 0 && loans[i].id_peminjaman == input_id) ||
                strcmp(loans[i].ocaid, input_str) == 0) {
                target_loan_idx = i;
                break;
            }
        }
    }

    if (target_loan_idx == -1) {
        printf("[!] Data pinjaman aktif dengan ID '%s' tidak ditemukan pada akun Anda!\n", input_str);
        press_enter_to_continue();
        return;
    }

    /* 1. Ubah status peminjaman menjadi KEMBALI */
    strcpy(loans[target_loan_idx].status, "KEMBALI");

    /* 2. Tambahkan kembali stok buku di katalog sebanyak 1 */
    book_idx = find_book_index_by_id(catalog, book_count, loans[target_loan_idx].ocaid);
    if (book_idx != -1) {
        catalog[book_idx].stock++;
    }

    /* 3. Sinkronisasi perubahan langsung ke books_db.txt dan peminjaman.txt */
    if (save_books_to_db(catalog, book_count) && save_loans(loans, loan_count)) {
        printf("\n[PENGEMBALIAN BERHASIL!]\n");
        printf("  ID Transaksi : %u\n", loans[target_loan_idx].id_peminjaman);
        printf("  ID Buku      : %s\n", loans[target_loan_idx].ocaid);
        if (book_idx != -1) {
            printf("  Judul Buku   : %s\n", catalog[book_idx].title);
            printf("  Stok Katalog : %d eksemplar (Bertambah +1)\n", catalog[book_idx].stock);
        }
        printf("  Status Baru  : %s\n", loans[target_loan_idx].status);
    }

    press_enter_to_continue();
}

/* 5. DAFTAR PEMINJAMAN PRIBADI: Menampilkan pinjaman yang sedang aktif milik user */
void user_view_personal_loans(const User *current_user, const Loan loans[], int loan_count, const Book catalog[], int book_count) {
    int i, book_idx, active_count = 0, history_count = 0;
    const char *book_title;

    printf("\n========================================================================================================\n");
    printf("                         DAFTAR PEMINJAMAN AKTIF PRIBADI (USER: %-15s)                       \n", current_user->username);
    printf("========================================================================================================\n");
    printf("%-8s | %-26s | %-38s | %-12s | %-10s\n",
           "ID TRX", "ID BUKU (OCAID)", "JUDUL BUKU", "TGL PINJAM", "STATUS");
    printf("--------------------------------------------------------------------------------------------------------\n");

    for (i = 0; i < loan_count; i++) {
        if (strcmp(loans[i].username, current_user->username) == 0) {
            if (strcmp(loans[i].status, "DIPINJAM") == 0) {
                book_idx = find_book_index_by_id(catalog, book_count, loans[i].ocaid);
                book_title = (book_idx != -1) ? catalog[book_idx].title : "[Buku Tidak Ditemukan]";

                printf("%-8u | %-26.26s | %-38.38s | %-12s | %-10s\n",
                       loans[i].id_peminjaman,
                       loans[i].ocaid,
                       book_title,
                       loans[i].tanggal_pinjam,
                       loans[i].status);
                active_count++;
            } else {
                history_count++;
            }
        }
    }

    if (active_count == 0) {
        printf("  Saat ini tidak ada buku yang sedang aktif Anda pinjam.\n");
    }

    printf("========================================================================================================\n");
    printf("Ringkasan Akun '%s': Sedang Dipinjam = %d buku | Sudah Dikembalikan = %d buku\n",
           current_user->username, active_count, history_count);
    press_enter_to_continue();
}

/* Loop Menu Utama User */
void run_user_session(const User *current_user, Book catalog[], int *book_count, Loan loans[], int *loan_count) {
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
        }

        switch (choice) {
            case 1:
                /* Hanya tampilkan buku dengan stock > 0 */
                interactive_sort_and_view_catalog(catalog, *book_count, 1);
                break;
            case 2:
                user_search_books(catalog, *book_count);
                break;
            case 3:
                user_borrow_book(current_user, catalog, *book_count, loans, loan_count, MAX_LOANS);
                break;
            case 4:
                user_return_book(current_user, catalog, *book_count, loans, *loan_count);
                break;
            case 5:
                user_view_personal_loans(current_user, loans, *loan_count, catalog, *book_count);
                break;
        }
    }
}