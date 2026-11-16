#ifndef ADMIN_H
#define ADMIN_H

#include "models.h"

/* Helper pencarian indeks buku berdasarkan ocaid (Exact Match).
 * Mengembalikan indeks (0 .. book_count-1) jika ditemukan, atau -1 jika tidak ada.
 */
int find_book_index_by_id(const Book catalog[], int book_count, const char *ocaid);

/* Fitur CRUD Admin */
void admin_create_book(Book catalog[], int *book_count, int max_books);
void admin_update_book(Book catalog[], int book_count);
void admin_delete_book(Book catalog[], int *book_count, const Loan loans[], int loan_count);

/* Fitur Laporan Transaksi Global */
void admin_view_all_loans(const Loan loans[], int loan_count, const Book catalog[], int book_count);

/* Loop Menu Utama Admin */
void run_admin_session(const User *current_admin, Book catalog[], int *book_count, const Loan loans[], int *loan_count);

#endif /* ADMIN_H */