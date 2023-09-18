#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


void freeTextStorage(TextStorage *storage) {
    for (size_t i = 0; i < storage->numLines; i++) {
        free(storage->lines[i].text);
    }
    free(storage->lines);
}

void appendText(TextStorage *storage, const char *text) {
    size_t newLength = strlen(text);
    size_t currentLineIndex = storage->numLines - 1;

    if (storage->lines == NULL) {
        storage->lines = (Line *)malloc(sizeof(Line));
        storage->numLines = 1;
        storage->lines[0].text = (char *)malloc(newLength + 1);
        strcpy(storage->lines[0].text, text);
        storage->lines[0].length = newLength;
    } else if (currentLineIndex < storage->numLines) {
        size_t currentLength = storage->lines[currentLineIndex].length;
        storage->lines[currentLineIndex].text = (char *)realloc(storage->lines[currentLineIndex].text, currentLength + newLength + 1);
        strcat(storage->lines[currentLineIndex].text, text);
        storage->lines[currentLineIndex].length += newLength;
    }
}
void startNewLine(TextStorage *storage) {
    storage->numLines++;
    storage->lines = (Line *)realloc(storage->lines, sizeof(Line) * storage->numLines);
    storage->lines[storage->numLines - 1].text = NULL;
    storage->lines[storage->numLines - 1].length = 0;
}

void saveTextToFile(const TextStorage *storage, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file != NULL) {
        for (size_t i = 0; i < storage->numLines; i++) {
            fprintf(file, "%s\n", storage->lines[i].text);
        }
        fclose(file);
        printf("Text saved to %s\n", filename);
    } else {
        printf("Error: Unable to open file for writing.\n");
    }
}
void loadTextFromFile(TextStorage *storage, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            appendText(storage, buffer);
        }
        fclose(file);
        printf("Text loaded from %s\n", filename);
    } else {
        printf("Error: Unable to open file for reading.\n");
    }
}
void printTextCharacterByCharacter(const TextStorage *storage) {
    for (size_t i = 0; i < storage->numLines; i++) {
        for (size_t j = 0; j < storage->lines[i].length; j++) {
            printf("%c", storage->lines[i].text[j]);
        }
        printf("\n");
    }
}
void insertSubstring(TextStorage *storage, size_t lineIndex, size_t symbolIndex, const char *substring) {
    size_t substringLength = strlen(substring);
    size_t lineLength = storage->lines[lineIndex].length;
    size_t totalLength = lineLength + substringLength;

    // Reallocate memory to accommodate the new substring
    storage->lines[lineIndex].text = (char *)realloc(storage->lines[lineIndex].text, totalLength + 1);

    // Shift the existing data to make space for the new substring
    memmove(storage->lines[lineIndex].text + symbolIndex + substringLength, storage->lines[lineIndex].text + symbolIndex, lineLength - symbolIndex + 1);

    // Insert the substring at the specified position
    memcpy(storage->lines[lineIndex].text + symbolIndex, substring, substringLength);

    // Update the length of the line
    storage->lines[lineIndex].length = totalLength;
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
                freeTextStorage(&storage);
                break;
            case 1:
                printf("Enter text to append: ");
                fgets(inputBuffer, sizeof(inputBuffer), stdin);
                inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
                appendText(&storage, inputBuffer);
                break;
            case 2:
                startNewLine(&storage);
                break;
            case 3:
                printf("Choose an action:\n");
                printf("1. Load text from a file\n");
                printf("2. Save text to a file\n");
                printf("0. Back to the main menu\n");
                printf("Enter your choice: ");
                int fileAction;
                scanf("%d", &fileAction);
                getchar();

                switch (fileAction) {
                    case 0:
                        break;
                    case 1:
                        printf("Enter the filename to load: ");
                        char loadFilename[256];
                        fgets(loadFilename, sizeof(loadFilename), stdin);
                        loadFilename[strcspn(loadFilename, "\n")] = '\0';
                        loadTextFromFile(&storage, loadFilename);
                        break;
                    case 2:
                        printf("Enter the filename to save: ");
                        char saveFilename[256];
                        fgets(saveFilename, sizeof(saveFilename), stdin);
                        saveFilename[strcspn(saveFilename, "\n")] = '\0';
                        saveTextToFile(&storage, saveFilename);
                        break;
                    default:
                        printf("Invalid choice for file action.\n");
                }
                break;
            case 4:
                printf("Current Text:\n");
                printTextCharacterByCharacter(&storage);
                break;
            case 5:
                printf("Enter the line index: ");
                size_t lineIndex;
                scanf("%zu", &lineIndex);
                getchar(); // Поглотить символ новой строки

                printf("Enter the symbol index: ");
                size_t symbolIndex;
                scanf("%zu", &symbolIndex);
                getchar(); // Поглотить символ новой строки

                printf("Enter the substring to insert: ");
                char insertion[256];
                fgets(insertion, sizeof(insertion), stdin);
                insertion[strcspn(insertion, "\n")] = '\0'; // Удаление символа новой строки

                insertSubstring(&storage, lineIndex, symbolIndex, insertion);
                break;
                default:
                    printf("The command is not implemented.\n");
        }
    } while (choice != 0);

    return 0;
}