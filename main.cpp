#include <bits/stdc++.h>

using namespace std;

string senderData, receiverData, polynomial;
int byteInARow, m;
double p;
int rowNum, colmNum;

string dataBlock[1000][1000];

vector<string> temDataBlock;
vector<string> finalBlock;
vector<int> checkBitPositions;

vector<char> deseralizedDataBlock[1000];
vector<int> change[1000];


string default_console = "\033[0m";
string greenColor = "\033[0;32m";
string cyanColor = "\033[0;36m";
string redColor = "\033[0;31m";

string colmWiseSerial;
string originalColmSerial;
string correctionString;

string sentData, receivedData;


void padString() {
    auto originalDataLength = static_cast<int>(senderData.length());
    if (originalDataLength % m != 0) {
        int rem = originalDataLength % m;
//        cout << originalDataLength << " " << m << " rem = " << rem << endl ;
        for (int i = 0; i < m - rem; i++) {
            senderData += "~";
        }
    }
    cout << endl << endl << "Data string after padding: " << senderData << endl;
}

string fromIntToBinaryString(int num) {
    string binary = bitset<8>(num).to_string();
    return binary;
}

int fromBinaryStringToInt(string binary) {
    unsigned long decimal = std::bitset<8>(binary).to_ulong();
    return static_cast<int>(decimal);
}

void createDataBlock() {
    colmNum = m;
    rowNum = static_cast<int>(senderData.length() / m);

    for (int i = 0; i < rowNum; i++) {
        for (int j = 0; j < colmNum; j++) {
            string tem = fromIntToBinaryString(senderData[i * colmNum + j]);
            dataBlock[i][j] = tem;
        }
    }

    int idx = 0;
    for (int i = 0; i < rowNum; i++) {
        string tem;
        for (int j = 0; j < colmNum; j++) {
            tem += dataBlock[i][j];
        }
        temDataBlock.push_back(tem);
    }
}


void printEverything() {
    cout << endl << "Data block (ascii code of m characters per row): " << endl;
    for (int i = 0; i < rowNum; i++) {
        for (int j = 0; j < colmNum; j++) {
            cout << dataBlock[i][j];
        }
        cout << endl;
    }

    cout << endl << endl;

}


void correctCheckBits() {
    for (int i = 0; i < temDataBlock.size(); i++) {

        for (int j = 0; j < checkBitPositions.size(); j++) {

            int checkBitPos = checkBitPositions[j];
            checkBitPos++;
            int targetBitIdx = static_cast<int>(ceil(log2(checkBitPos)));

            int countParity = 0;
            for (int k = 0; k < temDataBlock[i].size(); k++) {
                int mask = 1 << targetBitIdx;
                int idx = k;
                idx++;

                if ((find(checkBitPositions.begin(), checkBitPositions.end(), idx - 1) == checkBitPositions.end())
                    && (idx & mask)) {
                    if (temDataBlock[i][idx - 1] == '1') {
                        countParity++;
                    }
                }
            }

            temDataBlock[i][checkBitPositions[j]] = countParity % 2 == 0 ? '0' : '1';
        }
    }
}


void insertCheckBits() {
    for (int i = 1; i <= m * 8; i *= 2) {
        int idx = i - 1;
        checkBitPositions.push_back(idx);
    }

    for (int row = 0; row < temDataBlock.size(); row++) {
        for (int i = 0; i < checkBitPositions.size(); i++) {
            temDataBlock[row].insert(static_cast<unsigned long>(checkBitPositions[i]), "0");
        }
    }


    correctCheckBits();

    cout << "Data block after adding check bits: " << endl;
    for (int i = 0; i < temDataBlock.size(); i++) {
        for (int j = 0; j < temDataBlock[i].size(); j++) {
            if (find(checkBitPositions.begin(), checkBitPositions.end(), j) != checkBitPositions.end()) { // contatins j
                cout << greenColor << temDataBlock[i][j];
            } else {
                cout << default_console << temDataBlock[i][j];
            }
        }
        cout << default_console << endl;
    }
}


void columnWiseSerialize() {
    colmWiseSerial = "";
    for (int colm = 0; colm < temDataBlock[0].size(); colm++) {
        for (int row = 0; row < temDataBlock.size(); row++) {
            colmWiseSerial += temDataBlock[row][colm];
        }
    }

    cout << endl << "Data bits after columnwise serialization: " << endl;
    cout << colmWiseSerial << endl;
}

string trimBeginningZeroes(string s) {
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == '0') {
            s.erase(0, 1);
            i = i - 1;
        } else {
            return s;
        }
    }
    return "";
}

string XOR(string s1, string s2) {
    string ans = "";
    for (int i = 1; i < s2.size(); i++) {
        if (s1[i] == s2[i]) {
            ans += "0";
        } else {
            ans += "1";
        }
    }
    return ans;
}


string mod2div(string dividend, string divisor) {
    auto pick = static_cast<int>(divisor.size());

    string tem = dividend.substr(0, static_cast<unsigned long>(pick));

    while (pick < dividend.size()) {
        if (tem[0] == '1') {
            tem = XOR(divisor, tem) + dividend[pick];
        } else {
            string t = "";
            for (int i = 0; i < pick; i++) t += "0";
            tem = XOR(t, tem) + dividend[pick];
        }
        pick++;
    }

    if (tem[0] == '1') {
        tem = XOR(divisor, tem);
    } else {
        string t;
        for (int i = 0; i < pick; i++) t += "0";
        tem = XOR(t, tem);
    }

    return tem;
}


void getFinalCRC() {
    int divisorLen = static_cast<int>(polynomial.size());
    string dividend = colmWiseSerial;
    string divisor = polynomial;

    for (int i = 0; i < divisorLen - 1; i++) {
        dividend += "0";
    }

    string remainder = mod2div(dividend, divisor);

    colmWiseSerial = colmWiseSerial + remainder;
}

void calculateCRC() {
    int originalSerialLen = static_cast<int>(colmWiseSerial.size());

    getFinalCRC();

    cout << endl << "data bits after appending checksum CRC (sent frame): " << endl;
    for (int i = 0; i < colmWiseSerial.size(); i++) {
        if (i < originalSerialLen) {
            cout << default_console << colmWiseSerial[i];
        } else {
            cout << cyanColor << colmWiseSerial[i];
        }
    }
    cout << default_console << endl;
}

char toggle(char a) {
    if (a == '1')
        return '0';
    else if (a == '0')
        return '1';
}

double getRandomNumber() {
    const int range_from = 0;
    const int range_to = RAND_MAX;
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<int> distr(range_from, range_to);

    return (double) distr(generator) / RAND_MAX;
}

void simulatePhysicalLayer() {
    sentData = colmWiseSerial;

    string temp = sentData;

    cout << endl << "Received Frame: " << endl;
    for (int i = 0; i < temp.size(); i++) {
        if (getRandomNumber() < p) {
            temp[i] = toggle(temp[i]);
            cout << redColor << temp[i] << default_console;
        } else {
            cout << default_console << temp[i];
        }
    }
    cout << endl;

    receivedData = temp;
}


void sender() {
    padString();
    createDataBlock();
    printEverything();

    insertCheckBits();
    columnWiseSerialize();

    calculateCRC();
}


void verify() {
    cout << endl << "Result of CRC checksum matching: ";
    string check = mod2div(receivedData, polynomial);
    stringstream ss(check);
    int x = -1;
    ss >> x;

    if (x == 0) {
        cout << "No Error Detected." << endl;
    } else {
        cout << "Error Detected." << endl;
    }
}


void removeCRC() {
    receivedData.erase(receivedData.end() - (polynomial.size() - 1), receivedData.end());
}


void deserialize() {
    for (int i = 0; i < receivedData.size(); i++) {
        int r = i % rowNum;
        deseralizedDataBlock[r].push_back(receivedData[i]);
        if (receivedData[i] == colmWiseSerial[i]) {
            change[r].push_back(0);
        } else {
            change[r].push_back(1);
        }
    }

    cout << endl << "Data block after removing CRC checksum bits: " << endl;

    for (int i = 0; i < rowNum; i++) {
        for (int j = 0; j < deseralizedDataBlock[i].size(); j++) {
            if (change[i][j] == 1) {
                cout << redColor << deseralizedDataBlock[i][j] << default_console;
            } else {
                cout << default_console << deseralizedDataBlock[i][j];
            }
        }
        cout << default_console << endl;
    }
}


void errorCorrection() {
    for (int i = 0; i < rowNum; i++) {

        for (int j = 0; j < checkBitPositions.size(); j++) {

            int checkBitPos = checkBitPositions[j];
            checkBitPos++;
            int targetBitIdx = static_cast<int>(ceil(log2(checkBitPos)));

            int countParity = 0;

            for (int k = 0; k < deseralizedDataBlock[i].size(); k++) {
                int mask = 1 << targetBitIdx;
                int idx = k;
                idx++;

                if ((idx & mask)) {
                    if (deseralizedDataBlock[i][idx - 1] == '1') {
                        countParity++;
                    }
                }
            }

            deseralizedDataBlock[i][checkBitPositions[j]] = countParity % 2 == 0 ? '0' : '1';
            correctionString += deseralizedDataBlock[i][checkBitPositions[j]];
        }

        reverse(correctionString.begin(), correctionString.end());
        int wrong = fromBinaryStringToInt(correctionString) - 1;
        if (wrong != 0) { deseralizedDataBlock[i][wrong] = toggle(deseralizedDataBlock[i][wrong]); }
        correctionString = "";
    }
}


void removeCheckBits() {

    for (int i = 0; i < rowNum; i++) {
        string s(deseralizedDataBlock[i].begin(), deseralizedDataBlock[i].end());
        finalBlock.push_back(s);
    }


    for (int i = 0; i < finalBlock.size(); i++) {

        for (int j = 0; j < finalBlock[i].size(); j++) {
            if ((find(checkBitPositions.begin(), checkBitPositions.end(), j) != checkBitPositions.end())) {
                finalBlock[i][j] = '-';
            }
        }
    }

    for (int i = 0; i < finalBlock.size(); i++) {
        for (int j = 0; j < finalBlock[i].size(); j++) {
            if (finalBlock[i][j] == '-') {
                finalBlock[i].erase(j, 1);
                j--;
            }
        }
    }

    cout << endl << "DataBlock after removing check bits: " << endl;
    for (int i = 0; i < finalBlock.size(); i++) {
        cout << finalBlock[i] << endl;
    }
}


void decode() {
    string ans = "";
    for (int i = 0; i < finalBlock.size(); i++) {
        int startIdx = 0;
        int chunkNo = static_cast<int>(finalBlock[i].size() / 8);
        for (int j = 0; j < chunkNo; j++) {
            string tem = finalBlock[i].substr(startIdx, 8);
            startIdx += 8;

            int ascii = fromBinaryStringToInt(tem);
            char c = static_cast<char>(ascii);
            ans += c;
        }
    }

    cout << endl << "Output Frame: " << ans << endl;
}

void receiver() {
    verify();
    removeCRC();
    deserialize();
    errorCorrection();
    removeCheckBits();
    decode();
}


void simulate() {
    sender();
    simulatePhysicalLayer();
    receiver();
}


int main() {
    freopen("in.txt", "r", stdin);

    cout << "Enter data string: ";
    getline(cin, senderData);
    cout << "Enter number of data bytes in a row (m): ";
    cin >> m;
    byteInARow = m;
    cout << "Enter probability(p): ";
    cin >> p;
    cout << "Enter generator polynomial: ";
    cin >> polynomial;

    simulate();

    return 0;
}