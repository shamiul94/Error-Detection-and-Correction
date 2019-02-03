#include <bits/stdc++.h>

using namespace std;

string senderData, polynomial;
int m;
double p;
int rowNum, colmNum;

string dataBlock[1000][1000];

vector<string> temDataBlock;
vector<string> finalBlock;
vector<int> checkBitPositions;

vector<char> deserializedDataBlock[1000];
vector<int> change[1000];


string defaultColor = "\033[0m";
string greenColor = "\033[0;32m";
string cyanColor = "\033[0;36m";
string redColor = "\033[0;31m";

string columnWiseSerial;
string correctionString;

string sentData, receivedData;


void padString() {
    auto originalDataLength = static_cast<int>(senderData.length());
    if (originalDataLength % m != 0) {
        int rem = originalDataLength % m;
        for (int i = 0; i < m - rem; i++) {
            senderData += "~";
        }
    }
    cout << endl << endl << "Data string after padding: " << senderData << endl;
}

string fromIntToBinaryString(int num) {
    string binary = bitset<8>(static_cast<unsigned long long int>(num)).to_string();
    return binary;
}

int fromBinaryStringToInt(const string &binary) {
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
    for (auto &i : temDataBlock) {

        for (int j = 0; j < checkBitPositions.size(); j++) {

            int checkBitPos = checkBitPositions[j];
            checkBitPos++;
            auto targetBitIdx = static_cast<int>(ceil(log2(checkBitPos)));

            int countParity = 0;
            for (int k = 0; k < i.size(); k++) {
                int mask = (1 << targetBitIdx);
                int idx = k;
                idx++;

                if (idx & mask) {
                    if (find(checkBitPositions.begin(), checkBitPositions.end(), idx - 1) == checkBitPositions.end()) {
                        if (i[idx - 1] == '1') {
                            countParity++;
                        }
                    }
                }
            }

            i[checkBitPositions[j]] = countParity % 2 == 0 ? '0' : '1';
        }
    }
}


void insertCheckBits() {
    for (int i = 1; i <= m * 8; i *= 2) {
        int idx = i - 1;
        checkBitPositions.push_back(idx);
    }

    for (auto &row : temDataBlock) {
        for (int checkBitPosition : checkBitPositions) {
            row.insert(static_cast<unsigned long>(checkBitPosition), "0");
        }
    }


    correctCheckBits();

    cout << "Data block after adding check bits: " << endl;
    for (auto &i : temDataBlock) {
        for (int j = 0; j < i.size(); j++) {
            if (find(checkBitPositions.begin(), checkBitPositions.end(), j) != checkBitPositions.end()) { // contatins j
                cout << greenColor << i[j];
            } else {
                cout << defaultColor << i[j];
            }
        }
        cout << defaultColor << endl;
    }
}


void columnWiseSerialize() {
    columnWiseSerial = "";
    for (int column = 0; column < temDataBlock[0].size(); column++) {
        for (auto &row : temDataBlock) {
            columnWiseSerial += row[column];
        }
    }

    cout << endl << "Data bits after column wise serialization: " << endl;
    cout << columnWiseSerial << endl;
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
    auto divisorLen = static_cast<int>(polynomial.size());
    string dividend = columnWiseSerial;
    string divisor = polynomial;

    for (int i = 0; i < divisorLen - 1; i++) {
        dividend += "0";
    }

    string remainder = mod2div(dividend, divisor);

    columnWiseSerial = columnWiseSerial + remainder;
}

void calculateCRC() {
    auto originalSerialLen = static_cast<int>(columnWiseSerial.size());

    getFinalCRC();

    cout << endl << "data bits after appending checksum CRC (sent frame): " << endl;
    for (int i = 0; i < columnWiseSerial.size(); i++) {
        if (i < originalSerialLen) {
            cout << defaultColor << columnWiseSerial[i];
        } else {
            cout << cyanColor << columnWiseSerial[i];
        }
    }
    cout << defaultColor << endl;
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
    sentData = columnWiseSerial;

    string temp = sentData;

    cout << endl << "Received Frame: " << endl;
    for (char &i : temp) {
        if (getRandomNumber() < p) {
            i = toggle(i);
            cout << redColor << i << defaultColor;
        } else {
            cout << defaultColor << i;
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
        deserializedDataBlock[r].push_back(receivedData[i]);
        if (receivedData[i] == columnWiseSerial[i]) {
            change[r].push_back(0);
        } else {
            change[r].push_back(1);
        }
    }

    cout << endl << "Data block after removing CRC checksum bits: " << endl;

    for (int i = 0; i < rowNum; i++) {
        for (int j = 0; j < deserializedDataBlock[i].size(); j++) {
            if (change[i][j] == 1) {
                cout << redColor << deserializedDataBlock[i][j] << defaultColor;
            } else {
                cout << defaultColor << deserializedDataBlock[i][j];
            }
        }
        cout << defaultColor << endl;
    }
}


void errorCorrection() {
    for (int i = 0; i < rowNum; i++) {

        for (int checkBitPosition : checkBitPositions) {

            int checkBitPos = checkBitPosition;
            checkBitPos++;
            auto targetBitIdx = static_cast<int>(ceil(log2(checkBitPos)));

            int countParity = 0;

            for (int k = 0; k < deserializedDataBlock[i].size(); k++) {
                int mask = 1 << targetBitIdx;
                int idx = k;
                idx++;

                if ((idx & mask)) {
                    if (deserializedDataBlock[i][idx - 1] == '1') {
                        countParity++;
                    }
                }
            }

            deserializedDataBlock[i][checkBitPosition] = countParity % 2 == 0 ? '0' : '1';
            correctionString += deserializedDataBlock[i][checkBitPosition];
        }

        reverse(correctionString.begin(), correctionString.end());
        int wrong = fromBinaryStringToInt(correctionString) - 1;
        if (wrong != 0) { deserializedDataBlock[i][wrong] = toggle(deserializedDataBlock[i][wrong]); }
        correctionString = "";
    }
}


void removeCheckBits() {

    for (int i = 0; i < rowNum; i++) {
        string s(deserializedDataBlock[i].begin(), deserializedDataBlock[i].end());
        finalBlock.push_back(s);
    }


    for (auto &i : finalBlock) {
        for (int j = 0; j < i.size(); j++) {
            if ((find(checkBitPositions.begin(), checkBitPositions.end(), j) != checkBitPositions.end())) {
                i[j] = '-';
            }
        }
    }

    for (auto &i : finalBlock) {
        for (int j = 0; j < i.size(); j++) {
            if (i[j] == '-') {
                i.erase(static_cast<unsigned long>(j), 1);
                j--;
            }
        }
    }

    cout << endl << "DataBlock after removing check bits: " << endl;
    for (const auto &i : finalBlock) {
        cout << i << endl;
    }
}


void decode() {
    string ans = "";
    for (auto &i : finalBlock) {
        int startIdx = 0;
        auto chunkNo = static_cast<int>(i.size() / 8);
        for (int j = 0; j < chunkNo; j++) {
            string tem = i.substr(static_cast<unsigned long>(startIdx), 8);
            startIdx += 8;

            int ascii = fromBinaryStringToInt(tem);
            auto c = static_cast<char>(ascii);
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

//    cout << "Enter data string: ";
    getline(cin, senderData);
//    cout << "Enter number of data bytes in a row (m): ";
    cin >> m;
    //    cout << "Enter probability(p): ";
    cin >> p;
//    cout << "Enter generator polynomial: ";
    cin >> polynomial;

    cout << endl << "Input Data String: " << senderData << endl;
    cout << "Number of data bytes in a row (m):" << m << endl;
    cout << "Probability(p): " << p << endl;
    cout << "Generator polynomial: " << polynomial << endl;

    simulate();

    return 0;
}