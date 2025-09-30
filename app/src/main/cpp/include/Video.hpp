#pragma once
#include <RenderingUtility.hpp>
#include <mutex>
#include <filesystem>

class AndroidRenderer : public ggb::Renderer
{
public:
    AndroidRenderer(int width, int height);
	void renderNewFrame(const ggb::FrameBuffer& framebuffer) override;
	bool hasNewImage() const;
    std::vector<ggb::RGB> getCurrentFrame();
    bool writeLastImage(const std::filesystem::path& path);

private:
    std::vector<ggb::RGB> m_image;
	bool m_hasNewImage = false;
    mutable std::mutex m_mutex;
};
