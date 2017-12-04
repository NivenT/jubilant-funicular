#ifndef PRIMITIVEBATCH_H_INCLUDED
#define PRIMITIVEBATCH_H_INCLUDED

#include "SpriteBatch.h"

namespace nta {
    ///represents a primitive (point, line, triangle, etc.)
    struct Primitive {
        ///constructors
        Primitive(){}
        Primitive(const std::initializer_list<Vertex2D>& verts, GLuint texID, float d) : depth(d), textureID(texID) {
            vertices.insert(vertices.begin(), verts.begin(), verts.end());
        }
			  template<class Iterator>
				Primitive(Iterator first, Iterator last, GLuint texID, float d) : depth(d), textureID(texID) {
					  vertices.insert(vertices.begin(), first, last);
				}
        ///destructor
        ~Primitive() {
            depth = 0;
            textureID = 0;
            vertices.clear();
        }
        ///the depth of the primitive
        float depth;
        ///the texture used by the primitive
        GLuint textureID;
        ///the vertices that make up the primitive
        std::vector<Vertex2D> vertices;
    };
    ///represents a collection of primitives to be drawn
    class PrimitiveBatch {
    private:
        ///creates vertex array object
        void createVertexArrayObject();
        ///creates render batches
        void createRenderBatches();
        ///sorts primitives
        void sortPrimitives();
        ///comparers used for sorting
        static bool compareTexture(Primitive* lhs, Primitive* rhs);
        static bool comparePrimitive(Primitive* lhs, Primitive* rhs);
        static bool compareDepth(Primitive* lhs, Primitive* rhs);
        ///return primitive to be drawn
        GLenum toPrimitiveType(unsigned int numVertices) const;
        ///the primitives to be drawn
        std::vector<Primitive*> m_primitives;
        ///the render batches used to draw the primitives
        std::vector<RenderBatch> m_renderBatches;
        ///ids for the vertex buffer object and vertex array object used for rendering
        GLuint m_vao = 0;
        GLuint m_vbo = 0;
    public:
        ///constructor and destructor
        PrimitiveBatch();
        ~PrimitiveBatch();
        ///initializes the batch
        void init();
        ///begins collection of primitive
        void begin();
        ///ends collection of primitive and prepares for rendering
        void end();
        ///adds a primitive to the batch
        void addPrimitive(Primitive* primitive);
        void addPrimitive(const std::initializer_list<Vertex2D>& vertices, GLuint textureID, float depth = 1);
				template<class Iterator>
				void addPrimitive(Iterator first, Iterator last, GLuint textureID, float depth = 1);
        ///renders the primitives
        void render() const;
    };
    typedef PrimitiveBatch PrimBatch;

		template<class Iterator>
		void PrimitiveBatch::addPrimitive(Iterator first, Iterator last, GLuint textureID, float depth) {
        addPrimitive(new Primitive(first, last, textureID, depth));
		}
}

#endif // PRIMITIVEBATCH_H_INCLUDED
