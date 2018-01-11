#include "nta/Sprite.h"
#include "nta/Vertex.h"
#include "nta/ResourceManager.h"

namespace nta {
    Sprite::Sprite() : m_vboID(0) {
    }
    Sprite::~Sprite() {
        glDeleteBuffers(1, &m_vboID);
    }
    void Sprite::init(float x, float y, float w, float h, crstring imagePath, float d) {
        m_texture = ResourceManager::getTexture(imagePath);
        if (m_vboID == 0) {
            glGenBuffers(1, &m_vboID);
        }
        Vertex2D vertexData[6];
        ///first triangle
        vertexData[0].setPosition(x,y);         vertexData[0].setUV(0,0);
        vertexData[1].setPosition(x+w,y);       vertexData[1].setUV(1,0);
        vertexData[2].setPosition(x,y-h);       vertexData[2].setUV(0,1);
        ///second triangle
        vertexData[3].setPosition(x+w,y);       vertexData[3].setUV(1,0);
        vertexData[4].setPosition(x,y-h);       vertexData[4].setUV(0,1);
        vertexData[5].setPosition(x+w,y-h);     vertexData[5].setUV(1,1);
        for (int i = 0; i < 6; i++) {
            vertexData[i].setColor(1, 1, 1, 1);
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void Sprite::render() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
        for (int i = 0; i < NUM_VERTEX_ATTRIBS; i++) {
            glEnableVertexAttribArray(i);
        }
        glBindTexture(GL_TEXTURE_2D, m_texture.id);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, pos));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, color));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, uv));
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, hasTexture));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);
        for (int i = 0; i < NUM_VERTEX_ATTRIBS; i++) {
            glDisableVertexAttribArray(i);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
