#ifndef USER_H
#define USER_H

#include "models.h"

/* Fitur Pencarian Buku berdasarkan ID atau Judul */
void user_search_books(const Book catalog[], int book_count);

/* Fitur Peminjaman Buku (Stok berkurang 1 & transaksi tercatat di peminjaman.txt) */
void user_borrow_book(const User *current_user, Book catalog[], int book_count, Loan loans[], int *loan_count, int max_loans);

/* Fitur Pengembalian Buku (Stok bertambah 1 & status menjadi KEMBALI) */
void user_return_book(const User *current_user, Book catalog[], int book_count, Loan loans[], int loan_count);

/* Fitur Melihat Daftar Peminjaman Pribadi (Aktif & Riwayat) */
void user_view_personal_loans(const User *current_user, const Loan loans[], int loan_count, const Book catalog[], int book_count);

/* Loop Menu Utama User */
void run_user_session(const User *current_user, Book catalog[], int *book_count, Loan loans[], int *loan_count);

#endif /* USER_H */