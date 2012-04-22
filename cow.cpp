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
const float Cow::SLOW_DISTANCE = 30.f; // distance at which cows start slowing down
const float Cow::MAX_SPEED = 50.f; // movement speed pixels/second
const float Cow::MAX_FORWARD_FORCE = 1.5f;
const float Cow::MAX_LATERAL_FORCE = 1.f;
const float Cow::MAX_REVERSE_FORCE = 0.5f;

Cow::Cow(const sf::Vector2f & pos, const float dir) :
	body(sf::Vector2f(BLENGTH, BWIDTH)), head(sf::Vector2f(HLENGTH, HWIDTH)), velocity(0.f, 0.f), steering_direction(0.f, 0.f)
{
	debug = false;
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

	dbg_vel.setFillColor(sf::Color(255, 0, 0));
	dbg_str.setFillColor(sf::Color(255, 0, 255));
	dbg_dir.setFillColor(sf::Color(0, 0, 255));

	setDir(randm<float>(360.f));
}

void Cow::setColor(const sf::Color & color)
{
	body.setFillColor(color);
	head.setFillColor(color);
}

void Cow::setDir(const float deg)
{
	float rad = deg2rad(deg);
	velocity = sf::Vector2f(std::cos(rad), std::sin(rad)) * randm<float>(MAX_SPEED);
	body.setRotation(deg);
}

// set the position of the head, when the cow is facing dir
void Cow::setHead(float dir, float time)
{
	// TODO refactor
	head.setPosition(body.getTransform() * sf::Vector2f(BLENGTH, BWIDTH / 2.f));
	head.setRotation(dir + hd);
	float d = rad2deg(atan2(steering_direction.y - body.getPosition().y, steering_direction.x - body.getPosition().x));
	head.rotate(clamp<float>(-HTURN_SPEED, fmodp(d - dir - hd, 360.f), HTURN_SPEED) * time);
	hd = clamp<float>(-65.f, fmodp(head.getRotation() - dir, 360.f), 65.f);

}

void Cow::setPos(const sf::Vector2f & pos)
{
	body.setPosition(pos);
}

void Cow::step(float time)
{
	(this->*think)();
	// TODO make sure I didn't screw something up by making steering direction absolute
	float theta = deg2rad(body.getRotation());

	sf::Vector2f steering = steering_direction - body.getPosition();

	if (debug)
	{
		dbg_dir.setSize(sf::Vector2f(std::sqrt(steering.x * steering.x + steering.y * steering.y), 1.f));
		dbg_dir.setPosition(body.getPosition());
		dbg_dir.setRotation(rad2deg(std::atan2(steering.y, steering.x)));
	}

	// find local components of steering direction
	float a = steering.y * std::sin(theta) + steering.x * std::cos(theta);
	float b = -steering.x * std::sin(theta) + steering.y * std::cos(theta);

	// clamp forces
	a = clamp<float>(-MAX_REVERSE_FORCE, a, MAX_FORWARD_FORCE);
	b = clamp<float>(-MAX_LATERAL_FORCE, b, MAX_LATERAL_FORCE);

	// convert force to new velocity and move the cow
	steering = sf::Vector2f(a * std::cos(theta) - b * std::sin(theta), a * std::sin(theta) + b * std::cos(theta));
	steering /= MASS; // force to acceleration

	float distance = v2dist(pos(), steering_direction);
	float ramped_speed = MAX_SPEED * (distance / SLOW_DISTANCE);
	float clipped_speed = std::min(ramped_speed, MAX_SPEED);

	velocity = truncate<float>(velocity + steering, clipped_speed);
	body.move(velocity * time);

	// update debug lines
	if (debug)
	{
		dbg_vel.setSize(sf::Vector2f(std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y), 1.f));
		dbg_vel.setPosition(body.getPosition());
		dbg_vel.setRotation(body.getRotation());
		dbg_str.setSize(sf::Vector2f(std::sqrt(steering.x * steering.x + steering.y * steering.y) * 20, 1.f));
		dbg_str.setPosition(body.getPosition());
		dbg_str.setRotation(rad2deg(std::atan2(steering.y, steering.x)));
	}

	// update body, head orientation
	body.setRotation(rad2deg(atan2(velocity.y, velocity.x)));
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

void Cow::draw(sf::RenderWindow & window) const
{
	window.draw(body);
	window.draw(head);

	if (debug)
	{
		window.draw(dbg_dir);
		window.draw(dbg_vel);
		window.draw(dbg_str);
	}
}

void Cow::pursue(const Cow & cow)
{
	target.cow = &cow;
	think = &Cow::pursue_f;
}

void Cow::pursue_f()
{
	if (std::abs(v2angle(target.cow->pos() - pos(), target.cow->velocity)) > 135.f)
	{
		steering_direction = target.cow->pos();
		
	}
	else
	{
		steering_direction = predict_pos(*target.cow);
	}
}

sf::Vector2f Cow::predict_pos(const Cow & cow) const
{
	return (cow.vel() * v2dist(pos(), cow.pos()) / v2mag(velocity)) + cow.pos();
}

void Cow::flee(const Cow & cow)
{
	target.cow = &cow;
	think = &Cow::flee_f;
}

void Cow::flee_f()
{
	pursue_f();
	steering_direction = 2.f * body.getPosition() - steering_direction;
}

void Cow::move_to(const sf::Vector2f & pos)
{
	target.pos = &pos;
	steering_direction = pos;
	think = &Cow::move_to_f;
}

void Cow::move_to_f() {}
