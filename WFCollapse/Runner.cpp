#include "Runner.h"
#include"Utility.h"
Runner::Runner() :
	window(std::make_shared<sf::RenderWindow>(sf::VideoMode(Utility::WINDOW_SIZE.x, Utility::WINDOW_SIZE.y)
		, "SFML works!", sf::Style::Close)),
	event(), wfcStack(Utility::GRID_COUNT),
	wfcVector(Utility::GRID_COUNT){
	this->window->setFramerateLimit(Utility::MAX_FRAME);
}

void Runner::Init() {
	if (!Utility::USE_VECTOR) {
		this->wfcStack.Init();
	}
	else {
		this->wfcVector.Init();
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
	if (!Utility::USE_VECTOR){
		this->wfcStack.WaveOperation();
	}
	else{
		this->wfcVector.WaveOperation();
	}
	
}

void Runner::Draw() {
	if (!Utility::USE_VECTOR) {
		this->wfcStack.Draw(*this->window);
	}
	else {
		this->wfcVector.Draw(*this->window);
	}
	
}
