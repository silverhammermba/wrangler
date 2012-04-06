#include <iostream>
#include <cmath>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include "cow.hpp"
#include "helpers.hpp"

#define _USE_MATH_DEFINES

const float Cow::SPEED = 10;
const float Cow::TURN_SPEED = 72;

// set the position of the head, when the cow is facing dir
void Cow::setHead(float dir)
{
	head.setPosition(body.getTransform() * sf::Vector2f(LENGTH - 1.f, WIDTH / 2.f + 1.f)); // TODO why the + 1.f?
	head.setRotation(dir);
	// TODO kind of stupid and redundant
	head.rotate(clamp<float>(-60, fmodp(d - body.getRotation(), 360), 60));
}

Cow::Cow(const sf::Vector2f & pos, const float dir) : body(sf::Vector2f(LENGTH, WIDTH)), head(sf::Vector2f(LENGTH * 3.f / 5.f, WIDTH - 4.f))
{
	d = dir;
	body.setOrigin(LENGTH / 2.f, WIDTH / 2.f);
	body.setPosition(pos);
	body.setRotation(dir);

	head.setOrigin(2.f, (WIDTH - 2.f) / 2.f);
	setHead(dir);

	body.setFillColor(sf::Color(237, 224, 177));
	body.setOutlineColor(sf::Color(0, 0, 0));
	body.setOutlineThickness(1);

	head.setFillColor(sf::Color(237, 224, 177));
	head.setOutlineColor(sf::Color(0, 0, 0));
	head.setOutlineThickness(1);
}

void Cow::setPos(const sf::Vector2f & pos)
{
	body.setPosition(pos);
}

void Cow::setDir(const float dir)
{
	d = dir;
}

void Cow::step(float time)
{
	// convert the direction to [-180, 180] relative to d
	float dir = fmodp(d - body.getRotation(), 360);
	body.rotate(clamp<float>(-TURN_SPEED, dir, TURN_SPEED) * time);
	dir = body.getRotation();
	body.move(SPEED * time * cos(dir * M_PI / 180), SPEED * time * sin(dir * M_PI / 180));
	setHead(dir);
}

void Cow::think()
{
	using namespace std;
	d = randm<float>(360.f) - 180.f;
	cerr << "Current direction: " << fmodp(body.getRotation(), 360) << endl
	     << "New direction: " << d << " = " << fmodp(d - body.getRotation(), 360)
		 << " local\n";
}

void Cow::draw(sf::RenderWindow & window)
{
	window.draw(body);
	window.draw(head);
}
