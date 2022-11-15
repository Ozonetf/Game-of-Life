#pragma once
#include "Graphics.h"
#include "Main.h"
class Game 
{
	std::unique_ptr<Graphics>	_graphics;
	std::vector<DirectX::SimpleMath::Vector2>	activePixels;
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
	void ProcessInputs();

	int circle_x = 0;
	std::unique_ptr<StepTimer> _timer;
	std::unique_ptr<DirectX::Keyboard> _keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;

	std::queue<DirectX::SimpleMath::Vector2> _renderQueue;
	int _zoom = 1;
	DirectX::SimpleMath::Vector2 _cameraCoord = DirectX::SimpleMath::Vector2(0, 0);
	int speed = 5;
};