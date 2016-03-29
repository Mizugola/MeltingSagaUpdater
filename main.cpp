#include "main.hpp"

int main()
{
	std::ofstream out("log.txt");
	std::streambuf *coutbuf = std::cout.rdbuf();
	std::cout.rdbuf(out.rdbuf());

	fn::Coord::width = 800; fn::Coord::height = 600;
	fn::Coord::baseWidth = 800; fn::Coord::baseHeight = 600;
	sf::RenderWindow window(sf::VideoMode(800, 600), "", sf::Style::None);
	window.setMouseCursorVisible(false);
	sf::RectangleShape windowBorder(sf::Vector2f(798, 598));
	windowBorder.setPosition(1, 1); windowBorder.setFillColor(sf::Color(40, 40, 40)); windowBorder.setOutlineColor(sf::Color::White); windowBorder.setOutlineThickness(1);
	sf::Vertex linetop[] = { sf::Vertex(sf::Vector2f(0, 60)), sf::Vertex(sf::Vector2f(800, 60)) };
	sf::Vertex linemiddle[] = { sf::Vertex(sf::Vector2f(0, 450)), sf::Vertex(sf::Vector2f(800, 450)) };
	sf::Vertex linebottom[] = { sf::Vertex(sf::Vector2f(0, 550)), sf::Vertex(sf::Vector2f(800, 550)) };

	sf::Event sfevent;
	GUI::Container gui(&sfevent, &window, 800, 600);
	GUI::WidgetContainer* mainContainer = gui.createWidgetContainer("main", 1, 0, 0, 800, 600, GUI::ContainerMovement::Fixed, 0, 0);
	GUI::WidgetContainer* infoContainer = gui.createWidgetContainer("info", 1, 0, 60, 800, 390, GUI::ContainerMovement::Fixed, 0, 0);
	GUI::WidgetContainer* stgsContainer = gui.createWidgetContainer("stgs", 1, 0, 60, 800, 390, GUI::ContainerMovement::Fixed, 0, 0);
	GUI::WidgetContainer* logsContainer = gui.createWidgetContainer("logs", 1, 0, 450, 800, 100, GUI::ContainerMovement::Fixed, 0, 0);

	Cursor curs;
	curs.initialize(&window);

	sf::Font font;
	font.loadFromFile("Data/Fonts/weblysleekuil.ttf");



	//Main UI
	gui.createLabel("main", "titleLbl", 10, 10, "Melting Saga Updater", "weblysleekuil.ttf", 32, sf::Color::White);
	gui.createLabel("main", "updaterVerLbl", 400, 10, "Updater : <Nightly> v1.0.5", "weblysleekuil.ttf", 12, sf::Color::White);
	gui.createLabel("main", "updaterVerLbl", 400, 30, "Game : <Nightly> v0.0.1", "weblysleekuil.ttf", 12, sf::Color::White);
	gui.createCheckbox("main", "settingsBtn", 700, 15, "SETTINGS", false);
	gui.createButton("main", "quitBtn", 750, 15, true, true, "QUIT");
	gui.createButton("main", "updateBtn", 600, 560, true, true, "GREY");
	gui.createLoadingBar("main", "updateBar", 10, 560, "UPDATER");
	gui.createLabel("main", "updateBarLbl", 300, 560, "80%", "weblysleekuil.ttf", 24, sf::Color::White);

	//Infos UI
	gui.createLabel("info", "updateInfosTitleLbl", 10, 10, "Update Informations : ", "weblysleekuil.ttf", 24, sf::Color::Cyan);
	std::ifstream changelogFile("changelog.txt");
	std::string updateContent((std::istreambuf_iterator<char>(changelogFile)),
		std::istreambuf_iterator<char>());
	std::vector<std::string> updateVec = fn::String::split(updateContent, "@");
	sfe::RichText utext(font);
	sf::Color cColor(255, 255, 255);
	for (int i = 0; i < updateVec.size(); i++)
	{
		if (i % 2 != 0)
		{
			std::cout << updateVec[i] << std::endl;
			utext << cColor << updateVec[i];
		}
		else
		{
			std::vector<std::string> allC = fn::String::split(updateVec[i], ",");
			for (auto j : allC) { std::cout << "Col content : " << j << std::endl; }
			cColor = sf::Color(std::stoi(allC[0]), std::stoi(allC[1]), std::stoi(allC[2]));
		}
	}
	utext.setPosition(30, 105);
	utext.setCharacterSize(16);
	//gui.createLabel("info", "updateInfosContentLbl", 30, 45, updateContent, "weblysleekuil.ttf", 16, sf::Color::White);
	//gui.createScrollBar("info", "updateScrollBar", 790, 0, 300, 0, false, infoContainer);

	//Settings UI
	gui.createLabel("stgs", "settingsTitleLbl", 10, 10, "Settings : ", "weblysleekuil.ttf", 24, sf::Color::Cyan);

	//Logs UI
	gui.createLabel("logs", "logsTitleLbl", 10, 10, "Logs : ", "weblysleekuil.ttf", 24, sf::Color::Cyan);
	sfe::RichText rtext(font);
	rtext << sf::Color::White << "Connection to MeSa Server : 132.14.88.22:9022  < "
		<< sf::Color::Green << "Done"
		<< sf::Color::White << " >\n"
		<< sf::Color::White << "GET : changelog.txt 200 OK < "
		<< sf::Color::Green << "Done"
		<< sf::Color::White << " >\n";

	rtext.setPosition(30, 495);
	rtext.setCharacterSize(16);



	GUI::ButtonEvent* appQuitBool = GUI::Widget::getWidgetByID<GUI::Button>("quitBtn")->getHook();
	bool* appSettingsBool = GUI::Widget::getWidgetByID<GUI::Checkbox>("settingsBtn")->getHook();
	GUI::Widget::getWidgetByID<GUI::Button>("updateBtn")->setText("Update", "weblysleekuil.ttf", sf::Color::White, 18, true, 0, -3);
	GUI::Widget::getWidgetByID<GUI::LoadingBar>("updateBar")->fill(100, 1);
	sf::Vector2i grabbedOffset;

	bool grabbedWindow = false;
	while (window.isOpen())
	{
		while (window.pollEvent(sfevent))
		{
			if (sfevent.type == sf::Event::Closed)
				window.close();
			else if (sfevent.type == sf::Event::KeyPressed)
			{
				if (sfevent.key.code == sf::Keyboard::Escape)
					window.close();
			}
			else if (sfevent.type == sf::Event::MouseButtonPressed)
			{
				if (sf::Mouse::getPosition().y - window.getPosition().y < 60 && sf::Mouse::getPosition().x - window.getPosition().x < 680)
				{
					if (sfevent.mouseButton.button == sf::Mouse::Left)
					{
						grabbedOffset = window.getPosition() - sf::Mouse::getPosition();
						grabbedWindow = true;
					}
				}
			}
			else if (sfevent.type == sf::Event::MouseButtonReleased)
			{
				if (sfevent.mouseButton.button == sf::Mouse::Left)
					grabbedWindow = false;
			}
			else if (sfevent.type == sf::Event::MouseMoved)
			{
				if (grabbedWindow)
					window.setPosition(sf::Mouse::getPosition() + grabbedOffset);
			}
		}
		
		gui.updateAllContainer();
		curs.update();
		if (*appQuitBool == GUI::ButtonEvent::Pressed) {window.close();}
		if (*appSettingsBool) { gui.getContainerByContainerName("info")->setDisplayed(false); gui.getContainerByContainerName("stgs")->setDisplayed(true); }
		else { gui.getContainerByContainerName("info")->setDisplayed(true); gui.getContainerByContainerName("stgs")->setDisplayed(false); }

		window.clear(sf::Color(40, 40, 40));
		window.draw(windowBorder);
		gui.drawAllContainer(&window);
		window.draw(linetop, 2, sf::Lines); window.draw(linemiddle, 2, sf::Lines); window.draw(linebottom, 2, sf::Lines);
		window.draw(*curs.getSprite());
		window.draw(rtext);
		window.draw(utext);
		window.display();
	}
}