#include "Interpolate.h"

// 정적 멤버 변수 초기화
float    TexturedCoord::color_scale = 1.0f;
Texture* TexturedCoord::texture     = nullptr;

// 1. TextureClip::operator() (PDF 2, 3페이지 로직 반영)
bool TextureClip::operator()(std::vector<TexturedCoord>& vertices)
{
    if (vertices.empty())
        return false;

    for (size_t plane_idx = 0; plane_idx < half_spaces.size(); ++plane_idx)
    {
        const HalfSpace& hs = half_spaces[plane_idx];
        temp_vertices.clear();

        size_t num_verts = vertices.size();
        for (size_t v_idx = 0; v_idx < num_verts; ++v_idx)
        {
            const TexturedCoord& P_vert   = vertices[v_idx];
            size_t               next_idx = v_idx + 1;
            if (next_idx == num_verts)
                next_idx = 0;
            const TexturedCoord& Q_vert = vertices[next_idx];

            // 평면 방정식과 점의 내적 계산
            float dP = hs.x * P_vert.x + hs.y * P_vert.y + hs.z * P_vert.z + hs.w * P_vert.w;
            float dQ = hs.x * Q_vert.x + hs.y * Q_vert.y + hs.z * Q_vert.z + hs.w * Q_vert.w;

            bool p_inside = (dP >= 0.0f);
            bool q_inside = (dQ >= 0.0f);

            if (p_inside && q_inside)
            {
                temp_vertices.push_back(Q_vert);
            }
            else if (p_inside != q_inside)
            {
                float         s = dP / (dP - dQ);
                TexturedCoord I;

                I.x = P_vert.x + s * (Q_vert.x - P_vert.x);
                I.y = P_vert.y + s * (Q_vert.y - P_vert.y);
                I.z = P_vert.z + s * (Q_vert.z - P_vert.z);
                I.w = P_vert.w + s * (Q_vert.w - P_vert.w);
                I.u = P_vert.u + s * (Q_vert.u - P_vert.u);
                I.v = P_vert.v + s * (Q_vert.v - P_vert.v);

                temp_vertices.push_back(I);

                if (!p_inside && q_inside)
                {
                    temp_vertices.push_back(Q_vert);
                }
            }
        }
        vertices = temp_vertices;
        if (vertices.empty())
            return false;
    }
    return true;
}

void FillTriangle(Raster& r, const TexturedCoord& P, const TexturedCoord& Q, const TexturedCoord& R)
{
    if (!TexturedCoord::texture)
        return;

    // 1. Perspective Divide (w 나누기)
    // -> 이 단계를 거쳐야 동차 좌표계가 실제 스크린 픽셀 좌표로 변환됩니다.
    float inv_Pw = 1.0f / P.w;
    float inv_Qw = 1.0f / Q.w;
    float inv_Rw = 1.0f / R.w;

    // 실제 화면에 찍힐 픽셀 좌표 (x, y)와 깊이 값 (z)
    float P_px = P.x * inv_Pw, P_py = P.y * inv_Pw, P_dev_z = P.z * inv_Pw;
    float Q_px = Q.x * inv_Qw, Q_py = Q.y * inv_Qw, Q_dev_z = Q.z * inv_Qw;
    float R_px = R.x * inv_Rw, R_py = R.y * inv_Rw, R_dev_z = R.z * inv_Rw;

    // 텍스처 좌표 (u/w, v/w)
    float u_Pw = P.u * inv_Pw, v_Pw = P.v * inv_Pw;
    float u_Qw = Q.u * inv_Qw, v_Qw = Q.v * inv_Qw;
    float u_Rw = R.u * inv_Rw, v_Rw = R.v * inv_Rw;

    // 2. Bounding Box 계산
    float min_xf = P_px, max_xf = P_px, min_yf = P_py, max_yf = P_py;
    if (Q_px < min_xf)
        min_xf = Q_px;
    if (Q_px > max_xf)
        max_xf = Q_px;
    if (R_px < min_xf)
        min_xf = R_px;
    if (R_px > max_xf)
        max_xf = R_px;
    if (Q_py < min_yf)
        min_yf = Q_py;
    if (Q_py > max_yf)
        max_yf = Q_py;
    if (R_py < min_yf)
        min_yf = R_py;
    if (R_py > max_yf)
        max_yf = R_py;

    int min_x = static_cast<int>(min_xf);
    int max_x = static_cast<int>(max_xf + 1.0f);
    int min_y = static_cast<int>(min_yf);
    int max_y = static_cast<int>(max_yf + 1.0f);

    // 스크린 경계 클리핑
    if (min_x < 0)
        min_x = 0;
    if (min_y < 0)
        min_y = 0;
    if (max_x >= r.Width())
        max_x = r.Width() - 1;
    if (max_y >= r.Height())
        max_y = r.Height() - 1;

    // 3. 외적을 이용한 전체 면적 계산 (2D 기준)
    float area     = (Q_px - P_px) * (R_py - P_py) - (R_px - P_px) * (Q_py - P_py);
    float abs_area = (area < 0.0f) ? -area : area;
    if (abs_area < 0.00001f)
        return; // 면적이 0인 삼각형(직선) 무시
    float inv_area = 1.0f / area;

    // 4. 픽셀 루프 및 렌더링
    for (int px_y = min_y; px_y <= max_y; ++px_y)
    {
        float f_y = static_cast<float>(px_y) + 0.5f; // 픽셀의 정중앙
        for (int px_x = min_x; px_x <= max_x; ++px_x)
        {
            float f_x = static_cast<float>(px_x) + 0.5f;

            // 무게중심 좌표계 (Barycentric coordinates) w0, w1, w2 계산
            float w0 = ((Q_px - f_x) * (R_py - f_y) - (R_px - f_x) * (Q_py - f_y)) * inv_area;
            float w1 = ((R_px - f_x) * (P_py - f_y) - (P_px - f_x) * (R_py - f_y)) * inv_area;
            float w2 = 1.0f - w0 - w1;

            // 픽셀이 삼각형 내부에 있는지 확인
            if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f)
            {
                // 깊이(Z) 보간
                float z_pixel = w0 * P_dev_z + w1 * Q_dev_z + w2 * R_dev_z;

                r.GotoPoint(px_x, px_y);
                // Z-Buffer 테스트
                if (z_pixel < r.GetZ())
                {
                    r.WriteZ(z_pixel);

                    // 5. 원근 보정 보간 (Perspective-Correct Interpolation)
                    float inv_w = w0 * inv_Pw + w1 * inv_Qw + w2 * inv_Rw;
                    float u_w   = w0 * u_Pw + w1 * u_Qw + w2 * u_Rw;
                    float v_w   = w0 * v_Pw + w1 * v_Qw + w2 * v_Rw;

                    // 실제 텍스처 좌표 복원
                    Vector color = TexturedCoord::texture->uvToRGB(u_w / inv_w, v_w / inv_w);

                    // 빛/그림자 효과(Color Scale) 적용
                    float r_s = color.x * TexturedCoord::color_scale;
                    float g_s = color.y * TexturedCoord::color_scale;
                    float b_s = color.z * TexturedCoord::color_scale;

                    // 색상 클램핑 후 버퍼에 쓰기
                    r.SetColor(
                        static_cast<Raster::byte>(r_s > 255.0f ? 255.0f : (r_s < 0.0f ? 0.0f : r_s)), static_cast<Raster::byte>(g_s > 255.0f ? 255.0f : (g_s < 0.0f ? 0.0f : g_s)),
                        static_cast<Raster::byte>(b_s > 255.0f ? 255.0f : (b_s < 0.0f ? 0.0f : b_s)));
                    r.WritePixel();
                }
            }
        }
    }
}