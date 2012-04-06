#include <iostream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include "cow.hpp"
#include "helpers.hpp"

using std::cerr;
using std::endl;

const float AI_INTERVAL = 5;

int main(int argc, char *argv[])
{
	srand((unsigned)time(0));

	sf::Clock clock;
	sf::Clock ai;

	sf::RenderWindow window
	{
		sf::VideoMode(800, 600, 32),
		"Wrangle!",
		sf::Style::Titlebar
	};
	window.setVerticalSyncEnabled(true);

	const int COWS = (argc == 2 ? atoi(argv[1]) : 1);

	Cow *cows = new Cow[COWS];
	for(Cow *cow = cows; cow != cows + COWS; cow++)
	{
		cow->setPos(sf::Vector2f(400, 300)); //(randm<float>(800), randm<float>(600)));
		cow->setDir(randm<float>(360.f));
	}

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Escape))
				window.close();
		}

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		if(ai.getElapsedTime().asSeconds() >= AI_INTERVAL)
		{
			ai.restart();
			for(Cow *cow = cows; cow != cows + COWS; cow++)
				cow->think();
			//cerr << 1.f / time << " fps\n";
		}

		for(Cow *cow = cows; cow != cows + COWS; cow++)
			cow->step(time);

		window.clear(sf::Color(46, 196, 255));

		for(Cow *cow = cows; cow != cows + COWS; cow++)
			cow->draw(window);

		window.display();
	}

	delete [] cows;

	return EXIT_SUCCESS;
}
