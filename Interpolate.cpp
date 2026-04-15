#include "Interpolate.h"

float TexturedCoord::color_scale = 1.0f;
Texture* TexturedCoord::texture = nullptr;

bool TextureClip::operator()(std::vector<TexturedCoord>& vertices) {
    if (vertices.empty()) return false;

    for (size_t plane_idx = 0; plane_idx < half_spaces.size(); ++plane_idx) {
        const HalfSpace& hs = half_spaces[plane_idx];
        temp_vertices.clear();

        size_t num_verts = vertices.size();
        for (size_t v_idx = 0; v_idx < num_verts; ++v_idx) {
            const TexturedCoord& P_vert = vertices[v_idx];
            size_t next_idx = v_idx + 1;
            if (next_idx == num_verts) next_idx = 0;
            const TexturedCoord& Q_vert = vertices[next_idx];

            float dP = hs.x * P_vert.x + hs.y * P_vert.y + hs.z * P_vert.z + hs.w * P_vert.w;
            float dQ = hs.x * Q_vert.x + hs.y * Q_vert.y + hs.z * Q_vert.z + hs.w * Q_vert.w;

            bool p_inside = (dP >= 0.0f);
            bool q_inside = (dQ >= 0.0f);

            if (p_inside && q_inside) {
                temp_vertices.push_back(Q_vert);
            } 
            else if (p_inside != q_inside) { 
                float s = dP / (dP - dQ);
                TexturedCoord I;
                
                I.x = P_vert.x + s * (Q_vert.x - P_vert.x);
                I.y = P_vert.y + s * (Q_vert.y - P_vert.y);
                I.z = P_vert.z + s * (Q_vert.z - P_vert.z);
                I.w = P_vert.w + s * (Q_vert.w - P_vert.w);
                
                I.u = P_vert.u + s * (Q_vert.u - P_vert.u);
                I.v = P_vert.v + s * (Q_vert.v - P_vert.v);

                temp_vertices.push_back(I);
                
                if (!p_inside && q_inside) {
                    temp_vertices.push_back(Q_vert);
                }
            }
        }
        vertices = temp_vertices;
        if (vertices.empty()) return false;
    }
    return true;
}

void FillTriangle(Raster& r, const TexturedCoord& P, const TexturedCoord& Q, const TexturedCoord& R) {
    if (!TexturedCoord::texture) return;

    float inv_Pw = 1.0f / P.w;
    float inv_Qw = 1.0f / Q.w;
    float inv_Rw = 1.0f / R.w;

    float u_Pw = P.u * inv_Pw;
    float v_Pw = P.v * inv_Pw;
    float u_Qw = Q.u * inv_Qw;
    float v_Qw = Q.v * inv_Qw;
    float u_Rw = R.u * inv_Rw;
    float v_Rw = R.v * inv_Rw;

    float P_dev_x = P.x * inv_Pw, P_dev_y = P.y * inv_Pw, P_dev_z = P.z * inv_Pw;
    float Q_dev_x = Q.x * inv_Qw, Q_dev_y = Q.y * inv_Qw, Q_dev_z = Q.z * inv_Qw;
    float R_dev_x = R.x * inv_Rw, R_dev_y = R.y * inv_Rw, R_dev_z = R.z * inv_Rw;

    float width = static_cast<float>(r.Width());  
    float height = static_cast<float>(r.Height()); 
    
    float P_px = (P_dev_x + 1.0f) * 0.5f * width;
    float P_py = (P_dev_y + 1.0f) * 0.5f * height;
    float Q_px = (Q_dev_x + 1.0f) * 0.5f * width;
    float Q_py = (Q_dev_y + 1.0f) * 0.5f * height;
    float R_px = (R_dev_x + 1.0f) * 0.5f * width;
    float R_py = (R_dev_y + 1.0f) * 0.5f * height;

    float min_x_f = P_px;
    if (Q_px < min_x_f) min_x_f = Q_px;
    if (R_px < min_x_f) min_x_f = R_px;

    float max_x_f = P_px;
    if (Q_px > max_x_f) max_x_f = Q_px;
    if (R_px > max_x_f) max_x_f = R_px;

    float min_y_f = P_py;
    if (Q_py < min_y_f) min_y_f = Q_py;
    if (R_py < min_y_f) min_y_f = R_py;

    float max_y_f = P_py;
    if (Q_py > max_y_f) max_y_f = Q_py;
    if (R_py > max_y_f) max_y_f = R_py;

    int min_x = static_cast<int>(min_x_f);
    int max_x = static_cast<int>(max_x_f + 1.0f);
    int min_y = static_cast<int>(min_y_f);
    int max_y = static_cast<int>(max_y_f + 1.0f);

    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= r.Width()) max_x = r.Width() - 1;
    if (max_y >= r.Height()) max_y = r.Height() - 1;

    float area = (Q_px - P_px) * (R_py - P_py) - (R_px - P_px) * (Q_py - P_py);
    
    float abs_area = area;
    if (abs_area < 0.0f) abs_area = -abs_area;
    if (abs_area < 0.00001f) return;

    float inv_area = 1.0f / area;

    for (int px_y = min_y; px_y <= max_y; ++px_y) {
        float f_y = static_cast<float>(px_y) + 0.5f; 
        for (int px_x = min_x; px_x <= max_x; ++px_x) {
            float f_x = static_cast<float>(px_x) + 0.5f;

            float w0 = ((Q_px - f_x) * (R_py - f_y) - (R_px - f_x) * (Q_py - f_y)) * inv_area;
            float w1 = ((R_px - f_x) * (P_py - f_y) - (P_px - f_x) * (R_py - f_y)) * inv_area;
            float w2 = 1.0f - w0 - w1;

            if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
          
                float z_pixel = w0 * P_dev_z + w1 * Q_dev_z + w2 * R_dev_z;

                r.GotoPoint(px_x, px_y);
                if (z_pixel < r.GetZ()) { 
                    r.WriteZ(z_pixel);

                    float inv_w_pixel = w0 * inv_Pw + w1 * inv_Qw + w2 * inv_Rw;
                    float inv_u_pixel = w0 * u_Pw   + w1 * u_Qw   + w2 * u_Rw;
                    float inv_v_pixel = w0 * v_Pw   + w1 * v_Qw   + w2 * v_Rw;

                    float final_u = inv_u_pixel / inv_w_pixel;
                    float final_v = inv_v_pixel / inv_w_pixel;

                    Vector color = TexturedCoord::texture->uvToRGB(final_u, final_v);
                    
                    float r_scaled = color.x * TexturedCoord::color_scale;
                    float g_scaled = color.y * TexturedCoord::color_scale;
                    float b_scaled = color.z * TexturedCoord::color_scale;
                    
                    Raster::byte r_b = static_cast<Raster::byte>(r_scaled > 255.0f ? 255.0f : (r_scaled < 0.0f ? 0.0f : r_scaled));
                    Raster::byte g_b = static_cast<Raster::byte>(g_scaled > 255.0f ? 255.0f : (g_scaled < 0.0f ? 0.0f : g_scaled));
                    Raster::byte b_b = static_cast<Raster::byte>(b_scaled > 255.0f ? 255.0f : (b_scaled < 0.0f ? 0.0f : b_scaled));

                    r.SetColor(r_b, g_b, b_b);
                    r.WritePixel();
                }
            }
        }
    }
}