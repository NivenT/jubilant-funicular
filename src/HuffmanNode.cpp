#include "Compressor.h"

namespace nta {
    HuffmanNode::HuffmanNode() : m_lChild(nullptr), m_rChild(nullptr), m_freq(0) {
    }
    HuffmanNode::HuffmanNode(HuffmanNode* l, HuffmanNode* r) : m_lChild(l), m_rChild(r), m_freq(l->m_freq+r->m_freq) {
    }
    HuffmanNode::~HuffmanNode() {
        m_freq = 0;
        if (m_lChild) {
            delete m_lChild;
        }
        if (m_rChild) {
            delete m_rChild;
        }
    }
    std::map<GLubyte,std::string> HuffmanNode::getEncodings(crstring enc) const {
        std::map<GLubyte,std::string> ret;
        if (!hasChildren()) {
            ret.insert(std::make_pair(((HuffmanLeaf*)this)->getData(), enc));
        } else {
            std::map<GLubyte,std::string> lEncode = m_lChild->getEncodings(enc+"0"), rEncode = m_rChild->getEncodings(enc+"1");
            ret.insert(lEncode.begin(), lEncode.end());
            ret.insert(rEncode.begin(), rEncode.end());
        }
        return ret;
    }
    HuffmanNode* HuffmanNode::getLeft() const {
        return m_lChild;
    }
    HuffmanNode* HuffmanNode::getRight() const {
        return m_rChild;
    }
    bool HuffmanNode::hasChildren() const {
        return (m_lChild != nullptr || m_rChild != nullptr);
    }
    int HuffmanNode::getFrequency() const {
        return m_freq;
    }
}
