#include "CameraRender2D.h"
#include "Projection.h"
#include <cmath>

CameraRender2D::CameraRender2D(Render &r) : render(r)
{
    SetCamera(Camera());
}

CameraRender2D::~CameraRender2D(void)
{
}

void CameraRender2D::SetCamera(const Camera &cam)
{
    world2camera = WorldToCamera(cam);
    camera2ndc = CameraToNDC(cam);
}

void CameraRender2D::DisplayEdges(Mesh &m, const Affine &A, const Vector &color)
{
    int vCount = m.VertexCount();
    cam_vertices.resize(static_cast<size_t>(vCount));

    Affine model2cam = world2camera * A;

    for (int i = 0; i < vCount; ++i)
    {
        Point p = m.GetVertex(i);
        cam_vertices[static_cast<size_t>(i)] = Point(model2cam * p);
    }

    render.SetColor(color);
    int eCount = m.EdgeCount();
    for (int j = 0; j < eCount; ++j)
    {
        Mesh::Edge e = m.GetEdge(j);
        Point p1_cam = cam_vertices[static_cast<size_t>(e.index1)];
        Point p2_cam = cam_vertices[static_cast<size_t>(e.index2)];

        if (p1_cam.z >= 0.0f || p2_cam.z >= 0.0f)
        {
            continue;
        }

        Hcoord p1_ndc = camera2ndc * p1_cam;
        Hcoord p2_ndc = camera2ndc * p2_cam;

        p1_ndc.x /= p1_ndc.w;
        p1_ndc.y /= p1_ndc.w;
        p1_ndc.z /= p1_ndc.w;
        p2_ndc.x /= p2_ndc.w;
        p2_ndc.y /= p2_ndc.w;
        p2_ndc.z /= p2_ndc.w;
        p1_ndc.w = 1.0f;
        p2_ndc.w = 1.0f;

        render.DrawLine(p1_ndc, p2_ndc);
    }
}

void CameraRender2D::DisplayFaces(Mesh &m, const Affine &A, const Vector &color)
{
    int vert_count = m.VertexCount();
    cam_vertices.resize(static_cast<size_t>(vert_count));

    Affine model2cam = world2camera * A;

    for (int i = 0; i < vert_count; ++i)
    {
        Point p = m.GetVertex(i);
        cam_vertices[static_cast<size_t>(i)] = Point(model2cam * p);
    }

    int fCount = m.FaceCount();
    for (int k = 0; k < fCount; ++k)
    {
        Mesh::Face f = m.GetFace(k);
        Point p1_cam = cam_vertices[static_cast<size_t>(f.index1)];
        Point p2_cam = cam_vertices[static_cast<size_t>(f.index2)];
        Point p3_cam = cam_vertices[static_cast<size_t>(f.index3)];

        if (p1_cam.z >= 0.0f || p2_cam.z >= 0.0f || p3_cam.z >= 0.0f)
        {
            continue;
        }

        Vector u(p2_cam.x - p1_cam.x, p2_cam.y - p1_cam.y, p2_cam.z - p1_cam.z);
        Vector v(p3_cam.x - p1_cam.x, p3_cam.y - p1_cam.y, p3_cam.z - p1_cam.z);
        Vector n = cross(u, v);

        Vector p_to_e(-p1_cam.x, -p1_cam.y, -p1_cam.z);
        if (dot(n, p_to_e) <= 0.0f)
        {
            continue;
        }

        Vector L(0.0f, 0.0f, 1.0f);
        float dot_Ln = dot(L, n);
        float len_n = std::sqrt(dot(n, n));

        float mu = std::abs(dot_Ln) / len_n;
        Vector shadedColor(mu * color.x, mu * color.y, mu * color.z);
        render.SetColor(shadedColor);

        Hcoord p1_ndc = camera2ndc * p1_cam;
        Hcoord p2_ndc = camera2ndc * p2_cam;
        Hcoord p3_ndc = camera2ndc * p3_cam;

        p1_ndc.x /= p1_ndc.w;
        p1_ndc.y /= p1_ndc.w;
        p1_ndc.z /= p1_ndc.w;
        p2_ndc.x /= p2_ndc.w;
        p2_ndc.y /= p2_ndc.w;
        p2_ndc.z /= p2_ndc.w;
        p3_ndc.x /= p3_ndc.w;
        p3_ndc.y /= p3_ndc.w;
        p3_ndc.z /= p3_ndc.w;

        p1_ndc.w = 1.0f;
        p2_ndc.w = 1.0f;
        p3_ndc.w = 1.0f;

        render.FillTriangle(p1_ndc, p2_ndc, p3_ndc);
    }
}