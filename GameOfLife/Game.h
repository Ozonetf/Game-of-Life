#pragma once
#include "Graphics.h"
class Game 
{
	std::unique_ptr<Graphics>	_graphics;
public:
	Game();
	~Game();

	void Init(HWND windowhandle, int width, int height);

	void Tick();
	void Render();

	void OnWindowSizeChanged(long width, long height);
	void OnResize();
private:
	void Update();

	int circle_x = 0;
	std::unique_ptr<StepTimer> _timer;
	std::unique_ptr<DirectX::Keyboard> _keyboard;
	//gol::StepTimer timer;
	int speed = 5;
};