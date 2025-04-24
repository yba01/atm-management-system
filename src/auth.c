#include <termios.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "header.h"

#define MAX_INPUT_LENGTH 50

sqlite3 *createDatabase() {
    sqlite3 *db;
    char *err_msg = 0;
    
    // Open database file
    int rc = sqlite3_open("atm.db", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }
    
    // SQL statement to create 'user' table
    char *sql_user = "CREATE TABLE IF NOT EXISTS user ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "username TEXT NOT NULL UNIQUE,"
                     "password TEXT NOT NULL);";
                     
    // Execute SQL statement to create 'user' table
    rc = sqlite3_exec(db, sql_user, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return NULL;
    } else {
        printf("Table 'user' created successfully.\n");
    }
    
    // SQL statement to create 'record' table
    char *sql_record = "CREATE TABLE IF NOT EXISTS record ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "username TEXT NOT NULL,"
                       "account_id INTEGER NOT NULL,"
                       "date TEXT NOT NULL,"
                       "country TEXT NOT NULL,"
                       "phone INTEGER NOT NULL,"
                       "balance REAL NOT NULL,"
                       "type_of_account TEXT NOT NULL);";
                    //    "FOREIGN KEY (user_id) REFERENCES user(id));";

    // Execute SQL statement to create 'record' table
    rc = sqlite3_exec(db, sql_record, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return NULL;
    }    
    return db;
}

void insertData(sqlite3 *db) {
    inserUser(db);
    inserRecord(db);
}

void inserUser(sqlite3 *db) {
    int rc;
    sqlite3_stmt *stmt;
    // const char *sql = "SELECT COUNT(*) FROM table_name";

    // rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    // if (rc != SQLITE_OK) {
    //     fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
    //     sqlite3_close(db);
    //     return;   
    // }

    // rc = sqlite3_step(stmt);
    // if (rc == SQLITE_ROW) {
    //     int count = sqlite3_column_int(stmt, 0);

    //     if (count == 0) {
            // Prepare insert statement
            const char *insert_sql = "INSERT INTO user (username, password) VALUES (?, ?);";
            
            rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
            
            if (rc != SQLITE_OK) {
                fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return;
            }
            
            // Data to insert
            const char *data[][2] = {
                {"Alice", "q1w2e3r4t5y6"},
                {"Michel", "q1w2e3r4t5y6"},
            };
            
            for (int i = 0; i < 2; i++) {
                sqlite3_bind_text(stmt, 1, data[i][0], -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 2, data[i][1], -1, SQLITE_STATIC);        
                rc = sqlite3_step(stmt);
                
                if (rc != SQLITE_DONE) {
                    fprintf(stderr, "Failed to insert data: %s\n", sqlite3_errmsg(db));
                    return;
                }
                
                // Reset the statement
                sqlite3_reset(stmt);
            }
            
            // Finalize and close
            sqlite3_finalize(stmt);
    //     }
    // }
}

int has_non_printable_char(const char *str) {
    while (*str) {
        if (!isprint((unsigned char)*str)) {
            return 1;  // Return 1 (true) if a non-printable character is found
        }
        str++;
    }
    return 0;  // Return 0 (false) if all characters are printable
}

void inserRecord(sqlite3 *db) {
    typedef struct {
        const char *name;
        int id;
        const char *date;
        const char *country;
        int phone;
        double balance;
        const char *accountType;    
    } AccountData;
     int rc;
    sqlite3_stmt *stmt;
    const char *sql = "SELECT COUNT(*) FROM record";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;   
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);

        if (count == 0) {
            // Prepare insert statement
            const char *insert_sql = "INSERT INTO record (username, account_id, date, country, phone, balance, type_of_account) VALUES (?, ?, ?, ?, ?, ?, ?);";
            
            rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
            
            if (rc != SQLITE_OK) {
                fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                return ;
            }
            
            // Data to insert
            AccountData data[] = {
                {"Alice", 0, "10/10/2012", "Africa", 291321234, 22432.52, "saving"},
                {"Michel", 2, "02/05/2001", "Portugal", 123543455, 10023.230000, "fixed01"},
                {"Michel", 123, "10/10/2020", "UK", 1234123, 12345.30, "saving"},
            };
            
            for (int i = 0; i < 3; i++) {
                sqlite3_bind_text(stmt, 1, data[i].name, -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt, 2, data[i].id);  
                sqlite3_bind_text(stmt, 3, data[i].date, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 4, data[i].country, -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt, 5, data[i].phone);
                sqlite3_bind_double(stmt, 6, data[i].balance);
                sqlite3_bind_text(stmt, 7, data[i].accountType, -1, SQLITE_STATIC);
            
                rc = sqlite3_step(stmt);
                
                if (rc != SQLITE_DONE) {
                    fprintf(stderr, "Failed to insert data: %s\n", sqlite3_errmsg(db));
                    return;
                }
                
                // Reset the statement
                sqlite3_reset(stmt);
            }
            
            // Finalize and close
            sqlite3_finalize(stmt);
        }
    }
}

void loginMenu(char a[MAX_INPUT_LENGTH], char pass[MAX_INPUT_LENGTH]) {
    while(1) {
        struct termios oflags, nflags;
        // Clear screen (cross-platform)
        printf("\033[H\033[J");
        printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");
        int result = scanf("%s", a);
        if (strlen(a) > 15 || strlen(a)<1 || result != 1) {
            while (getchar() != '\n'); // Clear the input buffe
            continue;
        }

        while ((getchar()) != '\n') {
            // Discard any remaining characters in the buffer
        }

        // disabling echo
        tcgetattr(fileno(stdin), &oflags);
        nflags = oflags;
        nflags.c_lflag &= ~ECHO;
        nflags.c_lflag |= ECHONL;
        if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
        {
            perror("tcsetattr");
            return exit(1);
        }
        printf("\n\n\n\n\n\t\t\t\tEnter the password to login:");
        result = scanf("%s", pass);
        // restore terminal
        if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
        {
            perror("tcsetattr");
            return exit(1);
        }
        if (strlen(pass) > 15 || strlen(pass)<1 || result != 1) {
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        break;
    }
}
void registerMenu(char a[MAX_INPUT_LENGTH], char pass[MAX_INPUT_LENGTH])
{
    while(1){
        struct termios oflags, nflags;
        // Clear screen (cross-platform)
        printf("\033[H\033[J");
        printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\tEnter the username your want use:");
        int result = scanf("%s", a);
        if (strlen(a) > 15 || strlen(a)<1 || result != 1) {
            while (getchar() != '\n'); // Clear the input buffe
            continue;
        }

        while (getchar() != '\n'); // Clear the input buffer

        // disabling echo
        tcgetattr(fileno(stdin), &oflags);
        nflags = oflags;
        nflags.c_lflag &= ~ECHO;
        nflags.c_lflag |= ECHONL;
        if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
        {
            perror("tcsetattr");
            return exit(1);
        }
        printf("\n\n\n\n\n\t\t\t\tEnter the password you want use:");
        result = scanf("%s", pass);
        // restore terminal
        if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
        {
            perror("tcsetattr");
            return exit(1);
        }
        if (strlen(pass) > 15 || strlen(pass)<1 || result != 1) {
            while (getchar() != '\n'); // Clear the input buffe
            continue;
        }
        if (has_non_printable_char(a)|| has_non_printable_char(pass)) {
            printf("\nYour 're using unwanted character !!!\n");
            return exit(1);
        }
        break;
    }
}

const char *getPassword(sqlite3 *db, struct User u)
{
    const char *sql = "SELECT password FROM user WHERE username = ?";
    
    sqlite3_stmt *stmt;
    int rc;
    char *password = NULL;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // Bind the input username to the SQL statement
    rc = sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind parameter: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Allocate memory for the password and copy the value from the result set
        password = strdup((const char *)sqlite3_column_text(stmt, 0));
        if (password == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            sqlite3_finalize(stmt);
            return NULL;
        }
    } else {
        return "No user found";
    }

    // Finalize the statement and release resources
    sqlite3_finalize(stmt);
    return password;
}
int getRegistred(sqlite3 *db, struct User u) {
    // int rc = sqlite3_open("atm.db", &db);

    // if (rc != SQLITE_OK) {
    //     fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    //     sqlite3_close(db);
    //     return 0;
    // }

    char *sql = "INSERT INTO user (username, password) VALUES (?, ?)";
    
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    rc = sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_STATIC);
    rc = sqlite3_bind_text(stmt, 2, u.password, -1, SQLITE_STATIC);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind parameters: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 0;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement because this user already exist: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_finalize(stmt);
    return 1;
}

