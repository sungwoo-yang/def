/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "Mesh.hpp"
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <gsl/gsl>
#include <numbers>

namespace
{
    std::vector<graphics::MeshVertex> create_plane_vertices(int stacks, int slices);
    std::vector<unsigned>             build_index_buffer(int stacks, int slices);
    std::vector<unsigned>             convert_to_lines_pattern(const std::vector<unsigned>& indices);
    glm::vec3                         evaluate_trefoil(float row, float col);

}

namespace graphics
{
    Geometry create_plane(int stacks, int slices)
    {
        auto vertices = create_plane_vertices(stacks, slices);
        auto indices  = build_index_buffer(stacks, slices);
        return Geometry{ std::move(vertices), std::move(indices) };
    }

    Geometry create_cube(int stacks, int slices)
    {
        const auto plane_vertices = create_plane_vertices(stacks, slices);
        const auto plane_indices  = build_index_buffer(stacks, slices);

        enum Axis
        {
            X,
            Y
        };

        constexpr glm::vec3 AxisVectors[2] = { glm::vec3(1, 0, 0), glm::vec3(0, 1, 0) };

        constexpr glm::vec3 translate_array[] = {
            glm::vec3(+0.0f, +0.0f, +0.5f), // Z+
            glm::vec3(+0.0f, +0.0f, -0.5f), // Z-
            glm::vec3(+0.5f, +0.0f, +0.0f), // X+
            glm::vec3(-0.5f, +0.0f, +0.0f), // X-
            glm::vec3(+0.0f, +0.5f, +0.0f), // Y+
            glm::vec3(+0.0f, -0.5f, +0.0f), // Y-
        };

        struct rotation
        {
            Axis  axis;
            float angle;
        };

        constexpr rotation rotate_array[] = {
            { Axis::Y,                 0.0f }, // Z+
            { Axis::Y, glm::radians(180.0f) }, // Z-
            { Axis::Y,  glm::radians(90.0f) }, // X+
            { Axis::Y, glm::radians(-90.0f) }, // X-
            { Axis::X, glm::radians(-90.0f) }, // Y+
            { Axis::X,  glm::radians(90.0f) }  // Y-
        };

        std::vector<MeshVertex> vertices;
        std::vector<unsigned>   indices;
        vertices.reserve(plane_vertices.size() * 6u);
        indices.reserve(plane_indices.size() * 6u);
        /*  Transform the plane to 6 positions to form the faces of the cube */
        for (unsigned i = 0; i < 6; ++i)
        {
            constexpr glm::mat4 identity(1.0f);
            const auto          axis             = AxisVectors[rotate_array[i].axis];
            const auto          angle            = rotate_array[i].angle;
            const auto          rotation_matrix  = glm::rotate(identity, angle, axis);
            const auto          transform_matrix = glm::translate(identity, translate_array[i]) * rotation_matrix;
            for (const auto plane_vertex : plane_vertices)
            {
                MeshVertex v;
                v.position = glm::vec3(transform_matrix * glm::vec4(plane_vertex.position, 1.0));
                v.normal   = glm::vec3(rotation_matrix * glm::vec4(plane_vertex.normal, 1.0));
                v.uv       = plane_vertex.uv;
                vertices.push_back(v);
            }
            for (const auto plane_index : plane_indices)
            {
                indices.push_back(plane_index + static_cast<unsigned>(plane_vertices.size()) * i);
            }
        }
        return Geometry{ std::move(vertices), std::move(indices) };
    }

    Geometry create_sphere(int stacks, int slices)
    {
        /* TODO - implement generation of sphere geometry

            vertices = emptyList
            numVertices = (stacks + 1) * (slices + 1)
            reserve(vertices, numVertices)
            for stack in range(0, stacks + 1):
                row = stack / stacks
                beta = PI * (row - 0.5)
                sin_beta = sin(beta)
                cos_beta = cos(beta)
                for slice in range(0, slices + 1):
                    radius = 0.5
                    col = slice / slices
                    alpha = col * 2 * PI
                    v = MeshVertex()
                    v.position = Vec3(
                        radius * sin(alpha) * cos_beta,
                        radius * sin_beta,
                        radius * cos(alpha) * cos_beta
                    )
                    v.normal = v.position / radius
                    v.uv = Vec2(col, row)
                    append(vertices, v)
            indices = buildIndexBuffer(stacks, slices)
            return Geometry(vertices, indices)
        */
        return create_cube(stacks, slices);
    }

    Geometry create_torus(int stacks, int slices, float start_angle, float end_angle)
    {
        /* TODO - implement generation of torus geometry
        vertices = emptyList
        numVertices = (stacks + 1) * (slices + 1)
        reserve(vertices, numVertices)
        R = 0.35
        r = 0.15
        for stack in range(0, stacks + 1):
            row = stack / stacks
            alpha = lerp(startAngle, endAngle, row)
            sin_alpha = sin(alpha)
            cos_alpha = cos(alpha)
            for slice in range(0, slices + 1):
                col = slice / slices
                beta = lerp(2*PI, 0, col)
                cos_beta = cos(beta)
                v = MeshVertex()
                v.position.x = (R + r * cos_beta) * sin_alpha
                v.position.y = r * sin(beta)
                v.position.z = (R + r * cos_beta) * cos_alpha
                center = Vec3(R * sin_alpha, 0, R * cos_alpha)
                v.normal = (v.position - center) / r
                v.uv = Vec2(col, row)
                append(vertices, v)
        indices = buildIndexBuffer(stacks, slices)
        return Geometry(vertices, indices)
        */
        return create_cube(stacks, slices);
    }

    void add_cap(std::vector<MeshVertex>& vertices, std::vector<unsigned>& indices, float center_y, int slices)
    {
        /* TODO - implement logic to create a cap
         R = 0.5
        centerIndex = size(vertices)
        vertex = MeshVertex()

        textureCoordScale = if centerY > 0 then 1.0 else -1.0

        vertex.normal = if centerY > 0 then Vec3(0, 1, 0) else Vec3(0, -1, 0)
        vertex.position = Vec3(0, centerY, 0)
        vertex.uv = Vec2(0.5, 0.5)
        append(vertices, vertex)

        for slice in range(0, slices + 1):
            col = slice / slices
            alpha = col * PI * 2.0
            sinAlpha = sin(alpha)
            cosAlpha = cos(alpha)
            vertex.position.x = R * sinAlpha
            vertex.position.z = R * cosAlpha
            vertex.uv = Vec2(textureCoordScale * 0.5 * cosAlpha + 0.5, 0.5 * sinAlpha + 0.5)
            append(vertices, vertex)

        k = centerIndex + 1
        secondIndexOffset = if centerY > 0 then 0 else 1
        thirdIndexOffset = if centerY > 0 then 1 else 0
        for i in range(0, slices - 1):
            append(indices, centerIndex, k + secondIndexOffset, k + thirdIndexOffset)
            k++
        if centerY > 0:
            append(indices, centerIndex, k, centerIndex + 1)
        else:
            append(indices, centerIndex, centerIndex + 1, k)
        */
    }

    Geometry create_cylinder(int stacks, int slices)
    {
        /* TODO - implement logic to create cylinder
           vertices = emptyList
           numVertices = (stacks + 1) * (slices + 1) + (slices * 2 + 2)
           reserve(vertices, numVertices)
           R = 0.5
           for stack in range(0, stacks + 1):
               row = stack / stacks
               for slice in range(0, slices + 1):
                   col = slice / slices
                   alpha = col * PI * 2.0
                   vertex = MeshVertex()
                   sinAlpha = sin(alpha)
                   cosAlpha = cos(alpha)
                   vertex.position.x = R * sinAlpha
                   vertex.position.y = row - 0.5
                   vertex.position.z = R * cosAlpha
                   vertex.normal = Vec3(sinAlpha, 0, cosAlpha)
                   vertex.uv = Vec2(col, row)
                   append(vertices, vertex)

           indices = buildIndexBuffer(stacks, slices)
           addCap(vertices, indices, 0.5, slices)
           addCap(vertices, indices, -0.5, slices)

           return Geometry(vertices, indices)
       */
        return create_cube(stacks, slices);
    }

    Geometry create_cone(int stacks, int slices)
    {
        /* TODO implement logic to create cone
            vertices = emptyList
            numVertices = (stacks + 1) * (slices + 1) + (slices + 1)
            reserve(vertices, numVertices)
            R = 0.5
            TopRadius = 0.0
            BottomRadius = R
            TopYValue = 0.5
            BottomYValue = -0.5
            Rise = TopYValue - BottomYValue
            Run = TopRadius - BottomRadius
            Slope = Rise / Run
            TangentSlope = -1.0 / Slope
            for stack in range(0, stacks + 1):
                row = stack / stacks
                h = row - 0.5
                for slice in range(0, slices + 1):
                    col = slice / slices
                    alpha = col * PI * 2.0
                    sinAlpha = sin(alpha)
                    cosAlpha = cos(alpha)
                    vertex = MeshVertex()
                    vertex.position.x = R * (0.5 - h) * sinAlpha
                    vertex.position.y = h
                    vertex.position.z = R * (0.5 - h) * cosAlpha
                    if stack != stacks:
                        vertex.normal = normalize(Vec3(sinAlpha, TangentSlope, cosAlpha))
                    else:
                        vertex.normal = Vec3(0, 1, 0)
                    vertex.uv = Vec2(col, row)
                    append(vertices, vertex)

            indices = buildIndexBuffer(stacks, slices)
            addCap(vertices, indices, -0.5, slices)

            return Geometry(vertices, indices)
        */

        return create_cube(stacks, slices);
    }

    SubMesh to_submesh_as_triangles(const Geometry& geometry, Material* material)
    {
        SubMesh sub_mesh;
        sub_mesh.VertexArrayObj.SetPrimitivePattern(opengl::Primitive::Triangles);
        sub_mesh.VertexArrayObj.AddVertexBuffer(opengl::VertexBuffer(std::span{ geometry.Vertices }), get_meshvertex_layout());
        sub_mesh.VertexArrayObj.SetIndexBuffer(opengl::IndexBuffer(std::span{ geometry.Indicies }));
        sub_mesh.Material = material;
        return sub_mesh;
    }

    SubMesh to_submesh_as_lines(const Geometry& geometry, Material* material)
    {
        std::vector<unsigned> lines_indices = convert_to_lines_pattern(geometry.Indicies);
        SubMesh               sub_mesh;
        sub_mesh.VertexArrayObj.SetPrimitivePattern(opengl::Primitive::Lines);
        sub_mesh.VertexArrayObj.AddVertexBuffer(opengl::VertexBuffer(std::span{ geometry.Vertices }), get_meshvertex_layout());
        sub_mesh.VertexArrayObj.SetIndexBuffer(opengl::IndexBuffer(std::span{ lines_indices }));
        sub_mesh.Material = material;
        return sub_mesh;
    }
}

namespace
{
    std::vector<graphics::MeshVertex> create_plane_vertices(int stacks, int slices)
    {
        /* TODO Implement me second
           vertices = emptyList
           numVertices = (stacks + 1) * (slices + 1)
           reserve(vertices, numVertices)

           for stack in range(0, stacks + 1):
               row = stack / stacks
               for slice in range(0, slices + 1):
                   col = slice / slices
                   v = MeshVertex()
                   v.position = Vec3(col - 0.5, row - 0.5, 0.0)
                   v.normal = Vec3(0.0, 0.0, 1.0)
                   v.uv = Vec2(col, row)
                   append(vertices, v)

           return vertices
       */
        return {};
    }

    std::vector<unsigned> build_index_buffer(int stacks, int slices)
    {
        /* TODO Implement this FIRST!
            p0, p1, p2, p3, p4, p5 = 0, 0, 0, 0, 0, 0

           indices = emptyList
           numTriangles = stacks * slices * 2
           numIndices = numTriangles * 3
           reserve(indices, numIndices)
           stride = slices + 1

           for i in range(0, stacks):
               currRow = i * stride

               for j in range(0, slices):
                   # Compute indices for the first triangle
                   p0 = currRow + j
                   p1 = p0 + 1
                   p2 = p1 + stride

                   append(indices, p0, p1, p2)

                   # Compute indices for the second triangle
                   p3 = p2
                   p4 = p3 - 1
                   p5 = p0

                   append(indices, p3, p4, p5)

           return indices
       */
        return {};
    }

    std::vector<unsigned> convert_to_lines_pattern(const std::vector<unsigned>& indices)
    {
        /* TODO Implement me
                    linesIndices = emptyList
                    i = 0

                    if size(indices) > 6:
                        limit = if size(indices) % 6 == 0 then size(indices) else size(indices) - 5

                        while i < limit:
                            p0 = indices[i]
                            p1 = indices[i + 1]
                            p2 = indices[i + 2]
                            p3 = indices[i + 3]
                            p4 = indices[i + 4]
                            p5 = indices[i + 5]

                            if p1 != (p0 + 1) or p3 != p2 or p4 != (p3 - 1) or p5 != p0:
                                break

                            # Outline of the quad
                            append(linesIndices, p0, p1)
                            append(linesIndices, p1, p2)
                            append(linesIndices, p2, p4)
                            append(linesIndices, p4, p0)

                            i += 6

                    if i < size(indices) and size(indices) - i >= 3:
                        limit = if size(indices) % 3 == 0 then size(indices) else size(indices) - 2

                        while i < limit:
                            p0 = indices[i]
                            p1 = indices[i + 1]
                            p2 = indices[i + 2]

                            append(linesIndices, p0, p1)
                            append(linesIndices, p1, p2)
                            append(linesIndices, p2, p0)

                            i += 3

                    return linesIndices

                */
        return indices;
    }
}
