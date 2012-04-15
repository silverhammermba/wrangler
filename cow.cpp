// TODO debug
#include <iostream>
using std::cerr;
using std::endl;

#include <cmath>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include "cow.hpp"
#include "helpers.hpp"

#define _USE_MATH_DEFINES

const float Cow::D_THRESHOLD = 80; // how far away cows look
const float Cow::D_TOOCLOSE = 40; // when a cow is too close
const float Cow::BLENGTH = 14; // body length
const float Cow::BWIDTH = 7; // body width
const float Cow::HLENGTH = 7; // head length
const float Cow::HWIDTH = 4; // head width
const float Cow::SPEED = 20; // movement speed pixels/second
const float Cow::TURN_SPEED = 72; // turn speed degrees/second
const float Cow::HTURN_SPEED = 144; // head turn speed degrees/second

// set the position of the head, when the cow is facing dir
void Cow::setHead(float dir, float time)
{
	head.setPosition(body.getTransform() * sf::Vector2f(BLENGTH, BWIDTH / 2.f));
	head.setRotation(dir);
	hd += clamp<float>(-HTURN_SPEED, fmodp(d - dir, 360.f), HTURN_SPEED) * time;
	head.rotate(clamp<float>(-60, hd, 60));
	hd = fmodp(head.getRotation() - dir, 360.f);

	// TODO kind of stupid and redundant
	//head.rotate(clamp<float>(-60, fmodp(d - body.getRotation(), 360), 60));
	// TODO get smooth rotation working too
	//head.rotate(clamp<float>(-HTURN_SPEED, fmodp(d - body.getRotation(), 360), HTURN_SPEED) * time);
}

Cow::Cow(const sf::Vector2f & pos, const float dir) : body(sf::Vector2f(BLENGTH, BWIDTH)), head(sf::Vector2f(HLENGTH, HWIDTH))
{
	d = dir;
	hd = 0.f;
	speed = 0.f;
	body.setOrigin(BLENGTH / 2.f, BWIDTH / 2.f);
	body.setPosition(pos);
	body.setRotation(randm<float>(360.f));

	head.setOrigin(1.f, HWIDTH / 2.f);
	setHead(dir, 1);

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
	body.move(speed * time * cos(dir * M_PI / 180), speed * time * sin(dir * M_PI / 180));
	setHead(dir, time); // update the head
}

void Cow::addCow(const Cow * cow)
{
	neighbors.push_back(cow);
}

void Cow::resetN()
{
	neighbors.clear();
}

void Cow::think()
{
	d += randm<float>(120.f) - 60.f;
	speed = randm<float>(SPEED);
}

void Cow::draw(sf::RenderWindow & window) const
{
	window.draw(body);
	window.draw(head);
}
