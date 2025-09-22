#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<ctype.h>
#define TABLE_SIZE 10007
#include<conio.h>
#include <windows.h>


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

// ---------------- Hash Map for Used Words ----------------
struct HashNode
{
    char word[50];
    struct HashNode *next;
};

struct HashNode *hashTable[TABLE_SIZE] = {NULL};

// Simple hash function
unsigned int hash(char *word)
{
    unsigned long h = 0;
    for (int i = 0; word[i]; i++)
        h = h * 31 + tolower((unsigned char)word[i]); // cast to unsigned char
    return h % TABLE_SIZE;
}

// Insert word into hash map
void insertUsedWord(char *word)
{
    unsigned int idx = hash(word);
    struct HashNode *newNode = (struct HashNode *)malloc(sizeof(struct HashNode));
    strcpy(newNode->word, word);
    newNode->next = hashTable[idx];
    hashTable[idx] = newNode;
}

// Search word in hash map
int isWordUsed(char *word)
{
    unsigned int idx = hash(word);
    struct HashNode *temp = hashTable[idx];
    while (temp)
    {
        if (strcasecmp(temp->word, word) == 0)
            return 1; // word already used
        temp = temp->next;
    }
    return 0; // not used
}

void clearHashMap() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        struct HashNode *temp = hashTable[i];
        while (temp) {
            struct HashNode *toDelete = temp;
            temp = temp->next;
            free(toDelete);
        }
        hashTable[i] = NULL; // set head to NULL
    }
}
void clearScreen() {
    printf("\033[2J\033[H"); // Clear screen and move cursor to home
}

// Main game logic
void pauseScreen() {
    printf("\n    Press Enter to return to the main menu...");
    // Clear the input buffer before waiting for the final Enter
    while (getchar() != '\n'); 
    getchar();
}

void displayGameOver(const char *reason, const char *word, int score) {
    printf("\n\n");
    printf("        ╔═══════════════════════════════════════╗\n");
    printf("        ║                 GAME OVER              ║\n");
    printf("        ╠═══════════════════════════════════════╣\n");
    printf("        ║                                       ║\n");
    printf("        ║   Reason: %-28s ║\n", reason);
    if (word != NULL && word[0] != '\0') {
        printf("        ║   Word:   %-28s ║\n", word);
    }
    printf("        ║                                       ║\n");
    printf("        ║            FINAL SCORE: %-5d           ║\n", score);
    printf("        ║                                       ║\n");
    printf("        ╚═══════════════════════════════════════╝\n");
    pauseScreen(); // Reuse your existing pause function
}

// The score is now passed by reference (using a pointer)
void playGame(struct Node *dictionary, int totalWords, int *score) {
    char prevWord[50], userWord[50];
    int turn = 1;

    // Start the game with a random word
    strcpy(prevWord, getRandomWord(dictionary, totalWords));
    insertUsedWord(prevWord); // Mark starting word as used

    while (1) {
        clearScreen();
        char nextLetter = toupper(prevWord[strlen(prevWord) - 1]);

        // --- THE GAME UI ---
        printf("    ╔═════════════════════════════════════════╗\n");
        printf("    ║                 G A M E   O N           ║\n");
        printf("    ╠═════════════════════════════════════════╣\n");
        printf("    ║  Turn: %-5d        Current Score: %-5d ║\n", turn, *score);
        printf("    ╠═════════════════════════════════════════╣\n");
        printf("    ║                                         ║\n");
        printf("    ║   Current Word:   %-20s  ║\n", prevWord);
        printf("    ║   Next word must start with:   '%c'        ║\n", nextLetter);
        printf("    ║                                         ║\n");
        printf("    ╚═════════════════════════════════════════╝\n");

        // --- TIMER AND LIVE INPUT SECTION ---
        int timeLimit = 10;
        clock_t start = clock();
        int remaining;
        int index = 0;
        memset(userWord, 0, sizeof(userWord));

        while (1) {
            remaining = timeLimit - (int)((clock() - start) / CLOCKS_PER_SEC);
            if (remaining < 0) {
                clearScreen();
                displayGameOver("Time's up!", "", *score);
                return; // Exit playGame immediately
            }

            // Live-updating input line
            printf("\r    Time: %2d sec | Your Word > %-25s", remaining, userWord);
            fflush(stdout);

            if (kbhit()) {
                char c = getch();

                if (c == 13) { // Enter key
                    userWord[index] = '\0';
                    break;
                } else if (c == 8) { // Backspace key
                    if (index > 0) {
                        index--;
                        userWord[index] = '\0';
                        // Add a space at the end to clear any leftover characters from longer words
                        printf("\r    Time: %2d sec | Your Word > %-25s ", remaining, userWord); 
                        fflush(stdout);
                    }
                } else if (index < 49 && isalpha(c)) { // Only allow letters
                    userWord[index++] = c;
                    userWord[index] = '\0';
                }
            }
            Sleep(50); // Small delay to reduce CPU usage
        }
        printf("\n\n"); // Move to a new line after input is finalized

        // --- VALIDATION SECTION ---
        
        // 1. Check if the user entered anything
        if (strlen(userWord) == 0) {
            displayGameOver("No word entered!", "", *score);
            break;
        }
        
        // 2. Check if word has already been used
        if (isWordUsed(userWord)) {
            displayGameOver("Word already used!", userWord, *score);
            break;
        }

        // 3. Check if word exists in the dictionary
        if (!wordExists(dictionary, userWord)) {
            displayGameOver("Not in dictionary!", userWord, *score);
            break;
        }
        
        // 4. Check if it starts with the correct letter
        if (tolower(userWord[0]) != tolower(nextLetter)) {
            char reason[30];
            sprintf(reason, "Must start with '%c'!", nextLetter);
            displayGameOver(reason, userWord, *score);
            break;
        }

        // --- SUCCESS! Prepare for the next round ---
        *score += strlen(userWord); // Score based on word length
        turn++;
        insertUsedWord(userWord);
        strcpy(prevWord, userWord);
    }
}
//Rules
void displayRules() {
    // system("clear") or system("cls")
    printf("\n\n");
    printf("    ================ HOW TO PLAY ================\n");
    printf("    1. The computer will start with a random word.\n");
    printf("    2. You must enter a word that starts with the\n");
    printf("       last letter of the computer's word.\n");
    printf("    3. Words cannot be repeated in a single game!\n");
    printf("    4. The game ends when a player cannot think of a\n");
    printf("       valid word.\n");
    printf("    5. You will have a total of 10 seconds to enter a word.\n");
    printf("    =============================================\n\n");
    printf("    Press Enter to return to the menu...");
    while (getchar() != '\n'); // Clear any leftover newline characters
    getchar(); // Wait for user i
   
}


// Menu-driven approach
int main() {
    struct Node *dictionary = NULL;
    int totalWords;
    int currentScore = 0; // Track score across games

    srand(time(NULL)); // Seed random

    totalWords = loadDictionary("dictionary.txt", &dictionary);
    if (totalWords == 0) {
        printf("FATAL ERROR: dictionary.txt could not be loaded.\n");
        return 1;
    }

    int choice;
    do {
        // Clear screen for a cleaner look (optional, system-dependent)
        // system("cls"); // For Windows
        // system("clear"); // For Linux/macOS

        printf("\n");
        printf("    ╔═════════════════════════════════════════╗\n");
        printf("    ║   W O R D   C H A I N   C H A L L E N G E ║\n");
        printf("    ╚═════════════════════════════════════════╝\n");
        printf("         🔗Linking one word at a time🔗\n\n");
        printf("          ---------------------------------\n");
        printf("               CURRENT SCORE: %d\n", currentScore);
        printf("          ---------------------------------\n\n");

        printf("               [ 1 ]  Start New Game\n");
        printf("               [ 2 ]  How to Play\n");
        printf("               [ 3 ]  Exit Game\n\n");
        printf("    Select an option > ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // Clear invalid input
            choice = 0; // Force it to the default case
        }

       switch (choice) {
            case 1:
                // Reset game state and score for a new game
                clearHashMap();
                currentScore = 0; // <-- ADD THIS LINE

                playGame(dictionary, totalWords, &currentScore);
                break;
            case 2:
                displayRules();
                break;
            case 3:
                printf("\n    Thanks for playing! Final Score: %d\n\n", currentScore);
                break;
            default:
                printf("\n    (!) Invalid choice. Please select a valid option.\n");
                printf("    Press Enter to continue...");
                while (getchar() != '\n'); // Clear buffer
                getchar(); // Wait for user to press Enter
        }
    } while (choice != 3);

    
   

    return 0;
}
