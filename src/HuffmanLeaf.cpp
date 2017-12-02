#include "Compressor.h"

namespace nta {
    HuffmanLeaf::HuffmanLeaf() : m_data(0) {
    }
    HuffmanLeaf::HuffmanLeaf(GLubyte data, int freq) : m_data(data) {
        m_freq = freq;
    }
    HuffmanLeaf::~HuffmanLeaf() {
        m_freq = 0;
        m_data = 0;
    }
    GLubyte HuffmanLeaf::getData() const {
        return m_data;
    }
}
