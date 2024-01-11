#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TREE_HT 100
#define MAX_CHAR 256
#define MAX_CODE_LEN 20

// 哈夫曼树节点
struct MinHeapNode {
    char item;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

// 最小堆（用于构建哈夫曼树）
struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

// 创建一个新的最小堆节点
struct MinHeapNode* newNode(char item, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->item = item;
    temp->freq = freq;
    return temp;
}

// 创建一个最小堆
struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

// 交换两个最小堆节点
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// 最小堆化函数
void minHeapify(struct MinHeap* minHeap, int idx) {
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

// 检查最小堆的大小是否为1
int isSizeOne(struct MinHeap* minHeap) {
    return (minHeap->size == 1);
}

// 从最小堆中提取最小频率的节点
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// 插入一个新节点到最小堆中
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// 构建最小堆
void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->size - 1;
    int i;
    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

// 检查是否是叶节点
int isLeaf(struct MinHeapNode* root) {
    return !(root->left) && !(root->right);
}

// 创建并构建最小堆
struct MinHeap* createAndBuildMinHeap(char data[], int freq[], int size) {
    struct MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);
    minHeap->size = size;
    buildMinHeap(minHeap);
    return minHeap;
}

// 构建哈夫曼树
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;

    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

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

// 打印哈夫曼编码
void printCodes(struct MinHeapNode* root, int arr[], int top, FILE* codeFile) {
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1, codeFile);
    }
    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1, codeFile);
    }
    if (isLeaf(root)) {
        fprintf(codeFile, "%c: ", root->item);
        for (int i = 0; i < top; ++i)
            fprintf(codeFile, "%d", arr[i]);
        fprintf(codeFile, "\n");
    }
}

// 主要函数来构建哈夫曼树并打印编码
void HuffmanCodes(char data[], int freq[], int size) {
    struct MinHeapNode* root = buildHuffmanTree(data, freq, size);
    int arr[MAX_TREE_HT], top = 0;

    FILE *codeFile = fopen("2.txt", "w");
    if (!codeFile) {
        perror("无法打开编码表文件");
        return;
    }

    printCodes(root, arr, top, codeFile);
    fclose(codeFile);
}


void HuffmanDecode(struct MinHeapNode* root, char* encodedFile) {
    FILE *file = fopen("encodedFile.txt", "r");
    FILE *outputFile = fopen("3.txt", "w");
    struct MinHeapNode* current = root;
    char ch;

    if (!file) {
        perror("无法打开编码文件");
        return;
    }

    if (!outputFile) {
        perror("无法打开输出文件");
        fclose(file);
        return;
    }

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '0') {
            current = current->left;
        } else if (ch == '1') {
            current = current->right;
        }

        if (isLeaf(current)) {
            fputc(current->item, outputFile); // 写入解码字符
            current = root; // 重置为树的根节点
        }
    }

    fclose(file);
    fclose(outputFile);
}

char huffmanCodes[MAX_CHAR][MAX_CODE_LEN]; // 存储每个字符的哈夫曼编码

// 从文件读取哈夫曼编码表
void loadHuffmanCodes(const char* codeTableFile) {
    FILE* file = fopen("2.txt", "r");
    char line[100], code[MAX_CODE_LEN];
    char ch;

    if (!file) {
        perror("无法打开编码表文件");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%c: %s", &ch, code) == 2) {
            strcpy(huffmanCodes[(unsigned char)ch], code);
        }
    }

    fclose(file);
}

// 编码原始文件
void encodeFile(const char* inputFile, const char* outputFile) {
    FILE *inFile = fopen("1.txt", "r");
    FILE *outFile = fopen("encodedFile.txt", "w");
    char ch;

    if (!inFile || !outFile) {
        perror("无法打开文件");
        if (inFile) fclose(inFile);
        if (outFile) fclose(outFile);
        return;
    }

    while ((ch = fgetc(inFile)) != EOF) {
        fputs(huffmanCodes[(unsigned char)ch], outFile);
    }

    fclose(inFile);
    fclose(outFile);
}

int main() {
    FILE *file;
    int freq[256] = {0}; // ASCII 字符集，频率初始化为0
    char filename[] = "1.txt";
    int unique_chars = 0;

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("无法打开文件");
        return 1;
    }

    // 读取文件并统计字符频率
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (freq[(unsigned char)ch] == 0) {
            unique_chars++;
        }
        freq[(unsigned char)ch]++;
    }
    fclose(file);

    // 准备数据数组和频率数组
    char data[unique_chars];
    int frequencies[unique_chars];
    int index = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] != 0) {
            data[index] = (char)i;
            frequencies[index] = freq[i];
            index++;
        }
    }

    // 用户选择编码或解码
    int choice;
    printf("选择操作：\n1. 哈夫曼编码\n2. 哈夫曼解码\n");
    scanf("%d", &choice);

    if (choice == 1) {
    HuffmanCodes(data, frequencies, unique_chars);

    loadHuffmanCodes("2.txt");

    encodeFile("1.txt", "encodedFile.txt");
    }else if (choice == 2){
        // 这里已有哈夫曼树的结构和编码文件
        struct MinHeapNode* root = buildHuffmanTree(data, frequencies, unique_chars);
        HuffmanDecode(root, "encodedFile.txt"); // 编码文件名是 "encodedFile.txt"
    }

    return 0;
} 