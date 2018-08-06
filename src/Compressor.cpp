#include <algorithm>

#include "nta/Compressor.h"
#include "nta/Logger.h"
#include "nta/utils.h"

namespace nta {
    std::map<GLubyte,std::string>    Compressor::m_encodings;
    HuffmanNode*                     Compressor::m_root;
    std::string Compressor::toBinary(int num, int numBits) {
        std::string ret = "";
        for (int i = 0; i < numBits; i++) {
            if ((1 & (num >> i)) == 1) {
                ret += "1";
            } else {
                ret += "0";
            }
        }
        return ret;
    }
    int Compressor::fromBinary(crstring num) {
        int ret = 0;
        for (int i = 0; i < num.size(); i++) {
            if (num[i] == '1') {
                ret += (1 << i);
            }
        }
        return ret;
    }
    void Compressor::createTree(const std::vector<GLubyte>& data) {
        Logger::writeToLog("Creating Huffman tree...");
        std::map<GLubyte, int> frequencies;
        for (auto& byte : data) {
            if (frequencies.find(byte) == frequencies.end()) {
                frequencies[byte] = 1;
            } else {
                frequencies[byte]++;
            }
        }
        std::vector<HuffmanNode*> byteNodes;
        for (const auto& pair : frequencies) {
            byteNodes.push_back(new HuffmanLeaf(pair.first, pair.second));
        }
        HuffmanNode* smallestFreq = nullptr;
        HuffmanNode* scnsmallFreq = nullptr;
        while (byteNodes.size() > 1) {
            smallestFreq = *std::min_element(byteNodes.begin(), byteNodes.end(), [](HuffmanNode* lhs, HuffmanNode* rhs){
                    return (lhs->getFrequency() < rhs->getFrequency());
                });
            std::remove(byteNodes.begin(), byteNodes.end(), smallestFreq); byteNodes.pop_back();
            scnsmallFreq = *std::min_element(byteNodes.begin(), byteNodes.end(), [](HuffmanNode* lhs, HuffmanNode* rhs){
                    return (lhs->getFrequency() < rhs->getFrequency());
                });
            std::remove(byteNodes.begin(), byteNodes.end(), scnsmallFreq); byteNodes.pop_back();
            byteNodes.push_back(new HuffmanNode(smallestFreq, scnsmallFreq));
        }
        m_root = byteNodes.front();
        m_encodings = m_root->getEncodings();
        Logger::writeToLog("Created Huffman tree");
    }
    std::vector<GLubyte> Compressor::compress(const std::vector<GLubyte>& data) {
        Logger::writeToLog("Compressing data...");
        createTree(data);
        std::string compressedBits = "";
        int meta = 0; //The number of bits required to store the number of bits required to store the longest encoded byte
        for (const auto& byte : data) {
            compressedBits += m_encodings[byte];
            meta = std::max<float>(meta, log2(m_encodings[byte].length())+1);
        }
        unsigned long size = compressedBits.size();
        unsigned long numBytes = log2((double)size)/8.+1;
        compressedBits = toBinary(numBytes) + toBinary(size, 8*numBytes) + compressedBits;
        std::string encodingInfo = "";
        for (auto pair : m_encodings) {
            encodingInfo += toBinary(pair.first) + toBinary(pair.second.size(), meta) + pair.second;
        }
        size = encodingInfo.size();
        numBytes = log2((double)size)/8.+1;
        compressedBits = toBinary(numBytes) + toBinary(size, 8*numBytes) + toBinary(meta, 3) + encodingInfo + compressedBits;
        size = compressedBits.size();
        numBytes = size/8;
        std::vector<GLubyte> compressedData;
        for (int i = 0; i < numBytes; i++) {
            compressedData.push_back(fromBinary(compressedBits.substr(i*8,8)));
        }
        compressedData.push_back(fromBinary(compressedBits.substr(8*numBytes)));
        Logger::writeToLog("Compressed data (" + utils::to_string(100.*compressedData.size()/data.size(),4) + "%)");
        return compressedData;
    }
    std::vector<GLubyte> Compressor::decompress(const std::vector<GLubyte>& data) {
        std::vector<GLubyte> uncompressedData;
        std::string compressedBits = "";
        for (auto& byte : data) {
            compressedBits += toBinary(byte);
        }
        unsigned long bytesForSize = data.front();
        unsigned long eInfoLen = fromBinary(compressedBits.substr(8,8*bytesForSize));
        int meta = fromBinary(compressedBits.substr(8+8*bytesForSize,3));
        std::map<std::string, GLubyte> encodings;
        unsigned long pos;
        for (pos = 11+8*bytesForSize; pos < 11+8*bytesForSize+eInfoLen;) {
            std::pair<std::string, GLubyte> curr;
            curr.second = fromBinary(compressedBits.substr(pos,8));
            int codeLength = fromBinary(compressedBits.substr(pos+8,meta));
            curr.first = compressedBits.substr(pos+8+meta,codeLength);
            encodings.insert(curr);
            pos += 8+meta+codeLength;
        }
        bytesForSize = fromBinary(compressedBits.substr(pos,8));
        unsigned long cDataLen = fromBinary(compressedBits.substr(pos+8,8*bytesForSize));
        unsigned long eof = pos+8+8*bytesForSize+cDataLen;
        std::string curr = "";
        for (pos = pos+8+8*bytesForSize; pos < eof; pos++) {
            curr += compressedBits[pos];
            if (encodings.find(curr) != encodings.end()) {
                uncompressedData.push_back(encodings[curr]);
                curr = "";
            }
        }
        return uncompressedData;
    }
}
