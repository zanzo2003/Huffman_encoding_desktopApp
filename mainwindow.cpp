#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <iostream>
// Helper classes and functions from earlier (Node, Huffman Tree, encoding, decoding, etc.)
// ...

using namespace std;


class Node {
public:
    char data;
    int freq;
    Node *left, *right;

    Node(char data, int freq, Node* left = nullptr, Node* right = nullptr) {
        this->data = data;
        this->freq = freq;
        this->left = left;
        this->right = right;
    }
};

Node* rootTree;
// Comparator for the priority queue
class compare {
public:
    bool operator()(Node* l, Node* r) {
        return (l->freq > r->freq);
    }
};

Node* buildHuffmanTree(const std::map<char, int>& freqMap) {
    priority_queue<Node*, vector<Node*>, compare> minHeap;

    for (auto pair : freqMap) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    while (minHeap.size() != 1) {
        Node* left = minHeap.top(); minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();

        int sum = left->freq + right->freq;
        Node* newNode = new Node('$', sum, left, right);
        minHeap.push(newNode);
    }
    rootTree = minHeap.top();
    return minHeap.top(); // Root node
}

void generateCodes(Node* root, string str, map<char, string> &huffmanCode) {
    if (!root)
        return;

    if (!root->left && !root->right) {
        huffmanCode[root->data] = str;
    }

    generateCodes(root->left, str + "0", huffmanCode);
    generateCodes(root->right, str + "1", huffmanCode);
}

string encode(const string &text, map<char, string> &huffmanCode) {
    string encodedData = "";
    for (char ch : text) {
        encodedData += huffmanCode[ch];
    }
    return encodedData;
}

string decode(Node* root, const string &encodedStr) {
    string decodedStr = "";
    Node* current = root;
    for (char bit : encodedStr) {
        if (bit == '0')
            current = current->left;
        else
            current = current->right;

        if (!current->left && !current->right) {
            decodedStr += current->data;
            current = root;
        }
    }
    return decodedStr;
}






MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadFileButton_clicked()
{
    loadedFilePath = QFileDialog::getOpenFileName(this, tr("Open Text File"), "", tr("Text Files (*.txt)"));
    if (!loadedFilePath.isEmpty()) {
        ui->fileStatusLabel->setText("File Loaded: " + loadedFilePath);
    } else {
        QMessageBox::warning(this, "Error", "No file selected.");
    }
}



void MainWindow::on_encodeButton_clicked()
{
    if (loadedFilePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please load a file first.");
        return;
    }

    std::ifstream inputFile(loadedFilePath.toStdString());
    std::string text((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    // Generate frequency map
    std::map<char, int> freqMap;
    for (char ch : text) {
        freqMap[ch]++;
    }

    // Build Huffman Tree
    Node* root = buildHuffmanTree(freqMap);

    // Generate Huffman Codes
    std::map<char, std::string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    // Encode the file
    std::string encodedData = encode(text, huffmanCode);

    // Write the compressed data to a file
    std::ofstream compressedFile("/Users/shashwathbhaskar/Downloads/compressed.bin", ios::binary);
    compressedFile << encodedData;
    cout<<encodedData<<endl;
    compressedFile.close();

    QMessageBox::information(this, "Success", "File compressed successfully.");

}


void MainWindow::on_decodeButton_clicked()
{
    QString compressedFilePath = QFileDialog::getOpenFileName(this, tr("Open Compressed File"), "", tr("Binary Files (*.bin)"));
    if (compressedFilePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "No compressed file selected.");
        return;
    }

    std::ifstream compressedFile(compressedFilePath.toStdString(), std::ios::binary);
    std::string encodedData((std::istreambuf_iterator<char>(compressedFile)), std::istreambuf_iterator<char>());
    compressedFile.close();

    // Build Huffman Tree again (you'd need to either store or recreate the Huffman Tree structure)
    // Assuming the same tree is reconstructed
    std::map<char, int> freqMap;
    for (char ch : encodedData) {
        freqMap[ch]++;
    }

    // Decode the compressed data
    std::string decodedText = decode(rootTree, encodedData);

    // Write the decoded data to a file
    std::ofstream decompressedFile("/Users/shashwathbhaskar/Downloads/decompressed.txt", ios::out);
    decompressedFile << decodedText;
    cout<<decodedText<<endl;
    decompressedFile.close();

    QMessageBox::information(this, "Success", "File decompressed successfully.");

}

