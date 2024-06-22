#define _CRT_DISABLE_PERFCRIT_LOCKS
#include <cstdio>
#include <Windows.h>
#include <ctime>
#include <vector>

const unsigned short DELAY = 250;

const struct C
{
	static const unsigned short UP = 87, RIGHT = 68, DOWN = 83, LEFT = 65;
};

class IMovableObject
{
public:
	virtual void display() = 0;
};

class Snake;

class Map
{
public:

	static void init();

	static void update();
	static void addObject(IMovableObject *pObject);
private:
	class Apple : public IMovableObject
	{
	public:
		Apple();
		static unsigned short x, y;
		const static char model = 'A';
		void display() override;
		static void createNew();
	};

	static const unsigned short SIZE = 20;
	const static char mWall = '#', mSpace = '.';
	static char mMap[SIZE][SIZE];
	static bool mIsRunning;
	static std::vector<IMovableObject*> mMovableObjects;

	friend class Snake;
};

bool Map::mIsRunning = true;
char Map::mMap[SIZE][SIZE] = { ' ' };
std::vector<IMovableObject*> Map::mMovableObjects = std::vector<IMovableObject*>();

unsigned short Map::Apple::x = 0, Map::Apple::y = 0;

Map::Apple::Apple()
{
	x = rand() % 18 + 1;
	y = rand() % 18 + 1;
	Map::addObject(this);
}

void Map::Apple::createNew()
{
	x = rand() % 18 + 1;
	y = rand() % 18 + 1;
}

void Map::Apple::display()
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)x,(short)y });
	putchar(model);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0,(short)Map::SIZE });
}

class Snake : private IMovableObject
{
public:
	Snake(unsigned short length = 4);
	void controller();
	void display() override;
private:
	class Node
	{
	public:
		Node(Node* next = nullptr);
		Node(unsigned short xCoord, unsigned short yCoord, char model = 'o');
		unsigned short mX, mY;
		char model;
		Node* pNext;
		Node& at(unsigned short index);
	};

	Node* mpHead;
	unsigned short mLength;
	unsigned short mCurrentDirection = C::UP;

	void push_back(Node* newElement);
	void move();
	bool isCollisionFound();
};

void Map::init()
{
	Apple apple;
	Map::mIsRunning = true;

	for (unsigned short y = 0; y < SIZE; ++y)
	{
		for (unsigned short x = 0; x < SIZE; ++x)
		{
			if (x == 0 || y == 0 || x == SIZE - 1 || y == SIZE - 1)
			{
				Map::mMap[y][x] = mWall;
			}
			else
			{
				Map::mMap[y][x] += mSpace;
			}
			putchar(Map::mMap[y][x]);
		}
		putchar('\n');
	}
}

Snake::Snake(unsigned short length) : mLength(length)
{
	if (mLength < 0 || mLength > Map::SIZE * Map::SIZE)
		mLength = 4;
	push_back(new Node(rand() % 18 + 1, rand() % 15 + 1, '@'));
	for (unsigned short i = 0; i < mLength - 1; ++i)
	{
		push_back(new Node(mpHead->at(i).mX, mpHead->at(i).mY + 1));
	}
}

bool Snake::isCollisionFound()
{
	if (mLength < 5) return false;
	for (unsigned short i = 1; i < mLength; ++i)
	{
		if (mpHead->mX == mpHead->at(i).mX && mpHead->mY == mpHead->at(i).mY) return true;
	}
	return false;
}

void Snake::display()
{
	for (unsigned short i = 0; i < mLength; ++i)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)mpHead->at(i).mX,(short)mpHead->at(i).mY });
		putchar(mpHead->at(i).model);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0,(short)Map::SIZE });
	}
}

Snake::Node::Node(Node* next) : pNext(next)
{
	mX = mY = 0;
	model = 'o';
}

Snake::Node::Node(unsigned short xCoord, unsigned short yCoord, char model) : pNext(nullptr)
{
	mX = xCoord;
	mY = yCoord;
	this->model = model;
}

Snake::Node& Snake::Node::at(unsigned short index)
{
	Node* pTemp = this;
	for (unsigned short i = 0; i < index; ++i)
	{
		pTemp = pTemp->pNext;
	}
	return *pTemp;
}

void Snake::push_back(Node* newElement)
{
	if (mpHead == nullptr)
	{
		mpHead = newElement;
	}
	else
	{
		Node* temp = mpHead;
		while (temp->pNext != nullptr)
		{
			temp = temp->pNext;
		}
		temp->pNext = newElement;
	}
}

void Snake::move()
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)mpHead->at(this->mLength - 1).mX,(short)mpHead->at(this->mLength - 1).mY });
	if (mpHead->at(this->mLength - 1).mX == Map::Apple::x && mpHead->at(this->mLength - 1).mY == Map::Apple::y)
	{
		putchar(Map::Apple::model);
	}
	else
	{
		putchar(Map::mSpace);
	}
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0,(short)Map::SIZE });
	for (unsigned short i = mLength - 1; i > 0; --i)
	{
		mpHead->at(i).mX = mpHead->at(i - 1).mX;
		mpHead->at(i).mY = mpHead->at(i - 1).mY;
	}
	switch (mCurrentDirection)
	{
	case C::UP:
		mpHead->mY -= 1;
		return;
	case C::RIGHT:
		mpHead->mX += 1;
		return;
	case C::DOWN:
		mpHead->mY += 1;
		return;
	case C::LEFT:
		mpHead->mX -= 1;
		return;
	}
}

void Map::update()
{
	for (unsigned short i = 0; i < mMovableObjects.size(); ++i)
	{
		mMovableObjects[i]->display();
	}
}

void Map::addObject(IMovableObject *pObject)
{
	mMovableObjects.push_back(pObject);
}

void Snake::controller()
{
	while (true)
	{
		if (GetAsyncKeyState(C::UP) & 0x8000 || GetAsyncKeyState(C::RIGHT) & 0x8000 || GetAsyncKeyState(C::LEFT) & 0x8000)
		{
			break;
		}
	}

	unsigned short buffX, buffY;

	while (Map::mIsRunning)
	{
		if (GetAsyncKeyState(C::UP) & 0x8000 && this->mCurrentDirection != C::DOWN)
		{
			mCurrentDirection = C::UP;
		}
		else if (GetAsyncKeyState(C::RIGHT) & 0x8000 && this->mCurrentDirection != C::LEFT)
		{
			mCurrentDirection = C::RIGHT;
		}
		else if (GetAsyncKeyState(C::DOWN) & 0x8000 && this->mCurrentDirection != C::UP)
		{
			mCurrentDirection = C::DOWN;
		}
		else if (GetAsyncKeyState(C::LEFT) & 0x8000 && this->mCurrentDirection != C::RIGHT)
		{
			mCurrentDirection = C::LEFT;
		}

		buffX = this->mpHead->at(this->mLength - 1).mX;
		buffY = this->mpHead->at(this->mLength - 1).mY;

		move();
		Map::update();
		
		if (Snake::isCollisionFound())
		{
			Map::mIsRunning = false;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)this->mpHead->mX,(short)this->mpHead->mY });
			putchar(Map::mWall);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0,(short)Map::SIZE });
			printf("\nDEFEAT\nOops! You've hit a wall!\n\n");
		}
		else if (Map::mMap[mpHead->mY][mpHead->mX] == Map::mWall)
		{
			Map::mIsRunning = false;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)this->mpHead->mX,(short)this->mpHead->mY });
			putchar(Map::mWall);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0,(short)Map::SIZE });
			printf("\nDEFEAT\nOops! You've hit a wall!\n\n");
		}
		else if (this->mpHead->mX == Map::Apple::x && this->mpHead->mY == Map::Apple::y)
		{
			Map::Apple::createNew();
			this->push_back(new Node(buffX, buffY));
			++this->mLength;
		}
		
		Sleep(DELAY);
	}
}


int main()
{
	srand(std::time(0));

	Map::init();
	Snake pPlayer;
	Map::addObject((IMovableObject*)&pPlayer);
	pPlayer.display();
	pPlayer.controller();

	return 0;
}