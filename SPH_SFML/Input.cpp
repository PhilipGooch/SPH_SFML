#include "Input.h"

Input::Input() :
	m_mousePosition(Vector2i(0, 0)),
	m_mouseLeftDown(false)
{
}

bool Input::getKeyDown(int key)
{
	return m_keys[key];
}

void Input::setKeyDown(int key)
{
	if (key >= 0)
	{
		m_keys[key] = true;
	}
}

void Input::setKeyUp(int key)
{
	if (key >= 0)
	{
		m_keys[key] = false;
	}
}

