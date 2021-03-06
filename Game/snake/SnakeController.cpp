#include "SnakeController.hpp"
#include <stdlib.h>


Snake::Snake() : GameBase(640, 480)
{
	if (!m_squareTexture.loadFromFile("resources/square_shiny.png"))
	{
		stop();
	}

	if (!m_pickupSound.loadFromFile("resources/pickup.wav"))
	{
		stop();
	}

	if (!m_moveSound.loadFromFile("resources/move.wav"))
	{
		stop();
	}

	for(int y = 0; y < 28; ++y)
	{
		for(int x = 0; x < 38; ++x)
		{
			m_cellSprite.push_back(sf::Sprite(m_squareTexture));
			m_cellSprite[x+(y*38)].setPosition((x*16) + 16,(y*16) + 16);
			m_cellState.push_back(CellStatus());
		}
	}

	reset();
}


Snake::~Snake()
{

}


void Snake::init()
{

}


void Snake::onTick()
{
	if(m_timingTimer.getTicks() - m_lastUpdateTime < 200)
	{
		return;
	}
	else
	{
		m_lastUpdateTime = m_timingTimer.getTicks();
	}

	bool m_grow = false;
	unsigned int cellPointer = m_snakeHeadPositionId;
	unsigned int lastCellPointer;

	switch(m_snakeDirection)
	{
		case LeftDirection:
			if(m_cellState[cellPointer - 1].isSnake || ((cellPointer) % 38) == 0 )
			{
				reset();

				return;
			}
			else
			{
				m_snakeHeadPositionId = cellPointer - 1;

				if(m_cellState[m_snakeHeadPositionId].isFood)
				{
					m_grow = true;
				}

				m_cellState[m_snakeHeadPositionId].isFood = false;
				m_cellState[m_snakeHeadPositionId].isSnake = true;
				m_cellState[m_snakeHeadPositionId].nextNodeDirection = RightDirection;
			}
			break;

		case TopDirection:
			if(m_cellState[cellPointer - 38].isSnake || cellPointer < 28)
			{
				reset();

				return;
			}
			else
			{
				m_snakeHeadPositionId = cellPointer - 38;

				if(m_cellState[m_snakeHeadPositionId].isFood)
				{
					m_grow = true;
				}

				m_cellState[m_snakeHeadPositionId].isFood = false;
				m_cellState[m_snakeHeadPositionId].isSnake = true;
				m_cellState[m_snakeHeadPositionId].nextNodeDirection = BottomDirection;
			}
			break;

		case RightDirection:
			if(m_cellState[cellPointer + 1].isSnake || ((cellPointer + 1) % 38) == 0)
			{
				reset();

				return;
			}
			else
			{
				m_snakeHeadPositionId = cellPointer + 1;

				if(m_cellState[m_snakeHeadPositionId].isFood)
				{
					m_grow = true;
				}

				m_cellState[m_snakeHeadPositionId].isFood = false;
				m_cellState[m_snakeHeadPositionId].isSnake = true;
				m_cellState[m_snakeHeadPositionId].nextNodeDirection = LeftDirection;
			}
			break;

		case BottomDirection:
			if(m_cellState[cellPointer + 38].isSnake || cellPointer >= (27*38))
			{
				reset();

				return;
			}
			else
			{
				m_snakeHeadPositionId = cellPointer + 38;

				if(m_cellState[m_snakeHeadPositionId].isFood)
				{
					m_grow = true;
				}

				m_cellState[m_snakeHeadPositionId].isFood = false;
				m_cellState[m_snakeHeadPositionId].isSnake = true;
				m_cellState[m_snakeHeadPositionId].nextNodeDirection = TopDirection;
			}
			break;
	}

	while(m_cellState[cellPointer].isSnake)
	{
		lastCellPointer = cellPointer;

		switch(m_cellState[cellPointer].nextNodeDirection)
		{
			case LeftDirection:
				cellPointer -= 1;
				break;

			case TopDirection:
				cellPointer -= 38;
				break;

			case RightDirection:
				cellPointer += 1;
				break;

			case BottomDirection:
				cellPointer += 38;
				break;
		}
	}

	if(m_grow)
	{
		m_sound.setBuffer(m_pickupSound);
		createNewFood();
	}
	else
	{
		m_cellState[lastCellPointer].isSnake = false;
		m_sound.setBuffer(m_moveSound);
	}

	m_sound.play();
}


void Snake::onRender()
{
	m_video->clear(sf::Color::White);

	m_video->drawRectangle(sf::Vector2f(0, 0),sf::Vector2f(m_screenSize.x, 12), sf::Color::Black);
	m_video->drawRectangle(sf::Vector2f(0, m_screenSize.y - 12),sf::Vector2f(m_screenSize.x, 12), sf::Color::Black);
	m_video->drawRectangle(sf::Vector2f(0, 0),sf::Vector2f(12, m_screenSize.y), sf::Color::Black);
	m_video->drawRectangle(sf::Vector2f(m_screenSize.x - 12, 0),sf::Vector2f(12, m_screenSize.y), sf::Color::Black);

	for(int y = 0; y < 28; ++y)
	{
		for(int x = 0; x < 38; ++x)
		{
			if(m_cellState[x+(y*38)].isSnake || m_cellState[x+(y*38)].isFood)
			{
				m_cellSprite[x+(y*38)].setTextureRect(sf::IntRect(16, 0, 16, 16));
			}
			else
			{
				m_cellSprite[x+(y*38)].setTextureRect(sf::IntRect(0, 0, 16, 16));
			}

			m_video->draw(m_cellSprite[x+(y*38)]);
		}
	}

	m_video->swapBuffers();
}


void Snake::onEvent(const int eventType, const int param1, const int param2)
{
	m_eventHandler.trigger(eventType);

	switch (eventType)
	{
		case EVENT_KEYDOWN:
			switch (param1)
			{
				case KEY::Up:
					m_snakeDirection = TopDirection;
					break;

				case KEY::Right:
					m_snakeDirection = RightDirection;
					break;

				case KEY::Down:
					m_snakeDirection = BottomDirection;
					break;

				case KEY::Left:
					m_snakeDirection = LeftDirection;
					break;

				case KEY::Escape:
					stop();
					break;

				case KEY::F1:
					reset();
					break;
			}
			break;

		case EVENT_QUIT:
			stop();

			break;
	}
}


void Snake::reset()
{
	m_screenSize = sf::Vector2i(640, 480);

	for(int y = 0; y < 28; ++y)
	{
		for(int x = 0; x < 38; ++x)
		{
			m_cellState[x+(y*38)].nextNodeDirection = LeftDirection;
			m_cellState[x+(y*38)].isSnake = false;
			m_cellState[x+(y*38)].isFood = false;
		}
	}

	for(int x = 0; x < 4; ++x)
	{
		m_cellState[5+(12*38)+x].isSnake = true;
	}

	m_snakeHeadPositionId = 5+(12*38)+3;
	m_snakeDirection = RightDirection;

	m_timingTimer.start();
	m_lastUpdateTime = m_timingTimer.getTicks();

	createNewFood();
}


void Snake::end()
{

}


void Snake::createNewFood()
{
	bool done = false;

	while(!done)
	{
		unsigned int random = rand() % (38*28);

		if(!m_cellState[random].isSnake)
		{
			m_cellState[random].isFood = true;
			done = true;
		}
	}
}
