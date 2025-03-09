#include "Video.hpp"

AndroidRenderer::AndroidRenderer(int width, int height)
	: m_width(width), m_height(height)
{
}

void AndroidRenderer::renderNewFrame(const ggb::FrameBuffer& framebuffer)
{
    std::scoped_lock lock(m_mutex);
    m_hasNewImage = true;
    m_image = framebuffer.getRawData();
}

bool AndroidRenderer::hasNewImage() const
{
    std::scoped_lock lock(m_mutex);
	return m_hasNewImage;
}

std::vector<ggb::RGB> AndroidRenderer::getCurrentFrame()
{
    std::scoped_lock lock(m_mutex);
    return std::move(m_image);
}
