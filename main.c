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

void searchMedicines();
void userLoginRegister();
void pharmacyLoginRegister();

void handleNameError(const char *inputName);
void showNearbyPharmacies(int medicineId);
void insertMedicine(int id, const char *name, int qty, float price);

void loadData(); 
void landingMenu();   

int main() {
     
      loadData();
      landingMenu();

      return 0;
}

int getHash(int id) {
      return id % HASH_SIZE;
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

    int h = getHash(id);
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







void handleNameError(const char *inputName) {
      printf("[Name error handler not implemented for '%s']\n", inputName);
}

void showNearbyPharmacies(int medicineId) {
      printf("\n[Nearby pharmacies for medicine ID %d will be shown here]\n", medicineId);
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

      printf("Loaded sample medicines.\n");
}



