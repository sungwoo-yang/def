// Name       : Sungwoo Yang
// Assignment : Affine and Mesh Programming Assignment
// Course     : CS250 Computer Graphics II
// Term & Year: 2026 Spring

#include "CubeMesh.h"

// 1. Define 8 vertices (centered at origin, side length 2)
const Point CubeMesh::vertices[8] = {
    Point(-1, -1, 1),  // 0: Front-Bottom-Left
    Point(1, -1, 1),   // 1: Front-Bottom-Right
    Point(1, 1, 1),    // 2: Front-Top-Right
    Point(-1, 1, 1),   // 3: Front-Top-Left
    Point(-1, -1, -1), // 4: Back-Bottom-Left
    Point(1, -1, -1),  // 5: Back-Bottom-Right
    Point(1, 1, -1),   // 6: Back-Top-Right
    Point(-1, 1, -1)   // 7: Back-Top-Left
};

// 2. Define 12 triangular faces (Counter-Clockwise CCW winding)
const Mesh::Face CubeMesh::faces[12] = {
    Face(0, 1, 2), Face(0, 2, 3), // Front
    Face(5, 4, 7), Face(5, 7, 6), // Back
    Face(1, 5, 6), Face(1, 6, 2), // Right
    Face(4, 0, 3), Face(4, 3, 7), // Left
    Face(3, 2, 6), Face(3, 6, 7), // Top
    Face(4, 5, 1), Face(4, 1, 0)  // Bottom
};

// 3. Define 12 edges
const Mesh::Edge CubeMesh::edges[12] = {
    Edge(0, 1), Edge(1, 2), Edge(2, 3), Edge(3, 0), // Front face edges
    Edge(4, 5), Edge(5, 6), Edge(6, 7), Edge(7, 4), // Back face edges
    Edge(0, 4), Edge(1, 5), Edge(2, 6), Edge(3, 7)  // Connecting edges
};

int CubeMesh::VertexCount(void)
{
    return 8;
}

Point CubeMesh::GetVertex(int i)
{
    return vertices[i];
}

Vector CubeMesh::Dimensions(void)
{
    float min_x = vertices[0].x;
    float max_x = vertices[0].x;
    float min_y = vertices[0].y;
    float max_y = vertices[0].y;
    float min_z = vertices[0].z;
    float max_z = vertices[0].z;

    for (int i = 1; i < 8; i++)
    {
        min_x = (vertices[i].x < min_x) ? vertices[i].x : min_x;
        max_x = (vertices[i].x > max_x) ? vertices[i].x : max_x;

        min_y = (vertices[i].y < min_y) ? vertices[i].y : min_y;
        max_y = (vertices[i].y > max_y) ? vertices[i].y : max_y;

        min_z = (vertices[i].z < min_z) ? vertices[i].z : min_z;
        max_z = (vertices[i].z > max_z) ? vertices[i].z : max_z;
    }

    return Vector(max_x - min_x, max_y - min_y, max_z - min_z);
}

Point CubeMesh::Center(void)
{
    float min_x = vertices[0].x;
    float max_x = vertices[0].x;
    float min_y = vertices[0].y;
    float max_y = vertices[0].y;
    float min_z = vertices[0].z;
    float max_z = vertices[0].z;

    for (int i = 1; i < 8; i++)
    {
        min_x = (vertices[i].x < min_x) ? vertices[i].x : min_x;
        max_x = (vertices[i].x > max_x) ? vertices[i].x : max_x;

        min_y = (vertices[i].y < min_y) ? vertices[i].y : min_y;
        max_y = (vertices[i].y > max_y) ? vertices[i].y : max_y;

        min_z = (vertices[i].z < min_z) ? vertices[i].z : min_z;
        max_z = (vertices[i].z > max_z) ? vertices[i].z : max_z;
    }

    return Point((max_x + min_x) * 0.5f, (max_y + min_y) * 0.5f, (max_z + min_z) * 0.5f);
}

int CubeMesh::FaceCount(void)
{
    return 12;
}

Mesh::Face CubeMesh::GetFace(int i)
{
    return faces[i];
}

int CubeMesh::EdgeCount(void)
{
    return 12;
}

Mesh::Edge CubeMesh::GetEdge(int i)
{
    return edges[i];
}
