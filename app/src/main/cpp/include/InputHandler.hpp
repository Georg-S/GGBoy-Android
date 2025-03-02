#pragma once
#include <Input.hpp>
#include <Emulator.hpp>
#include <unordered_map>


class InputHandler : public ggb::Input
{
public:
	InputHandler();
	~InputHandler();
	bool isAPressed() override;
	bool isBPressed() override;
	bool isStartPressed() override;
	bool isSelectPressed() override;
	bool isUpPressed() override;
	bool isDownPressed() override;
	bool isLeftPressed() override;
	bool isRightPressed() override;
//	void update();

private:
};