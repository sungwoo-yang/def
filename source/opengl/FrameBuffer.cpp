/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "FrameBuffer.hpp"

#include "Environment.hpp"
#include "GL.hpp"
#include "util/Logger.hpp"

namespace opengl
{

    FrameBuffer::~FrameBuffer() noexcept
    {
        delete_resources();
    }

    FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept : frameBufferHandle(other.frameBufferHandle), depthTexture(std::move(other.depthTexture)), colorTexture(std::move(other.colorTexture))
    {
        other.frameBufferHandle = 0;
    }

    FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept
    {
        std::swap(frameBufferHandle, other.frameBufferHandle);
        std::swap(depthTexture, other.depthTexture);
        std::swap(colorTexture, other.colorTexture);
        return *this;
    }

    // TODO remove [[maybe_unused]]
    void FrameBuffer::Use([[maybe_unused]] bool bind) const
    {
        // TODO GL::BindFramebuffer - https://docs.gl/es3/glBindFramebuffer
    }

    void FrameBuffer::LoadWithSpecification(Specification spec)
    {
        delete_resources();

        const int width  = spec.Width;
        const int height = spec.Height;

        if (spec.DepthFormat != Texture::DepthComponentSize::None)
        {
            if (const auto loaded = depthTexture.LoadAsDepthTexture(width, height, spec.DepthFormat); !loaded)
            {
                util::log_error("Failed to create depth texture");
                return;
            }
        }
        else
        {
            depthTexture = Texture();
        }

        GLenum draw_buffers[] = { GL_NONE };

        if (spec.ColorFormat != ColorComponent::None)
        {
            if (const auto loaded = colorTexture.LoadAsRGBA(width, height); !loaded)
            {
                util::log_error("Failed to create color texture");
                return;
            }
            draw_buffers[0] = GL_COLOR_ATTACHMENT0;
        }
        else
        {
            colorTexture = Texture();
        }


        GLenum status_result{};
        /* TODO
                GL::GenFramebuffers - https://docs.gl/es3/glGenFramebuffers
                GL::BindFramebuffer - https://docs.gl/es3/glBindFramebuffer
                if depth format is not None
                    GL::FramebufferTexture2D - add depth attachment https://docs.gl/es3/glFramebufferTexture2D
                if color format is not None
                    GL::FramebufferTexture2D - add color 0 attachment
                GL::DrawBuffers - https://docs.gl/es3/glDrawBuffers
                GL::CheckFramebufferStatus - https://docs.gl/es3/glCheckFramebufferStatus
                GL::BindFramebuffer - unbind framebuffer
        */

        if (status_result != GL_FRAMEBUFFER_COMPLETE)
        {
            std::string message = "Framebuffer is not complete.\n";
            switch (status_result)
            {
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: message += "\tERROR: Framebuffer incomplete: Attachment is NOT complete.\n"; break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: message += "\tERROR: Framebuffer incomplete: No image is attached to FBO.\n"; break;
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: message += "\tERROR: Framebuffer incomplete: Draw buffer.\n"; break;
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: message += "\tERROR: Framebuffer incomplete: Read buffer.\n"; break;
                case GL_FRAMEBUFFER_UNSUPPORTED: message += "\tERROR: Framebuffer incomplete: Unsupported by FBO implementation.\n"; break;
            }
            util::log_error("{}", message);
            throw std::runtime_error{ message };
        }
    }

    void FrameBuffer::delete_resources() noexcept
    {
        // TODO GL::DeleteFramebuffers - https://docs.gl/es3/glDeleteFramebuffers
        frameBufferHandle = 0;
    }
}
