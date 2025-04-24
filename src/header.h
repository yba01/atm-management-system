#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
struct Date
{
    int month, day, year;
};

// all fields for each record of an account
struct Record
{
    int id;
    char name[100];
    char country[100];
    int phone;
    int month, day, year;
    char accountType[10];
    int accountNbr;
    double amount;
    char date[10];
    struct Date deposit;
    struct Date withdraw;
};

struct User
{
    int id;
    char name[50];
    char password[50];
};

// databases 's query
sqlite3 *createDatabase(); 
void insertData(sqlite3 *db);
void inserUser(sqlite3 *db);
void inserRecord(sqlite3 *db);
// authentication functions
#define MAX_INPUT_LENGTH 50
void loginMenu(char a[MAX_INPUT_LENGTH], char pass[MAX_INPUT_LENGTH]);
void registerMenu(char a[MAX_INPUT_LENGTH], char pass[MAX_INPUT_LENGTH]);
int getRegistred(sqlite3 *db, struct User u);
const char *getPassword(sqlite3 *db, struct User u);


// system function
void createNewAcc(sqlite3 *db, struct User u);
void mainMenu(sqlite3 *db, struct User u);
void checkAllAccounts(sqlite3 *db, struct User u);
void removeAccount(sqlite3 *db, int account_id, struct User u);
void updateInformation(sqlite3 *db, struct User u);
void checkAccounts(sqlite3 *db, struct User u);
void transferOwner(sqlite3 *db, struct User u);
void makeTransaction(sqlite3 *db, struct User u);
int has_non_printable_char(const char *str);