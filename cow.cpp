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
	// TODO kind of stupid and redundant
	head.rotate(clamp<float>(-60, fmodp(d - body.getRotation(), 360), 60));
	// TODO get smooth rotation working too
	//head.rotate(clamp<float>(-HTURN_SPEED, fmodp(d - body.getRotation(), 360), HTURN_SPEED) * time);
}

Cow::Cow(const sf::Vector2f & pos, const float dir) : body(sf::Vector2f(BLENGTH, BWIDTH)), head(sf::Vector2f(HLENGTH, HWIDTH))
{
	d = dir;
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

void Cow::addNhb(const Cow * cow)
{
	neighbors.push_back(cow);
}

void Cow::addCrd(const Cow * cow)
{
	crowders.push_back(cow);
}

void Cow::addCll(const Cow * cow)
{
	colliders.push_back(cow);
}

void Cow::resetN()
{
	neighbors.clear();
	crowders.clear();
	colliders.clear();
}

void Cow::think()
{
	using namespace std;
	if (colliders.size() > 0)
	{
		sf::Vector2f repl (0, 0);
		if (colliders.size() > 0)
		{
			for (const Cow * c : colliders)
			{
				repl += c->pos();
			}
			repl /= (float)colliders.size();
			repl -= body.getPosition();
			repl *= -1.f;
		}
		d = atan2(repl.y, repl.x) * 180.f / M_PI;
		speed = SPEED; //clamp<float>(0.f, v2dist(repl, sf::Vector2f(0, 0)), SPEED);
	}
	else if (neighbors.size() > 0 || crowders.size() > 0)
	{
		sf::Vector2f attr (0, 0);
		if (neighbors.size() > 0)
		{
			for (const Cow * c : neighbors)
			{
				attr += c->pos();
			}
			attr /= (float)neighbors.size();
			attr -= body.getPosition();
		}
		sf::Vector2f repl (0, 0);
		if (crowders.size() > 0)
		{
			//cerr << "Cow " << this << "crowded\n";
			for (const Cow * c : crowders)
			{
				//cerr << "  " << c << endl;
				repl += c->pos();
			}
			repl /= (float)crowders.size();
			repl -= body.getPosition();
			repl *= -2.f;
			//cerr << "going " << (atan2(-1.f * repl.y, -1.f * -repl.x) * 180.f / M_PI) << endl;
		}
		d = atan2(attr.y + repl.y, attr.x + repl.x) * 180.f / M_PI;
		speed = clamp<float>(0.f, v2dist(attr + repl, sf::Vector2f(0, 0)), SPEED);
	}
	else
	{
		d += randm<float>(120.f) - 60.f;
		speed = randm<float>(SPEED);
	}
}

void Cow::draw(sf::RenderWindow & window) const
{
	window.draw(body);
	window.draw(head);
}
