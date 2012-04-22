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

	//cows[0].setColor(sf::Color(200.f, 0.f, 0.f));
	cows[1].setColor(sf::Color(10.f, 10.f, 10.f));
	cows[2].setColor(sf::Color(127.f, 127.f, 0.f));

	cows[1].setPos(sf::Vector2f(400.f, 300.f));

	//cows[0].move_to(sf::Vector2f(400.f, 300.f));
	cows[0].pursue(cows[2]);
	cows[1].flee(cows[2]);
	cows[2].pursue(cows[1]);

	unsigned int i = 0;
	int freq = 1;
	bool unpaused = true;

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
			else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::R))
			{
				for(Cow *cow = cows; cow != cows + COWS; cow++)
				{
					cow->setPos(sf::Vector2f(randm<float>(800), randm<float>(600)));
				}
				cows[1].setPos(sf::Vector2f(200.f, 300.f));
				cows[1].setDir(0.f);
				cows[2].setPos(sf::Vector2f(600.f, 300.f));
				cows[2].setDir(180.f);
			}
			else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::D))
			{
				for(Cow *cow = cows; cow != cows + COWS; cow++)
				{
					cow->debug = !(cow->debug);
				}
			}
			else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::J))
			{
				cows[1].pursue(cows[2]);
				cows[2].flee(cows[1]);
			}
			else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::K))
			{
				cows[2].pursue(cows[1]);
				cows[1].flee(cows[2]);
			}
			else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Down))
			{
				freq += 1;
			}
			else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Up))
			{
				freq -= 1;
				if (freq < 1) freq = 1;
			}
			else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Return))
			{
				unpaused = !unpaused;
			}
			else if (event.type == sf::Event::Resized)
			{
				view.setSize(sf::Vector2f(event.size.width, event.size.height));
				window.setView(view);
				fps.setPosition(view.getCenter() - view.getSize() / 2.f);
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


		if (unpaused && (i += 1) % freq == 0)
		{
			// update entities
			for(Cow *cow = cows; cow != cows + COWS; cow++)
				cow->step(time);
			// TODO twitch detection
			for(Cow *cw = cows; cw != cows + COWS; cw++)
				if (v2mag(cw->vel()) < 2.f)
				{
					for(Cow *cow = cows; cow != cows + COWS; cow++)
					{
						cerr << "Cow " << cow << endl;
						cerr << "  Vel " << cow->vel().x << " " << cow->vel().y << endl;
						cerr << "  Str " << cow->str().x << " " << cow->str().y << endl;
						cerr << "  Pos " << cow->pos().x << " " << cow->pos().y << endl;
					}
					unpaused = false;
				}
		}

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
