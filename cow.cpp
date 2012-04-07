// TODO debug
#include <iostream>

#include <cmath>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include "cow.hpp"
#include "helpers.hpp"

#define _USE_MATH_DEFINES

const float Cow::BLENGTH = 14;
const float Cow::BWIDTH = 7;
const float Cow::HLENGTH = 6;
const float Cow::HWIDTH = 4;
const float Cow::SPEED = 20;
const float Cow::TURN_SPEED = 72;

// set the position of the head, when the cow is facing dir
void Cow::setHead(float dir)
{
	head.setPosition(body.getTransform() * sf::Vector2f(BLENGTH, BWIDTH / 2.f));
	head.setRotation(dir);
	// TODO kind of stupid and redundant
	head.rotate(clamp<float>(-60, fmodp(d - body.getRotation(), 360), 60));
}

Cow::Cow(const sf::Vector2f & pos, const float dir) : body(sf::Vector2f(BLENGTH, BWIDTH)), head(sf::Vector2f(HLENGTH, HWIDTH))
{
	d = dir;
	body.setOrigin(BLENGTH / 2.f, BWIDTH / 2.f);
	body.setPosition(pos);
	body.setRotation(dir);

	head.setOrigin(1.f, HWIDTH / 2.f);
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
	dir = body.getRotation(); // where we ended up facing
	body.move(SPEED * time * cos(dir * M_PI / 180), SPEED * time * sin(dir * M_PI / 180));
	setHead(dir); // update the head
}

void Cow::think() // TODO
{
	using namespace std;
	d = randm<float>(360.f) - 180.f;
}

void Cow::draw(sf::RenderWindow & window)
{
	window.draw(body);
	window.draw(head);
}
