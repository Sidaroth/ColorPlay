#include "WindowHandler.hpp"

// Probably shouldn't be called... 
WindowHandler::WindowHandler()
{
	window.create(sf::VideoMode(800, 600), "Default Name huehue");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	this -> width = 800;
	this -> height = 600;

	logger -> LogEvent("Default basic constructor ran, intended?");
}

WindowHandler::WindowHandler(std::string windowName,
							 LogModule* logger,
							 bool* running,
							 BulbHandler* bulbHandler,
							 bool* finished,
							 int width,          /* 1280  */ 
							 int height, 		 /* 720  */
							 bool verticalSync , /* true */ 
							 int frameRateLimit) /* 60   */
{
	window.create(sf::VideoMode(width, height), windowName);
	window.setVerticalSyncEnabled(verticalSync);
	
	this -> logger = logger;
	this -> running = running;
	this -> bulbHandler = bulbHandler;
	this -> finished = finished;

	if(verticalSync != true)			// VSync and forced framerate should not be used at the same time. It can make the screen do weird things... - SFML Docs.
	{
		window.setFramerateLimit(frameRateLimit);
	}

	windowType = WindowType::Game;

	this -> width = width;
	this -> height = height;

	logger -> LogEvent("Window created");
}

bool WindowHandler::init()
{
	std::wstringstream tempWStringStream;

	//RGB
	tempWStringStream	<< L"Velkommen til Color Play!\n\n"
						<< L"Beveg kontrolleren opp og ned forran pærene for å stille på fargene.\n"
						<< L"Prøv å blande fargen vist til høyre.\n"
						<< L"Avslutt med MOVE knappen.";
	this->instructions.push_back(tempWStringStream.str());
	
	//HSV
	tempWStringStream.str(L"");
	tempWStringStream	<< L"Velkommen til Color Play!\n\n"
						<< L"Dette fargerommet mangler instruksjoner.\n"
						<< L"Vennligst legg til instruksjoner i WindowHandler::init().\n";
	this->instructions.push_back(tempWStringStream.str());

	//CMYK
	tempWStringStream.str(L"");
	tempWStringStream	<< L"Velkommen til Color Play!\n\n"
						<< L"Dette fargerommet mangler instruksjoner.\n"
						<< L"Vennligst legg til instruksjoner i WindowHandler::init().\n";
	this->instructions.push_back(tempWStringStream.str());

	//XYZ
	tempWStringStream.str(L"");
	tempWStringStream	<< L"Velkommen til Color Play!\n\n"
						<< L"Dette fargerommet mangler instruksjoner.\n"
						<< L"Vennligst legg til instruksjoner i WindowHandler::init().\n";
	this->instructions.push_back(tempWStringStream.str());

	//Lab
	tempWStringStream.str(L"");
	tempWStringStream	<< L"Velkommen til Color Play!\n\n"
						<< L"Dette fargerommet mangler instruksjoner.\n"
						<< L"Vennligst legg til instruksjoner i WindowHandler::init().\n";
	this->instructions.push_back(tempWStringStream.str());


	if(!font.loadFromFile("./assets/ABeeZee-Regular.otf"))
	{
		this -> logger -> LogEvent("ERROR: Loading font failed!");

		return 1;
	}

	text.setFont(font);

	return 0;
}

void WindowHandler::processEvents()
{
	if(this -> windowType == WindowType::Game)
	{
		this -> gameProcessEvents();
	}
	else if(this -> windowType == WindowType::Config)
	{
		this -> configProcessEvents();
	}
	else
	{
		this -> logger -> LogEvent("ERROR: WindowHandler processEvents could not identify WindowType!");
	}
}

/// Identifies the windowType and runs the appropriate update call. 
void WindowHandler::update()
{
	if(this -> windowType == WindowType::Game)
	{
		this -> gameUpdate();
	}
	else if(this -> windowType == WindowType::Config)
	{
		this -> configUpdate();
	}
	else
	{
		this -> logger -> LogEvent("ERROR: WindowHandler update could not identify WindowType!");
	}
}

/// Identifies the windowType and runs the appropriate render call. 
void WindowHandler::render()
{
	if(this -> windowType == WindowType::Game)
	{
		this -> gameRender();
	}
	else if(this -> windowType == WindowType::Config)
	{
		this -> configRender();
	}
	else
	{
		this -> logger -> LogEvent("ERROR: WindowHandler render could not identify WindowType!");
	}

}

void WindowHandler::close()
{
	window.close();
}

void WindowHandler::setWindowType(WindowType windowType)
{
	this -> windowType = windowType;
}


///////////// GAME RELATED /////////////////
void WindowHandler::gameProcessEvents()
{
	while(window.pollEvent(event))
	{
		switch(event.type)
		{
			case sf::Event::Closed:
				window.close();
				(*running) = false;
				break;

			case sf::Event::KeyReleased:
			// Switch to determine which button is pressed. 
				switch(event.key.code)
				{
					case sf::Keyboard::Escape:
						window.close();
						(*running) = false;
						break;

					case sf::Keyboard::Space:
						this -> bulbHandler -> generateNewGoalColor();
						break;

					case sf::Keyboard::A:
						this -> bulbHandler -> setColorSpace(BulbHandler::ColorSpace::RGB);
						break;

					case sf::Keyboard::S:
						this -> bulbHandler -> setColorSpace(BulbHandler::ColorSpace::HSV);
						break;

					case sf::Keyboard::D:
						this -> bulbHandler -> setColorSpace(BulbHandler::ColorSpace::CMY);
						break;

					case sf::Keyboard::F:
						this -> bulbHandler -> setColorSpace(BulbHandler::ColorSpace::XYZ);
						break;

					case sf::Keyboard::G:
						this -> bulbHandler -> setColorSpace(BulbHandler::ColorSpace::Lab);
						break;

					case sf::Keyboard::Return:
						this -> logger -> LogEvent("WindowType Changed to Config!");
						this -> windowType = WindowType::Config;
						break;
						
					default: break;
				}

			default: break;
		}
	}
}

void WindowHandler::gameUpdate()
{

}

void WindowHandler::gameRender()
{
	window.clear(sf::Color::Black);
	renderGoalColor();
	renderInstructions();
	
	if(*this->finished)
	{
		renderScore();
	}

	window.display();
}

void WindowHandler::renderGoalColor()
{
	sf::RectangleShape goalColorBGShape(sf::Vector2f(this->width / 2, this->height));
	goalColorBGShape.setPosition(this -> width / 2, 0);
	goalColorBGShape.setFillColor(sf::Color(128, 128, 128));

	sf::RectangleShape goalColorShape(sf::Vector2f(this->width / 4, this->height / 2));
	goalColorShape.setPosition(goalColorBGShape.getPosition().x + goalColorShape.getSize().x / 2, goalColorBGShape.getPosition().y + goalColorShape.getSize().y / 2);
	goalColorShape.setFillColor(sf::Color(this -> bulbHandler -> getGoalColor()));	

	window.draw(goalColorBGShape);
	window.draw(goalColorShape);
}

void WindowHandler::renderInstructions()
{
	std::wstring tempWString;
	sf::String string;

	if(this->bulbHandler->currentColorSpace == BulbHandler::ColorSpace::RGB)
	{
		tempWString = this->instructions[0];
	}
	else if(this->bulbHandler->currentColorSpace == BulbHandler::ColorSpace::HSV)
	{
		tempWString = this->instructions[1];
	}
	else if(this->bulbHandler->currentColorSpace == BulbHandler::ColorSpace::CMY)
	{
		tempWString = this->instructions[2];
	}
	else if(this->bulbHandler->currentColorSpace == BulbHandler::ColorSpace::XYZ)
	{
		tempWString = this->instructions[3];
	}
	else if(this->bulbHandler->currentColorSpace == BulbHandler::ColorSpace::Lab)
	{
		tempWString = this->instructions[4];
	}
	else
	{
		tempWString = L"Gjenkjenner ikke fargerommet.";
	}
	string = wrapText(tempWString, (this->width / 2) - this->edgeOffset, this->font, this->textSize);
	
	this->text.setString(string);
	this->text.setPosition(this->edgeOffset, this->edgeOffset);
	text.setColor(sf::Color::White);
	this->window.draw(text);
}

void WindowHandler::renderScore()
{
	std::wstringstream tempWStringStream;
	sf::String string;

	tempWStringStream << L"Poeng: " << (int)*this->bulbHandler->currentScore;
	string = tempWStringStream.str();

	this->text.setString(string);
	this->text.setPosition((this->width / 2) - (text.getLocalBounds().width / 2), (this->height / 2)  - (text.getLocalBounds().height / 2));
	text.setColor(sf::Color::White);

	window.clear(sf::Color::Black);
	this->window.draw(text);
}

sf::String WindowHandler::wrapText(sf::String string, unsigned width, const sf::Font &font, unsigned charicterSize, bool bold /*Default: false*/)
{
	unsigned currentOffset = 0;
	bool firstWord = true;
	std::size_t wordBegining = 0;
 
	for (std::size_t pos(0); pos < string.getSize(); ++pos)
	{
		auto currentChar = string[pos];
		
		if (currentChar == '\n')
		{
			currentOffset = 0;
			firstWord = true;
			continue;
		} 
		else if (currentChar == ' ')
		{
			wordBegining = pos;
			firstWord = false;
		}
 
		auto glyph = font.getGlyph(currentChar, charicterSize, bold);
		currentOffset += glyph.advance;
 
		if (!firstWord && currentOffset > width)
		{
			pos = wordBegining;
			string[pos] = '\n';
			firstWord = true;
			currentOffset = 0;
		}
	}
 
	return string;
}

///////////////// CONFIG RELATED ////////////////
void WindowHandler::configProcessEvents()
{
	while(window.pollEvent(event))
	{
		switch(event.type)
		{
			case sf::Event::Closed:
				window.close();
				(*running) = false;
				break;

			case sf::Event::KeyReleased:
			// Switch to determine which button is pressed. 
				switch(event.key.code)
				{
					case sf::Keyboard::Escape:
						window.close();
						(*running) = false;
						break;

					case sf::Keyboard::Return:
						this -> logger -> LogEvent("WindowType Changed to Game!");
						this -> windowType = WindowType::Game;
						break;

					default: break;
				}

			default: break;
		}
	}
}

void WindowHandler::configUpdate()
{

}

void WindowHandler::configRender()
{
	sf::CircleShape shape(250.f);
	shape.setFillColor(sf::Color::Blue);
	text.setString("Config Render!");
	text.setCharacterSize(24);
	text.setColor(sf::Color::Black);
	text.setPosition(750, 0);

	sf::RectangleShape rectShape(sf::Vector2f(300, 300));
	rectShape.setFillColor(sf::Color(123, 123, 255));
	rectShape.setPosition(500, 300);

	window.clear(sf::Color(128,128,128));
	window.draw(rectShape);
	window.draw(text);
	window.display();
}

