#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_TREE_HT 256


typedef struct MinHeapNode {
    unsigned char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
} MinHeapNode;

typedef struct MinHeap {
    unsigned size;
    unsigned capacity;
    MinHeapNode** array;
} MinHeap;


MinHeapNode* newNode(unsigned char data, unsigned freq);
MinHeap* createMinHeap(unsigned capacity);
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b);
void minHeapify(MinHeap* minHeap, int idx);
int isSizeOne(MinHeap* minHeap);
MinHeapNode* extractMin(MinHeap* minHeap);
void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode);
void buildMinHeap(MinHeap* minHeap);
MinHeap* createAndBuildMinHeap(unsigned char data[], int freq[], int size);
int isLeaf(MinHeapNode* root);
MinHeapNode* buildHuffmanTree(unsigned char data[], int freq[], int size);
void storeCodes(MinHeapNode* root, int arr[], int top, char codes[256][MAX_TREE_HT]);
void compress(char* inputFile, char* outputFile);
void decompress(char* inputFile, char* outputFile);

MinHeapNode* newNode(unsigned char data, unsigned freq) {
    MinHeapNode* temp = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**)malloc(minHeap->capacity * sizeof(MinHeapNode*));
    return minHeap;
}


void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}


void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isSizeOne(MinHeap* minHeap) {
    return (minHeap->size == 1);
}


MinHeapNode* extractMin(MinHeap* minHeap) {
    MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}


void buildMinHeap(MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

MinHeap* createAndBuildMinHeap(unsigned char data[], int freq[], int size) {
    MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);
    minHeap->size = size;
    buildMinHeap(minHeap);
    return minHeap;
}


int isLeaf(MinHeapNode* root) {
    return !(root->left) && !(root->right);
}

MinHeapNode* buildHuffmanTree(unsigned char data[], int freq[], int size) {
    MinHeapNode *left, *right, *top;
    MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}


void storeCodes(MinHeapNode* root, int arr[], int top, char codes[256][MAX_TREE_HT]) {
    if (root->left) {
        arr[top] = 0;
        storeCodes(root->left, arr, top + 1, codes);
    }
    if (root->right) {
        arr[top] = 1;
        storeCodes(root->right, arr, top + 1, codes);
    }
    if (isLeaf(root)) {
        for (int i = 0; i < top; i++)
            codes[root->data][i] = arr[i] + '0';
        codes[root->data][top] = '\0';
    }
}

void compress(char* inputFile, char* outputFile) {
    FILE *input = fopen(inputFile, "rb");
    if (!input) {
        printf("Error: Cannot open input file '%s'\n", inputFile);
        return;
    }


    int freq[256] = {0};
    unsigned char buffer;
    long fileSize = 0;

    while (fread(&buffer, 1, 1, input) == 1) {
        freq[buffer]++;
        fileSize++;
    }

    if (fileSize == 0) {
        printf("Error: Input file is empty\n");
        fclose(input);
        return;
    }

 
    unsigned char data[256];
    int frequencies[256];
    int uniqueChars = 0;

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            data[uniqueChars] = (unsigned char)i;
            frequencies[uniqueChars] = freq[i];
            uniqueChars++;
        }
    }

    if (uniqueChars == 0) {
        printf("Error: No data to compress\n");
        fclose(input);
        return;
    }

    
    MinHeapNode* root = buildHuffmanTree(data, frequencies, uniqueChars);
    char codes[256][MAX_TREE_HT] = {0};
    int arr[MAX_TREE_HT];
    storeCodes(root, arr, 0, codes);

    FILE *output = fopen(outputFile, "wb");
    if (!output) {
        printf("Error: Cannot create output file '%s'\n", outputFile);
        fclose(input);
        return;
    }

    fwrite(&uniqueChars, sizeof(int), 1, output);
    fwrite(&fileSize, sizeof(long), 1, output);
    
    for (int i = 0; i < uniqueChars; i++) {
        fwrite(&data[i], sizeof(unsigned char), 1, output);
        fwrite(&frequencies[i], sizeof(int), 1, output);
    }

    // Compress and write data
    rewind(input);
    unsigned char outByte = 0;
    int bitCount = 0;

    while (fread(&buffer, 1, 1, input) == 1) {
        char* code = codes[buffer];
        for (int i = 0; code[i] != '\0'; i++) {
            if (code[i] == '1')
                outByte |= (1 << (7 - bitCount));
            bitCount++;
            
            if (bitCount == 8) {
                fwrite(&outByte, 1, 1, output);
                outByte = 0;
                bitCount = 0;
            }
        }
    }

    // Write remaining bits
    if (bitCount > 0)
        fwrite(&outByte, 1, 1, output);

    fclose(input);
    fclose(output);

    printf("Compression successful!\n");
    printf("Original size: %ld bytes\n", fileSize);
    
    FILE* check = fopen(outputFile, "rb");
    fseek(check, 0, SEEK_END);
    long compressedSize = ftell(check);
    fclose(check);
    
    printf("Compressed size: %ld bytes\n", compressedSize);
    printf("Compression ratio: %.2f%%\n", (1.0 - (double)compressedSize / fileSize) * 100);
}

void decompress(char* inputFile, char* outputFile) {
    FILE *input = fopen(inputFile, "rb");
    if (!input) {
        printf("Error: Cannot open compressed file '%s'\n", inputFile);
        return;
    }

   
    int uniqueChars;
    long fileSize;
    
    fread(&uniqueChars, sizeof(int), 1, input);
    fread(&fileSize, sizeof(long), 1, input);

    unsigned char data[256];
    int frequencies[256];

    for (int i = 0; i < uniqueChars; i++) {
        fread(&data[i], sizeof(unsigned char), 1, input);
        fread(&frequencies[i], sizeof(int), 1, input);
    }

    MinHeapNode* root = buildHuffmanTree(data, frequencies, uniqueChars);

    FILE *output = fopen(outputFile, "wb");
    if (!output) {
        printf("Error: Cannot create output file '%s'\n", outputFile);
        fclose(input);
        return;
    }

    MinHeapNode* current = root;
    unsigned char byte;
    long decoded = 0;

    while (decoded < fileSize && fread(&byte, 1, 1, input) == 1) {
        for (int i = 7; i >= 0 && decoded < fileSize; i--) {
            if (byte & (1 << i))
                current = current->right;
            else
                current = current->left;

            if (isLeaf(current)) {
                fwrite(&current->data, 1, 1, output);
                decoded++;
                current = root;
            }
        }
    }

    fclose(input);
    fclose(output);

    printf("Decompression successful!\n");
    printf("Decompressed size: %ld bytes\n", fileSize);
}



int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage:\n");
        printf("Compress:   %s 1 inputFile outputFile\n", argv[0]);
        printf("Decompress: %s 2 inputFile outputFile\n", argv[0]);
        return 1;
    }

    int choice = atoi(argv[1]);
    char *inputFile = argv[2];
    char *outputFile = argv[3];

    if (choice == 1) {
        compress(inputFile, outputFile);
    } else if (choice == 2) {
        decompress(inputFile, outputFile);
    } else {
        printf("Invalid choice!\n");
    }

    return 0;
}
