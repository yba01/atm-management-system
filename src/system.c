#include "header.h"
#include<sqlite3.h>
#include<string.h>

int saveAccountToFile(sqlite3 *db, struct User u, struct Record r) {
    const char *sql = "INSERT INTO record (username, account_id, date, country, phone, balance, type_of_account) VALUES (?, ?, ?, ?, ?, ?, ?)";
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    // Allocate memory for the date string
    char date[11]; // Format: DD/MM/YYYY\0
    snprintf(date, sizeof(date), "%02d/%02d/%04d", r.day, r.month, r.year);

    rc = sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind username: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    rc = sqlite3_bind_int(stmt, 2, r.accountNbr);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind account number: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    rc = sqlite3_bind_text(stmt, 3, date, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind date: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    rc = sqlite3_bind_text(stmt, 4, r.country, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind country: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    rc = sqlite3_bind_int(stmt, 5, r.phone);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind phone: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    rc = sqlite3_bind_double(stmt, 6, r.amount);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind amount: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    rc = sqlite3_bind_text(stmt, 7, r.accountType, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind account type: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_finalize(stmt);
    return 1;
}

void stayOrReturn(sqlite3 *db, void f(sqlite3 *db, struct User u), struct User u, char *message)
{
    int option;
    int result;
    // if (notGood == 0)
    {
        while (getchar() != '\n');
        system("clear");
        printf("%s", message);
    invalid:
        printf("\nEnter 0 to try again, 1 to return to main menu and 2 to exit:");
        result = scanf("%d", &option);
        if (result != 1) {
            printf("Insert a valid operation!\n");
            while (getchar() != '\n'); // Clear the input buffer
            goto invalid;
        }
        while (getchar() != '\n');
        if (option == 0)
            f(db, u);
        else if (option == 1)
            mainMenu(db, u);
        else if (option == 2)
            exit(0);
        else
        {
            printf("Insert a valid operation!\n");
            while (getchar() != '\n'); // Clear the input buffer
            goto invalid;
        }
    }
    // else
    // {
    //     printf("\nEnter 1 to go to the main menu and 0 to exit:");
    //     scanf("%d", &option);
    // }
    // if (option == 1)
    // {
    //     system("clear");
    //     mainMenu(db, u);
    // }
    // else
    // {
    //     system("clear");
    //     exit(1);
    // }
}

void success(sqlite3 *db, struct User u)
{
    int option;
    int result;
    printf("\n✔ Success!\n\n");
invalid:
    printf("Enter 1 to go to the main menu and 0 to exit!\n");
    result =  scanf("%d", &option);
    if (result != 1) {
            printf("Insert a valid operation!\n");
            while (getchar() != '\n'); // Clear the input buffer
            goto invalid;
    }
    while (getchar() != '\n');
    system("clear");
    if (option == 1)
    {
        mainMenu(db, u);
    }
    else if (option == 0)
    {
        exit(1);
    }
    else
    {
        printf("Insert a valid operation!\n");
        while (getchar() != '\n'); // Clear the input buffer
        goto invalid;
    }
}

int count_rows(sqlite3 *db, const char *username, const int accountNbr) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM user JOIN record ON user.username = record.username WHERE record.username = ? AND account_id = ?";
    int count = 0;
    int rc;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Bind the parameters
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, accountNbr);

    // Execute the query and count the number of rows
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        count++;
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution error: %s\n", sqlite3_errmsg(db));
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    return count;
}

void createNewAcc(sqlite3 *db, struct User u) {
    struct Record r;
    int result;

    while (1) {
        printf("\033[H\033[J");
        printf("\t\t\t===== New record =====\n");

        printf("\nEnter today's date (mm/dd/yyyy): ");
        result = scanf("%d/%d/%d", &r.month, &r.day, &r.year);
        if (result != 3 || r.day < 1 || r.day > 31 || r.month < 1 || r.month > 12 || r.year < 1960 || r.year > 2024) {
            printf("✖ Use correct date format\n\n");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        if ((r.year % 4 == 0 && r.year % 100 != 0) || (r.year % 400 == 0)) {
            if (r.month == 2 && r.day > 29) {
                printf("✖ Use correct date format\n\n");
                while (getchar() != '\n'); // Clear the input buffer
                continue;
            }
        } else {
            if (r.month == 2 && r.day > 28) {
                printf("✖ Use correct date format\n\n");
                while (getchar() != '\n'); // Clear the input buffer
                continue;
            }
        }
        while (getchar() != '\n'); // Clear the input buffer

        printf("\nEnter the account number: ");
        result = scanf("%d", &r.accountNbr);
        if (result != 1 || count_rows(db, u.name, r.accountNbr) > 0) {
            printf("✖ Choose a valid account number\n\n");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        while (getchar() != '\n'); // Clear the input buffer

        printf("\nEnter the country: ");
        result = scanf("%s", r.country);
        if (result != 1 || has_non_printable_char(r.country)) {
            printf("✖ Choose a valid country format.\n\n");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        while (getchar() != '\n'); // Clear the input buffer

        printf("\nEnter phone number: ");
        result = scanf("%d", &r.phone);
        if (result != 1 || r.phone < 0 || r.phone > 999999999) {
            printf("✖ Choose a valid phone number format: <integer>\n\n");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        while (getchar() != '\n'); // Clear the input buffer

        printf("\nEnter amount to deposit: $");
        result = scanf("%lf", &r.amount);
        if (result != 1 || r.amount > 2000000 || r.amount < 0) {
            printf("✖ Choose a valid amount format: <double>  >  -1 && <double> < 2.000.000$\n\n");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        while (getchar() != '\n'); // Clear the input buffer

        printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice: ");
        result = scanf("%s", r.accountType);
        if (result != 1 ||
            (strcmp(r.accountType, "saving") != 0 &&
             strcmp(r.accountType, "current") != 0 &&
             strcmp(r.accountType, "fixed01") != 0 &&
             strcmp(r.accountType, "fixed02") != 0 &&
             strcmp(r.accountType, "fixed03") != 0)) {
            printf("✖ This account type does not exist!!!\n\n");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        while (getchar() != '\n'); // Clear the input buffer

        saveAccountToFile(db, u, r);

        success(db, u);
        break; // Exit loop if everything is correct
    }
}

void checkAllAccounts(sqlite3 *db, struct User u)
{
    printf("\033[H\033[J");
    const char *sql = 
        "SELECT account_id, date, country, phone, balance, type_of_account "
        "FROM record "
        "JOIN user ON user.username = record.username "
        "WHERE user.username = ?";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return ;
    }

    // Bind the username parameter
    rc = sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind username: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return ;
    }

    // Execute the statement and display the results
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Callback function prints each row
        for (int i = 0; i < sqlite3_column_count(stmt); i++) {
            printf("%s = %s\n", sqlite3_column_name(stmt, i), sqlite3_column_text(stmt, i));
        }
        printf("\n\n");
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to fetch results: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return ;
    }

    // Finalize the statement to release resources
    sqlite3_finalize(stmt);
    success(db, u);
}

void removeAccount(sqlite3 *db, int account_id, struct User u) {
    printf("\033[H\033[J");
    while(1){
        printf("\nEnter the account id of the one you want to remove:");
        int result = scanf("%d", &account_id);
        if (result != 1)
        {
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }else if(count_rows(db, u.name, account_id) != 1) 
        {
            printf("\033[H\033[J");
            printf("This account doesn't exist. Try again!!!");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        break;
    }
    while (getchar() != '\n'); // Clear the input buffer
    const char *sql = "DELETE FROM record WHERE (account_id = ? AND username = ?);";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return ;
    }

    // Bind the username parameter
    rc = sqlite3_bind_int(stmt, 1, account_id);
     if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind username: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return ;
    }
    rc = sqlite3_bind_text(stmt, 2, u.name, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind username: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return ;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return ;
    }
     // Finalize the statement to release resources
    sqlite3_finalize(stmt);
    success(db, u);
}

void updateInformation(sqlite3 *db, struct User u) {
    int result;
    int account_id;
    int choice;
    char country[20];
    int phone;
    int rc;
    printf("\033[H\033[J");
    while(1){
        printf("\n\n\t\t======= ATM =======\n");
        printf("\nEnter the account id of the one you want to update:");
        result = scanf("%d", &account_id);
        if (result != 1)
        {
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }else if(count_rows(db, u.name, account_id) != 1) 
        {
            printf("\033[H\033[J");
            printf("This account doesn't exist. Try again!!!");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        break;
    }
    while (getchar() != '\n'); // Clear the input buffer
    while(1){
        printf("\033[H\033[J");
        printf("\n\n\t\t======= ATM =======\n");
        printf("\n\t\t-->> What do you want to update<<--\n");
        printf("\n\t\t[1]- Phone number\n");
        printf("\n\t\t[2]- Country\n");
        result = scanf("%d", &choice);
        if (result != 1 || (choice != 1 && choice != 2))
        {
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        break;
    }
    while (getchar() != '\n'); // Clear the input buffer
    if (choice == 1) {
        while(1){
            printf("\033[H\033[J");
            printf("\n\n\t\t======= ATM =======\n");
            printf("\nEnter the new phone number:");
            result = scanf("%d", &phone);
            if (result != 1 || phone < 0 || phone > 999999999)
            {
                while (getchar() != '\n'); // Clear the input buffer
                continue;
            }
            break;
        }
        const char *sql = "UPDATE record SET phone = ? WHERE (account_id = ? AND username = ?);";
        sqlite3_stmt *stmt;
        
        // Prepare the SQL statement
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
        
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return ;
        }

        // Bind values to the SQL statement
        sqlite3_bind_int(stmt, 1, phone); // new value to set
        sqlite3_bind_int(stmt, 2, account_id); // ID of the row to update
        sqlite3_bind_text(stmt, 3, u.name, -1, SQLITE_STATIC);

        // Execute the SQL statement
        rc = sqlite3_step(stmt);
        
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        } else {
            fprintf(stdout, "Record updated successfully\n");
        }

        // Clean up
        sqlite3_finalize(stmt);
        success(db, u);
    }else {
        while(1){
            printf("\033[H\033[J");
            printf("\n\n\t\t======= ATM =======\n");
            printf("\nEnter the new country name:");
            result = scanf("%s", country);
            if (result != 1 || has_non_printable_char(country))
            {
                while (getchar() != '\n'); // Clear the input buffer
                continue;
            }
            break;
        }
        while (getchar() != '\n'); // Clear the input buffer

        const char *sql = "UPDATE record SET country = ? WHERE (username = ? AND account_id = ?);";
        sqlite3_stmt *stmt;
                    // Prepare the SQL statement
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);        
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return ;
        }

            // Bind values to the SQL statement
        sqlite3_bind_text(stmt, 1, country, -1, SQLITE_STATIC); // new value to set
        sqlite3_bind_text(stmt, 2, u.name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, account_id); // ID of the row to update

            // Execute the SQL statement
        rc = sqlite3_step(stmt);
            
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        } else {
            fprintf(stdout, "Record updated successfully\n");
        }

            // Clean up
        sqlite3_finalize(stmt);
        success(db, u);
    }
}

void checkAccounts(sqlite3 *db, struct User u){
    int account_id;
    int result;
    int rc;
    int day, month, year;
    printf("\033[H\033[J");
    while(1){
        printf("\n\n\t\t======= ATM =======\n");
        printf("\nEnter the account id of the one you want to update:");
        result = scanf("%d", &account_id);
        if (result != 1)
        {
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }else if(count_rows(db, u.name, account_id) != 1) 
        {
            printf("\033[H\033[J");
            printf("This account doesn't exist. Try again!!!");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        break;
    }
    while (getchar() != '\n'); // Clear the input buffer
    const char *sql= "SELECT date, balance, type_of_account FROM record WHERE (username = ? AND account_id = ?);";
    // Prepare the SQL statement
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }
    // Bind values to the placeholders
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, account_id);
    // Execute the SQL statement
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char *date = sqlite3_column_text(stmt, 0);
        double solde = sqlite3_column_double(stmt, 1);
        const unsigned char *type = sqlite3_column_text(stmt, 2);
        
        result = sscanf(date, "%d/%d/%d", &day, &month, &year);
        if (result != 3) {
            return;
        }
        if(strcmp(type, "saving") == 0) {
            printf("\n\nYou will get $%.2f as interest on day %d of every month.\n", solde * 0.07 / 12, day);
            success(db, u);
        }else if (strcmp(type, "fixed01") == 0) {
            printf("\n\nYou will get $%.2f as interest on %d/%d/%d.\n", solde * 0.04, day, month, year+1);
            success(db, u);
        }else if (strcmp(type, "fixed02")  == 0) {
            printf("\n\nYou will get $%.2f as interest on %d/%d/%d.\n", solde * 0.05 * 2, day, month, year+2);
            success(db, u);        
        }else if (strcmp(type, "fixed03") == 0) {
            printf("\n\nYou will get $%.2f as interest on %d/%d/%d.\n", solde * 0.08 * 3, day, month, year+3);
            success(db, u);        
        }else {
            printf("\n\nYou will not get interests because the account is of type current.\n");
            success(db, u);
        }
    }
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        return;
    }
}

int isUserExist(sqlite3 *db, const char *username) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM user WHERE username = ?";
    int count = 0;
    int rc;

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Bind the parameters
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    // Execute the query and count the number of rows
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        count++;
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution error: %s\n", sqlite3_errmsg(db));
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    return count;
}

void transferOwner(sqlite3 *db, struct User u) {
    printf("\033[H\033[J");
    int account_id;
    int result;
    int rc;
    char username[50];
    while(1){
        printf("\n\n\t\t======= ATM =======\n");
        printf("\nEnter the account id of the one you want to transfer:");
        result = scanf("%d", &account_id);
        if (result != 1)
        {
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }else if(count_rows(db, u.name, account_id) != 1) 
        {
            printf("\033[H\033[J");
            printf("This account doesn't exist. Try again!!!");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        break;
    }

    while (getchar() != '\n'); // Clear the input buffer
    while(1){
        printf("\nEnter the username of the one you want to transfer the account:");
        result = scanf("%s", username);
        if (result != 1 || has_non_printable_char(username))
        {
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }else if(isUserExist(db, username) != 1) 
        {
            printf("\033[H\033[J");
            printf("This account doesn't exist. Try again!!!");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        break;
    }
    while (getchar() != '\n');
     const char *sql = "UPDATE record SET username = ? WHERE (account_id = ? AND username = ?);";
        sqlite3_stmt *stmt;
        
        // Prepare the SQL statement
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
        
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return ;
        }

        // Bind values to the SQL statement
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, account_id); // ID of the row to update
        sqlite3_bind_text(stmt, 3, u.name, -1, SQLITE_STATIC);

        // Execute the SQL statement
        rc = sqlite3_step(stmt);
        
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        } else {
            fprintf(stdout, "Record updated successfully\n");
        }

        // Clean up
        sqlite3_finalize(stmt);
        success(db, u);

}

void makeTransaction(sqlite3 *db, struct User u) {
    int result;
    int option;
    int account_id;
    double amount;
    int rc;
    double value;
    while (1)
    {
        printf("\033[H\033[J");
        printf("\n\n\t\t======= ATM =======\n");
        printf("\n\t\t-->> What kinf of transaction do you espect to do:<<--\n");
        printf("\n\t\t[1]- Deposit\n");
        printf("\n\t\t[2]- Withdraw\n");
        result = scanf("%d", &option);
        if (option !=1 && option!= 2)
        {
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        break;
    }
    while (getchar() != '\n'); // Clear the input buffer
    while(1){
        printf("\n\t\t-->> Enter the account id of the one you want to do a transaction:");
        result = scanf("%d", &account_id);
        if (result != 1)
        {
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }else if(count_rows(db, u.name, account_id) != 1) 
        {
            printf("\033[H\033[J");
            printf("This account doesn't exist. Try again!!!");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        break;
    }
    while (getchar() != '\n'); // Clear the input buffer
    while (1)
    {
        printf("\n\t\t-->> Enter amount of the transation: $");
        result = scanf("%lf", &amount);
        if (result != 1 || amount > 2000000 || amount < 0) {
            printf("✖ Choose a valid amount format: <double> && <double> < 2.000.000$\n\n");
            while (getchar() != '\n'); // Clear the input buffer
            continue;
        }
        break;
    }
    const char *sql= "SELECT balance, type_of_account FROM record WHERE (username = ? AND account_id = ?);";
    // Prepare the SQL statement
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }
    // Bind values to the placeholders
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, account_id);
    // Execute the SQL statement
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        double solde = sqlite3_column_double(stmt, 0);
        const unsigned char *type = sqlite3_column_text(stmt, 1);
        
        if (strcmp(type, "fixed01") == 0 || strcmp(type, "fixed02")  == 0 || strcmp(type, "fixed03") == 0) {
            stayOrReturn(db, makeTransaction, u,"\n✖ You can't make a transaction with this type of account!!\n");
        }

        if (option == 1) {
            value = amount + solde;
            if (value > 2000000){
            stayOrReturn(db, makeTransaction, u,"\n✖ You can't make a deposit with this amount!!\n");
            }
            
        }else{
            value = solde - amount;
              if (value < 0){
            stayOrReturn(db, makeTransaction, u,"\n✖ You can't make a withdraw with this amount!!\n");                
            }
        }

        const char *sql = "UPDATE record SET balance = ? WHERE (account_id = ? AND username = ?);";
        sqlite3_stmt *stmt;
        
        // Prepare the SQL statement
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
        
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return ;
        }

        // Bind values to the SQL statement
        sqlite3_bind_int(stmt, 1, value);
        sqlite3_bind_int(stmt, 2, account_id); // ID of the row to update
        sqlite3_bind_text(stmt, 3, u.name, -1, SQLITE_STATIC);

        // Execute the SQL statement
        rc = sqlite3_step(stmt);
        
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        } else {
            fprintf(stdout, "Record updated successfully\n");
        }

        // Clean up
        sqlite3_finalize(stmt);
        success(db, u);   
    }
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        return;
    }
    
}