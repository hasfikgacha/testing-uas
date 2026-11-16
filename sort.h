#ifndef SORT_H
#define SORT_H

#include "models.h"

/* Pilihan Algoritma Sorting */
#define ALG_BUBBLE    1
#define ALG_SELECTION 2
#define ALG_INSERTION 3

/* Pilihan Atribut Pengurutan */
#define ATTR_ID         1
#define ATTR_TITLE      2
#define ATTR_YEAR       3
#define ATTR_PAGE_COUNT 4
#define ATTR_STOCK      5

/* Fungsi utama 3 algoritma pengurutan manual (Tanpa qsort) */
void bubble_sort_books(Book arr[], int n, int attr, int ascending);
void selection_sort_books(Book arr[], int n, int attr, int ascending);
void insertion_sort_books(Book arr[], int n, int attr, int ascending);

/* Antarmuka menu pemilihan sorting & penampil tabel katalog berhalaman (Pagination)
 * Parameter only_available:
 *   0 -> Tampilkan semua buku (untuk Admin)
 *   1 -> Hanya tampilkan buku dengan stock > 0 (untuk User)
 */
void interactive_sort_and_view_catalog(Book catalog[], int book_count, int only_available);

#endif /* SORT_H */