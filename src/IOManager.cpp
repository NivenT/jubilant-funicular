#include "nta/IOManager.h"
#include "nta/Logger.h"

namespace nta {
    void IOManager::readFileToBuffer(crstring filePath, FileBuffer& buffer) {
        Logger::writeToLog("Reading " + filePath + " to a buffer");
        std::ifstream file(filePath.c_str(), std::ios::binary);
        if (file.fail()) {
            Logger::writeErrorToLog("Failed to open file " + filePath);
        }
        file.seekg(0, std::ios::end);
        unsigned int fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        fileSize -= file.tellg();
        buffer.resize(fileSize);
        file.read((char*)&buffer[0], fileSize);
        file.close();
        Logger::writeToLog("Read file");
    }
    void IOManager::writeFileFromBuffer(crstring filePath, const FileBuffer& buffer) {
        Logger::writeToLog("Writing buffer to " + filePath);
        std::ofstream file(filePath.c_str(), std::ios::binary);
        file.write((char*)&buffer[0], buffer.size());
        Logger::writeToLog("Wrote buffer");
    }
    void IOManager::writeFloatLE(float val, std::ofstream& file) {
        char bytes[4];
        for (int i = 0; i < 4; i++) {
            bytes[i] = ((char*)&val)[i];
        }
        file.write(bytes, 4);
    }
    void IOManager::writeFloatLE(float val, FileBuffer& buffer) {
        char bytes[4];
        for (int i = 0; i < 4; i++) {
            bytes[i] = ((char*)&val)[i];
        }
        buffer.insert(buffer.end(), bytes, bytes+4);
    }
    float IOManager::readFloatLE(std::ifstream& file) {
        char bytes[4];
        file.read(bytes, 4);
        float ret;
        for (int i = 0; i < 4; i++) {
            ((char*)&ret)[i] = bytes[i];
        }
        return ret;
    }
    float IOManager::readFloatLE(const FileBuffer& buffer, int pos) {
        float ret;
        for (int i = 0; i < 4; i++) {
            ((char*)&ret)[i] = buffer[i+pos];
        }
        return ret;
    }
    void IOManager::writeIntLE(int val, std::ofstream& file) {
        char bytes[4];
        for (int i = 0; i < 4; i++) {
            bytes[i] = (val >> (8*i)) & 0xFF;
        }
        file.write(bytes, 4);
    }
    void IOManager::writeIntLE(int val, FileBuffer& buffer) {
        char bytes[4];
        for (int i = 0; i < 4; i++) {
            bytes[i] = (val >> (8*i)) & 0xFF;
        }
        buffer.insert(buffer.end(), bytes, bytes+4);
    }
    int IOManager::readIntLE(std::ifstream& file) {
        char bytes[4];
        file.read(bytes, 4);
        int ret = 0;
        for (int i = 0; i < 4; i++) {
            ret |= bytes[i] << (i*8);
        }
        return ret;
    }
    int IOManager::readIntLE(const FileBuffer& buffer, int pos) {
        int ret = 0;
        for (int i = 0; i < 4; i++) {
            ret |= buffer[pos+i] << (i*8);
        }
        return ret;
    }
    int IOManager::readIntBE(std::ifstream& file) {
        char bytes[4];
        file.read(bytes, 4);
        int ret = 0;
        for (int i = 0; i < 4; i++) {
            ret |= bytes[i] << ((3-i)*8);
        }
        return ret;
    }
    void IOManager::writeShortLE(short val, std::ofstream& file) {
        char bytes[2];
        for (int i = 0; i < 2; i++) {
            bytes[i] = (val >> (8*i)) & 0xFF;
        }
        file.write(bytes, 2);
    }
    short IOManager::readShortLE(std::ifstream& file) {
        char bytes[2];
        file.read(bytes, 2);
        return bytes[0] | (bytes[1] << 8);
    }
}
