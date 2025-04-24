#include "header.h"
#include<sqlite3.h>

void initMenu(sqlite3 *db, struct User *u)
{
    int r = 0;
    int option;
    int good;
    system("clear");
    printf("\n\n\t\t======= ATM =======\n");
    printf("\n\t\t-->> Feel free to login / register :\n");
    printf("\n\t\t[1]- login\n");
    printf("\n\t\t[2]- register\n");
    printf("\n\t\t[3]- exit\n");
    while (!r)
    {
        if (scanf("%d", &good)==1)
        {
            option = good;
        }
        else
        {
            while (getchar() != '\n');
            option = 0;
        }

        switch (option)
        {
            case 1:
                while (getchar() != '\n');
                loginMenu(u->name, u->password);
                if (strcmp(u->password, getPassword(db, *u)) == 0)
                {
                    printf("\n\nPassword Match!");
                }
                else
                {
                    printf("\nWrong password!! or User Name\n");
                    exit(1);
                }
                r = 1;                    
                break;
            case 2:
                    // student TODO : add your **Registration** function
                    // here
                while (getchar() != '\n');
                registerMenu(u->name, u->password);   
                int i = getRegistred(db, *u);
                if (i == 1) 
                {
                    printf("\n\n✔ Successfully registered");
                }else 
                {
                    printf("✖ Registration go wrong\n");
                    exit(1);
                }
                r = 1;
                break;
            case 3:
                exit(1);
                break;
            default:
                printf("Insert a valid operation!\n");
        }
    }
};

void mainMenu(sqlite3 *db, struct User u)
{
    int option;
    system("clear");
    printf("\n\n\t\t======= ATM =======\n\n");
    printf("\n\t\t-->> Feel free to choose one of the options below <<--\n");
    printf("\n\t\t[1]- Create a new account\n");
    printf("\n\t\t[2]- Update account information\n");
    printf("\n\t\t[3]- Check accounts\n");
    printf("\n\t\t[4]- Check list of owned account\n");
    printf("\n\t\t[5]- Make Transaction\n");
    printf("\n\t\t[6]- Remove existing account\n");
    printf("\n\t\t[7]- Transfer ownership\n");
    printf("\n\t\t[8]- Logout\n");
    printf("\n\t\t[9]- Exit\n");
    scanf("%d", &option);

    switch (option)
    {
    case 1:
        while (getchar() != '\n');
        createNewAcc(db, u);
        break;
    case 2:
        while (getchar() != '\n');
        updateInformation(db, u);
        break;
    case 3:
        while (getchar() != '\n');
        checkAccounts(db, u);
        break;
    case 4:
        while (getchar() != '\n');
        checkAllAccounts(db, u);
        break;
    case 5:
        while (getchar() != '\n');
        makeTransaction(db, u);
        break;
    case 6:
        while (getchar() != '\n');
        int account_id;
        removeAccount(db, account_id, u);
        break;
    case 7:
        while (getchar() != '\n');
        transferOwner(db, u);
        break;
    case 8:
        while (getchar() != '\n');
        initMenu(db, &u);
        while (getchar() != '\n');
        mainMenu(db, u);
        break;
    case 9:
        exit(1);
        break;
    default:
        printf("Invalid operation!\n");
    }
};


int main()
{
    sqlite3 *db = createDatabase();
    if (db != NULL) {
        insertData(db);
        struct User u;
        initMenu(db, &u);
        while (getchar() != '\n');
        mainMenu(db, u);
        return 0;
    }
}