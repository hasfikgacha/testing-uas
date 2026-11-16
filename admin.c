#include <stdio.h>
#include <string.h>
#include "admin.h"
#include "file_io.h"
#include "sort.h"
#include "utils.h"

int find_book_index_by_id(const Book catalog[], int book_count, const char *ocaid) {
    int i;
    for (i = 0; i < book_count; i++) {
        if (strcmp(catalog[i].ocaid, ocaid) == 0) {
            return i;
        }
    }
    return -1;
}

/* 1. CREATE: Menambahkan data buku baru ke katalog dan menyimpannya ke books_db.txt */
void admin_create_book(Book catalog[], int *book_count, int max_books) {
    Book nb;
    char input_buf[256];

    if (*book_count >= max_books) {
        printf("\n[!] Kapasitas katalog sudah penuh (%d buku)! Tidak dapat menambah buku baru.\n", max_books);
        press_enter_to_continue();
        return;
    }

    printf("\n============================================================\n");
    printf("             [CREATE] TAMBAH DATA BUKU BARU                 \n");
    printf("============================================================\n");

    read_string("Masukkan ID Buku (ocaid, tanpa spasi) : ", nb.ocaid, sizeof(nb.ocaid));
    if (nb.ocaid[0] == '\0' || strchr(nb.ocaid, ' ') != NULL) {
        printf("[!] Batal: ID Buku (ocaid) tidak boleh kosong atau mengandung spasi.\n");
        press_enter_to_continue();
        return;
    }

    /* Pastikan ocaid belum pernah digunakan */
    if (find_book_index_by_id(catalog, *book_count, nb.ocaid) != -1) {
        printf("[!] Gagal: Buku dengan ID '%s' sudah terdaftar di dalam katalog!\n", nb.ocaid);
        press_enter_to_continue();
        return;
    }

    read_string("Masukkan Judul Buku                   : ", nb.title, sizeof(nb.title));
    if (nb.title[0] == '\0') {
        printf("[!] Batal: Judul buku tidak boleh kosong.\n");
        press_enter_to_continue();
        return;
    }

    read_string("Masukkan Nama Pengarang (Author)      : ", nb.author, sizeof(nb.author));
    if (nb.author[0] == '\0') {
        strcpy(nb.author, "Anonim");
    }

    nb.year = read_int_range("Masukkan Tahun Terbit [1000 - 2026]   : ", 1000, 2026);
    nb.page_count = read_int_range("Masukkan Jumlah Halaman [1 - 10000]   : ", 1, 10000);
    nb.stock = read_int_range("Masukkan Jumlah Stok Awal [0 - 1000]  : ", 0, 1000);

    /* Metadata opsional (agar tetap kompatibel dengan format 9 kolom books_db.txt) */
    read_string("OpenLibrary Edition (Enter utk lewati): ", input_buf, sizeof(input_buf));
    if (input_buf[0] == '\0' || strchr(input_buf, ' ') != NULL) {
        strcpy(nb.openlibrary_edition, "-");
    } else {
        strncpy(nb.openlibrary_edition, input_buf, sizeof(nb.openlibrary_edition) - 1);
        nb.openlibrary_edition[sizeof(nb.openlibrary_edition) - 1] = '\0';
    }

    read_string("OpenLibrary Work (Enter utk lewati)   : ", input_buf, sizeof(input_buf));
    if (input_buf[0] == '\0' || strchr(input_buf, ' ') != NULL) {
        strcpy(nb.openlibrary_work, "-");
    } else {
        strncpy(nb.openlibrary_work, input_buf, sizeof(nb.openlibrary_work) - 1);
        nb.openlibrary_work[sizeof(nb.openlibrary_work) - 1] = '\0';
    }

    snprintf(nb.full_text_url, sizeof(nb.full_text_url),
             "https://archive.org/download/%s/%s_djvu.txt", nb.ocaid, nb.ocaid);

    /* Tambahkan ke array memori dan langsung sinkronisasi ke berkas books_db.txt */
    catalog[*book_count] = nb;
    (*book_count)++;

    if (save_books_to_db(catalog, *book_count)) {
        printf("\n[SUKSES] Buku \"%s\" (ID: %s) berhasil ditambahkan dan disinkronkan ke %s!\n",
               nb.title, nb.ocaid, DB_BOOK_FILE);
    }
    press_enter_to_continue();
}

/* 3. UPDATE: Memperbarui atribut buku atau menambah/mengurangi stok */
void admin_update_book(Book catalog[], int book_count) {
    char target_id[MAX_ID_LEN];
    int idx, sub_choice, delta;
    Book *b;

    printf("\n============================================================\n");
    printf("           [UPDATE] PERBARUI DATA / STOK BUKU               \n");
    printf("============================================================\n");
    read_string("Masukkan ID Buku (ocaid) yang ingin diubah: ", target_id, sizeof(target_id));

    idx = find_book_index_by_id(catalog, book_count, target_id);
    if (idx == -1) {
        printf("[!] Buku dengan ID '%s' tidak ditemukan di dalam katalog!\n", target_id);
        press_enter_to_continue();
        return;
    }

    b = &catalog[idx];

    while (1) {
        printf("\n--- RINCIAN BUKU SAAT INI ---\n");
        printf("  ID Buku (ocaid) : %s\n", b->ocaid);
        printf("  1. Judul Buku   : %s\n", b->title);
        printf("  2. Pengarang    : %s\n", b->author);
        printf("  3. Tahun Terbit : %d\n", b->year);
        printf("  4. Jml Halaman  : %d halaman\n", b->page_count);
        printf("  5. Stok Saat Ini: %d eksemplar\n", b->stock);
        printf("  6. Tambah / Kurangi Stok (+/-)\n");
        printf("  0. Selesai & Simpan Perubahan ke Berkas\n");
        printf("-----------------------------\n");

        sub_choice = read_int_range("Pilih atribut yang ingin diperbarui [0-6]: ", 0, 6);
        if (sub_choice == 0) {
            break;
        }

        switch (sub_choice) {
            case 1:
                read_string("Masukkan Judul Baru     : ", b->title, sizeof(b->title));
                break;
            case 2:
                read_string("Masukkan Pengarang Baru : ", b->author, sizeof(b->author));
                break;
            case 3:
                b->year = read_int_range("Masukkan Tahun Baru [1000 - 2026]  : ", 1000, 2026);
                break;
            case 4:
                b->page_count = read_int_range("Masukkan Halaman Baru [1 - 10000]  : ", 1, 10000);
                break;
            case 5:
                b->stock = read_int_range("Masukkan Stok Baru [0 - 1000]      : ", 0, 1000);
                break;
            case 6:
                delta = read_int_range("Masukkan perubahan stok (misal 5 atau -2): ", -1000, 1000);
                if (b->stock + delta < 0) {
                    printf("[!] Gagal: Stok tidak boleh menjadi negatif (Stok saat ini: %d)!\n", b->stock);
                } else {
                    b->stock += delta;
                    printf("[INFO] Stok berhasil diubah menjadi %d eksemplar.\n", b->stock);
                }
                break;
        }
    }

    if (save_books_to_db(catalog, book_count)) {
        printf("\n[SUKSES] Perubahan pada buku '%s' berhasil disimpan ke %s!\n",
               b->ocaid, DB_BOOK_FILE);
    }
    press_enter_to_continue();
}

/* 4. DELETE: Menghapus buku dari katalog (dilengkapi proteksi pinjaman aktif) */
void admin_delete_book(Book catalog[], int *book_count, const Loan loans[], int loan_count) {
    char target_id[MAX_ID_LEN];
    char confirm[16];
    int idx, i;

    printf("\n============================================================\n");
    printf("            [DELETE] HAPUS BUKU DARI KATALOG                \n");
    printf("============================================================\n");
    read_string("Masukkan ID Buku (ocaid) yang akan dihapus: ", target_id, sizeof(target_id));

    idx = find_book_index_by_id(catalog, *book_count, target_id);
    if (idx == -1) {
        printf("[!] Buku dengan ID '%s' tidak ditemukan!\n", target_id);
        press_enter_to_continue();
        return;
    }

    /* Cek apakah buku ini sedang berstatus DIPINJAM oleh user */
    for (i = 0; i < loan_count; i++) {
        if (strcmp(loans[i].ocaid, target_id) == 0 &&
            strcmp(loans[i].status, "DIPINJAM") == 0) {
            printf("\n[DITOLAK] Buku '%s' sedang dipinjam oleh user '%s' (ID Transaksi: %u)!\n",
                   target_id, loans[i].username, loans[i].id_peminjaman);
            printf("Buku tidak boleh dihapus sebelum dikembalikan.\n");
            press_enter_to_continue();
            return;
        }
    }

    printf("\nAnda akan menghapus buku berikut:\n");
    printf("  ID Buku   : %s\n", catalog[idx].ocaid);
    printf("  Judul     : %s\n", catalog[idx].title);
    printf("  Pengarang : %s\n", catalog[idx].author);
    read_string("\nKetik 'YA' (huruf besar/kecil) untuk mengonfirmasi penghapusan: ", confirm, sizeof(confirm));

    if (strcmp(confirm, "YA") == 0 || strcmp(confirm, "ya") == 0) {
        /* Geser seluruh elemen setelah idx satu langkah ke kiri */
        for (i = idx; i < (*book_count) - 1; i++) {
            catalog[i] = catalog[i + 1];
        }
        (*book_count)--;

        if (save_books_to_db(catalog, *book_count)) {
            printf("\n[SUKSES] Buku '%s' berhasil dihapus dan %s telah diperbarui!\n",
                   target_id, DB_BOOK_FILE);
        }
    } else {
        printf("\n[INFO] Penghapusan buku dibatalkan.\n");
    }

    press_enter_to_continue();
}

/* 5. REPORT: Menampilkan seluruh riwayat transaksi peminjaman buku oleh semua user */
void admin_view_all_loans(const Loan loans[], int loan_count, const Book catalog[], int book_count) {
    int i, book_idx;
    int active_count = 0, returned_count = 0;
    const char *book_title;

    printf("\n========================================================================================================\n");
    printf("                              LAPORAN RIWAYAT TRANSAKSI PEMINJAMAN GLOBAL                               \n");
    printf("========================================================================================================\n");

    if (loan_count <= 0) {
        printf("  Belum ada riwayat transaksi peminjaman yang tercatat di dalam sistem.\n");
        printf("========================================================================================================\n");
        press_enter_to_continue();
        return;
    }

    printf("%-6s | %-16s | %-26s | %-30s | %-12s | %-10s\n",
           "ID TRX", "USERNAME", "ID BUKU (OCAID)", "JUDUL BUKU", "TGL PINJAM", "STATUS");
    printf("--------------------------------------------------------------------------------------------------------\n");

    for (i = 0; i < loan_count; i++) {
        book_idx = find_book_index_by_id(catalog, book_count, loans[i].ocaid);
        book_title = (book_idx != -1) ? catalog[book_idx].title : "[Buku Telah Dihapus]";

        printf("%-6u | %-16.16s | %-26.26s | %-30.30s | %-12s | %-10s\n",
               loans[i].id_peminjaman,
               loans[i].username,
               loans[i].ocaid,
               book_title,
               loans[i].tanggal_pinjam,
               loans[i].status);

        if (strcmp(loans[i].status, "DIPINJAM") == 0) {
            active_count++;
        } else {
            returned_count++;
        }
    }

    printf("========================================================================================================\n");
    printf("Ringkasan: Total Transaksi = %d | Sedang Dipinjam = %d | Sudah Dikembalikan = %d\n",
           loan_count, active_count, returned_count);
    press_enter_to_continue();
}

/* Loop Menu Utama Admin */
void run_admin_session(const User *current_admin, Book catalog[], int *book_count, const Loan loans[], int *loan_count) {
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
        }

        switch (choice) {
            case 1:
                admin_create_book(catalog, book_count, MAX_BOOKS);
                break;
            case 2:
                interactive_sort_and_view_catalog(catalog, *book_count, 0);
                break;
            case 3:
                admin_update_book(catalog, *book_count);
                break;
            case 4:
                admin_delete_book(catalog, book_count, loans, *loan_count);
                break;
            case 5:
                admin_view_all_loans(loans, *loan_count, catalog, *book_count);
                break;
        }
    }
}