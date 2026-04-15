#include "Projection.h"

Affine CameraToWorld(const Camera &cam)
{
    return Affine(cam.Right(), cam.Up(), cam.Back(), cam.Eye());
}
Affine WorldToCamera(const Camera &cam)
{
    return inverse(CameraToWorld(cam));
}
Matrix CameraToNDC(const Camera &cam)
{
    Vector geom = cam.ViewportGeometry();
    float w_val = geom.x;
    float h_val = geom.y;
    float d_val = geom.z;
    float n_val = cam.NearDistance();
    float f_val = cam.FarDistance();
    Matrix m;

    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 4; ++c)
        {
            m[r][c] = 0.0f;
        }
    }

    m[0][0] = (2.0f * d_val) / w_val;
    m[1][1] = (2.0f * d_val) / h_val;
    m[2][2] = (n_val + f_val) / (n_val - f_val);
    m[2][3] = (2.0f * n_val * f_val) / (n_val - f_val);
    m[3][2] = -1.0f;
    m[3][3] = 0.0f;

    return m;
}