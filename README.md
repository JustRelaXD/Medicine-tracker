# Medicine Tracker

A C console-based system that allows users to search medicines, check availability across pharmacies, and reserve items for pickup. The program maintains a medicine database, pharmacy inventories, and a reservation queue.

  
###### **Features**  
- Search medicines by exact name  
- Show details of each medicine (name, price, quantity)  
- Add multiple medicines before checking availability  
- Display pharmacies that have all selected medicines  
- Show pharmacy name, pincode, and available stock  
- Reserve medicines for pickup  
- Ask user for name and phone number during booking  
- Update stock automatically after reservation  
- Generate a unique reservation ID  

  
###### **Data Structures Used**  
- Hash table for fast medicine lookup  
- Linked lists for medicine lists and pharmacy inventories
- Queue for reservations  
- Arrays for storing selected medicine IDs  
- Structs for pharmacies, medicines, inventory nodes, and reservations  

  
###### **How It Works**  
- Program loads sample medicines and pharmacies during startup  
- User searches for a medicine  
- User can add more medicines to the selection  
- System finds pharmacies that have all selected medicines  
- User chooses a pharmacy and enters quantity for each item  
- System collects name and phone number  
- Reservation is created and stock is reduced  

  
###### **Project Structure**  
main.c
- Medicine Hash Table  
- Pharmacy List and Inventory  
- Reservation Queue  
- Search and Selection System  
- Reservation / Pickup Logic  

  
###### **Compilation**
```
clang main.c -o medicine_tracker
```

###### **Run the program:**
```
./medicine_tracker
```

  
 ###### **Future Enhancements**  
- File-based loading and saving of data  
- Pharmacy login and dashboard  
- Reservation confirmation interface  
- Error-tolerant / fuzzy medicine search  
- Map-based nearest pharmacy display  
- Export reservation logs  

  
###### **About This Project**  
- Created for a Project-Based Learning (PBL) requirement  
- Demonstrates data structure usage in C  
- Focuses on practical problem-solving  
- Implements inventory and reservation management  
- Provides a clean menu-driven interface  

  
