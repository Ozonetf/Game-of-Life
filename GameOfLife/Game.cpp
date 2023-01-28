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
	circle_x = 0;
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
	int left, right, top, bottom;
	int height = _graphics->GetWinHeight();
	int width = _graphics->GetWinWidth();
	left = _cameraCoord.x - (width / 2);
	right = _cameraCoord.x + (width / 2);
	top = _cameraCoord.y - (height / 2);
	bottom = _cameraCoord.y + (height / 2);
	for (size_t i = 0; i < activePixels.size(); i++)
	{
		if (activePixels[i].x >left && activePixels[i].x<right)
		{
			if (activePixels[i].y >top && activePixels[i].y<bottom)
			{
				DirectX::SimpleMath::Vector2 absV;
				absV.x = activePixels[i].x - (left);
				absV.y = activePixels[i].y - (top);
				_renderQueue.push(absV);
			}
		}
	}
}

void Game::Render()
{
	_graphics->BeginDraw();
	_graphics->ClearScreen(0.0f, 0.0f, 0.0f);
	while (!_renderQueue.empty())
	{
		_graphics->FillRect(_renderQueue.front(), _zoom);
		_renderQueue.pop();
	}
	//for (size_t i = 0; i < activePixels.size(); i++)
	//{
	//	_graphics->FillRect(activePixels[i]);

	//}
	_graphics->DrawCircle(circle_x, _graphics->GetWinHeight()/2, 30, 1, 1, 1, 1);
	_graphics->EndDraw();
}

void Game::ProcessInputs()
{
	using ButtonState = DirectX::Mouse::ButtonStateTracker::ButtonState;
	using namespace DirectX::SimpleMath;
	auto kb = _keyboard->GetState();
	if (kb.A)
	{
		speed = -speed;
	}
	if (kb.B)
	{
		std::srand(time(NULL));
		activePixels.clear();
		for (size_t i = 0; i < 50000; i++)
		{
			activePixels.push_back(Vector2(std::rand() % _graphics->GetWinWidth(), std::rand() % _graphics->GetWinHeight()));
		}
	}
	if (kb.C)
	{
		activePixels.clear();
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
	auto mouse = m_mouse->GetState();
	_traker.Update(mouse);
	if (_traker.middleButton == ButtonState::PRESSED)
	{
		m_mouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	}
	else if (_traker.middleButton == ButtonState::RELEASED)
	{
		m_mouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
	}
	else if (_traker.middleButton == ButtonState::HELD)
	{
		if (mouse.positionMode == DirectX::Mouse::MODE_RELATIVE)
		{
			_cameraCoord.x -= mouse.x;
			_cameraCoord.y -= mouse.y;
		}

	}
	if (mouse.leftButton)
	{
		int left, top;
		int height = _graphics->GetWinHeight();
		int width = _graphics->GetWinWidth();
		left = _cameraCoord.x - (width / 2);
		top = _cameraCoord.y - (height / 2);
		activePixels.push_back(Vector2(mouse.x + left, mouse.y + top));
		//PRINT_DEBUG("mouse x: %d, y: %d\n", mouse.x, mouse.y);
	}
	//one "click" on MWheel typically 120 in value
	_zoom = mouse.scrollWheelValue / 120;
	if (_zoom < 0) _zoom = 0;

}

void Game::OnWindowSizeChanged(long width, long height)
{
	_graphics->Resize(width, height);
	//keep camera centered when resizing window
	_cameraCoord.x = width / 2;
	_cameraCoord.y = height / 2;
}

void Game::OnResize()
{
	//_graphics->Resize();
}
