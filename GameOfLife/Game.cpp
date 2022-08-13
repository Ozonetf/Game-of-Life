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
	circle_x = 0;
}

void Game::Tick()
{
	_timer->Tick([&]()
	{
		Update();
	});
	Render();
}

void Game::Update()
{
	auto kb = _keyboard->GetState();
	if (kb.A)
	{
		speed = -speed;
	}
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
	auto mouse = m_mouse->GetState();
}

void Game::Render()
{
	_graphics->BeginDraw();
	_graphics->ClearScreen(0.0f, 0.0f, 0.0f);
	//for (size_t i = 0; i < 1000; i++)
	//{
	//	_graphics->DrawCircle(rand() % 800, rand() % 600, rand() % 300, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f);
	//}
	_graphics->DrawCircle(circle_x, _graphics->GetWinHeight()/2, 30, 1, 1, 1, 1);
	_graphics->EndDraw();
}

void Game::OnWindowSizeChanged(long width, long height)
{
	_graphics->Resize(width, height);
}

void Game::OnResize()
{
	//_graphics->Resize();
}
