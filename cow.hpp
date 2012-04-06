#ifndef COW_H_
#define COW_H_

#include <SFML/Graphics.hpp>

class Cow
{
	enum {LENGTH = 20, WIDTH = 12};
	sf::RectangleShape body;
	sf::RectangleShape head;
	float d;
	void setHead(float);
public:
	static const float SPEED;
	static const float TURN_SPEED;
	Cow(const sf::Vector2f & position = sf::Vector2f(0, 0), const float direction = 0);
	const sf::Vector2f & pos() const { return body.getPosition(); }
	const float dir() const { return d; }
	void setPos(const sf::Vector2f &);
	void setDir(const float);
	void step(float time = 1);
	void think();
	void draw(sf::RenderWindow &);
};

#endif
