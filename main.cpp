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
	std::pair<int, int> index;
	bool active;
};

int generateRandomInt(int min, int max);
const sf::Color DEFAULT_BG_COLOR(0x00, 0x01, 0x33);
void InitializeParticlesMatrix(std::vector<std::vector<Sand>>& particles, int size, float sandSize, sf::Vector2f startPoint);
void DrawAllParticles(std::vector<std::vector<Sand>>& particles, sf::RenderWindow& window);
void RandomColorSand(std::vector<std::vector<Sand>>& particles);
void TryPlaceSand(sf::RenderWindow& window, std::vector<std::vector<Sand>>& particles, std::vector<std::pair<int, int>>& toProcess);
void ProcessOneTick(std::vector<std::vector<Sand>>& particles, std::vector<std::pair<int, int>>& toProcess, int bottom);
void HandleMouseButtonDown();


int main() {

	sf::RenderWindow window(sf::VideoMode(800, 800), "Test");
	sf::Event e;
	sf::Clock placeClock;
	sf::Time sandPlaceCd = sf::milliseconds(50); // Adjust the cooldown duration as needed

	sf::Clock sandFallClock;
	sf::Time sandFallCd = sf::milliseconds(20); // Adjust the cooldown duration as needed



	std::vector<std::vector<Sand>> particles;
	std::vector<std::pair<int, int>> toProcess;


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
				TryPlaceSand(window, particles, toProcess);

				// Reset the clock to start counting for the next action
				placeClock.restart();
			}
		}
	
		if (sandFallClock.getElapsedTime() >= sandFallCd) {
			ProcessOneTick(particles, toProcess, 100);

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
			sand.shape.setPosition(sf::Vector2f(startPoint.x + sandSize * i, startPoint.y + sandSize * k));
			sand.active = false;
			particles[i][k] = sand;
		}
	}
}

void ProcessOneTick(std::vector<std::vector<Sand>>& particles, std::vector<std::pair<int, int>>& toProcess, int bottom) {
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
		}
	}

}
void DrawAllParticles(std::vector<std::vector<Sand>>& particles, sf::RenderWindow& window) {
	for (int i = 0; i < particles.size(); i++) {
		for (int k = 0; k < particles.size(); k++) {

			if (particles[i][k].active) {
				window.draw(particles[i][k].shape);
			}

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

void TryPlaceSand(sf::RenderWindow& window, std::vector<std::vector<Sand>>& particles, std::vector<std::pair<int, int>>& toProcess) {
	sf::Vector2i pos = sf::Mouse::getPosition(window);
	
	for (int i = 0; i < particles.size(); i++) {
		for (int k = 0; k < particles.size(); k++) {
			if (particles[i][k].shape.getGlobalBounds().contains((sf::Vector2f)pos)) {
				if (particles[i][k].active) {
					return;
				}

				// place sand here
				particles[i][k].active = true;
				toProcess.push_back(std::make_pair(i, k));
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