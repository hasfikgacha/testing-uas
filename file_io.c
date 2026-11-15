#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "file_io.h"

/* Helper internal: Mengambil satu kolom (token) dari baris teks.
 * Mendukung teks bertanda petik ("..."), escaped quotes (""), dan tanpa petik.
 */
static const char* extract_next_token(const char *cursor, char *out_buf, int max_len) {
    int i = 0;

    /* 1. Lewati spasi atau tab di awal */
    while (*cursor != '\0' && isspace((unsigned char)*cursor)) {
        cursor++;
    }

    if (*cursor == '\0') {
        out_buf[0] = '\0';
        return cursor;
    }

    /* 2. Cek apakah kolom dimulai dengan tanda petik dua (") */
    if (*cursor == '"') {
        cursor++; /* Lewati tanda petik pembuka */
        while (*cursor != '\0') {
            if (*cursor == '"') {
                /* Cek apakah ini escaped quote ("") di dalam judul */
                if (*(cursor + 1) == '"') {
                    if (i < max_len - 1) out_buf[i++] = '"';
                    cursor += 2;
                } else {
                    /* Tanda petik penutup ditemukan */
                    cursor++;
                    break;
                }
            } else {
                if (i < max_len - 1) out_buf[i++] = *cursor;
                cursor++;
            }
        }
    } else {
        /* 3. Kolom biasa (tanpa tanda petik), baca sampai ketemu spasi/newline */
        while (*cursor != '\0' && !isspace((unsigned char)*cursor)) {
            if (i < max_len - 1) out_buf[i++] = *cursor;
            cursor++;
        }
    }

    out_buf[i] = '\0';
    return cursor;
}

/* Memparse satu baris teks menjadi struct Book */
static int parse_book_line(const char *line, Book *b, int is_operational_db) {
    const char *cursor = line;
    char temp_num[32];

    cursor = extract_next_token(cursor, b->ocaid, MAX_ID_LEN);
    if (b->ocaid[0] == '\0') return 0; /* Baris kosong */

    cursor = extract_next_token(cursor, b->title, MAX_TITLE_LEN);
    cursor = extract_next_token(cursor, b->author, MAX_AUTHOR_LEN);

    cursor = extract_next_token(cursor, temp_num, sizeof(temp_num));
    b->year = atoi(temp_num);

    cursor = extract_next_token(cursor, temp_num, sizeof(temp_num));
    b->page_count = atoi(temp_num);

    cursor = extract_next_token(cursor, b->openlibrary_edition, MAX_META_LEN);
    cursor = extract_next_token(cursor, b->openlibrary_work, MAX_META_LEN);
    cursor = extract_next_token(cursor, b->full_text_url, MAX_URL_LEN);

    if (is_operational_db) {
        cursor = extract_next_token(cursor, temp_num, sizeof(temp_num));
        b->stock = (temp_num[0] != '\0') ? atoi(temp_num) : DEFAULT_STOCK;
    } else {
        b->stock = DEFAULT_STOCK;
    }

    return 1;
}

/* Menyimpan seluruh array buku ke dalam berkas operasional books_db.txt */
int save_books_to_db(const Book catalog[], int book_count) {
    FILE *fp = fopen(DB_BOOK_FILE, "w");
    int i;
    if (!fp) {
        printf("[ERROR] Gagal menulis ke berkas %s!\n", DB_BOOK_FILE);
        return 0;
    }

    /* Tulis header dengan tambahan kolom stock di akhir */
    fprintf(fp, "ocaid title author year page_count openlibrary_edition openlibrary_work full_text_url stock\n");

    for (i = 0; i < book_count; i++) {
        fprintf(fp, "%s \"%s\" \"%s\" %d %d %s %s %s %d\n",
                catalog[i].ocaid,
                catalog[i].title,
                catalog[i].author,
                catalog[i].year,
                catalog[i].page_count,
                catalog[i].openlibrary_edition,
                catalog[i].openlibrary_work,
                catalog[i].full_text_url,
                catalog[i].stock);
    }

    fclose(fp);
    return 1;
}

/* Membaca books_db.txt jika sudah ada; jika belum, salin dari Books.txt asli */
int init_and_load_books(Book catalog[], int max_capacity) {
    FILE *fp = fopen(DB_BOOK_FILE, "r");
    int is_operational = 1;
    char line[MAX_LINE_LEN];
    int count = 0;

    if (!fp) {
        /* Berkas operasional belum ada, buka master dataset Books.txt (Read-Only) */
        printf("[INFO] Berkas '%s' belum ditemukan. Menginisialisasi salinan dari '%s'...\n",
               DB_BOOK_FILE, MASTER_BOOK_FILE);
        fp = fopen(MASTER_BOOK_FILE, "r");
        is_operational = 0;

        if (!fp) {
            printf("[ERROR] Master dataset '%s' tidak ditemukan!\n", MASTER_BOOK_FILE);
            return -1;
        }
    }

    /* Lewati baris pertama (Header) */
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 0;
    }

    /* Baca baris demi baris hingga EOF atau batas kapasitas */
    while (fgets(line, sizeof(line), fp) != NULL && count < max_capacity) {
        if (parse_book_line(line, &catalog[count], is_operational)) {
            count++;
        }
    }

    fclose(fp);

    /* Jika baru pertama kali dimuat dari Books.txt, langsung buat salinan books_db.txt */
    if (!is_operational && count > 0) {
        if (save_books_to_db(catalog, count)) {
            printf("[INFO] Berhasil menyalin %d data buku ke '%s' (Stok awal: %d).\n",
                   count, DB_BOOK_FILE, DEFAULT_STOCK);
        }
    }

    return count;
}

/* Membaca user.txt ( membuat akun default jika berkas belum ada ) */
int load_users(User users[], int max_capacity) {
    FILE *fp = fopen(DB_USER_FILE, "r");
    int count = 0;

    if (!fp) {
        /* Buat akun default untuk pengujian jika user.txt belum ada */
        strcpy(users[0].username, "admin");
        strcpy(users[0].password, "admin123");
        strcpy(users[0].role, "admin");

        strcpy(users[1].username, "mahasiswa");
        strcpy(users[1].password, "mhs123");
        strcpy(users[1].role, "user");

        count = 2;
        save_users(users, count);
        printf("[INFO] Berkas '%s' dibuat otomatis dengan 2 akun default.\n", DB_USER_FILE);
        return count;
    }

    while (count < max_capacity &&
           fscanf(fp, "%31s %31s %11s",
                  users[count].username,
                  users[count].password,
                  users[count].role) == 3) {
        count++;
    }

    fclose(fp);
    return count;
}

int save_users(const User users[], int user_count) {
    FILE *fp = fopen(DB_USER_FILE, "w");
    int i;
    if (!fp) return 0;

    for (i = 0; i < user_count; i++) {
        fprintf(fp, "%s %s %s\n", users[i].username, users[i].password, users[i].role);
    }
    fclose(fp);
    return 1;
}

/* Membaca peminjaman.txt */
int load_loans(Loan loans[], int max_capacity) {
    FILE *fp = fopen(DB_LOAN_FILE, "r");
    int count = 0;

    if (!fp) {
        /* Buat berkas kosong jika belum ada */
        fp = fopen(DB_LOAN_FILE, "w");
        if (fp) fclose(fp);
        return 0;
    }

    while (count < max_capacity &&
           fscanf(fp, "%u %31s %63s %19s %19s",
                  &loans[count].id_peminjaman,
                  loans[count].username,
                  loans[count].ocaid,
                  loans[count].tanggal_pinjam,
                  loans[count].status) == 5) {
        count++;
    }

    fclose(fp);
    return count;
}

int save_loans(const Loan loans[], int loan_count) {
    FILE *fp = fopen(DB_LOAN_FILE, "w");
    int i;
    if (!fp) return 0;

    for (i = 0; i < loan_count; i++) {
        fprintf(fp, "%u %s %s %s %s\n",
                loans[i].id_peminjaman,
                loans[i].username,
                loans[i].ocaid,
                loans[i].tanggal_pinjam,
                loans[i].status);
    }
    fclose(fp);
    return 1;
}