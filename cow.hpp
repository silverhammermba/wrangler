#ifndef COW_H_
#define COW_H_

#include <vector>
#include <SFML/Graphics.hpp>

class Cow
{
	sf::RectangleShape body;
	sf::RectangleShape head;
	float d;
	void setHead(float);
	std::vector<const Cow *> neighbors;
public:
	static const float D_THRESHOLD;
	static const float BLENGTH;
	static const float BWIDTH;
	static const float HLENGTH;
	static const float HWIDTH;
	static const float SPEED;
	static const float TURN_SPEED;
	Cow(const sf::Vector2f & position = sf::Vector2f(0, 0), const float direction = 0);
	const sf::Vector2f & pos() const { return body.getPosition(); }
	const float dir() const { return d; }
	void setPos(const sf::Vector2f &);
	void setDir(const float);
	void step(float time = 1);
	void addCow(const Cow *); // add a nearby cow
	void resetN(); // reset the cow's neighbor vector
	void think();
	void draw(sf::RenderWindow &) const;
};

#endif
