#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include <tiny_obj_loader.h>

namespace gl {

    class Vertex
    {
    public:
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texture;
    };

	class Mesh
    {
    public:
    	unsigned int VAO;
		unsigned int VBO;
		unsigned int EBO;
        unsigned int nb_vertices;

        Mesh(const std::string& filename)
        {
            tinyobj::ObjReader reader;
            if (!reader.ParseFromFile(filename))
            {
                throw std::runtime_error("Cannot load file: " + filename);
            }
            if (!reader.Warning().empty())
            {
                std::cerr <<
                    "Warning parsing file " << filename << " : " << 
                    reader.Warning() << std::endl;
            }
            auto& attrib = reader.GetAttrib();
            auto& shapes = reader.GetShapes();
            auto& materials = reader.GetMaterials();
            assert(shapes.size() == 1);
            std::vector<Vertex> vertices;
            std::vector<std::uint32_t> indices;
            int index_offset = 0;
            for (std::size_t f = 0; 
                f < shapes[0].mesh.num_face_vertices.size(); 
                ++f)
            {
                int fv = shapes[0].mesh.num_face_vertices[f];
                if (fv != 3) throw std::runtime_error("Should be triangles?");
                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) 
                {
                    Vertex vertex{};
                    // access to vertex
                    tinyobj::index_t idx = 
                        shapes[0].mesh.indices[index_offset + v];
                    vertex.position.x = attrib.vertices[3 * idx.vertex_index + 0];
                    vertex.position.y = attrib.vertices[3 * idx.vertex_index + 1];
                    vertex.position.z = attrib.vertices[3 * idx.vertex_index + 2];
                    vertex.normal.x = attrib.normals[3 * idx.normal_index + 0];
                    vertex.normal.y = attrib.normals[3 * idx.normal_index + 1];
                    vertex.normal.z = attrib.normals[3 * idx.normal_index + 2];
                    vertex.texture.x = 
                        attrib.texcoords[2 * idx.texcoord_index + 0];
                    vertex.texture.y = 
                        attrib.texcoords[2 * idx.texcoord_index + 1];
                    vertices.push_back(vertex);
                    indices.push_back(static_cast<int>(indices.size()));
                }
                index_offset += fv;
            }

            nb_vertices = index_offset;
            // VAO binding should be before VAO.
            glGenVertexArrays(1, &VAO);
            IsError(__FILE__, __LINE__);
            glBindVertexArray(VAO);
            IsError(__FILE__, __LINE__);

            // EBO.
            glGenBuffers(1, &EBO);
            IsError(__FILE__, __LINE__);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            IsError(__FILE__, __LINE__);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(float),
                indices.data(),
                GL_STATIC_DRAW);
            IsError(__FILE__, __LINE__);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            IsError(__FILE__, __LINE__);

            // VBO.
            glGenBuffers(1, &VBO);
            IsError(__FILE__, __LINE__);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            IsError(__FILE__, __LINE__);
            glBufferData(
                GL_ARRAY_BUFFER,
                vertices.size() * sizeof(Vertex),
                vertices.data(),
                GL_STATIC_DRAW);
            IsError(__FILE__, __LINE__);

            GLintptr vertex_color_offset = 3 * sizeof(float);
            GLintptr vertex_tex_offset = 6 * sizeof(float);
            glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                8 * sizeof(float),
                0);
            IsError(__FILE__, __LINE__);
            glVertexAttribPointer(
                1,
                3,
                GL_FLOAT,
                GL_FALSE,
                8 * sizeof(float),
                (GLvoid*)vertex_color_offset);
            IsError(__FILE__, __LINE__);
            glVertexAttribPointer(
                2,
                2,
                GL_FLOAT,
                GL_FALSE,
                8 * sizeof(float),
                (GLvoid*)vertex_tex_offset);
            IsError(__FILE__, __LINE__);
            glEnableVertexAttribArray(0);
            IsError(__FILE__, __LINE__);
            glEnableVertexAttribArray(1);
            IsError(__FILE__, __LINE__);
            glEnableVertexAttribArray(2);
            IsError(__FILE__, __LINE__);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            IsError(__FILE__, __LINE__);
        }
    protected:
        void IsError(const std::string& file, int line)
        {
            auto error_code = glGetError();
			if (error_code != GL_NO_ERROR)
			{
				throw std::runtime_error(
					std::to_string(error_code) +
					" in file: " + file +
					" at line: " + std::to_string(line));
			}
        }
    };

} // End namespace gl.
