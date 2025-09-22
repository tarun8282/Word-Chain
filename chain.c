#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> // for Sleep()
#include <conio.h>   // already included for kbhit()
#include <time.h>
#include <ctype.h>
#define TABLE_SIZE 10007

struct Node
{
    char word[50];
    struct Node *next;
};

// Create a new node
struct Node *createNode(char *w)
{
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    strcpy(newNode->word, w);
    newNode->next = NULL;
    return newNode;
}

// Insert at end, returns new head
struct Node *insertWord(struct Node *head, char *w)
{
    struct Node *newNode = createNode(w);
    if (!head)
        return newNode; // empty list
    struct Node *temp = head;
    while (temp->next != NULL)
        temp = temp->next;
    temp->next = newNode;
    return head;
}

// Load dictionary into linked list
struct Node *loadDictionary(const char *dictionary, int *totalWords)
{
    FILE *file = fopen(dictionary, "r");
    if (!file)
    {
        printf("Error: Cannot open dictionary file.\n");
        *totalWords = 0;
        return NULL;
    }
    char buffer[50];
    struct Node *head = NULL;
    int count = 0;
    while (fgets(buffer, sizeof(buffer), file))
    {
        buffer[strcspn(buffer, "\n")] = '\0';
        head = insertWord(head, buffer);
        count++;
    }
    fclose(file);
    *totalWords = count;
    return head;
}

// Pick random word from dictionary
char *getRandomWord(struct Node *head, int totalWords)
{
    int randomIndex = rand() % totalWords;
    struct Node *temp = head;
    for (int i = 0; i < randomIndex; i++)
        temp = temp->next;
    return temp->word;
}

// Check if word exists in dictionary
int wordExists(struct Node *head, char *word)
{
    while (head)
    {
        if (strcasecmp(head->word, word) == 0)
            return 1;
        head = head->next;
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
        h = h * 31 + tolower((unsigned char)word[i]);
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
            return 1;
        temp = temp->next;
    }
    return 0;
}

// Clear hash map
void clearHashMap()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        struct HashNode *temp = hashTable[i];
        while (temp)
        {
            struct HashNode *toDelete = temp;
            temp = temp->next;
            free(toDelete);
        }
        hashTable[i] = NULL;
    }
}

// Main game logic
void playGame(struct Node *dictionary, int totalWords)
{
    char prevWord[50], userWord[50];
    strcpy(prevWord, getRandomWord(dictionary, totalWords));
    printf("\nStarting word: %s\n", prevWord);
    insertUsedWord(prevWord);
    unsigned int score = 0;

    while (1)
    {
        printf("Score-->%d\n", score);
        // ---------------- NEW: Live 10-second timer ----------------
        int timeLimit = 10;
        clock_t start = clock();
        int remaining;
        int index = 0;

        memset(userWord, 0, sizeof(userWord));

        while (1)
        {
            remaining = timeLimit - (int)((clock() - start) / CLOCKS_PER_SEC);
            if (remaining < 0)
            {
                printf("\n⏰ Time's up! Game Over!\n");
                return; // exit playGame
            }

            // Clear line : move cursor to start and overwrite
            printf("\r%50s", ""); // clear previous content
            printf("\rTime left: %2d sec | Enter next word: %s", remaining, userWord);

            fflush(stdout);
            if (kbhit())
            {
                char c = getch(); // get character without waiting for enter

                if (c == 13) // Enter pressed
                {
                    userWord[index] = '\0'; // finalize input
                    break;
                }
                else if (c == 8) // Backspace
                {
                    if (index > 0)
                    {
                        index--;
                        userWord[index] = '\0';
                        printf("\rTime left: %2d sec | Enter next word: %s ", remaining, userWord);
                        fflush(stdout);
                    }
                }
                else if (index < 49) // max word length
                {
                    userWord[index++] = c;
                    userWord[index] = '\0';
                }
            }

            Sleep(100); // 0.1 sec delay to reduce CPU usage
        }
        printf("\n"); // new line after input / time display
        // ---------------- END OF TIMER ----------------

        if (isWordUsed(userWord))
        {
            printf("❌ Word already used. Game Over!\n");
            break;
        }

        if (!wordExists(dictionary, userWord))
        {
            printf("❌ Word not found in dictionary. Game Over!\n");
            break;
        }

        char lastChar = prevWord[strlen(prevWord) - 1];
        if (tolower(userWord[0]) != tolower(lastChar))
        {
            printf("❌ Word must start with '%c'. Game Over!\n", lastChar);
            break;
        }

        score += strlen(userWord);
        insertUsedWord(userWord);
        printf("✅ Good! Next round...\n");
        strcpy(prevWord, userWord);
    }
}

// Menu-driven approach
int main()
{
    struct Node *dictionary = NULL;
    int totalWords = 0;

    srand(time(NULL));

    dictionary = loadDictionary("dictionary.txt", &totalWords);
    if (!dictionary)
        return 0;

    int choice;
    do
    {
        printf("\n====== WORD CHAIN GAME ======\n");
        printf("1. Play\n");
        printf("2. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            clearHashMap();
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
