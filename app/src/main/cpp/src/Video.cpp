#include "Video.hpp"

AndroidRenderer::AndroidRenderer(int width, int height)
	: m_width(width), m_height(height)
{
}

void AndroidRenderer::renderNewFrame(const ggb::FrameBuffer& framebuffer)
{
    m_image = framebuffer.getRawData();
	m_hasNewImage = true;
}

bool AndroidRenderer::hasNewImage() const
{
	return m_hasNewImage;
}
