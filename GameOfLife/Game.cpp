#include "pch.h"
#include "Game.h"
#include <exception>

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND windowhandle, int width, int height)
{
	_graphics = std::make_unique<Graphics>();
	_timer = std::make_unique<StepTimer>();
	_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(windowhandle);

	//sets the window handle as ther rendertarget for d2d
	if (!_graphics->init(windowhandle))
	{
		throw 20;
	};
	_cameraCoord.x = width / 2;
	_cameraCoord.y = height / 2;	
	_zoomRatioX = width / 48;
	_zoomRatioY = height / 48;
	circle_x = 0;
	alive.emplace(point{ 1, 0 }, 0);
	alive.emplace(point{ 2, 1 }, 0);
	alive.emplace(point{ 0, 2 }, 0);
	alive.emplace(point{ 1, 2 }, 0);
	alive.emplace(point{ 2, 2 }, 0);
}

void Game::Tick()
{
	_timer->Tick([&]()
	{
		Update();
	});
	//PRINT_DEBUG("frame time: %d\n", _timer->GetFramesPerSecond());
	Render();
}

void Game::Update()
{
	PRINT_DEBUG("size: %i\n", alive.size());

	using namespace DirectX::SimpleMath;
	ProcessInputs();
	if (circle_x>_graphics->GetWinWidth())
	{
		circle_x = 0;
	}
	else if (circle_x < 0)
	{
		circle_x = _graphics->GetWinWidth();
	}
	else
	{
		circle_x += speed;
	}


	//figures out which pixels are in camera's view, get their relative postion and push to render queue
	//cam coord is center of the screen
	int left, right, top, bottom;
	int height = _graphics->GetWinHeight();
	int width = _graphics->GetWinWidth();
	left = _cameraCoord.x - (width / 2) + ((_zoom - 1) * _zoomRatioX);
	right = _cameraCoord.x + (width / 2) - ((_zoom - 1) * _zoomRatioX);
	top = _cameraCoord.y - (height / 2) + ((_zoom - 1) * _zoomRatioY);
	bottom = _cameraCoord.y + (height / 2) - ((_zoom - 1) * _zoomRatioY);
	float xx, yy;
	xx = float(width) / (right - left);
	yy = float(height) / (bottom - top);

	_pixelScale = float(width) / float(right - left);
	for (auto iter = alive.begin(); iter!= alive.end(); ++iter)
	{
		auto cur = iter->first;
		if (cur.x > left &&
			cur.x < right)
		{
			if (cur.y >= top &&
				cur.y < bottom)
			{
				DirectX::SimpleMath::Vector2 absV;

				absV.x = (cur.x - left) * xx;
				absV.y = (cur.y - top) * xx;
				_renderQueue.push(absV);
			}
		}
	}
	//for (size_t i = 0; i < activePixels.size(); i++)
	//{
	//	if (activePixels[i].x > left && 
	//		activePixels[i].x <	right)
	//	{
	//		if (activePixels[i].y >= top && 
	//			activePixels[i].y <	bottom)
	//		{
	//			DirectX::SimpleMath::Vector2 absV;

	//			absV.x = (activePixels[i].x - left) * xx;
	//			absV.y = (activePixels[i].y - top) * xx;
	//			_renderQueue.push(absV);
	//		}
	//	}
	//}
}

void Game::Render()
{
	_graphics->BeginDraw();
	_graphics->ClearScreen(0.0f, 0.0f, 0.0f);

	//rect's right and bottom are not insclusive in fillrect, workaround to avoid gap
	//in cells when zoomed in
	float zoom = (_pixelScale == 1 ? 1 : _pixelScale+1);
	while (!_renderQueue.empty())
	{
		_graphics->FillRect(_renderQueue.front(), zoom);
		_renderQueue.pop();
	}
	_graphics->DrawCircle(circle_x, _graphics->GetWinHeight()/2, 30, 1, 1, 1, 1);
	_graphics->EndDraw();
}

void Game::ProcessInputs()
{
	using ButtonState = DirectX::Mouse::ButtonStateTracker::ButtonState;
	using namespace DirectX::SimpleMath;
	//KEYBOARD
	auto kb = _keyboard->GetState();
	_kTraker.Update(kb);
	if (_kTraker.pressed.A) speed = -speed;
	if (kb.B)
	{
		std::srand(time(NULL));
		activePixels.clear();
		for (size_t i = 0; i < 5000; i++)
		{
			alive.emplace(point{ std::rand() % _graphics->GetWinWidth(), std::rand() % _graphics->GetWinHeight() }, 0);
			//activePixels.push_back(Vector2(std::rand() % _graphics->GetWinWidth(), std::rand() % _graphics->GetWinHeight()));
		}
	}
	if (kb.C)
	{
		alive.clear();
	}
	if (kb.Right)
	{
		_cameraCoord.x += 10;
	}
	if (kb.Left)
	{
		_cameraCoord.x -= 10;
	}
	if (kb.Up)
	{
		_cameraCoord.y -= 10;
	}
	if (kb.Down)
	{
		_cameraCoord.y += 10;
	}	
	if (kb.P)
	{
		for (auto iter = alive.begin(); iter != alive.end(); ++iter)
		{
			//we dont need to check if a neighbour is already an active cell,
			//the cell will stay alive if it has 3 neighbours and will simply
			//be ignored when adding new cells sice there life condition overlaps
			addCellNeighbours(iter->first);
		}
		for (auto iter = cellNeighbour.begin(); iter != cellNeighbour.end();)
		{
			if (neighboutCount(iter->first) != 3) iter = cellNeighbour.erase(iter);
			else ++iter;
		}
		for (auto iter = alive.begin(); iter != alive.end();++iter)
		{
			int i = neighboutCount(iter->first);
			if (i >= 2 && i <= 3)
			{
				iter->second = 1;
			}
			else iter->second = 0;
		}
		for (auto iter = alive.begin(); iter != alive.end();)
		{
			if (iter->second == 0) iter = alive.erase(iter);
			else ++iter;
		}
		alive.merge(cellNeighbour);
		cellNeighbour.clear();
	}
	if (_kTraker.pressed.LeftControl) _zoomFactor = 30; 
	//one "scroll" on MWheel typically 120 in value
	if (_kTraker.released.LeftControl) _zoomFactor = 120;

	//MOUSE
	auto mouse = m_mouse->GetState();
	_mTraker.Update(mouse);
	if (_mTraker.rightButton == ButtonState::PRESSED)
	{
		m_mouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	}
	else if (_mTraker.rightButton == ButtonState::RELEASED)
	{
		m_mouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
	}
	else if (_mTraker.rightButton == ButtonState::HELD)
	{
		if (mouse.positionMode == DirectX::Mouse::MODE_RELATIVE)
		{
			_cameraCoord.x -= mouse.x;
			_cameraCoord.y -= mouse.y;
		}

	}
	if (mouse.leftButton)
	{
		int left, right, top;
		int height = _graphics->GetWinHeight();
		int width = _graphics->GetWinWidth();
		left = _cameraCoord.x - (width / 2) + ((_zoom - 1) * _zoomRatioX);
		right = _cameraCoord.x + (width / 2) - ((_zoom - 1) * _zoomRatioX);
		top = _cameraCoord.y - (height / 2) + ((_zoom - 1) * _zoomRatioY);
		_pixelScale = float(width) / float(right - left);
		float x = left + (mouse.x) / _pixelScale;
		float y = top + (mouse.y) / _pixelScale;
		activePixels.push_back(DirectX::SimpleMath::Vector2(int(x), int(y)));
		addLive(x, y);
		//PRINT_DEBUG("mouse x: %d, y: %d		x: %f y: %f\n", mouse.x, mouse.y, x, y);
	}
	_zoom -= (_scrollTemp - mouse.scrollWheelValue) / _zoomFactor;
	_scrollTemp = mouse.scrollWheelValue;
	if (_zoom < 1) _zoom = 1;
}

void Game::OnWindowSizeChanged(long width, long height)
{
	PRINT_DEBUG("this resized");
	_graphics->Resize(width, height);
	//keep camera centered when resizing window
	_cameraCoord.x = width / 2;
	_cameraCoord.y = height / 2;
	_zoomRatioX = width / 48;
	_zoomRatioY = height / 48;
	PRINT_DEBUG("x ratio: %d, y ratio: %d\n", _zoomRatioX, _zoomRatioY);
}

void Game::OnResize()
{
	//_graphics->Resize();
}

void Game::addLive(int x, int y)
{
	auto b = alive.emplace(point{x, y}, 0);
	if (!b.second)
	{
		PRINT_DEBUG("skipped\n");
	}
	PRINT_DEBUG("size: %i\n", alive.size());
	//point key{x, y};
	//if (!alive.contains(key))
	//{
	//	alive.insert({ key, 0 });
	//	return;
	//}
	return;
}

void Game::addCellNeighbours(point p)
{
	cellNeighbour.emplace(point{p.x-1, p.y-1}, 0);
	cellNeighbour.emplace(point{p.x-1, p.y}, 0);
	cellNeighbour.emplace(point{p.x-1, p.y+1}, 0);
	cellNeighbour.emplace(point{p.x, p.y-1}, 0);
	cellNeighbour.emplace(point{p.x, p.y+1}, 0);
	cellNeighbour.emplace(point{p.x+1, p.y-1}, 0);
	cellNeighbour.emplace(point{p.x+1, p.y}, 0);
	cellNeighbour.emplace(point{p.x+1, p.y+1}, 0);
}

int Game::neighboutCount(point p)
{
	int i = 0;
	i += alive.contains(point{ p.x-1, p.y-1 });
	i += alive.contains(point{ p.x-1, p.y });
	i += alive.contains(point{ p.x-1, p.y+1 });
	i += alive.contains(point{ p.x, p.y-1 });
	i += alive.contains(point{ p.x, p.y+1 });
	i += alive.contains(point{ p.x+1, p.y-1 });
	i += alive.contains(point{ p.x+1, p.y });
	i += alive.contains(point{ p.x+1, p.y+1 });
	return i;
}


