#include "RasterUtilities.h"
#include <cmath>
#include <algorithm>

void ClearBuffers(Raster &r, float z)
{
    int const buf_width = r.Width();
    int const buf_height = r.Height();

    for (int row_idx = 0; row_idx < buf_height; ++row_idx)
    {
        r.GotoPoint(0, row_idx);
        for (int col_idx = 0; col_idx < buf_width; ++col_idx)
        {
            r.WritePixel();
            r.WriteZ(z);
            if (col_idx < buf_width - 1)
            {
                r.IncrementX();
            }
        }
    }
}

void FillTriangle(Raster &r, const Hcoord &P, const Hcoord &Q, const Hcoord &R)
{
    int min_x = static_cast<int>(std::max(0.0f, std::floor(std::min({P.x, Q.x, R.x}))));
    int max_x = static_cast<int>(std::min(static_cast<float>(r.Width() - 1), std::ceil(std::max({P.x, Q.x, R.x}))));
    int min_y = static_cast<int>(std::max(0.0f, std::floor(std::min({P.y, Q.y, R.y}))));
    int max_y = static_cast<int>(std::min(static_cast<float>(r.Height() - 1), std::ceil(std::max({P.y, Q.y, R.y}))));

    if (min_x > max_x || min_y > max_y)
        return;

    float dx01 = Q.x - P.x, dy01 = Q.y - P.y;
    float dx12 = R.x - Q.x, dy12 = R.y - Q.y;
    float dx20 = P.x - R.x, dy20 = P.y - R.y;

    float area = dx01 * (R.y - P.y) - dy01 * (R.x - P.x);
    if (area == 0.0f)
        return;

    float vx1 = Q.x - P.x, vy1 = Q.y - P.y, vz1 = Q.z - P.z;
    float vx2 = R.x - P.x, vy2 = R.y - P.y, vz2 = R.z - P.z;

    float nx = vy1 * vz2 - vz1 * vy2;
    float ny = vz1 * vx2 - vx1 * vz2;
    float nz = vx1 * vy2 - vy1 * vx2;

    if (nz == 0.0f)
        return;

    float dzdx = -nx / nz;
    float dzdy = -ny / nz;

    float start_x = static_cast<float>(min_x);
    float start_y = static_cast<float>(min_y);

    float z_base = P.z + dzdx * (start_x - P.x) + dzdy * (start_y - P.y);
    float e01_base = dx01 * (start_y - P.y) - dy01 * (start_x - P.x);
    float e12_base = dx12 * (start_y - Q.y) - dy12 * (start_x - Q.x);
    float e20_base = dx20 * (start_y - R.y) - dy20 * (start_x - R.x);

    float de01_x = -dy01, de12_x = -dy12, de20_x = -dy20;
    float de01_y = dx01, de12_y = dx12, de20_y = dx20;

    float e01_row = e01_base;
    float e12_row = e12_base;
    float e20_row = e20_base;
    float z_row = z_base;

    bool is_ccw = (area > 0.0f);

    for (int y_coord = min_y; y_coord <= max_y; ++y_coord)
    {
        r.GotoPoint(min_x, y_coord);

        float e01 = e01_row;
        float e12 = e12_row;
        float e20 = e20_row;
        float current_z = z_row;

        for (int x_coord = min_x; x_coord <= max_x; ++x_coord)
        {
            bool inside = is_ccw ? (e01 >= 0.0f && e12 >= 0.0f && e20 >= 0.0f)
                                 : (e01 <= 0.0f && e12 <= 0.0f && e20 <= 0.0f);

            if (inside)
            {
                if (current_z < r.GetZ())
                {
                    r.WriteZ(current_z);
                    r.WritePixel();
                }
            }

            if (x_coord < max_x)
            {
                r.IncrementX();
            }
            e01 += de01_x;
            e12 += de12_x;
            e20 += de20_x;
            current_z += dzdx;
        }

        e01_row += de01_y;
        e12_row += de12_y;
        e20_row += de20_y;
        z_row += dzdy;
    }
}