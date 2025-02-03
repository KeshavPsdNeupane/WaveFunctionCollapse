#pragma once
#include<SFML/Graphics.hpp>
#include<iostream>
#include<memory>
#include"WaveCollapse/WFCStack.h"
#include"WaveCollapse/WFCQueue.h"
#include"Utility.h"


class Runner {
	std::shared_ptr<sf::RenderWindow> window;
	sf::Event event;
	WFCStack wfcStack;
	WFCPriorityQueue wfcQueue;
public:
	Runner();
public:
	void Init();
	void SfmlLoop();
	void Update();
	void Draw();

};

