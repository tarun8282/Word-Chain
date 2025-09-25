#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>
#include <windows.h>

#define TABLE_SIZE 10007

// ---------------- GLOBAL VARIABLES ----------------
struct Node
{
    char word[50];
    struct Node *next;
};

int ifpaused = 0;
struct Node *dictionaryHead = NULL;
int totalWords = 0;
int currentScore = 0;
int currentTurn = 1;
char prevWord[50];
int gamePaused = 0;
int remainingTimeOnPause = 0;

// ---------------- LINKED LIST FUNCTIONS ----------------
struct Node *createNode(char *w)
{
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    strcpy(newNode->word, w);
    newNode->next = NULL;
    return newNode;
}

void insertWord(char *w)
{
    struct Node *newNode = createNode(w);
    if (dictionaryHead == NULL)
    {
        dictionaryHead = newNode;
        return;
    }
    struct Node *temp = dictionaryHead;
    while (temp->next != NULL)
        temp = temp->next;
    temp->next = newNode;
}

int loadDictionary(const char *dictionary)
{
    FILE *file = fopen(dictionary, "r");
    if (!file)
    {
        printf("Error: Cannot open dictionary file.\n");
        return 0;
    }
    char buffer[50];
    int count = 0;
    while (fgets(buffer, sizeof(buffer), file))
    {
        buffer[strcspn(buffer, "\n")] = '\0';
        insertWord(buffer);
        count++;
    }
    fclose(file);
    return count;
}

char *getRandomWord()
{
    int randomIndex = rand() % totalWords;
    struct Node *temp = dictionaryHead;
    for (int i = 0; i < randomIndex; i++)
        temp = temp->next;
    return temp->word;
}

int wordExists(char *word)
{
    struct Node *temp = dictionaryHead;
    while (temp)
    {
        if (strcasecmp(temp->word, word) == 0)
            return 1;
        temp = temp->next;
    }
    return 0;
}

// ---------------- HASH MAP ----------------
struct HashNode
{
    char word[50];
    struct HashNode *next;
};

struct HashNode *hashTable[TABLE_SIZE] = {NULL};

unsigned int hash(char *word)
{
    unsigned long h = 0;
    for (int i = 0; word[i]; i++)
        h = h * 31 + tolower((unsigned char)word[i]);
    return h % TABLE_SIZE;
}

void insertUsedWord(char *word)
{
    unsigned int idx = hash(word);
    struct HashNode *newNode = (struct HashNode *)malloc(sizeof(struct HashNode));
    strcpy(newNode->word, word);
    newNode->next = hashTable[idx];
    hashTable[idx] = newNode;
}

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

// ---------------- UTILITIES ----------------
void clearScreen()
{
    printf("\033[2J\033[H");
}

void pauseScreen()
{
    printf("\n    Press Enter to return to the main menu...");
    while (getchar() != '\n')
        ;
    getchar();
}

void displayGameOver(const char *reason, const char *word)
{
    printf("\n\n");
    printf("        ╔═══════════════════════════════════════╗\n");
    printf("        ║                 GAME OVER              ║\n");
    printf("        ╠═══════════════════════════════════════╣\n");
    printf("        ║   Reason: %-28s ║\n", reason);
    if (word && word[0])
    {
        printf("        ║   Word:   %-28s ║\n", word);
    }
    printf("        ║            FINAL SCORE: %-5d           ║\n", currentScore);
    printf("        ╚═══════════════════════════════════════╝\n");
    pauseScreen();
}

// ---------------- PAUSE MENU ----------------
void showPauseMenu()
{
    clearScreen();
    printf("\n\n");
    printf("        ╔═══════════════════════════════════════╗\n");
    printf("        ║               GAME PAUSED             ║\n");
    printf("        ╠═══════════════════════════════════════╣\n");
    printf("        ║   1. Resume Game                      ║\n");
    printf("        ║   2. Quit to Main Menu                ║\n");
    printf("        ╚═══════════════════════════════════════╝\n");
    printf("        Select an option > ");

    int choice;
    scanf("%d", &choice);
    getchar();
    if (choice == 1)
    {
        gamePaused, ifpaused = 1;
    }
    else
    {
        gamePaused = 1;
    }
}

// ---------------- GAMEPLAY ----------------
void playGame(int resume)
{
    char userWord[50];
    if (!resume)
    {
        strcpy(prevWord, getRandomWord());
        insertUsedWord(prevWord);
        currentScore = 0;
        currentTurn = 1;
        remainingTimeOnPause = 0;
    }

    while (1)
    {
        ifpaused=0;
        if (gamePaused)
            return;
        clearScreen();
        char nextLetter = toupper(prevWord[strlen(prevWord) - 1]);

        printf("    ╔═════════════════════════════════════════╗\n");
        printf("    ║                 G A M E   O N           ║\n");
        printf("    ╠═════════════════════════════════════════╣\n");
        printf("    ║  Turn: %-5d        Current Score: %-5d ║\n", currentTurn, currentScore);
        printf("    ║           (Press '0' to Pause)          ║\n");
        printf("    ╠═════════════════════════════════════════╣\n");
        printf("    ║   Current Word:   %-20s  ║\n", prevWord);
        printf("    ║   Next word must start with:   '%c'        ║\n", nextLetter);
        printf("    ╚═════════════════════════════════════════╝\n");

        int timeLimit = (remainingTimeOnPause > 0) ? remainingTimeOnPause : 15;
        remainingTimeOnPause = 0;
        clock_t start = clock();
        int index = 0;
        memset(userWord, 0, sizeof(userWord));

        while (1)
        {
            int elapsed = (int)((clock() - start) / CLOCKS_PER_SEC);
            int remaining = timeLimit - elapsed;
            if (remaining < 0)
            {
                displayGameOver("Time's up!", "");
                return;
            }

            if(ifpaused == 1)
            {
                printf("\r    Time: %2d sec |it must start with %c | Your Word > %-25s", remaining, nextLetter, userWord);
            }else
            {
                printf("\r    Time: %2d sec | Your Word > %-25s", remaining, userWord);
            }

            fflush(stdout);

            if (kbhit())
            {
                char c = getch();
                if (c == '0')
                {
                    
                    remainingTimeOnPause = remaining;
                    showPauseMenu();
                    if (gamePaused)
                        return;
                    start = clock();
                    timeLimit = remainingTimeOnPause;
                    remainingTimeOnPause = 0;
                    ifpaused = 1;
                    continue;
                }
                if (c == 13)
                    break;
                else if (c == 8 && index > 0)
                {
                    index--;
                    userWord[index] = '\0';
                }
                else if (isalpha(c) && index < 49)
                {
                    userWord[index++] = c;
                    userWord[index] = '\0';
                }
            }
            Sleep(50);
        }

        printf("\n\n");
        if (strlen(userWord) == 0)
        {
            displayGameOver("No word entered!", "");
            break;
        }
        if (isWordUsed(userWord))
        {
            displayGameOver("Word already used!", userWord);
            break;
        }
        if (!wordExists(userWord))
        {
            displayGameOver("Not in dictionary!", userWord);
            break;
        }
        if (tolower(userWord[0]) != tolower(nextLetter))
        {
            displayGameOver("Wrong starting letter!", userWord);
            break;
        }

        currentScore += strlen(userWord);
        currentTurn++;
        insertUsedWord(userWord);
        strcpy(prevWord, userWord);
    }
}

// ---------------- MENU ----------------
void displayRules()
{
    printf("\n\n");
    printf("    ================ HOW TO PLAY ================\n");
    printf("    1. The computer will start with a random word.\n");
    printf("    2. You must enter a word that starts with the\n");
    printf("       last letter of the computer's word.\n");
    printf("    3. Words cannot be repeated in a single game!\n");
    printf("    4. You will have 10 seconds per turn.\n");
    printf("    =============================================\n\n");
    printf("    Press Enter to return to the menu...");
    while (getchar() != '\n')
        ;
    getchar();
}

int main()
{
    srand(time(NULL));
    totalWords = loadDictionary("dictionary.txt");
    if (totalWords == 0)
        return 1;

    int choice;
    do
    {
        clearScreen();
        printf("\n");
        printf("    ╔═════════════════════════════════════════╗\n");
        printf("    ║   W O R D   C H A I N   C H A L L E N G E ║\n");
        printf("    ╚═════════════════════════════════════════╝\n");
        printf("         🔗Linking one word at a time🔗\n\n");
        printf("          ---------------------------------\n");
        printf("               CURRENT SCORE: %d\n", currentScore);
        printf("          ---------------------------------\n\n");

        printf("               [ 1 ]  Start New Game\n");
        if (currentTurn > 1 && !gamePaused)
            printf("               [ 2 ]  Resume Game\n");
        else
            printf("               [ 2 ]  How to Play\n");
        printf("               [ 3 ]  Exit Game\n\n");
        printf("    Select an option > ");

        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            choice = 0;
        }

        switch (choice)
        {
        case 1:
            clearHashMap();
            playGame(0);
            break;
        case 2:
            if (currentTurn > 1 && !gamePaused)
                playGame(1);
            else
                displayRules();
            break;
        case 3:
            printf("\n    Thanks for playing! Final Score: %d\n\n", currentScore);
            break;
        default:
            printf("\n    (!) Invalid choice. Please select a valid option.\n");
            printf("    Press Enter to continue...");
            while (getchar() != '\n')
                ;
            getchar();
        }
    } while (choice != 3);

    return 0;
}
