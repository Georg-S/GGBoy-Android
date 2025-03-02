#pragma once
#include <RenderingUtility.hpp>

class AndroidRenderer : public ggb::Renderer
{
public:
    AndroidRenderer(int width, int height);
	void renderNewFrame(const ggb::FrameBuffer& framebuffer) override;
	bool hasNewImage() const;

private:
    std::vector<ggb::RGB> m_image;
	bool m_hasNewImage = false;
	int m_width;
	int m_height;
};
