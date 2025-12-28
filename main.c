#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define HASH_SIZE 101

typedef struct {
      int id;
      char name[50];
      int quantity;
      float price;
} Medicine;

typedef struct MedicineNode {
      Medicine data;
      struct MedicineNode *next;
} MedicineNode;

typedef struct {
      int reservationId;
      int pharmacyId;
      int medicineId;
      int qty;
      int status;
      char customerName[100];
      char customerPhone[20];
} Reservation;

typedef struct ReservationNode {
    Reservation data;
    struct ReservationNode *next;
} ReservationNode;

ReservationNode *resFront = NULL;
ReservationNode *resRear = NULL;
int nextReservationId = 1;

MedicineNode *medicineHash[HASH_SIZE] = { NULL };

typedef struct PharmMed {
      int medId;
      int qty;
      struct PharmMed *next;
} PharmMed;

typedef struct {
      int id;
      char name[50];
      int pincode;
      char phone[20];
      float lat;
      float lon;
      PharmMed *inventory;
} Pharmacy;

typedef struct {
    int pharmacyId;
    char username[50];
    char password[50];
} PharmacyAccount;

#define MAX_PHARMACIES 200
PharmacyAccount pharmacyAccounts[MAX_PHARMACIES];
int accountCount = 0;

Pharmacy pharmacies[MAX_PHARMACIES];
int pharmacyCount = 0;

void userLoginRegister();
void pharmacyLoginRegister();
void loadData();
void loadMedicines();
void loadPharmacies();
void loadInventory();

void landingMenu();

void searchMedicines();
void handleNameError(const char *inputName);
void showNearbyPharmacies(int medicineId);
Pharmacy* findPharmacyById(int id);
int getHash(const char *name);
void insertMedicine(int id, const char *name, int qty, float price);
MedicineNode* findMedicineByName(const char *name);

void addPharmacy(int id, const char *name, int pincode, float lat, float lon);
void addMedicineToPharmacy(int pharmacyId, int medId, int qty);
PharmMed* findMedicineInPharmacy(Pharmacy *p, int medId);

void saveReservationToFile(Reservation r);
void loadReservations();
void viewAllReservations();  // optional
void enqueueReservation(Reservation r);
void createAndEnqueueReservation(int pharmacyId, int medicineId, int qty, const char *custName, const char *custPhone);

void loadPharmacyAccounts();
int pharmacyLogin();
void pharmacyDashboard(int pharmacyId);
void viewPharmacyInventory(int pharmacyId);
void addMedicineToInventory(int pharmacyId);
void updateMedicineQuantity(int pharmacyId);
void removeMedicineFromInventory(int pharmacyId);
void viewPharmacyReservations(int pharmacyId);
void updateReservationStatus(int pharmacyId);
void saveAllReservationsToFile();

int main() {
     
      loadData();
      landingMenu();

      return 0;
}

void landingMenu() {
    int choice;

    while (1) {
        printf("\n===============================\n");
        printf("         Medicine Tracker\n");
        printf("===============================\n");
        printf("1. Search Medicines (No login required)\n");
        printf("2. User Login / Register\n");
        printf("3. Pharmacy Login / Register\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                searchMedicines();
                break;

            case 2:
                userLoginRegister();
                break;

            case 3:
                pharmacyLoginRegister();
                break;

            case 4:
                printf("Exiting...\n");
                return;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}

int getHash(const char *name) {
    unsigned long hash = 5381;
    int c;
    while ((c = *name++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

void insertMedicine(int id, const char *name, int qty, float price) {
    MedicineNode *node = (MedicineNode*)malloc(sizeof(MedicineNode));
    if (node == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    node->data.id = id;
    strcpy(node->data.name, name);
    node->data.quantity = qty;
    node->data.price = price;

    int h = getHash(name);
    node->next = medicineHash[h];
    medicineHash[h] = node;
}

void searchMedicines() {
      char input[50];
      int found = 0;

      printf("\n----- Search Medicines -----\n");
      printf("Enter medicine name : ");
      scanf(" %[^\n]", input);   

      for (int i = 0; i < HASH_SIZE; i++) {
            MedicineNode *curr = medicineHash[i];
            while (curr != NULL) {
                  if (strcmp(curr->data.name, input) == 0) {
                        printf("\nMedicine Found:\n");
                        printf("ID       : %d\n", curr->data.id);
                        printf("Name     : %s\n", curr->data.name);
                        printf("Quantity : %d\n", curr->data.quantity);
                        printf("Price    : %.2f\n", curr->data.price);
                        showNearbyPharmacies(curr->data.id);

                        found = 1;
                        break;
                    }
                  curr = curr->next;
              }
            if (found) break;
        }

      if (!found) {
            printf("\nMedicine '%s' not found.\n", input);
            handleNameError(input);
        }
}

Pharmacy* findPharmacyById(int id) {
    for (int i = 0; i < pharmacyCount; i++) {
        if (pharmacies[i].id == id)
            return &pharmacies[i];
    }
    return NULL;
}

void enqueueReservation(Reservation r) {
      ReservationNode *node = (ReservationNode*)malloc(sizeof(ReservationNode));
      if (!node) return;
      node->data = r;
      node->next = NULL;
      if (!resRear) {
            resFront = resRear = node;
        } else {
            resRear->next = node;
            resRear = node;
        }
}

void createAndEnqueueReservation(int pharmacyId, int medicineId, int qty, 
                                  const char *custName, const char *custPhone) {
    Reservation r;
    r.reservationId = nextReservationId++;
    r.pharmacyId = pharmacyId;
    r.medicineId = medicineId;
    r.qty = qty;
    r.status = 0; 
    strncpy(r.customerName, custName, sizeof(r.customerName) - 1);
    r.customerName[sizeof(r.customerName) - 1] = '\0';
    strncpy(r.customerPhone, custPhone, sizeof(r.customerPhone) - 1);
    r.customerPhone[sizeof(r.customerPhone) - 1] = '\0';
    
    enqueueReservation(r);
    saveReservationToFile(r);  
}

void saveReservationToFile(Reservation r) {
    FILE *fp = fopen("reservation.txt", "a");
    if (!fp) {
        printf("Error: Could not open reservation.txt for writing\n");
        return;
    }
    
    fprintf(fp, "%d|%d|%d|%d|%d|%s|%s\n",
            r.reservationId,
            r.pharmacyId,
            r.medicineId,
            r.qty,
            r.status,
            r.customerName,
            r.customerPhone);
    
    fclose(fp);
}

void viewAllReservations() {
    if (!resFront) {
        printf("\nNo reservations found.\n");
        return;
    }
    
    printf("\n========== ALL RESERVATIONS ==========\n");
    ReservationNode *cur = resFront;
    
    while (cur) {
        Reservation r = cur->data;

        Pharmacy *p = findPharmacyById(r.pharmacyId);
        const char *pharmName = p ? p->name : "Unknown";

        MedicineNode *mnode = NULL;
        for (int i = 0; i < HASH_SIZE && !mnode; i++) {
            MedicineNode *m = medicineHash[i];
            while (m) {
                if (m->data.id == r.medicineId) {
                    mnode = m;
                    break;
                }
                m = m->next;
            }
        }
        const char *medName = mnode ? mnode->data.name : "Unknown";

        const char *statusText;
        switch(r.status) {
            case 0: statusText = "Pending"; break;
            case 1: statusText = "Completed"; break;
            case 2: statusText = "Cancelled"; break;
            default: statusText = "Unknown";
        }
        
        printf("\nReservation ID: %d\n", r.reservationId);
        printf("Customer: %s (Phone: %s)\n", r.customerName, r.customerPhone);
        printf("Pharmacy: %s\n", pharmName);
        printf("Medicine: %s\n", medName);
        printf("Quantity: %d\n", r.qty);
        printf("Status: %s\n", statusText);
        printf("-----------------------------------\n");
        
        cur = cur->next;
    }
}

void addPharmacy(int id, const char *name, int pincode, float lat, float lon) {
      if (pharmacyCount >= MAX_PHARMACIES) return;
      pharmacies[pharmacyCount].id = id;
      strncpy(pharmacies[pharmacyCount].name, name, sizeof(pharmacies[pharmacyCount].name)-1);
      pharmacies[pharmacyCount].name[sizeof(pharmacies[pharmacyCount].name)-1] = '\0';
      pharmacies[pharmacyCount].pincode = pincode;
      pharmacies[pharmacyCount].lat = lat;
      pharmacies[pharmacyCount].lon = lon;
      pharmacies[pharmacyCount].inventory = NULL;
      pharmacyCount++;
}

PharmMed* findMedicineInPharmacy(Pharmacy *p, int medId) {
      PharmMed *cur = p->inventory;
      while (cur) {
            if (cur->medId == medId) return cur;
            cur = cur->next;
        }
      return NULL;
}

MedicineNode* findMedicineByName(const char *name) {

    int index = getHash(name);
    MedicineNode *cur = medicineHash[index];

    while (cur) {
        if (strcmp(cur->data.name, name) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

void addMedicineToPharmacy(int pharmacyId, int medId, int qty) {
      int idx = -1;
      for (int i = 0; i < pharmacyCount; i++) {
            if (pharmacies[i].id == pharmacyId) { idx = i; break; }
        }
      if (idx == -1) return;
      Pharmacy *p = &pharmacies[idx];
      PharmMed *node = findMedicineInPharmacy(p, medId);
      if (node) {
            node->qty += qty;
        } else {
            node = (PharmMed*)malloc(sizeof(PharmMed));
            if (!node) return;
            node->medId = medId;
            node->qty = qty;
            node->next = p->inventory;
            p->inventory = node;
        }
}

void showNearbyPharmacies(int initialMedicineId) {
      int selected[100];
      int selCount = 0;
      if (initialMedicineId != 0) selected[selCount++] = initialMedicineId;
      char input[100];

      while (1) {
            printf("\nSelected medicines (%d): ", selCount);
            for (int i = 0; i < selCount; i++) {
                  MedicineNode *mnode = NULL;
                  for (int b = 0; b < HASH_SIZE && !mnode; b++) {
                        MedicineNode *cur = medicineHash[b];
                        while (cur) {
                              if (cur->data.id == selected[i]) { mnode = cur; break; }
                              cur = cur->next;
                          }
                    }
                  if (mnode) printf("%s%s", mnode->data.name, (i==selCount-1) ? "" : ", ");
                  else printf("%d%s", selected[i], (i==selCount-1) ? "" : ", ");
              }
            printf("\n");

            printf("\nOptions:\n");
            printf("1. Add another medicine\n");
            printf("2. Find pharmacies that have ALL selected medicines\n");
            printf("3. Book reservation at a pharmacy for all selected medicines\n");
            printf("4. Return to previous menu\n");
            printf("Enter choice: ");

            int choice;
            if (scanf(" %d", &choice) != 1) {
                  int ch;
                  while ((ch = getchar()) != '\n' && ch != EOF) { }
                  choice = 4;
              }

            if (choice == 1) {
                  printf("Enter medicine name : ");
                  scanf(" %[^\n]", input);
                  MedicineNode *mnode = findMedicineByName(input);
                  if (!mnode) {
                        printf("Medicine '%s' not found.\n", input);
                        handleNameError(input);
                        printf("1. Try again  2. Return\n");
                        int c;
                        if (scanf(" %d", &c) != 1) {
                              int ch;
                              while ((ch = getchar()) != '\n' && ch != EOF) { }
                              c = 2;
                          }
                        if (c == 1) continue;
                        else return;
                    }
                  int exists = 0;
                  for (int i = 0; i < selCount; i++) if (selected[i] == mnode->data.id) { exists = 1; break; }
                  if (!exists && selCount < 100) {
                        selected[selCount++] = mnode->data.id;
                        printf("Added '%s'. Total selected: %d\n", mnode->data.name, selCount);

                        /* show details immediately after adding */
                        printf("Details:\n");
                        printf("Medicine : %s\n", mnode->data.name);
                        printf("Price    : %.2f\n", mnode->data.price);
                        printf("In stock : %d (global)\n", mnode->data.quantity);
                    } else {
                        printf("Already selected or selection full.\n");
                    }
                  continue;
              }

            else if (choice == 2) {
                  int foundAny = 0;
                  printf("\nPharmacies with ALL selected medicines:\n");
                  for (int i = 0; i < pharmacyCount; i++) {
                        int hasAll = 1;
                        for (int s = 0; s < selCount; s++) {
                              PharmMed *pm = findMedicineInPharmacy(&pharmacies[i], selected[s]);
                              if (!(pm && pm->qty > 0)) { hasAll = 0; break; }
                          }
                        if (hasAll) {
                              foundAny = 1;
                              printf(" - %s | Pincode: %d\n", pharmacies[i].name, pharmacies[i].pincode);
                          }
                    }
                  if (!foundAny) printf(" No pharmacies found with all selected medicines.\n");
                  continue;
              }

            else if (choice == 3) {
                  if (selCount == 0) { printf("No medicines selected.\n"); continue; }

                  int matches[1000];
                  int matchCount = 0;

                  for (int i = 0; i < pharmacyCount; i++) {
                        int hasAll = 1;
                        for (int s = 0; s < selCount; s++) {
                              PharmMed *pm = findMedicineInPharmacy(&pharmacies[i], selected[s]);
                              if (!(pm && pm->qty > 0)) { hasAll = 0; break; }
                          }
                        if (hasAll) {
                              matches[matchCount++] = i;
                              if (matchCount >= 1000) break;
                          }
                    }

                  if (matchCount == 0) { printf("No pharmacies currently have all selected medicines.\n"); continue; }

                  printf("\nPharmacies that have all selected medicines:\n");
                  for (int i = 0; i < matchCount; i++) {
                        int pidx = matches[i];
                        printf("%d) %s | Pincode: %d\n", i+1, pharmacies[pidx].name, pharmacies[pidx].pincode);
                    }

                  printf("Enter pharmacy number to book from (1-%d): ", matchCount);
                  int pick;
                  if (scanf(" %d", &pick) != 1) {
                        int ch;
                        while ((ch = getchar()) != '\n' && ch != EOF) { }
                        continue;
                    }
                  if (pick < 1 || pick > matchCount) { printf("Invalid choice.\n"); continue; }

                  int selectedIndex = matches[pick - 1];
                  int ok = 1;
                  int reqQtys[100];
                  char customerName[100];
                  char customerPhone[40];

                  for (int s = 0; s < selCount; s++) {
                        PharmMed *pm = findMedicineInPharmacy(&pharmacies[selectedIndex], selected[s]);
                        int avail = pm ? pm->qty : 0;

                        MedicineNode *mnode = NULL;
                        for (int b = 0; b < HASH_SIZE && !mnode; b++) {
                              MedicineNode *cur = medicineHash[b];
                              while (cur) {
                                    if (cur->data.id == selected[s]) { mnode = cur; break; }
                                    cur = cur->next;
                                }
                          }
                        const char *mname = mnode ? mnode->data.name : "Unknown";

                        printf("Enter quantity for '%s' (available %d): ", mname, avail);
                        if (scanf(" %d", &reqQtys[s]) != 1) {
                              int ch;
                              while ((ch = getchar()) != '\n' && ch != EOF) { }
                              ok = 0;
                              break;
                          }
                        if (reqQtys[s] <= 0 || reqQtys[s] > avail) { printf("Invalid qty for %s.\n", mname); ok = 0; break; }
                    }

                  if (!ok) { printf("Booking aborted due to invalid quantities.\n"); continue; }

                  printf("Enter customer name: ");
                  while ((getchar()) != '\n' && getchar() != EOF) { } /* clear newline */
                  scanf(" %[^\n]", customerName);
                  printf("Enter phone number: ");
                  scanf(" %[^\n]", customerPhone);

                  for (int s = 0; s < selCount; s++) {
                        PharmMed *pm = findMedicineInPharmacy(&pharmacies[selectedIndex], selected[s]);
                        pm->qty -= reqQtys[s];

                        MedicineNode *mnode = NULL;
                        for (int b = 0; b < HASH_SIZE && !mnode; b++) {
                              MedicineNode *cur = medicineHash[b];
                              while (cur) {
                                    if (cur->data.id == selected[s]) { mnode = cur; break; }
                                    cur = cur->next;
                                }
                          }
                        const char *mname = mnode ? mnode->data.name : "Unknown";

                       createAndEnqueueReservation(pharmacies[selectedIndex].id, selected[s], reqQtys[s], customerName, customerPhone);

                        printf("Reserved %d of %s at %s for %s (Phone: %s). Reservation ID: %d\n",
                                                      reqQtys[s], mname, pharmacies[selectedIndex].name, customerName, customerPhone, nextReservationId - 1);
                    }
                  return;
              }

            else if (choice == 4) {
                  return;
              }

            else {
                  printf("Invalid choice.\n");
                  continue;
              }
        }
}

void handleNameError(const char *inputName) {
      printf("[Name error handler not implemented for '%s']\n", inputName);
}


void userLoginRegister() {
      printf("\n[User Login / Register feature not implemented yet]\n");
}

void pharmacyLoginRegister() {
      int pharmacyId = pharmacyLogin();
    if (pharmacyId != -1) {
        pharmacyDashboard(pharmacyId);
    }
}

void loadData() {
      loadMedicines();
      loadPharmacies();
      loadInventory();
      loadReservations();
      loadPharmacyAccounts();
      printf("Loaded sample medicines and pharmacies.\n");
}

void loadMedicines() {
    FILE *fp = fopen("medicines.txt", "r");
    if (!fp) {
        printf("Error: Could not open medicines.txt\n");
        return;
    }

    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        char *token;
        int id;
        char name[100];
        float price;

        token = strtok(line, "|");
        if (!token) continue;
        id = atoi(token);

        token = strtok(NULL, "|");
        if (!token) continue;
        strcpy(name, token);
        name[strcspn(name, "\n")] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        price = atof(token);

        insertMedicine(id, name, 0, price);
    }

    fclose(fp);
}

void loadPharmacies() {
    FILE *fp = fopen("pharmacies.txt", "r");
    if (!fp) {
        printf("Error: Could not open pharmacies.txt\n");
        return;
    }

    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        char *token;
        int id, pincode;
        char name[100];
        char phone[20];

        token = strtok(line, "|");
        if (!token) continue;
        id = atoi(token);

        token = strtok(NULL, "|");
        if (!token) continue;
        strcpy(name, token);
        name[strcspn(name, "\n")] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        pincode = atoi(token);

        token = strtok(NULL, "|");
        if (!token) continue;
        strcpy(phone, token);
        phone[strcspn(phone, "\n")] = '\0';

        pharmacies[pharmacyCount].id = id;
        strcpy(pharmacies[pharmacyCount].name, name);
        pharmacies[pharmacyCount].pincode = pincode;
        strcpy(pharmacies[pharmacyCount].phone, phone);
        pharmacies[pharmacyCount].inventory = NULL;

        pharmacyCount++;
    }

    fclose(fp);
}

void loadInventory() {
    FILE *fp = fopen("inventory.txt", "r");
    if (!fp) {
        printf("Error: Could not open inventory.txt\n");
        return;
    }

    printf("Loading inventory from file...\n");
    char line[256];
    int lineCount = 0;

    while (fgets(line, sizeof(line), fp)) {
        lineCount++;
        char *token;
        int pharmacyId, medId, qty;

        token = strtok(line, "|");
        if (!token) {
            printf("Line %d: Failed to parse pharmacy ID\n", lineCount);
            continue;
        }
        pharmacyId = atoi(token);

        token = strtok(NULL, "|");
        if (!token) {
            printf("Line %d: Failed to parse medicine ID\n", lineCount);
            continue;
        }
        medId = atoi(token);

        token = strtok(NULL, "|");
        if (!token) {
            printf("Line %d: Failed to parse quantity\n", lineCount);
            continue;
        }
        qty = atoi(token);

        Pharmacy *p = findPharmacyById(pharmacyId);
        if (!p) {
            printf("Line %d: Pharmacy ID %d not found!\n", lineCount, pharmacyId);
            continue;
        }

        PharmMed *node = malloc(sizeof(PharmMed));
        if (!node) {
            printf("Line %d: Memory allocation failed\n", lineCount);
            continue;
        }
        node->medId = medId;
        node->qty = qty;
        node->next = p->inventory;
        p->inventory = node;

        printf("Added: Pharmacy %d (%s) - Medicine %d - Qty %d\n", 
               pharmacyId, p->name, medId, qty);
    }

    fclose(fp);
    
    printf("\n===== INVENTORY SUMMARY =====\n");
    printf("Total pharmacies loaded: %d\n\n", pharmacyCount);
    
    for (int i = 0; i < pharmacyCount; i++) {
        printf("Pharmacy: %s (ID: %d, Pincode: %d)\n", 
               pharmacies[i].name, pharmacies[i].id, pharmacies[i].pincode);
        
        PharmMed *cur = pharmacies[i].inventory;
        if (!cur) {
            printf("  No inventory loaded\n");
        } else {
            while (cur) {
                printf("  - Medicine ID %d: Qty %d\n", cur->medId, cur->qty);
                cur = cur->next;
            }
        }
        printf("\n");
    }
    printf("===========================\n\n");
}

void loadReservations() {
    FILE *fp = fopen("reservation.txt", "r");
    if (!fp) {
        printf("No existing reservations found (reservation.txt doesn't exist)\n");
        return;
    }
    
    char line[512];
    int count = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        Reservation r;
        char *token;

        token = strtok(line, "|");
        if (!token) continue;
        r.reservationId = atoi(token);

        if (r.reservationId >= nextReservationId) {
            nextReservationId = r.reservationId + 1;
        }
        
        token = strtok(NULL, "|");
        if (!token) continue;
        r.pharmacyId = atoi(token);
        
        token = strtok(NULL, "|");
        if (!token) continue;
        r.medicineId = atoi(token);

        token = strtok(NULL, "|");
        if (!token) continue;
        r.qty = atoi(token);
        
        token = strtok(NULL, "|");
        if (!token) continue;
        r.status = atoi(token);
        
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(r.customerName, token, sizeof(r.customerName) - 1);
        r.customerName[sizeof(r.customerName) - 1] = '\0';
        
        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(r.customerPhone, token, sizeof(r.customerPhone) - 1);
        r.customerPhone[sizeof(r.customerPhone) - 1] = '\0';  
        r.customerPhone[strcspn(r.customerPhone, "\n")] = '\0';
        
        enqueueReservation(r);
        count++;
    }
    
    fclose(fp);
    printf("Loaded %d existing reservations\n", count);
    printf("Next reservation ID will be: %d\n\n", nextReservationId);
}

void loadPharmacyAccounts() {
    FILE *fp = fopen("accounts.txt", "r");
    if (!fp) {
        printf("Warning: Could not open accounts.txt. No pharmacy accounts loaded.\n");
        return;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char *token;
        int pharmacyId;
        char username[50], password[50];
        
        token = strtok(line, "|");
        if (!token) continue;
        pharmacyId = atoi(token);
        
        token = strtok(NULL, "|");
        if (!token) continue;
        strcpy(username, token);
        
        token = strtok(NULL, "|");
        if (!token) continue;
        strcpy(password, token);
        password[strcspn(password, "\n")] = '\0';
        
        pharmacyAccounts[accountCount].pharmacyId = pharmacyId;
        strcpy(pharmacyAccounts[accountCount].username, username);
        strcpy(pharmacyAccounts[accountCount].password, password);
        accountCount++;
    }
    
    fclose(fp);
    printf("Loaded %d pharmacy accounts\n", accountCount);
}

int pharmacyLogin() {
    char username[50], password[50];
    
    printf("\n===== Pharmacy Login =====\n");
    printf("Username: ");
    scanf(" %[^\n]", username);
    printf("Password: ");
    scanf(" %[^\n]", password);
    
    for (int i = 0; i < accountCount; i++) {
        if (strcmp(pharmacyAccounts[i].username, username) == 0 &&
            strcmp(pharmacyAccounts[i].password, password) == 0) {
            
            Pharmacy *p = findPharmacyById(pharmacyAccounts[i].pharmacyId);
            if (p) {
                printf("\n✓ Login successful! Welcome %s\n", p->name);
                return pharmacyAccounts[i].pharmacyId;
            }
        }
    }
    
    printf("\n✗ Invalid username or password\n");
    return -1;
}

void pharmacyDashboard(int pharmacyId) {
    Pharmacy *pharmacy = findPharmacyById(pharmacyId);
    if (!pharmacy) {
        printf("Error: Pharmacy not found\n");
        return;
    }
    
    while (1) {
        printf("\n========================================\n");
        printf("  %s - Dashboard\n", pharmacy->name);
        printf("========================================\n");
        printf("1. View Inventory\n");
        printf("2. Add Medicine to Inventory\n");
        printf("3. Update Medicine Quantity\n");
        printf("4. View Reservations\n");
        printf("5. Update Reservation Status\n");
        printf("6. Logout\n");
        printf("Enter choice: ");
        
        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        
        switch (choice) {
            case 1:
                viewPharmacyInventory(pharmacyId);
                break;
            case 2:
                addMedicineToInventory(pharmacyId);
                break;
            case 3:
                updateMedicineQuantity(pharmacyId);
                break;
            case 4:
                viewPharmacyReservations(pharmacyId);
                break;
            case 5:
                updateReservationStatus(pharmacyId);
                break;
            case 6:
                printf("Logging out...\n");
                return;
            default:
                printf("Invalid choice\n");
        }
    }
}

void viewPharmacyInventory(int pharmacyId) {
    Pharmacy *p = findPharmacyById(pharmacyId);
    if (!p) return;
    
    printf("\n===== Inventory for %s =====\n", p->name);
    
    PharmMed *cur = p->inventory;
    if (!cur) {
        printf("No medicines in inventory\n");
        return;
    }
    
    printf("%-10s %-30s %-10s %-10s\n", "Med ID", "Name", "Quantity", "Price");
    printf("----------------------------------------------------------------\n");
    
    while (cur) {
        MedicineNode *mnode = NULL;
        for (int i = 0; i < HASH_SIZE && !mnode; i++) {
            MedicineNode *m = medicineHash[i];
            while (m) {
                if (m->data.id == cur->medId) {
                    mnode = m;
                    break;
                }
                m = m->next;
            }
        }
        
        if (mnode) {
            printf("%-10d %-30s %-10d %-10.2f\n", 
                   cur->medId, mnode->data.name, cur->qty, mnode->data.price);
        } else {
            printf("%-10d %-30s %-10d %-10s\n", 
                   cur->medId, "Unknown", cur->qty, "N/A");
        }
        
        cur = cur->next;
    }
}

void addMedicineToInventory(int pharmacyId) {
    Pharmacy *p = findPharmacyById(pharmacyId);
    if (!p) return;
    
    char medName[100];
    int qty;
    
    printf("\n===== Add Medicine to Inventory =====\n");
    printf("Enter medicine name: ");
    scanf(" %[^\n]", medName);
    
    MedicineNode *mnode = findMedicineByName(medName);
    if (!mnode) {
        printf("Medicine '%s' not found in database\n", medName);
        printf("Available medicines:\n");
        for (int i = 0; i < HASH_SIZE; i++) {
            MedicineNode *cur = medicineHash[i];
            while (cur) {
                printf("  - %s (ID: %d)\n", cur->data.name, cur->data.id);
                cur = cur->next;
            }
        }
        return;
    }
    
    PharmMed *existing = findMedicineInPharmacy(p, mnode->data.id);
    if (existing) {
        printf("✗ Medicine already exists in your inventory (Current qty: %d)\n", existing->qty);
        printf("Please use 'Update Medicine Quantity' option instead.\n");
        return;
    }
    
    printf("Enter quantity: ");
    if (scanf("%d", &qty) != 1 || qty <= 0) {
        printf("Invalid quantity\n");
        while (getchar() != '\n');
        return;
    }
    
    PharmMed *node = (PharmMed*)malloc(sizeof(PharmMed));
    if (!node) {
        printf("Memory allocation failed\n");
        return;
    }
    node->medId = mnode->data.id;
    node->qty = qty;
    node->next = p->inventory;
    p->inventory = node;
    
    printf("✓ Added %d units of %s to inventory\n", qty, mnode->data.name);
}

void updateMedicineQuantity(int pharmacyId) {
    Pharmacy *p = findPharmacyById(pharmacyId);
    if (!p) return;
    
    int medId, newQty;
    
    printf("\n===== Update Medicine Quantity =====\n");
    viewPharmacyInventory(pharmacyId);
    
    printf("\nEnter Medicine ID to update: ");
    if (scanf("%d", &medId) != 1) {
        while (getchar() != '\n');
        return;
    }
    
    PharmMed *pm = findMedicineInPharmacy(p, medId);
    if (!pm) {
        printf("Medicine not found in your inventory\n");
        return;
    }
    
    printf("Current quantity: %d\n", pm->qty);
    printf("Enter new quantity: ");
    if (scanf("%d", &newQty) != 1 || newQty < 0) {
        printf("Invalid quantity\n");
        while (getchar() != '\n');
        return;
    }
    
    pm->qty = newQty;
    printf("✓ Quantity updated to %d\n", newQty);
}

void viewPharmacyReservations(int pharmacyId) {
    printf("\n===== Reservations for Pharmacy ID %d =====\n", pharmacyId);
    
    ReservationNode *cur = resFront;
    int found = 0;
    
    while (cur) {
        if (cur->data.pharmacyId == pharmacyId) {
            found = 1;
            Reservation r = cur->data;
            
            MedicineNode *mnode = NULL;
            for (int i = 0; i < HASH_SIZE && !mnode; i++) {
                MedicineNode *m = medicineHash[i];
                while (m) {
                    if (m->data.id == r.medicineId) {
                        mnode = m;
                        break;
                    }
                    m = m->next;
                }
            }
            const char *medName = mnode ? mnode->data.name : "Unknown";
            
            const char *statusText;
            switch(r.status) {
                case 0: statusText = "Pending"; break;
                case 1: statusText = "Completed"; break;
                case 2: statusText = "Cancelled"; break;
                default: statusText = "Unknown";
            }
            
            printf("\n[Reservation ID: %d]\n", r.reservationId);
            printf("  Customer: %s (Phone: %s)\n", r.customerName, r.customerPhone);
            printf("  Medicine: %s\n", medName);
            printf("  Quantity: %d\n", r.qty);
            printf("  Status: %s\n", statusText);
            printf("  ------------------------\n");
        }
        cur = cur->next;
    }
    
    if (!found) {
        printf("No reservations found\n");
    }
}

void updateReservationStatus(int pharmacyId) {
    printf("\n===== Update Reservation Status =====\n");
    viewPharmacyReservations(pharmacyId);
    
    int resId, newStatus;
    
    printf("\nEnter Reservation ID to update: ");
    if (scanf("%d", &resId) != 1) {
        while (getchar() != '\n');
        return;
    }
    
    ReservationNode *cur = resFront;
    while (cur) {
        if (cur->data.reservationId == resId && cur->data.pharmacyId == pharmacyId) {
            printf("Current status: %d (0=Pending, 1=Completed, 2=Cancelled)\n", cur->data.status);
            printf("Enter new status (0/1/2): ");
            
            if (scanf("%d", &newStatus) != 1 || newStatus < 0 || newStatus > 2) {
                printf("Invalid status\n");
                while (getchar() != '\n');
                return;
            }
            
            cur->data.status = newStatus;
            saveAllReservationsToFile();
            printf("✓ Reservation status updated\n");
            return;
        }
        cur = cur->next;
    }
    
    printf("Reservation not found or doesn't belong to your pharmacy\n");
}

void saveAllReservationsToFile() {
    FILE *fp = fopen("reservation.txt", "w");
    if (!fp) {
        printf("Error: Could not open reservation.txt for writing\n");
        return;
    }
    
    ReservationNode *cur = resFront;
    while (cur) {
        fprintf(fp, "%d|%d|%d|%d|%d|%s|%s\n",
                cur->data.reservationId,
                cur->data.pharmacyId,
                cur->data.medicineId,
                cur->data.qty,
                cur->data.status,
                cur->data.customerName,
                cur->data.customerPhone);
        cur = cur->next;
    }
    
    fclose(fp);
}