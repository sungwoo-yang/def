#include "RasterUtilities.h"
#include <cmath>

void ClearBuffers(Raster &r, float z)
{
    int const width = r.Width();
    int const height = r.Height();

    for (int y = 0; y < height; ++y)
    {
        r.GotoPoint(0, y);
        for (int x = 0; x < width; ++x)
        {
            r.WritePixel();
            r.WriteZ(z);
            if (x < width - 1)
            {
                r.IncrementX();
            }
        }
    }
}

void FillTriangle(Raster &r, const Hcoord &P, const Hcoord &Q, const Hcoord &R)
{
    float const invW_P = 1.0f / P.w;
    float const invW_Q = 1.0f / Q.w;
    float const invW_R = 1.0f / R.w;    

    Hcoord p(P.x * invW_P, P.y * invW_P, P.z * invW_P, 1.0f);
    Hcoord q(Q.x * invW_Q, Q.y * invW_Q, Q.z * invW_Q, 1.0f);
    Hcoord r_coord(R.x * invW_R, R.y * invW_R, R.z * invW_R, 1.0f);

    float temp_min_x = (p.x < q.x) ? p.x : q.x;
    temp_min_x = (temp_min_x < r_coord.x) ? temp_min_x : r_coord.x;
    float temp_max_x = (p.x > q.x) ? p.x : q.x;
    temp_max_x = (temp_max_x > r_coord.x) ? temp_max_x : r_coord.x;

    float temp_min_y = (p.y < q.y) ? p.y : q.y;
    temp_min_y = (temp_min_y < r_coord.y) ? temp_min_y : r_coord.y;
    float temp_max_y = (p.y > q.y) ? p.y : q.y;
    temp_max_y = (temp_max_y > r_coord.y) ? temp_max_y : r_coord.y;

    int min_x = static_cast<int>(std::floor(temp_min_x));
    int max_x = static_cast<int>(std::ceil(temp_max_x));
    int min_y = static_cast<int>(std::floor(temp_min_y));
    int max_y = static_cast<int>(std::ceil(temp_max_y));

    if (min_x < 0)
        min_x = 0;
    if (max_x >= r.Width())
        max_x = r.Width() - 1;
    if (min_y < 0)
        min_y = 0;
    if (max_y >= r.Height())
        max_y = r.Height() - 1;

    if (min_x > max_x || min_y > max_y)
        return;

    float dx01 = q.x - p.x, dy01 = q.y - p.y;
    float dx12 = r_coord.x - q.x, dy12 = r_coord.y - q.y;
    float dx20 = p.x - r_coord.x, dy20 = p.y - r_coord.y;

    float area = dx01 * (r_coord.y - p.y) - dy01 * (r_coord.x - p.x);
    if (std::abs(area) < 0.0001f)
        return;

    float vx1 = q.x - p.x, vy1 = q.y - p.y, vz1 = q.z - p.z;
    float vx2 = r_coord.x - p.x, vy2 = r_coord.y - p.y, vz2 = r_coord.z - p.z;
    float nx = vy1 * vz2 - vz1 * vy2;
    float ny = vz1 * vx2 - vx1 * vz2;
    float nz = vx1 * vy2 - vy1 * vx2;

    if (std::abs(nz) < 0.000001f)
        return;

    float dzdx = -nx / nz;
    float dzdy = -ny / nz;

    float s_x = static_cast<float>(min_x);
    float s_y = static_cast<float>(min_y);

    float z_row = p.z + dzdx * (s_x - p.x) + dzdy * (s_y - p.y);
    float e01_row = dx01 * (s_y - p.y) - dy01 * (s_x - p.x);
    float e12_row = dx12 * (s_y - q.y) - dy12 * (s_x - q.x);
    float e20_row = dx20 * (s_y - r_coord.y) - dy20 * (s_x - r_coord.x);

    float de01_x = -dy01, de12_x = -dy12, de20_x = -dy20;
    float de01_y = dx01, de12_y = dx12, de20_y = dx20;

    bool is_ccw = (area > 0.0f);

    for (int y = min_y; y <= max_y; ++y)
    {
        r.GotoPoint(min_x, y);

        float e01 = e01_row;
        float e12 = e12_row;
        float e20 = e20_row;
        float z = z_row;

        for (int x = min_x; x <= max_x; ++x)
        {
            bool inside = is_ccw ? (e01 >= 0 && e12 >= 0 && e20 >= 0)
                                 : (e01 <= 0 && e12 <= 0 && e20 <= 0);

            if (inside)
            {
                if (z < r.GetZ())
                {
                    r.WriteZ(z);
                    r.WritePixel();
                }
            }

            if (x < max_x)
            {
                r.IncrementX();
            }
            e01 += de01_x;
            e12 += de12_x;
            e20 += de20_x;
            z += dzdx;
        }

        e01_row += de01_y;
        e12_row += de12_y;
        e20_row += de20_y;
        z_row += dzdy;
    }
}