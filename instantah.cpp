#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

// Convert hexadecimal string to bytes
vector<uint8_t> hexStringToBytes(const string& hex) {
    vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);
        uint8_t byte = strtol(byteString.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

int main() {
    string filename = "FFXiMain.dll";
    string searchStringHex = "C233D2F7F1"; // Hexadecimal representation of the string to search for
    int modified_offset = 23;

    // Convert hexadecimal search string to bytes
    vector<uint8_t> searchStringBytes = hexStringToBytes(searchStringHex);

    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    // Get the size of the file
    file.seekg(0, ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, ios::beg);

    // Allocate buffer dynamically based on file size
    vector<uint8_t> buffer(fileSize);

    file.read(reinterpret_cast<char*>(&buffer[0]), fileSize);

    file.close();

    // Search for the search string in the buffer
    for (size_t i = 0; i < fileSize - searchStringBytes.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < searchStringBytes.size(); ++j) {
            if (buffer[i + j] != searchStringBytes[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            cout << "String found at offset: " << hex << i << endl;

            // Change '74' to 'EB' in hex (114 to 235 in decimal)
            if (buffer[i + modified_offset + 1] == 0x74) {
                buffer[i + modified_offset + 1] = 0xEB;
		cout << "Patching 0x74 to 0xEB for instantah" << endl;
            } else if (buffer[i + modified_offset + 1] == 0xEB) {
		cout << "dll already patched." << endl;
	    } else {
		cout << "did not find 0x74 to modify or 0xEB for already patched" << endl;
	    }

            // You can break here if you only want to modify the first occurrence
            // break;
        }
    }

    // Write the modified buffer back to the file
    ofstream outFile(filename, ios::binary | ios::trunc);
    if (!outFile) {
        cerr << "Error opening output file." << endl;
        return 1;
    }

    outFile.write(reinterpret_cast<char*>(&buffer[0]), fileSize);
    outFile.close();

    return 0;
}
