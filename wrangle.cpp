#include <iostream>
#include <sstream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include "cow.hpp"
#include "helpers.hpp"

using std::cerr;
using std::endl;

const float AI_INTERVAL = 0.5f;
const float DIST_INTERVAL = 5.f;

int main(int argc, char *argv[])
{
	// parse command line
	const int COWS = (argc == 2 ? atoi(argv[1]) : 1);

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
	Cow *cows = new Cow[COWS];
	for(Cow *cow = cows; cow != cows + COWS; cow++)
	{
		cow->setPos(sf::Vector2f(randm<float>(800), randm<float>(600)));
		cow->think();
	}

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

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		if (dist.getElapsedTime().asSeconds() >= DIST_INTERVAL)
		{
			dist.restart();
			for (Cow *cow = cows; cow != cows + COWS; cow++)
				cow->resetN();
			
			for (Cow *cow1 = cows; cow1 != cows + COWS - 1; cow1++)
			{
				for (Cow *cow2 = cow1 + 1; cow2 != cows + COWS; cow2++)
				{
					if (v2dist(cow1->pos(), cow2->pos()) <= Cow::D_THRESHOLD)
					{
						cow1->addCow(cow2);
						cow2->addCow(cow1);
					}
				}
			}
		}

		// AI
		if(ai.getElapsedTime().asSeconds() >= AI_INTERVAL)
		{
			ai.restart();
			for(Cow *cow = cows; cow != cows + COWS; cow++)
				cow->think();
			// TODO get a proper clock for this
			fps_s.str("");
			fps_s << 1.f / time << " FPS";
			fps.setString(fps_s.str());
		}


		// update entities
		for(Cow *cow = cows; cow != cows + COWS; cow++)
			cow->step(time);

		// draw everything
		window.clear(sf::Color(46, 196, 255));

		for(Cow *cow = cows; cow != cows + COWS; cow++)
			cow->draw(window);

		window.draw(fps);

		window.display();
	}

	delete [] cows;

	return EXIT_SUCCESS;
}
