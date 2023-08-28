#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to hold patient information
struct Patient {
    char name[50];
    int age;
    char gender;
    char condition[100];
};

// Function to add a new patient
void addPatient(struct Patient *patients, int *numPatients) {
    if (*numPatients >= 100) {
        printf("Maximum number of patients reached.\n");
        return;
    }

    struct Patient newPatient;

    printf("Enter patient name: ");
    scanf("%s", newPatient.name);

    printf("Enter patient age: ");
    scanf("%d", &newPatient.age);

    printf("Enter patient gender (M/F): ");
    scanf(" %c", &newPatient.gender);

    printf("Enter patient condition: ");
    scanf("%s", newPatient.condition);

    patients[*numPatients] = newPatient;
    (*numPatients)++;

    printf("Patient added successfully.\n");
}

// Function to display patient information
void displayPatients(struct Patient *patients, int numPatients) {
    if (numPatients == 0) {
        printf("No patients in the database.\n");
        return;
    }

    printf("Patient List:\n");
    for (int i = 0; i < numPatients; i++) {
        printf("Patient %d\n", i + 1);
        printf("Name: %s\n", patients[i].name);
        printf("Age: %d\n", patients[i].age);
        printf("Gender: %c\n", patients[i].gender);
        printf("Condition: %s\n", patients[i].condition);
        printf("-----------------\n");
    }
}

// Function to search for a patient by name
void searchPatient(struct Patient *patients, int numPatients, char *searchName) {
    int found = 0;

    for (int i = 0; i < numPatients; i++) {
        if (strcmp(patients[i].name, searchName) == 0) {
            printf("Patient found:\n");
            printf("Name: %s\n", patients[i].name);
            printf("Age: %d\n", patients[i].age);
            printf("Gender: %c\n", patients[i].gender);
            printf("Condition: %s\n", patients[i].condition);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Patient not found.\n");
    }
}

int main() {
    struct Patient patients[100];
    int numPatients = 0;
    int choice;
    char searchName[50];

    do {
        printf("\nHospital Management System\n");
        printf("1. Add Patient\n");
        printf("2. Display Patients\n");
        printf("3. Search Patient\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addPatient(patients, &numPatients);
                break;
            case 2:
                displayPatients(patients, numPatients);
                break;
            case 3:
                printf("Enter the name of the patient to search: ");
                scanf("%s", searchName);
                searchPatient(patients, numPatients, searchName);
                break;
            case 4:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Please select a valid option.\n");
        }
    } while (choice != 4);

    return 0;
}

