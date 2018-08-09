#ifndef IOMANAGER_H_INCLUDED
#define IOMANAGER_H_INCLUDED

#include <vector>
#include <fstream>

#include "MyEngine.h"

typedef std::vector<unsigned char> FileBuffer;

namespace nta {
    /// Handles binary file operations
    class IOManager {
    public:
        /// Checks to see if a file exists
        static bool fileExists(crstring path);
        /// stores the entire contents of a file in a buffer
        static void readFileToBuffer(crstring filePath, FileBuffer& buffer);
        static void readFileToBuffer(crstring filePath, std::string& buffer);
        /// stores the entire contents of a buffer in a file
        static void writeFileFromBuffer(crstring filePath, const FileBuffer& buffer);
        static void writeFileFromBuffer(crstring filePath, crstring buffer);
        /// writes/reads a float to/from a file
        static void writeFloatLE(float val, std::ofstream& file); //LE - little endian
        static void writeFloatLE(float val, FileBuffer& buffer);
        static float readFloatLE(std::ifstream& file);
        static float readFloatLE(const FileBuffer& buffer, int pos);
        static void writeFloatBE(float val, std::ofstream& file); //BE - big endian
        static void writeFloatBE(float val, FileBuffer& buffer);
        static float readFloatBE(std::ifstream& file);
        static float readFloatBE(const FileBuffer& buffer, int pos);
        /// writes/reads an int to/from a file
        static void writeIntLE(int val, std::ofstream& file);
        static void writeIntLE(int val, FileBuffer& buffer);
        static int readIntLE(std::ifstream& file);
        static int readIntLE(const FileBuffer& buffer, int pos);
        static void writeIntBE(int val, std::ofstream& file);
        static void writeIntBE(int val, FileBuffer& buffer);
        static int readIntBE(std::ifstream& file);
        static int readIntBE(const FileBuffer& buffer, int pos);
        /// writes/reads a short to/from a file
        static void writeShortLE(short val, std::ofstream& file);
        static void writeShortLE(short val, FileBuffer& buffer);
        static short readShortLE(std::ifstream& file);
        static short readShortLE(const FileBuffer& buffer, int pos);
        static void writeShortBE(short val, std::ofstream& file);
        static void writeShortBE(short val, FileBuffer& buffer);
        static short readShortBE(std::ifstream& file);
        static short readShortBE(const FileBuffer& buffer, int pos);
    };
}

#endif // IOMANAGER_H_INCLUDED
