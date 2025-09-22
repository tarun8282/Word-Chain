#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<ctype.h>

struct Node {
    char word[50];
    struct Node *next;
};

// Create a new node
struct Node* createNode(char *w) {
    struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
    strcpy(newNode->word, w);
    newNode->next = NULL;
    return newNode;
}

// Insert at end
void insertWord(struct Node **head, char *w) {
    struct Node *newNode = createNode(w);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    struct Node *temp = *head;
    while (temp->next != NULL)
        temp = temp->next;
    temp->next = newNode;
}

// Load dictionary into linked list
int loadDictionary(const char *dictionary, struct Node **head) {
    FILE *file = fopen(dictionary, "r");
    if (!file) {
        printf("Error: Cannot open dictionary file.\n");
        return 0;
    }
    char buffer[50];
    int count = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // remove newline
        insertWord(head, buffer);
        count++;
    }
    fclose(file);
    return count;
}

// Pick random word from dictionary
char* getRandomWord(struct Node *head, int totalWords) {
    int randomIndex = rand() % totalWords;
    struct Node *temp = head;
    for (int i = 0; i < randomIndex; i++)
        temp = temp->next;
    return temp->word;
}

// Check if word exists in dictionary
int wordExists(struct Node *head, char *word) {
    struct Node *temp = head;
    while (temp) {
        if (strcasecmp(temp->word, word) == 0)
            return 1;
        temp = temp->next;
    }
    return 0;
}

// Main game logic
void playGame(struct Node *dictionary, int totalWords) {
    char prevWord[50], userWord[50];
    strcpy(prevWord, getRandomWord(dictionary, totalWords));
    printf("\nStarting word: %s\n", prevWord);
    int counter=0;
    while (1) {
        printf("Score-->%d\n",counter);
        printf("Enter next word (or type 'quit' to end): ");
        scanf("%s", userWord);

        if (strcmp(userWord, "quit") == 0)
            break;

        // Check if word exists in dictionary
        if (!wordExists(dictionary, userWord)) {
            printf("❌ Word not found in dictionary. Game Over!\n");
            break;
        }

        // Check if it starts with last letter of prev word
        char lastChar = prevWord[strlen(prevWord) - 1];
        if (tolower(userWord[0]) != tolower(lastChar)) {
            printf("❌ Word must start with '%c'. Game Over!\n", lastChar);
            break;
        }

        printf("✅ Good! Next round...\n");
        strcpy(prevWord, userWord);
    }
}

// Menu-driven approach
int main() {
    struct Node *dictionary = NULL;
    int totalWords;

    srand(time(NULL)); // seed random

    totalWords = loadDictionary("dictionary.txt", &dictionary);
    if (totalWords == 0) return 0; // dictionary not loaded

    int choice;
    do {
        printf("\n====== WORD CHAIN GAME ======\n");    // Current Score:
        printf("1. Play\n");
        printf("2. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                playGame(dictionary, totalWords);
                break;
            case 2:
                printf("Exiting game...\n");
                break;
            default:
                printf("Invalid choice! Try again.\n");
        }
    } while (choice != 2);

    return 0;
}
