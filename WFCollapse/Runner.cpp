#include "Runner.h"
Runner::Runner() :
	window(std::make_shared<sf::RenderWindow>(sf::VideoMode(Utility::WINDOW_SIZE.x, Utility::WINDOW_SIZE.y)
		, "SFML works!", sf::Style::Close)),
	event(), wfcStack(Utility::GRID_COUNT),
	wfcQueue(Utility::GRID_COUNT) {}

void Runner::Init() {
	if (Utility::USE_STACK) {
		this->wfcStack.Init();
	}
	else {
		this->wfcQueue.Init();
	}
}

void Runner::SfmlLoop() {
	Init();

	while (this->window->isOpen()) {
		while (this->window->pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				this->window->close();
		}
		Update();

		this->window->clear();
		this->Draw();
		this->window->display();
	}
}

void Runner::Update() {
	if (Utility::USE_STACK){
		this->wfcStack.WaveOperation();
	}
	else{
		this->wfcQueue.WaveOperation();
	}
	
}

void Runner::Draw() {
	if (Utility::USE_STACK) {
		this->wfcStack.Draw(*this->window);
	}
	else {
		this->wfcQueue.Draw(*this->window);
	}
	
}
