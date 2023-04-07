#pragma once
#include "Graphics.h"
#include "Main.h"
#include <unordered_map>
#include <string>
struct point { int32_t x, y; };
//simple hash for 2d vector, combines 2 32int into a 64int as the has key
struct vecHash{
	int64_t operator()(const point& p) const
	{
		//need to cast x into int64 first or we loose bits
		int64_t val = (int64_t)p.x << 32 | p.y;
		return val;
	}
};
struct vecEqual
{
	bool operator()(const point& p1, const point& p2)const
	{
		return p1.x == p2.x && p1.y == p2.y;
	}
};
class Game 
{
	std::unique_ptr<Graphics>	_graphics;
	std::vector<DirectX::SimpleMath::Vector2>	activePixels;	
	std::unordered_map<point, uint32_t, vecHash, vecEqual>	alive;
	std::unordered_map<point, uint32_t, vecHash, vecEqual>	cellNeighbour;
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
	void addLive(int x, int y);
	void addCellNeighbours(point p);
	int neighboutCount(point p);

	int circle_x = 0;
	std::unique_ptr<StepTimer> _timer;
	std::unique_ptr<DirectX::Keyboard> _keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;
	DirectX::Mouse::ButtonStateTracker _mTraker;
	DirectX::Keyboard::KeyboardStateTracker _kTraker;


	std::queue<DirectX::SimpleMath::Vector2> _renderQueue;
	bool curBuffer = 0;
	bool paused = true;
	int _zoom = 1;
	int _scrollTemp = 0;
	int _zoomFactor = 120;
	int _zoomRatioX = 0;
	int _zoomRatioY = 0;
	float _pixelScale = 0;
	DirectX::SimpleMath::Vector2 _cameraCoord = DirectX::SimpleMath::Vector2(0, 0);
	int speed = 5;
};