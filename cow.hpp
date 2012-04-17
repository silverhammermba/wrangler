#ifndef COW_H_
#define COW_H_

#include <vector>
#include <SFML/Graphics.hpp>

class Cow
{
	sf::RectangleShape body;
	sf::RectangleShape head;
	sf::RectangleShape dbg_vel;
	sf::RectangleShape dbg_dir;
	sf::RectangleShape dbg_str;
	float hd; // head direction
	float speed;
	void setHead(float, float);
	sf::Vector2f velocity;
	sf::Vector2f steering_direction;
	std::vector<const Cow *> neighbors;
public:
	static const float D_THRESHOLD;
	static const float BLENGTH;
	static const float BWIDTH;
	static const float HLENGTH;
	static const float HWIDTH;
	static const float HTURN_SPEED;
	static const float MAX_SPEED;
	static const float MASS;
	static const float MAX_FORWARD_FORCE;
	static const float MAX_LATERAL_FORCE;
	static const float MAX_REVERSE_FORCE;
	bool debug;
	Cow(const sf::Vector2f & position = sf::Vector2f(0, 0), const float direction = 0);
	const sf::Vector2f & pos() const { return body.getPosition(); }
	void setPos(const sf::Vector2f &);
	void setDir(const float);
	void step(float time = 1);
	void addCow(const Cow *); // add a nearby cow
	void resetN(); // reset the cow's neighbor vector
	void think(const Cow &);
	void draw(sf::RenderWindow &) const;
};

#endif
