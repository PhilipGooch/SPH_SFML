#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Input
{
public:

	Input();

	bool getKeyDown(int);
	void setKeyDown(int);
	void setKeyUp(int);
	
	inline Vector2i getMousePosition() { return m_mousePosition; }
	inline bool getMouseLeftDown() { return m_mouseLeftDown; }
	inline void setMousePosition(int x, int y) { m_mousePosition.x = x; m_mousePosition.y = y; }
	inline void setMouseLeftDown(bool mouseLeftDown) { m_mouseLeftDown = mouseLeftDown; }

private:
	

	Vector2i m_mousePosition;

	bool m_mouseLeftDown;

	bool m_keys[256]{ false };

};


