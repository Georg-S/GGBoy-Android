#include "Video.hpp"

AndroidRenderer::AndroidRenderer(int width, int height)
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
    return m_image;
}

bool AndroidRenderer::writeLastImage(const std::filesystem::path &path)
{
    auto image = getCurrentFrame();

    if (m_image.empty())
        return false;

    std::ofstream outFile(path, std::ios::binary);
    if (!outFile)
        return false;

    outFile.write(reinterpret_cast<const char*>(image.data()), image.size() * sizeof(decltype(image)::value_type));
    return true;
}
