#pragma once
#include <RenderingUtility.hpp>
#include <mutex>

class AndroidRenderer : public ggb::Renderer
{
public:
    AndroidRenderer(int width, int height);
	void renderNewFrame(const ggb::FrameBuffer& framebuffer) override;
	bool hasNewImage() const;
    std::vector<ggb::RGB> getCurrentFrame();

private:
    std::vector<ggb::RGB> m_image;
	bool m_hasNewImage = false;
    mutable std::mutex m_mutex;
    int m_width;
    int m_height;
};
