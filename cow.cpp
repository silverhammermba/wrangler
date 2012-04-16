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

const float Cow::D_THRESHOLD = 80.f; // how far away cows look
const float Cow::BLENGTH = 14.f; // body length
const float Cow::BWIDTH = 7.f; // body width
const float Cow::HLENGTH = 7.f; // head length
const float Cow::HWIDTH = 4.f; // head width
const float Cow::HTURN_SPEED = 144.f; // head turn speed degrees/second
const float Cow::MASS = 1.f;
const float Cow::MAX_SPEED = 50.f; // movement speed pixels/second
const float Cow::MAX_FORWARD_FORCE = 1.5f;
const float Cow::MAX_LATERAL_FORCE = 1.f;
const float Cow::MAX_REVERSE_FORCE = 0.5f;

// set the position of the head, when the cow is facing dir
void Cow::setHead(float dir, float time)
{
	// TODO refactor
	head.setPosition(body.getTransform() * sf::Vector2f(BLENGTH, BWIDTH / 2.f));
	head.setRotation(dir + hd);
	float d = atan2(steering_direction.y, steering_direction.x) * 180.f / M_PI;
	head.rotate(clamp<float>(-HTURN_SPEED, fmodp(d - dir - hd, 360.f), HTURN_SPEED) * time);
	hd = clamp<float>(-65.f, fmodp(head.getRotation() - dir, 360.f), 65.f);

}

Cow::Cow(const sf::Vector2f & pos, const float dir) : body(sf::Vector2f(BLENGTH, BWIDTH)), head(sf::Vector2f(HLENGTH, HWIDTH)), velocity(0.f, 0.f), steering_direction(0.f, 0.f)
{
	hd = 0.f;
	speed = 0.f;
	body.setOrigin(BLENGTH / 2.f, BWIDTH / 2.f);
	body.setPosition(pos);

	head.setOrigin(1.f, HWIDTH / 2.f);
	setHead(dir, 1);

	body.setFillColor(sf::Color(237, 224, 177));
	body.setOutlineColor(sf::Color(186, 186, 186));
	body.setOutlineThickness(1);

	head.setFillColor(sf::Color(237, 224, 177));
	head.setOutlineColor(sf::Color(186, 186, 186));
	head.setOutlineThickness(1);

	float rad = randm<float>(2 * M_PI);
	velocity = sf::Vector2f(std::cos(rad), std::sin(rad)) * randm<float>(MAX_SPEED);
	body.setRotation(rad * 180.f / M_PI);
}

void Cow::setPos(const sf::Vector2f & pos)
{
	body.setPosition(pos);
}

void Cow::step(float time)
{
	float theta = body.getRotation() * M_PI / 180.f;

	// find local components of steering direction
	float a = steering_direction.y * std::sin(theta) + steering_direction.x * std::cos(theta);
	float b = -steering_direction.x * std::sin(theta) + steering_direction.y * std::cos(theta);

	// clamp forces
	a = clamp<float>(-MAX_REVERSE_FORCE, a, MAX_FORWARD_FORCE);
	b = clamp<float>(-MAX_LATERAL_FORCE, b, MAX_LATERAL_FORCE);

	// convert force to new velocity and move the cow
	sf::Vector2f steering (a * std::cos(theta) - b * std::sin(theta), a * std::sin(theta) + b * std::cos(theta));
	steering /= MASS; // force to acceleration
	velocity = truncate<float>(velocity + steering, MAX_SPEED);
	body.move(velocity * time);

	// update body, head orientation
	body.setRotation(atan2(velocity.y, velocity.x) * 180.f / M_PI);
	setHead(body.getRotation(), time); // update the head
}

void Cow::addCow(const Cow * cow)
{
	neighbors.push_back(cow);
}

void Cow::resetN()
{
	neighbors.clear();
}

// currently just converts absolute position vector to relative direction
void Cow::think(sf::Vector2f dir)
{
	steering_direction = dir - body.getPosition();
}

void Cow::draw(sf::RenderWindow & window) const
{
	window.draw(body);
	window.draw(head);
}
