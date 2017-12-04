#ifndef COMPRESSOR_H_INCLUDED
#define COMPRESSOR_H_INCLUDED

#include <vector>
#include <map>

#include "MyEngine.h"

typedef unsigned char GLubyte;

namespace nta {
    ///A node in a Huffman tree
    class HuffmanNode {
    private:
        ///the children of the node
        HuffmanNode *m_lChild, *m_rChild;
    protected:
        ///the frequency of the nodes associated bytes
        int m_freq;
    public:
        ///basic constructor
        HuffmanNode();
        ///sets l and r as children of this and sets m_freq to the sum of their frequencies
        HuffmanNode(HuffmanNode* l, HuffmanNode* r);
        ///recursively destroys node
        virtual ~HuffmanNode();
        ///returns map of all the bytes and how they are encoded
        auto getEncodings(crstring enc = "") const -> std::map<GLubyte,std::string>;
        ///returns children
        HuffmanNode* getLeft() const;
        HuffmanNode* getRight() const;
        ///returns whether or not the node has children
        bool hasChildren() const;
        ///returns the frequency of the node
        int getFrequency() const;
    };
    ///represents a leaf in a Huffman tree
    class HuffmanLeaf : public HuffmanNode {
    private:
        ///the byte associated with the leaf
        GLubyte m_data;
    public:
        ///basic constructor
        HuffmanLeaf();
        ///constructs a leaf with given data and freq
        HuffmanLeaf(GLubyte data, int freq);
        ///destroys leaf
        ~HuffmanLeaf();
        ///returns m_data
        GLubyte getData() const;
    };
    ///Static class for compressing byte buffers
    class Compressor {
    private:
        ///converts number to its binary representation
        static std::string toBinary(int num, int numBits = 8);
        ///reads a binary number from a string
        static int fromBinary(crstring num);
        ///creates a Huffman tree from a buffer
        static void createTree(const std::vector<GLubyte>& data);
        ///the encodings generated from the tree
        static std::map<GLubyte,std::string> m_encodings;
        ///the root of the created Huffman tree
        static HuffmanNode* m_root;
    public:
        ///decompressed data that was compressed by this class
        static std::vector<GLubyte> decompress(const std::vector<GLubyte>& data);
        ///compresses a bye buffer
        static std::vector<GLubyte> compress(const std::vector<GLubyte>& data);
    };
}

#endif // COMPRESSOR_H_INCLUDED
