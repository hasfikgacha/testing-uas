#ifndef MODELS_H
#define MODELS_H

/* Kapasitas maksimum array statis */
#define MAX_BOOKS 10500
#define MAX_USERS 500
#define MAX_LOANS 5000

/* Ukuran maksimum buffer karakter per kolom */
#define MAX_LINE_LEN 2048
#define MAX_ID_LEN 64
#define MAX_TITLE_LEN 512
#define MAX_AUTHOR_LEN 512
#define MAX_META_LEN 64
#define MAX_URL_LEN 256

/* Nama berkas database */
#define MASTER_BOOK_FILE "Books.txt"
#define DB_BOOK_FILE     "books_db.txt"
#define DB_USER_FILE     "user.txt"
#define DB_LOAN_FILE     "peminjaman.txt"

#define DEFAULT_STOCK 5

/* 1. Struktur Data Katalog Buku */
typedef struct {
    char ocaid[MAX_ID_LEN];
    char title[MAX_TITLE_LEN];
    char author[MAX_AUTHOR_LEN];
    int year;
    int page_count;
    char openlibrary_edition[MAX_META_LEN];
    char openlibrary_work[MAX_META_LEN];
    char full_text_url[MAX_URL_LEN];
    int stock; /* Atribut tambahan untuk sirkulasi peminjaman */
} Book;

/* 2. Struktur Data Akun Pengguna (user.txt) */
typedef struct {
    char username[32];
    char password[32];
    char role[12]; /* "admin" atau "user" */
} User;

/* 3. Struktur Data Transaksi Peminjaman (peminjaman.txt) */
typedef struct {
    unsigned int id_peminjaman;
    char username[32];
    char ocaid[64];
    char tanggal_pinjam[20]; /* Format: DD-MM-YYYY */
    char status[20];         /* "DIPINJAM" atau "KEMBALI" */
} Loan;

#endif /* MODELS_H */