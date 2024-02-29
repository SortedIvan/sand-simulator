#include <vector>
#include <string>
#include "SFML/Graphics.hpp"
#include <iostream>
#include <iostream>
#include <random>

#include <chrono>
#include <thread>


struct Sand {
	sf::RectangleShape shape;
	sf::RectangleShape outline;
	std::pair<int, int> index;
	bool active;
};

int generateRandomInt(int min, int max);
const sf::Color DEFAULT_BG_COLOR(0x00, 0x01, 0x33);
void InitializeParticlesMatrix(std::vector<std::vector<Sand>>& particles, int size, float sandSize, sf::Vector2f startPoint);
void DrawAllParticles(std::vector<std::vector<Sand>>& particles, sf::RenderWindow& window);
void RandomColorSand(std::vector<std::vector<Sand>>& particles);
void TryPlaceSand(sf::RenderWindow& window, std::vector<std::vector<Sand>>& particles, std::vector<std::pair<int, int>>& toProcess, int& counter, float& hue, float &hueIncrement);
void ProcessOneTick(std::vector<std::vector<Sand>>& particles, std::vector<std::pair<int, int>>& toProcess, int bottom, bool& fancySandSimulation, int& counter, float& hue, float& hueIncrement);
void HandleMouseButtonDown();


int main() {

	sf::RenderWindow window(sf::VideoMode(800, 800), "Test");
	sf::Event e;
	sf::Clock placeClock;
	sf::Time sandPlaceCd = sf::milliseconds(15); // Adjust the cooldown duration as needed

	sf::Clock sandFallClock;
	sf::Time sandFallCd = sf::milliseconds(15); // Adjust the cooldown duration as needed

	bool fancySandSimulation = false;

	sf::Clock hueClock;
	float hue = fmod(hueClock.getElapsedTime().asSeconds() * 50, 360.0f);  // Initial hue based on time
	float hueIncrement = 0.1f;  // Adjust the increment based on your preference


	std::vector<std::vector<Sand>> particles;
	std::vector<std::pair<int, int>> toProcess;
	int counter = 0;


	InitializeParticlesMatrix(particles, 100, 5, sf::Vector2f(50, 50));

	// ---- test -----
	while (window.isOpen()) {
	

		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}

			if (e.type == sf::Event::MouseButtonReleased) {
				
			}

		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			if (placeClock.getElapsedTime() >= sandPlaceCd) {
				TryPlaceSand(window, particles, toProcess, counter, hue, hueIncrement);

				// Reset the clock to start counting for the next action
				placeClock.restart();
			}
		}
	
		if (sandFallClock.getElapsedTime() >= sandFallCd) {
			ProcessOneTick(particles, toProcess, 100, fancySandSimulation, counter, hue, hueIncrement);

			// Reset the clock to start counting for the next action
			sandFallClock.restart();
		}


		//RandomColorSand(particles);
		

		// --------- clear the screen ----------
		window.clear(DEFAULT_BG_COLOR);

		// --------- draw on the screen ----------
		DrawAllParticles(particles, window);

		// --------- display on the screen --------
		window.display();

		hue = fmod(hueClock.getElapsedTime().asSeconds() * 50, 360.0f);

	}
}

void InitializeParticlesMatrix(std::vector<std::vector<Sand>>& particles, int size, float sandSize, sf::Vector2f startPoint) {
	std::vector<std::vector<Sand>> v(size, std::vector<Sand>(size));
	particles = v;

	for (int i = 0; i < size; i++) {
		for (int k = 0; k < size; k++) {
			Sand sand;
			sand.index.first = i;
			sand.index.second = k;
			sand.shape = sf::RectangleShape(sf::Vector2f(sandSize, sandSize));
			//sand.outline = sf::RectangleShape(sf::Vector2f(sandSize, sandSize));
			//sand.outline.setOutlineThickness(0.5f);
			//sand.outline.setFillColor(sf::Color::Transparent);

			sand.shape.setPosition(sf::Vector2f(startPoint.x + sandSize * i, startPoint.y + sandSize * k));
			//sand.outline.setPosition(sf::Vector2f(startPoint.x + sandSize * i, startPoint.y + sandSize * k));
			sand.active = false;
			particles[i][k] = sand;
		}
	}
}

void ProcessOneTick(std::vector<std::vector<Sand>>& particles, std::vector<std::pair<int, int>>& toProcess, int bottom, bool& fancySandSimulation, int& counter, float& hue, float& hueIncrement) {
	for (int i = 0; i < toProcess.size(); i++) {
		if (toProcess[i].second + 1 == bottom) {
			particles[toProcess[i].first][toProcess[i].second].active = true;
			toProcess.erase(toProcess.begin() + i);
			continue;
		}

		if (!particles[toProcess[i].first][toProcess[i].second + 1].active) {
			// Nothing below it, move the sand particle one tick down
			particles[toProcess[i].first][toProcess[i].second].active = false;
			toProcess[i].second = toProcess[i].second + 1;
			particles[toProcess[i].first][toProcess[i].second].active = true;

			particles[toProcess[i].first][toProcess[i].second].shape
				.setFillColor(particles[toProcess[i].first][toProcess[i].second-1].shape.getFillColor());

			particles[toProcess[i].first][toProcess[i].second - 1].shape.setFillColor(sf::Color::White);
			continue;
		}
		else {
			
			bool leftAvailable = false;
			bool rightAvailable = false;

			if (toProcess[i].first + 1 < bottom) {
				rightAvailable = !particles[toProcess[i].first + 1][toProcess[i].second + 1].active;
			}

			if (toProcess[i].first - 1 >= 0) {
				leftAvailable = !particles[toProcess[i].first - 1][toProcess[i].second + 1].active;
			}

			if (leftAvailable && rightAvailable) {
				int randomResult = generateRandomInt(0, 1);

				if (randomResult == 0) {
					// go left
					particles[toProcess[i].first][toProcess[i].second].active = false;
					toProcess[i].first = toProcess[i].first - 1;
					toProcess[i].second = toProcess[i].second + 1;
					particles[toProcess[i].first][toProcess[i].second].active = true;

					particles[toProcess[i].first][toProcess[i].second].shape
						.setFillColor(particles[toProcess[i].first + 1][toProcess[i].second - 1].shape.getFillColor());

					particles[toProcess[i].first + 1][toProcess[i].second - 1].shape.setFillColor(sf::Color::White);

					continue;
				}
				else {
					// go right
					particles[toProcess[i].first][toProcess[i].second].active = false;
					toProcess[i].first = toProcess[i].first + 1;
					toProcess[i].second = toProcess[i].second + 1;
					particles[toProcess[i].first][toProcess[i].second].active = true;

					particles[toProcess[i].first][toProcess[i].second].shape
						.setFillColor(particles[toProcess[i].first - 1][toProcess[i].second - 1].shape.getFillColor());

					particles[toProcess[i].first - 1][toProcess[i].second - 1].shape.setFillColor(sf::Color::White);

					continue;
				}

			}

			if (leftAvailable) {
				particles[toProcess[i].first][toProcess[i].second].active = false;
				toProcess[i].first = toProcess[i].first - 1;
				toProcess[i].second = toProcess[i].second + 1;
				particles[toProcess[i].first][toProcess[i].second].active = true;

				particles[toProcess[i].first][toProcess[i].second].shape
					.setFillColor(particles[toProcess[i].first + 1][toProcess[i].second - 1].shape.getFillColor());

				particles[toProcess[i].first + 1][toProcess[i].second - 1].shape.setFillColor(sf::Color::White);

				continue;
			}

			if (rightAvailable) {
				// go right
				particles[toProcess[i].first][toProcess[i].second].active = false;
				toProcess[i].first = toProcess[i].first + 1;
				toProcess[i].second = toProcess[i].second + 1;
				particles[toProcess[i].first][toProcess[i].second].active = true;

				particles[toProcess[i].first][toProcess[i].second].shape
					.setFillColor(particles[toProcess[i].first - 1][toProcess[i].second - 1].shape.getFillColor());

				particles[toProcess[i].first - 1][toProcess[i].second - 1].shape.setFillColor(sf::Color::White);

				continue;
			}

			if (!leftAvailable && !rightAvailable) {
				particles[toProcess[i].first][toProcess[i].second].active = true;
				toProcess.erase(toProcess.begin() + i);
				continue;
			}


		}
	}

}
void DrawAllParticles(std::vector<std::vector<Sand>>& particles, sf::RenderWindow& window) {
	for (int i = 0; i < particles.size(); i++) {
		for (int k = 0; k < particles.size(); k++) {

			if (particles[i][k].active) {
				window.draw(particles[i][k].shape);
			}

			//window.draw(particles[i][k].outline);

		}
	}
}


void RandomColorSand(std::vector<std::vector<Sand>>& particles) {
	int randomCoordinateOne = generateRandomInt(0, particles.size() - 1);
	int randomCoordinateTwo = generateRandomInt(0, particles.size() - 1);
	int trueOrFalse = generateRandomInt(0, 1);

	if (trueOrFalse == 0) {
		particles[randomCoordinateOne][randomCoordinateTwo].shape.setFillColor(sf::Color::White);
	}
	else {
		particles[randomCoordinateOne][randomCoordinateTwo].shape.setFillColor(sf::Color::Yellow);
	}


}

sf::Color changeColorByHue(float hue, float increment) {
	// Increment hue (range: 0 to 360)
	hue = fmod(hue + increment, 360.0f);

	// Convert hue to RGB
	float c = 1.0f;
	float x = (1.0f - std::abs(fmod(hue / 60.0f, 2.0f) - 1.0f)) * c;
	float m = 0.0f;

	float r, g, b;

	if (hue < 60.0f) {
		r = c;
		g = x;
		b = 0.0f;
	}
	else if (hue < 120.0f) {
		r = x;
		g = c;
		b = 0.0f;
	}
	else if (hue < 180.0f) {
		r = 0.0f;
		g = c;
		b = x;
	}
	else if (hue < 240.0f) {
		r = 0.0f;
		g = x;
		b = c;
	}
	else if (hue < 300.0f) {
		r = x;
		g = 0.0f;
		b = c;
	}
	else {
		r = c;
		g = 0.0f;
		b = x;
	}

	return sf::Color(static_cast<sf::Uint8>((r + m) * 255),
		static_cast<sf::Uint8>((g + m) * 255),
		static_cast<sf::Uint8>((b + m) * 255));
}

void TryPlaceSand(sf::RenderWindow& window, std::vector<std::vector<Sand>>& particles, std::vector<std::pair<int, int>>& toProcess, int& counter, float& hue, float& hueIncrement) {
	sf::Vector2i pos = sf::Mouse::getPosition(window);
	
	for (int i = 0; i < particles.size(); i++) {
		for (int k = 0; k < particles.size(); k++) {
			if (particles[i][k].shape.getGlobalBounds().contains((sf::Vector2f)pos)) {
				if (particles[i][k].active) {
					return;
				}

				// place sand here
				particles[i][k].active = true;
				particles[i][k].shape.setFillColor(changeColorByHue(hue, hueIncrement));

				toProcess.push_back(std::make_pair(i, k));
				counter++;
			}
		}
	}
}

int generateRandomInt(int min, int max) {
	std::random_device rd; // Initialize a random device
	std::mt19937 gen(rd()); // Initialize a Mersenne Twister engine with a random seed
	std::uniform_int_distribution<> distribution(min, max); // Define the range

	return distribution(gen); // Generate a random integer within the specified range
}