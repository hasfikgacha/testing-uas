#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "sort.h"
#include "utils.h"

#define ITEMS_PER_PAGE 15

/* Helper internal: Perbandingan dua string secara case-insensitive (A-Z sama dengan a-z) */
static int str_compare_ci(const char *s1, const char *s2) {
    unsigned char c1, c2;
    while (*s1 && *s2) {
        c1 = (unsigned char)tolower((unsigned char)*s1);
        c2 = (unsigned char)tolower((unsigned char)*s2);
        if (c1 != c2) {
            return c1 - c2;
        }
        s1++;
        s2++;
    }
    return (unsigned char)tolower((unsigned char)*s1) -
           (unsigned char)tolower((unsigned char)*s2);
}

/* Fungsi pembanding terpusat (Comparator) untuk dua struct Book.
 * Mengembalikan nilai > 0 jika buku 'a' dan 'b' berada pada urutan yang salah (perlu ditukar),
 * atau <= 0 jika urutannya sudah benar.
 */
static int should_swap(const Book *a, const Book *b, int attr, int ascending) {
    int cmp = 0;

    switch (attr) {
        case ATTR_ID:
            cmp = str_compare_ci(a->ocaid, b->ocaid);
            break;
        case ATTR_TITLE:
            cmp = str_compare_ci(a->title, b->title);
            break;
        case ATTR_YEAR:
            cmp = a->year - b->year;
            break;
        case ATTR_PAGE_COUNT:
            cmp = a->page_count - b->page_count;
            break;
        case ATTR_STOCK:
            cmp = a->stock - b->stock;
            break;
        default:
            cmp = 0;
            break;
    }

    /* Jika atribut utama bernilai sama (misal tahun terbit sama),
     * gunakan ID buku (ocaid) sebagai pemecah seri (tie-breaker) agar urutan konsisten.
     */
    if (cmp == 0 && attr != ATTR_ID) {
        cmp = str_compare_ci(a->ocaid, b->ocaid);
    }

    return ascending ? (cmp > 0) : (cmp < 0);
}

/* 1. Implementasi Manual BUBBLE SORT */
void bubble_sort_books(Book arr[], int n, int attr, int ascending) {
    int i, j, swapped;
    Book temp;

    for (i = 0; i < n - 1; i++) {
        swapped = 0;
        for (j = 0; j < n - 1 - i; j++) {
            if (should_swap(&arr[j], &arr[j + 1], attr, ascending)) {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
            }
        }
        /* Optimasi: Jika tidak ada pertukaran pada satu iterasi penuh, array sudah terurut */
        if (!swapped) {
            break;
        }
    }
}

/* 2. Implementasi Manual SELECTION SORT */
void selection_sort_books(Book arr[], int n, int attr, int ascending) {
    int i, j, target_idx;
    Book temp;

    for (i = 0; i < n - 1; i++) {
        target_idx = i;
        for (j = i + 1; j < n; j++) {
            /* Jika arr[j] lebih layak berada di depan daripada arr[target_idx] */
            if (should_swap(&arr[target_idx], &arr[j], attr, ascending)) {
                target_idx = j;
            }
        }
        if (target_idx != i) {
            temp = arr[i];
            arr[i] = arr[target_idx];
            arr[target_idx] = temp;
        }
    }
}

/* 3. Implementasi Manual INSERTION SORT */
void insertion_sort_books(Book arr[], int n, int attr, int ascending) {
    int i, j;
    Book key;

    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;

        /* Geser elemen yang urutannya salah terhadap 'key' satu posisi ke kanan */
        while (j >= 0 && should_swap(&arr[j], &key, attr, ascending)) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

/* Menampilkan daftar katalog dengan sistem halaman (Pagination) */
static void display_paginated_catalog(const Book catalog[], int book_count, int only_available) {
    /* Indeks penampung agar filter stok > 0 tetap akurat saat paginasi */
    static int filtered_indices[MAX_BOOKS];
    int total_filtered = 0;
    int i, total_pages, current_page, start_idx, end_idx, row_idx;
    char cmd[32];

    for (i = 0; i < book_count; i++) {
        if (!only_available || catalog[i].stock > 0) {
            filtered_indices[total_filtered++] = i;
        }
    }

    if (total_filtered == 0) {
        printf("\n[!] Tidak ada data buku yang dapat ditampilkan.\n");
        press_enter_to_continue();
        return;
    }

    total_pages = (total_filtered + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    current_page = 1;

    while (1) {
        start_idx = (current_page - 1) * ITEMS_PER_PAGE;
        end_idx = start_idx + ITEMS_PER_PAGE;
        if (end_idx > total_filtered) {
            end_idx = total_filtered;
        }

        printf("\n========================================================================================================\n");
        printf("                                  DAFTAR KATALOG BUKU PERPUSTAKAAN                                      \n");
        printf("                         Menampilkan %d - %d dari %d Buku (Halaman %d / %d)                             \n",
               start_idx + 1, end_idx, total_filtered, current_page, total_pages);
        printf("========================================================================================================\n");
        printf("%-4s | %-26s | %-34s | %-20s | %-5s | %-4s | %-4s\n",
               "NO", "ID BUKU (OCAID)", "JUDUL BUKU", "PENGARANG", "TAHUN", "HAL", "STOK");
        printf("--------------------------------------------------------------------------------------------------------\n");

        for (i = start_idx; i < end_idx; i++) {
            row_idx = filtered_indices[i];
            printf("%-4d | %-26.26s | %-34.34s | %-20.20s | %-5d | %-4d | %-4d\n",
                   i + 1,
                   catalog[row_idx].ocaid,
                   catalog[row_idx].title,
                   catalog[row_idx].author,
                   catalog[row_idx].year,
                   catalog[row_idx].page_count,
                   catalog[row_idx].stock);
        }
        printf("========================================================================================================\n");
        printf("Navigasi: [N] Halaman Berikutnya | [P] Halaman Sebelumnya | [G] Lompat Halaman | [Q] Kembali ke Menu\n");
        read_string("Pilihan navigasi: ", cmd, sizeof(cmd));

        if (cmd[0] == 'q' || cmd[0] == 'Q') {
            break;
        } else if (cmd[0] == 'n' || cmd[0] == 'N') {
            if (current_page < total_pages) {
                current_page++;
            } else {
                printf("[!] Anda sudah berada di halaman terakhir.\n");
            }
        } else if (cmd[0] == 'p' || cmd[0] == 'P') {
            if (current_page > 1) {
                current_page--;
            } else {
                printf("[!] Anda sudah berada di halaman pertama.\n");
            }
        } else if (cmd[0] == 'g' || cmd[0] == 'G') {
            current_page = read_int_range("Masukkan nomor halaman tujuan: ", 1, total_pages);
        } else {
            printf("[!] Perintah navigasi tidak dikenali. Gunakan N, P, G, atau Q.\n");
        }
    }
}

void interactive_sort_and_view_catalog(Book catalog[], int book_count, int only_available) {
    int mode_choice, alg_choice, attr_choice, order_choice, ascending;
    clock_t start_time, end_time;
    double elapsed_ms;
    const char *alg_names[] = {"", "Bubble Sort", "Selection Sort", "Insertion Sort"};

    if (book_count <= 0) {
        printf("\n[!] Katalog buku masih kosong.\n");
        press_enter_to_continue();
        return;
    }

    printf("\n--- OPSI TAMPILAN & PENGURUTAN KATALOG (SORTING) ---\n");
    printf("  1. Urutkan Katalog Terlebih Dahulu (Pilih Algoritma Sorting)\n");
    printf("  2. Langsung Tampilkan Katalog Saat Ini (Tanpa Sorting Ulang)\n");
    printf("  0. Batal / Kembali\n");
    mode_choice = read_int_range("Pilih opsi [0-2]: ", 0, 2);

    if (mode_choice == 0) {
        return;
    }

    if (mode_choice == 1) {
        printf("\n[Langkah 1/3] Pilih Algoritma Pengurutan Manual:\n");
        printf("  1. Bubble Sort\n");
        printf("  2. Selection Sort\n");
        printf("  3. Insertion Sort\n");
        alg_choice = read_int_range("Pilih algoritma [1-3]: ", 1, 3);

        printf("\n[Langkah 2/3] Pilih Atribut Data Pengurutan:\n");
        printf("  1. ID Buku (ocaid)\n");
        printf("  2. Judul Buku (title)\n");
        printf("  3. Tahun Publikasi (year)\n");
        printf("  4. Ketebalan Halaman (page_count)\n");
        printf("  5. Jumlah Stok Ketersediaan (stock)\n");
        attr_choice = read_int_range("Pilih atribut [1-5]: ", 1, 5);

        printf("\n[Langkah 3/3] Pilih Arah Urutan:\n");
        printf("  1. Ascending  (A-Z / Terkecil ke Terbesar / Terlama ke Terbaru)\n");
        printf("  2. Descending (Z-A / Terbesar ke Terkecil / Terbaru ke Terlama)\n");
        order_choice = read_int_range("Pilih arah urutan [1-2]: ", 1, 2);
        ascending = (order_choice == 1) ? 1 : 0;

        printf("\n[PROSES] Mengurutkan %d buku menggunakan %s... Mohon tunggu...\n",
               book_count, alg_names[alg_choice]);

        start_time = clock();
        if (alg_choice == ALG_BUBBLE) {
            bubble_sort_books(catalog, book_count, attr_choice, ascending);
        } else if (alg_choice == ALG_SELECTION) {
            selection_sort_books(catalog, book_count, attr_choice, ascending);
        } else if (alg_choice == ALG_INSERTION) {
            insertion_sort_books(catalog, book_count, attr_choice, ascending);
        }
        end_time = clock();

        elapsed_ms = ((double)(end_time - start_time) * 1000.0) / CLOCKS_PER_SEC;
        printf("[SELESAI] Pengurutan %d data dengan %s berhasil dalam %.2f ms (%.3f detik)!\n",
               book_count, alg_names[alg_choice], elapsed_ms, elapsed_ms / 1000.0);
        press_enter_to_continue();
    }

    /* Tampilkan tabel katalog dengan fitur halaman */
    display_paginated_catalog(catalog, book_count, only_available);
}