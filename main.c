#include <stdio.h>

typedef struct {
    char *text;
    size_t length;
} Line;


typedef struct {
    Line *lines;
    size_t numLines;
} TextStorage;




void printHelp() {
    printf("Choose the command:\n");
    printf("1. Append text symbols to the end\n");
    printf("2. Start a new line\n");
    printf("3. Use files to load/save the information\n");
    printf("4. Print the current text to console\n");
    printf("5. Insert text by line and symbol index\n");
    printf("6. Search for text\n");
    printf("7. Clear the console (optional)\n");
    printf("0. Exit\n");
}


TextStorage initializeTextStorage() {
    TextStorage storage;
    storage.lines = NULL;
    storage.numLines = 0;
    return storage;
}

int main() {
    int choice;
    TextStorage storage = initializeTextStorage();
    char inputBuffer[256];

    do {
        printHelp();
        printf("Choose the command: ");
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 0:
                printf("Exiting the text editor.\n");
                break;
                printf("The command is not implemented.\n");
        }
    } while (choice != 0);

    return 0;
}