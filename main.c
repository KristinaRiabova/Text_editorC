#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dlfcn.h>


typedef struct {
    char *text;
    size_t length;
} Line;


typedef struct {
    Line *lines;
    size_t numLines;
} TextStorage;

typedef char* (*EncryptFunction)(char*, int);
typedef char* (*DecryptFunction)(char*, int);



void printHelp() {
    printf("Choose the command:\n");
    printf("1. Append text symbols to the end\n");
    printf("2. Start a new line\n");
    printf("3. Use files to load/save the information\n");
    printf("4. Print the current text to console\n");
    printf("5. Insert text by line and symbol index\n");
    printf("6. Search for text\n");
    printf("7. Encrypt\n");
    printf("8. Decrypt\n");
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
        if (storage->lines == NULL) {
            printf("Memory allocation failed.\n");
            return;
        }
        storage->numLines = 1;
        storage->lines[0].text = (char *)malloc(newLength + 1);
        if (storage->lines[0].text == NULL) {
            printf("Memory allocation failed.\n");
            free(storage->lines);
            storage->lines = NULL;
            return;
        }
        strcpy(storage->lines[0].text, text);
        storage->lines[0].length = newLength;
    } else if (currentLineIndex < storage->numLines) {
        size_t currentLength = storage->lines[currentLineIndex].length;
        char *newText = (char *)realloc(storage->lines[currentLineIndex].text, currentLength + newLength + 1);
        if (newText == NULL) {
            printf("Memory allocation failed.\n");
            return;
        }
        storage->lines[currentLineIndex].text = newText;
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

    char *newText = (char *)realloc(storage->lines[lineIndex].text, totalLength + 1);
    if (newText == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    storage->lines[lineIndex].text = newText;
    memmove(storage->lines[lineIndex].text + symbolIndex + substringLength,
            storage->lines[lineIndex].text + symbolIndex, lineLength - symbolIndex + 1);
    memcpy(storage->lines[lineIndex].text + symbolIndex, substring, substringLength);
    storage->lines[lineIndex].length = totalLength;
}
void searchForSubstring(const TextStorage *storage, const char *substring) {
    size_t substringLength = strlen(substring);

    for (size_t i = 0; i < storage->numLines; i++) {
        char *lineText = storage->lines[i].text;
        size_t lineLength = storage->lines[i].length;

        for (size_t j = 0; j <= lineLength - substringLength; j++) {
            if (strncmp(lineText + j, substring, substringLength) == 0) {

                printf("Found at Line %zu, Position %zu: %s\n", i + 1, j + 1, lineText + j);
            }
        }
    }
}
void encryptText(TextStorage *storage, int key, EncryptFunction encryptFunction) {
    for (size_t i = 0; i < storage->numLines; i++) {
        storage->lines[i].text = encryptFunction(storage->lines[i].text, key);
    }
}

void decryptText(TextStorage *storage, int key, DecryptFunction decryptFunction) {
    for (size_t i = 0; i < storage->numLines; i++) {
        storage->lines[i].text = decryptFunction(storage->lines[i].text, key);
    }
}
void clear_console(){
    system("clear");
}
int main() {
    void* handle = dlopen("/Users/kristina_mbp/CLionProjects/CaesarEncryptionAlgorithm/caesar.dylib", RTLD_LAZY);
    EncryptFunction encryptFunction = (EncryptFunction)dlsym(handle, "encrypt");
    DecryptFunction decryptFunction = (DecryptFunction)dlsym(handle, "decrypt");

    if (!handle || !encryptFunction || !decryptFunction) {
        printf("Error loading the library or functions.\n");
        return 1;
    }

    int choice;
    TextStorage storage = initializeTextStorage();
    char inputBuffer[256];
    int encryptKey = 0;
    int decryptKey = 0;

    do {
        printHelp();
        clear_console();
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
                printf("New line is started.\n");
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
                if (scanf("%zu", &lineIndex) != 1 || lineIndex >= storage.numLines) {
                    printf("Invalid line index. Please enter a valid index.\n");
                    getchar();
                    break;
                }
                getchar();
                printf("Enter the symbol index: ");
                size_t symbolIndex;
                if (scanf("%zu", &symbolIndex) != 1 || symbolIndex > storage.lines[lineIndex].length) {
                    printf("Invalid symbol index. Please enter a valid index.\n");
                    getchar();
                    break;
                }
                getchar();
                printf("Enter the substring to insert: ");
                char insertion[256];
                fgets(insertion, sizeof(insertion), stdin);
                insertion[strcspn(insertion, "\n")] = '\0';

                insertSubstring(&storage, lineIndex, symbolIndex, insertion);
                break;

            case 6:
                printf("Enter the substring to search for: ");
                char searchSubstring[256];
                fgets(searchSubstring, sizeof(searchSubstring), stdin);
                searchSubstring[strcspn(searchSubstring, "\n")] = '\0';
                searchForSubstring(&storage, searchSubstring);
                break;
            case 7:

                printf("Enter the encryption key: ");
                scanf("%d", &encryptKey);
                getchar();
                encryptText(&storage, encryptKey, encryptFunction);
                printf("Text encrypted with key: %d\n", encryptKey);
                break;
            case 8:

                printf("Enter the decryption key: ");
                scanf("%d", &decryptKey);
                getchar();
                decryptText(&storage, decryptKey, decryptFunction);
                printf("Text decrypted with key: %d\n", decryptKey);
                break;


            default:
                printf("The command is not implemented.\n");
        }
    } while (choice != 0);
    dlclose(handle);
    return 0;
}
