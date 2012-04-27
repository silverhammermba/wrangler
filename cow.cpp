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

const float Cow::D_THRESHOLD = 80.f; // how far away cows look TODO combine with MAX_THREAT_DIST?
const float Cow::BLENGTH = 14.f; // body length
const float Cow::BWIDTH = 7.f; // body width
const float Cow::HLENGTH = 7.f; // head length
const float Cow::HWIDTH = 4.f; // head width
const float Cow::HTURN_SPEED = 180.f; // head turn speed degrees/second
const float Cow::MASS = 1.f;
const float Cow::MAX_THREAT_DIST = 200.f; // distance at which cows consider threads
const float Cow::SLOW_DISTANCE = 30.f; // distance at which cows start slowing down
const float Cow::MAX_SPEED = 50.f; // movement speed pixels/second
const float Cow::MAX_FORWARD_FORCE = 1.5f;
const float Cow::MAX_LATERAL_FORCE = 1.f;
const float Cow::MAX_REVERSE_FORCE = 0.5f;
const float Cow::MAX_WANDER_DTHETA = M_PI; // rate at which wander direction changes in rads/sec

Cow::Cow(const v2f & pos, const float dir) :
	body(v2f(BLENGTH, BWIDTH)), head(v2f(HLENGTH, HWIDTH)), velocity(0.f, 0.f), steering_direction(0.f, 0.f)
{
	debug = false;
	hd = 0.f;
	speed = 0.f;
	body.setOrigin(BLENGTH / 2.f, BWIDTH / 2.f);
	body.setPosition(pos);

	head.setOrigin(1.f, HWIDTH / 2.f);
	setHead(dir, 1);

	setColor(sf::Color(237, 224, 177));

	body.setOutlineColor(sf::Color(186, 186, 186));
	body.setOutlineThickness(1);

	head.setOutlineColor(sf::Color(186, 186, 186));
	head.setOutlineThickness(1);

	dbg_vel.setFillColor(sf::Color(255, 0, 0));
	dbg_str.setFillColor(sf::Color(255, 0, 255));
	dbg_dir.setFillColor(sf::Color(0, 0, 255));

	setDir(randm<float>(360.f));
}

void Cow::setColor(const sf::Color & clr)
{
	color = clr;
	body.setFillColor(color);
	head.setFillColor(color);
}

void Cow::setDir(const float deg)
{
	velocity = Vec2f(deg2rad(deg), randm<float>(MAX_SPEED));
	body.setRotation(deg);
}

// set the position of the head, when the cow is facing dir
void Cow::setHead(float dir, float time)
{
	// TODO refactor
	head.setPosition(body.getTransform() * v2f(BLENGTH, BWIDTH / 2.f));
	head.setRotation(dir + hd);
	float d = rad2deg(atan2(steering_direction.y - pos().y, steering_direction.x - pos().x));
	head.rotate(clamp<float>(-HTURN_SPEED, fmodp(d - dir - hd, 360.f), HTURN_SPEED) * time);
	hd = clamp<float>(-65.f, fmodp(head.getRotation() - dir, 360.f), 65.f);

}

void Cow::setPos(const v2f & pos)
{
	body.setPosition(pos);
}

const float Cow::C_MAX_LATERAL_FORCE() const
{
	return MAX_LATERAL_FORCE * v2mag<float>(velocity) / MAX_SPEED;
}

void Cow::step(float time)
{
	(this->*think)();
	// TODO make sure I didn't screw something up by making steering direction absolute
	float theta = deg2rad(body.getRotation());

	v2f steering = steering_direction - pos();

	if (debug)
	{
		dbg_dir.setSize(v2f(std::sqrt(steering.x * steering.x + steering.y * steering.y), 1.f));
		dbg_dir.setPosition(pos());
		dbg_dir.setRotation(rad2deg(std::atan2(steering.y, steering.x)));
	}

	// find local components of steering direction
	float a = steering.y * std::sin(theta) + steering.x * std::cos(theta);
	float b = -steering.x * std::sin(theta) + steering.y * std::cos(theta);

	// clamp forces
	float max_lateral_force = C_MAX_LATERAL_FORCE();
	a = clamp<float>(-MAX_REVERSE_FORCE, a, MAX_FORWARD_FORCE);
	b = clamp<float>(-max_lateral_force, b, max_lateral_force);

	// convert force to new velocity and move the cow
	steering = v2f(a * std::cos(theta) - b * std::sin(theta), a * std::sin(theta) + b * std::cos(theta));
	steering /= MASS; // force to acceleration

	float distance = v2dist(pos(), steering_direction);
	float ramped_speed = MAX_SPEED * (distance / SLOW_DISTANCE);
	float clipped_speed = std::min(ramped_speed, MAX_SPEED);

	velocity = truncate<float>(velocity + steering, clipped_speed);
	body.move(velocity * time);

	// update debug lines
	if (debug)
	{
		dbg_vel.setSize(v2f(std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y), 1.f));
		dbg_vel.setPosition(pos());
		dbg_vel.setRotation(body.getRotation());
		dbg_str.setSize(v2f(std::sqrt(steering.x * steering.x + steering.y * steering.y) * 20, 1.f));
		dbg_str.setPosition(pos());
		dbg_str.setRotation(rad2deg(std::atan2(steering.y, steering.x)));
	}

	// update body, head orientation
	// TODO somehow give cows a direction when they have no velocity!!!
	body.setRotation(rad2deg(atan2(velocity.y, velocity.x)));
	setHead(body.getRotation(), time);

	// TODO perhaps this should go elsewhere...
	if (think == &Cow::wander_f)
		target.theta += (randm<float>(2 * MAX_WANDER_DTHETA) - MAX_WANDER_DTHETA) * time;
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
	// if you and the target are aligned, going straight towards the target is best
	if (std::abs(v2angle(target.cow->pos() - pos(), target.cow->velocity)) > 135.f)
	{
		steering_direction = target.cow->pos();
		if (debug) body.setFillColor(sf::Color(255.f, 0.f, 0.f));
	}
	else // otherwise use a simple predicted position
	{
		steering_direction = predict_pos(*target.cow);
		if (debug) setColor(color);
	}
}

// predict the future position of a cow (simply)
v2f Cow::predict_pos(const Cow & cow) const
{
	float v = v2mag(velocity);
	if (v == 0.f) v = MAX_SPEED;
	return (cow.vel() * v2dist(pos(), cow.pos()) / v) + cow.pos();
}

// (literally) the opposite of pursue
void Cow::flee(const Cow & cow)
{
	target.cow = &cow;
	think = &Cow::flee_f;
}

void Cow::flee_f()
{
	pursue_f();
	//debug_s.str("");
	//debug_s << "orig:(" << steering_direction.x << "," << steering_direction.y << ") ";
	// temporarily make the steering direction relative
	steering_direction = pos() - steering_direction;
	float mag = v2mag<float>(steering_direction);
	if (mag == 0.f) // somehow caught, run randomly
	{
		steering_direction = pos() + Vec2f(randm<float>(2.f * M_PI), MAX_THREAT_DIST);
	}
	else // otherwise run with speed inversly proportional to distance
	{
		steering_direction = pos() + steering_direction * (MAX_THREAT_DIST - (mag > MAX_THREAT_DIST ? MAX_THREAT_DIST : mag)) / mag;
	}
}

void Cow::wander()
{
	target.theta = 0.f;
	think = &Cow::wander_f;
}

void Cow::wander_f()
{
	float phi = deg2rad(dir());
	//                   your position      + relative center circle    + radius of the wander circle oriented at relative theta
	steering_direction = pos() + Vec2f(phi, SLOW_DISTANCE) + Vec2f(phi + target.theta, SLOW_DISTANCE - BLENGTH / 2.f);
}

void Cow::move_to(const v2f & pos)
{
	target.pos = &pos;
	steering_direction = pos;
	think = &Cow::move_to_f;
}

void Cow::move_to_f() {}

void Cow::flock()
{
	think = &Cow::flock_f;
}

void Cow::flock_f()
{
	// TODO separate different factors, define different
	// parameters for each?
	
	// these are what control flocking
	v2f match_alignment, follow, uncrowd;

	v2f total (0, 0);
	int count = 0;
	float m;
	for (auto cow : neighbors)
	{
		m = v2mag(cow->vel());
		if (m > 0)
		{
			count++;
			total += cow->vel() / m;
		}
	}
	if (count != 0)
	{
		match_alignment = total * (60.f / count);
	}
	else // TODO ehhhh...
		match_alignment = Vec2f(deg2rad(body.getRotation()), 60.f);

////////////////////////////////////////////////////////////////////////////////

	// TODO maybe want to use actual position?
	total = v2f(0, 0);
	for (auto cow: neighbors)
	{
		total += predict_pos(*cow);
	}
	total /= float (neighbors.size());

	follow = truncate<float>(total, 90.f);

////////////////////////////////////////////////////////////////////////////////

	total = v2f(0, 0);
	for (auto cow: neighbors)
	{
		total += cow->pos();
	}
	total /= float (neighbors.size());

	uncrowd = pos() - total;
	// TODO scaling?

	// TODO how to take different weights into account...
	steering_direction = pos() + (match_alignment + follow + uncrowd) / 3.f;
}

// TODO really need unaligned obstacle avoidance
