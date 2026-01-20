#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Account {
    int accNo;
    char name[50];
    int pin;
    float balance;
};

struct Transaction {
    int accNo;
    char type[20];
    float amount;
};

void createAccount();
int login(struct Account *user);
void depositMoney(struct Account user);
void withdrawMoney(struct Account user);
void viewBalance(struct Account user);
void deleteAccount();
void sortByBalance();
void saveTransaction(int accNo, char type[], float amount);

int main() {
    int choice;
    struct Account user;

    while (1) {
        printf("\n===== BANK MANAGEMENT SYSTEM =====\n");
        printf("1. Create Account\n");
        printf("2. Login\n");
        printf("3. Delete Account\n");
        printf("4. Sort Accounts by Balance\n");
        printf("5. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: createAccount(); break;
            case 2:
                if (login(&user)) {
                    int ch;
                    do {
                        printf("\n--- ACCOUNT MENU ---\n");
                        printf("1. Deposit\n");
                        printf("2. Withdraw\n");
                        printf("3. View Balance\n");
                        printf("4. Logout\n");
                        printf("Choice: ");
                        scanf("%d", &ch);

                        if (ch == 1) depositMoney(user);
                        else if (ch == 2) withdrawMoney(user);
                        else if (ch == 3) viewBalance(user);
                    } while (ch != 4);
                }
                break;
            case 3: deleteAccount(); break;
            case 4: sortByBalance(); break;
            case 5: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
}

// ---------------- CREATE ACCOUNT ----------------
void createAccount() {
    FILE *fp = fopen("bank.dat", "ab");
    struct Account a;

    printf("Account No: ");
    scanf("%d", &a.accNo);
    printf("Name: ");
    scanf(" %[^\n]", a.name);
    printf("Set 4-digit PIN: ");
    scanf("%d", &a.pin);
    printf("Initial Balance: ");
    scanf("%f", &a.balance);

    fwrite(&a, sizeof(a), 1, fp);
    fclose(fp);
    printf("Account Created Successfully!\n");
}

// ---------------- LOGIN SYSTEM ----------------
int login(struct Account *user) {
    FILE *fp = fopen("bank.dat", "rb");
    int acc, pin;

    printf("Account No: ");
    scanf("%d", &acc);
    printf("PIN: ");
    scanf("%d", &pin);

    while (fread(user, sizeof(*user), 1, fp)) {
        if (user->accNo == acc && user->pin == pin) {
            fclose(fp);
            printf("Login Successful!\n");
            return 1;
        }
    }
    fclose(fp);
    printf("Invalid credentials!\n");
    return 0;
}

// ---------------- DEPOSIT ----------------
void depositMoney(struct Account user) {
    FILE *fp = fopen("bank.dat", "rb+");
    struct Account a;
    float amt;

    printf("Amount: ");
    scanf("%f", &amt);

    while (fread(&a, sizeof(a), 1, fp)) {
        if (a.accNo == user.accNo) {
            a.balance += amt;
            fseek(fp, -sizeof(a), SEEK_CUR);
            fwrite(&a, sizeof(a), 1, fp);
            saveTransaction(a.accNo, "Deposit", amt);
            printf("Deposited Successfully!\n");
            break;
        }
    }
    fclose(fp);
}

// ---------------- WITHDRAW ----------------
void withdrawMoney(struct Account user) {
    FILE *fp = fopen("bank.dat", "rb+");
    struct Account a;
    float amt;

    printf("Amount: ");
    scanf("%f", &amt);

    while (fread(&a, sizeof(a), 1, fp)) {
        if (a.accNo == user.accNo) {
            if (amt > a.balance) {
                printf("Insufficient Balance!\n");
            } else {
                a.balance -= amt;
                fseek(fp, -sizeof(a), SEEK_CUR);
                fwrite(&a, sizeof(a), 1, fp);
                saveTransaction(a.accNo, "Withdraw", amt);
                printf("Withdrawal Successful!\n");
            }
            break;
        }
    }
    fclose(fp);
}

// ---------------- VIEW BALANCE ----------------
void viewBalance(struct Account user) {
    printf("Current Balance: %.2f\n", user.balance);
}

// ---------------- DELETE ACCOUNT ----------------
void deleteAccount() {
    FILE *fp = fopen("bank.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    struct Account a;
    int acc;

    printf("Account No to delete: ");
    scanf("%d", &acc);

    while (fread(&a, sizeof(a), 1, fp)) {
        if (a.accNo != acc)
            fwrite(&a, sizeof(a), 1, temp);
    }

    fclose(fp);
    fclose(temp);
    remove("bank.dat");
    rename("temp.dat", "bank.dat");

    printf("Account deleted (if existed).\n");
}

// ---------------- SORT BY BALANCE ----------------
void sortByBalance() {
    FILE *fp = fopen("bank.dat", "rb");
    struct Account a[100], temp;
    int n = 0, i, j;

    while (fread(&a[n], sizeof(a[n]), 1, fp))
        n++;
    fclose(fp);

    for (i = 0; i < n - 1; i++)
        for (j = i + 1; j < n; j++)
            if (a[i].balance < a[j].balance) {
                temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }

    printf("\n--- Accounts Sorted by Balance ---\n");
    for (i = 0; i < n; i++)
        printf("%d | %s | %.2f\n", a[i].accNo, a[i].name, a[i].balance);
}

// ---------------- TRANSACTION HISTORY ----------------
void saveTransaction(int accNo, char type[], float amount) {
    FILE *fp = fopen("transactions.dat", "ab");
    struct Transaction t;

    t.accNo = accNo;
    strcpy(t.type, type);
    t.amount = amount;

    fwrite(&t, sizeof(t), 1, fp);
    fclose(fp);
}
