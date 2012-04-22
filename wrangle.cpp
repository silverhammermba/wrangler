#include <iostream>
#include <sstream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include "cow.hpp"
#include "helpers.hpp"

using std::cerr;
using std::endl;

const float AI_INTERVAL = 0.1f;
const float DIST_INTERVAL = 0.5f;

int main(int argc, char *argv[])
{
	// parse command line
	int COWS = (argc == 2 ? atoi(argv[1]) : 1);

	// create objects
	srand((unsigned)time(0));

	sf::Clock clock;
	sf::Clock ai;
	sf::Clock dist;

	sf::Text fps;
	fps.setCharacterSize(12);
	std::ostringstream fps_s;

	sf::RenderWindow window
	{
		sf::VideoMode(800, 600, 32),
		"Wrangle!",
		sf::Style::Titlebar
	};
	window.setVerticalSyncEnabled(true);

	sf::View view (window.getView());

	// set up the cows TODO would be nice to move this to the contstructor
	COWS = 3;
	Cow *cows = new Cow[COWS];
	for(Cow *cow = cows; cow != cows + COWS; cow++)
	{
		cow->setPos(sf::Vector2f(randm<float>(800), randm<float>(600)));
		//cow->setPos(sf::Vector2f(400, 300));
	}

	cows[1].setPos(sf::Vector2f(400.f, 300.f));

	cows[0].move_to(sf::Vector2f(400.f, 300.f));
	cows[1].flee(cows[2]);
	cows[2].pursue(cows[0]);

	// game loop
	while (window.isOpen())
	{
		// process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Escape))
				window.close();
			else if (event.type == sf::Event::Resized)
			{
				view.setSize(sf::Vector2f(event.size.width, event.size.height));
				window.setView(view);
				fps.setPosition(view.getCenter() - view.getSize() / 2.f);
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		{
			for(Cow *cow = cows; cow != cows + COWS; cow++)
			{
				cow->setPos(sf::Vector2f(randm<float>(800), randm<float>(600)));
			}
			cows[1].setPos(sf::Vector2f(400.f, 300.f));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			for(Cow *cow = cows; cow != cows + COWS; cow++)
			{
				cow->debug = !(cow->debug);
			}
		}

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		// AI
		if(ai.getElapsedTime().asSeconds() >= AI_INTERVAL)
		{
			ai.restart();
			// TODO get a proper clock for this
			fps_s.str("");
			fps_s << "FPS " << int (1.f / time);
			fps.setString(fps_s.str());
		}


		// update entities
		for(Cow *cow = cows; cow != cows + COWS; cow++)
			cow->step(time);

		// draw everything
		window.clear(sf::Color(61, 201, 56));

		for(Cow *cow = cows; cow != cows + COWS; cow++)
			cow->draw(window);

		window.draw(fps);

		window.display();
	}

	delete [] cows;

	return EXIT_SUCCESS;
}
