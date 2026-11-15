#ifndef FILE_IO_H
#define FILE_IO_H

#include "models.h"

/* Fungsi inisialisasi dan sinkronisasi data buku */
int init_and_load_books(Book catalog[], int max_capacity);
int save_books_to_db(const Book catalog[], int book_count);

/* Fungsi pembacaan dan penyimpanan akun user.txt */
int load_users(User users[], int max_capacity);
int save_users(const User users[], int user_count);

/* Fungsi pembacaan dan penyimpanan transaksi peminjaman.txt */
int load_loans(Loan loans[], int max_capacity);
int save_loans(const Loan loans[], int loan_count);

#endif /* FILE_IO_H */