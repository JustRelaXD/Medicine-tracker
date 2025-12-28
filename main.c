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
      float lat;
      float lon;
      PharmMed *inventory;
} Pharmacy;

#define MAX_PHARMACIES 200
Pharmacy pharmacies[MAX_PHARMACIES];
int pharmacyCount = 0;

void userLoginRegister();
void pharmacyLoginRegister();
void loadData();
void landingMenu();
void searchMedicines();
void handleNameError(const char *inputName);
void showNearbyPharmacies(int medicineId);

int getHash(const char *name);
void insertMedicine(int id, const char *name, int qty, float price);
MedicineNode* findMedicineByName(const char *name);

void addPharmacy(int id, const char *name, int pincode, float lat, float lon);
void addMedicineToPharmacy(int pharmacyId, int medId, int qty);
PharmMed* findMedicineInPharmacy(Pharmacy *p, int medId);

void enqueueReservation(Reservation r);
void createAndEnqueueReservation(int pharmacyId, int medicineId, int qty);

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

void createAndEnqueueReservation(int pharmacyId, int medicineId, int qty) {
      Reservation r;
      r.reservationId = nextReservationId++;
      r.pharmacyId = pharmacyId;
      r.medicineId = medicineId;
      r.qty = qty;
      r.status = 0;
      enqueueReservation(r);
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

                  /* ask for customer details */
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

                        createAndEnqueueReservation(pharmacies[selectedIndex].id, selected[s], reqQtys[s]);

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
      printf("\n[Pharmacy Login / Register feature not implemented yet]\n");
}

void loadData() {
      insertMedicine(101, "Paracetamol 500mg", 50, 25.0f);
      insertMedicine(102, "Ibuprofen 200mg",   30, 40.0f);
      insertMedicine(103, "Amoxicillin 250mg", 20, 80.0f);
      insertMedicine(104, "Cetirizine 10mg",   40, 15.0f);
      insertMedicine(105, "Dolo 650",          60, 28.0f);

      pharmacyCount = 0;

      addPharmacy(1, "City Medicals", 400001, 19.0760f, 72.8777f);
      addPharmacy(2, "Health Plus",   400002, 19.0800f, 72.8800f);
      addPharmacy(3, "Care Pharmacy", 400003, 19.0700f, 72.8700f);

      addMedicineToPharmacy(1, 101, 20);
      addMedicineToPharmacy(1, 102, 10);
      addMedicineToPharmacy(1, 105, 15);

      addMedicineToPharmacy(2, 101, 5);
      addMedicineToPharmacy(2, 104, 15);
      addMedicineToPharmacy(2, 103, 7);

      addMedicineToPharmacy(3, 103, 8);
      addMedicineToPharmacy(3, 101, 12);
      addMedicineToPharmacy(3, 105, 6);

      printf("Loaded sample medicines and pharmacies.\n");
}

