#include "main.hpp"

std::string serverURL;
LogQueue* lgQu = new LogQueue();
std::vector<std::string> downloadVec;
double dlProg = 10;
double addProg = 0;
std::future<void> future = std::async(std::launch::async, [] {});
double clsTDP = 1;

std::string calc_sha256(char* path)
{
	std::ifstream input(path, std::ios::binary);
	std::ostringstream buffer;
	buffer << input.rdbuf();
	return str_sha256(buffer.str());
}

std::string str_sha256(const std::string str)
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str.c_str(), str.size());
	SHA256_Final(hash, &sha256);
	std::stringstream ss;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}
	return ss.str();
}

void playMesa()
{
	ShellExecute(NULL, "open", "MeltingSaga/MeltingSaga.exe", NULL, NULL, SW_SHOWDEFAULT);
}

void updateMesa(int* step)
{
	switch (*step)
	{
		case 0:
		{
			addLog("<color:255,255,255>Download : remote.upd [ <color:0,255,0>...<color:255,255,255> ]");
			break;
		}
		case 1:
		{
			fn::Curl::downloadFile(serverURL + "remote.upd", "remote.upd");
			setBarPercent(5);
			setLog("<color:255,255,255>Download : remote.upd [ <color:0,255,0>Done<color:255,255,255> ]");
			break;
		}
		case 2:
		{
			addLog("<color:255,255,255>Checking local files checksum [ <color:0,255,0>...<color:255,255,255> ]");
			break;
		}
		case 3:
		{
			
				future = std::async(std::launch::async, [] {
					std::ifstream remoteChecksumFile("remote.upd");
					std::string remoteChecksum((std::istreambuf_iterator<char>(remoteChecksumFile)), std::istreambuf_iterator<char>());
					std::vector<std::string> allLines = fn::String::split(remoteChecksum, "\n");

					for (int i = 0; i < allLines.size(); i++)
					{
						std::string cPath = fn::String::split(allLines[i], ":")[1];
						std::string cSum = fn::String::split(allLines[i], ":")[0];
						if (calc_sha256(&cPath[0u]) == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855" || calc_sha256(&cPath[0u]) != &cSum[0u])
						{
							downloadVec.push_back(cPath);
						}
					}
				});
			
			break;
		}
		case 4:
		{
			std::_Future_status status = future.wait_for(std::chrono::milliseconds(0));
			if (status == std::future_status::ready)
			{
				setBarPercent(10);
				setLog("<color:255,255,255>Checking local files checksum [ <color:0,255,0>Done<color:255,255,255> ]");
				dlProg = 10;
				if (downloadVec.size() > 0)
					addProg = (double)90 / (double)(downloadVec.size());
				else
					addProg = 0;
			}
			else
			{
				std::string crTD = "";
				for (int i = 0; i < (int)clsTDP; i++)
				{
					crTD += ".";
				}
				clsTDP += 0.001;
				if (clsTDP > 4)
				{
					clsTDP = 1;
				}
				setLog("<color:255,255,255>Checking local files checksum [ <color:0,255,0>" + crTD + "<color:255,255,255> ]");
				*step = *step - 1;
			}
			break;
		}
		default:
		{
			if (*step - 5 < downloadVec.size())
			{	
				std::_Future_status status = future.wait_for(std::chrono::milliseconds(0));
				if (status == std::future_status::ready)
				{
					if (*step - 5 != 0)
					{
						setLog("<color:255,255,255>Downloading : " + downloadVec[*step - 6] + "[ <color:0,255,0>Done<color:255,255,255> ]");
					}
					std::string toDl = downloadVec[*step - 5];
					addLog("<color:255,255,255>Downloading : " + toDl + "[ <color:0,255,0>...<color:255,255,255> ]");
					std::cout << serverURL + toDl << std::endl;
					std::vector<std::string> splittedPath = fn::String::split(toDl, "/");
					std::string dirPath = fn::Vector::join(splittedPath, "/", 0, 1);
					boost::filesystem::create_directories(dirPath);
					future = std::async(std::launch::async, [toDl] {
						fn::Curl::downloadFile(serverURL + toDl, toDl);
					});
					dlProg += addProg;
					setBarPercent(dlProg);
				}
				else
				{
					*step = *step - 1;
				}
				
			}
			else
			{
				if (*step - 5 != 0)
				{
					setLog("<color:255,255,255>Downloading : " + downloadVec[*step - 6] + "[ <color:0,255,0>Done<color:255,255,255> ]");
				}
				*step = -2;
				addLog("<color:0,255,0>Update Done !");
				GUI::Widget::getWidgetByID<GUI::Button>("updateBtn")->changeText("Play", sf::Color::White, sf::Text::Style::Regular);
				GUI::Widget::getWidgetByID<GUI::Button>("updateBtn")->bindFunction(&playMesa);
				downloadVec.clear();
				setBarPercent(100);
			}
			break;
		}
	}
	*step = *step + 1;
}

void setBarPercent(int percent)
{
	GUI::Widget::getWidgetByID<GUI::LoadingBar>("updateBar")->fill(percent, 0.05);
	GUI::Widget::getWidgetByID<GUI::Label>("updateBarLbl")->setText(std::to_string(percent) + "%", sf::Color::White);
}

void addLog(std::string content)
{
	std::cout << content << std::endl;
	lgQu->push_back(content);
	GUI::Widget::getWidgetByID<GUI::Label>("logsContLbl")->setComplexText(lgQu->returnComplexText());
}

void setLog(std::string content)
{
	lgQu->set(content);
	GUI::Widget::getWidgetByID<GUI::Label>("logsContLbl")->setComplexText(lgQu->returnComplexText());
}

int main()
{
	DataParser configFile;
	configFile.parseFile("Data/config.cfg.msd", true);
	bool useLog = true;
	std::string logFile = "log.txt";
	if (configFile.attributeExists("LauncherConfig", "", "useLog")) configFile.getAttribute("LauncherConfig", "", "useLog")->getData(&useLog);
	if (configFile.attributeExists("LauncherConfig", "", "logFile")) configFile.getAttribute("LauncherConfig", "", "logFile")->getData(&logFile);
	configFile.getAttribute("LauncherConfig", "", "serverURL")->getData(&serverURL);

	if (useLog)
	{
		std::ofstream out(logFile);
		std::streambuf *coutbuf = std::cout.rdbuf();
		std::cout.rdbuf(out.rdbuf());
	}

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
	GUI::Container* gui = new GUI::Container(&sfevent, &window, 800, 600);
	GUI::WidgetContainer* mainContainer = gui->createWidgetContainer("main", 1, 0, 0, 800, 600, GUI::ContainerMovement::Fixed, 0, 0);
	GUI::WidgetContainer* infoContainer = gui->createWidgetContainer("info", 1, 0, 60, 800, 390, GUI::ContainerMovement::Fixed, 0, 0);
	GUI::WidgetContainer* stgsContainer = gui->createWidgetContainer("stgs", 1, 0, 60, 800, 390, GUI::ContainerMovement::Fixed, 0, 0);
	GUI::WidgetContainer* logsContainer = gui->createWidgetContainer("logs", 1, 0, 450, 800, 100, GUI::ContainerMovement::Fixed, 0, 0);

	Cursor curs;
	curs.initialize(&window);

	sf::Font font;
	font.loadFromFile("Data/Fonts/weblysleekuil.ttf");

	//Main UI
	gui->createLabel("main", "titleLbl", 10, 10, "Melting Saga Updater", "weblysleekuil.ttf", 32, sf::Color::White);
	gui->createLabel("main", "updaterVerLbl", 400, 10, "Updater : <Nightly> v1.0.6", "weblysleekuil.ttf", 12, sf::Color::White);
	gui->createLabel("main", "updaterVerLbl", 400, 30, "Game : <Nightly> v0.0.1", "weblysleekuil.ttf", 12, sf::Color::White);
	gui->createCheckbox("main", "settingsBtn", 700, 15, "SETTINGS", false);
	gui->createButton("main", "quitBtn", 750, 15, true, true, "QUIT");
	gui->createButton("main", "updateBtn", 600, 560, true, true, "GREY");
	gui->createLoadingBar("main", "updateBar", 10, 560, "UPDATER");
	gui->createLabel("main", "updateBarLbl", 300, 560, "0%", "weblysleekuil.ttf", 24, sf::Color::White);
	GUI::ButtonEvent* updBtn = GUI::Widget::getWidgetByID<GUI::Button>("updateBtn")->getHook();
	std::thread* tupd = nullptr;

	//Infos UI
	fn::Curl::downloadFile(serverURL + "changelog.txt", "changelog.txt");
	gui->createLabel("info", "updateInfosTitleLbl", 10, 10, "Update Informations : ", "weblysleekuil.ttf", 24, sf::Color::Cyan);
	gui->createLabel("info", "updateInfosContLbl", 30, 45, "", "weblysleekuil.ttf", 16, sf::Color::White);
	std::ifstream changelogFile("changelog.txt");
	std::string updateContent((std::istreambuf_iterator<char>(changelogFile)),
		std::istreambuf_iterator<char>());
	GUI::Widget::getWidgetByID<GUI::Label>("updateInfosContLbl")->setComplexText(updateContent);
	infoContainer->addScrollBar();

	//Settings UI
	gui->createLabel("stgs", "settingsTitleLbl", 10, 10, "Settings : ", "weblysleekuil.ttf", 24, sf::Color::Cyan);
	gui->createCheckbox("stgs", "settingsSaveLogCB", 30, 45, "GREY", false);
	gui->createLabel("stgs", "settingsSaveLogLbl", 55, 43, "Save log.txt", "weblysleekuil.ttf", 16, sf::Color::White);
	gui->createLabel("stgs", "settingsCustomUrlLbl", 30, 70, "Custom URL : ", "weblysleekuil.ttf", 16, sf::Color::White);
	gui->createTextInput("stgs", "settingsCustomUrlE", 130, 70, serverURL, "weblysleekuil.ttf", 16, sf::Color::White, "GREY");

	//Logs UI
	gui->createLabel("logs", "logsTitleLbl", 10, 10, "Logs : ", "weblysleekuil.ttf", 24, sf::Color::Cyan);
	gui->createLabel("logs", "logsContLbl", 30, 45, "", "weblysleekuil.ttf", 16, sf::Color::White);
	addLog("<color:255,255,255>Connection to MeSa Server : " + serverURL +  " [ <color:0,255,0>Done<color:255,255,255> ]");
	addLog("<color:255,255,255>Download : changelog.txt [ <color:0,255,0>Done<color:255,255,255> ]");


	GUI::ButtonEvent* appQuitBool = GUI::Widget::getWidgetByID<GUI::Button>("quitBtn")->getHook();
	bool* appSettingsBool = GUI::Widget::getWidgetByID<GUI::Checkbox>("settingsBtn")->getHook();
	GUI::Widget::getWidgetByID<GUI::Button>("updateBtn")->setText("Update", "weblysleekuil.ttf", sf::Color::White, 18, true, 0, -3);
	GUI::Widget::getWidgetByID<GUI::LoadingBar>("updateBar")->fill(0, 0);
	sf::Vector2i grabbedOffset;
	int cstep = -1;

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
		
		if (*updBtn == GUI::ButtonEvent::Clicked && cstep < 0) cstep++;
		if (cstep >= 0) updateMesa(&cstep);
		
		gui->updateAllContainer();
		curs.update();
		if (*appQuitBool == GUI::ButtonEvent::Pressed) {window.close();}
		if (*appSettingsBool) { gui->getContainerByContainerName("info")->setDisplayed(false); gui->getContainerByContainerName("stgs")->setDisplayed(true); }
		else { gui->getContainerByContainerName("info")->setDisplayed(true); gui->getContainerByContainerName("stgs")->setDisplayed(false); }

		window.clear(sf::Color(40, 40, 40));
		window.draw(windowBorder);
		gui->drawAllContainer(&window);
		window.draw(linetop, 2, sf::Lines); window.draw(linemiddle, 2, sf::Lines); window.draw(linebottom, 2, sf::Lines);
		window.draw(*curs.getSprite());
		window.display();
	}
}

void LogQueue::push_back(std::string content)
{
	logContent.push_back(content);
	if (logContent.size() > 2)
		logContent.erase(logContent.begin());
}

void LogQueue::set(std::string content)
{
	logContent[logContent.size() - 1] = content;
}

std::string LogQueue::returnComplexText()
{
	std::string complexText;
	for (int i = 0; i < logContent.size(); i++)
	{
		complexText += logContent[i] + "\n";
	}
	return complexText;
}
