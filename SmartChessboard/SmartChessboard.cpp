#include <iostream>
#include <SFML/Graphics.hpp>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>



using namespace std;
using namespace sf;

httplib::Server svr;



void drawCircle(RenderWindow* window) {
    CircleShape shape(100.f);
    shape.setFillColor(Color::Green);

    Texture texture;
    texture.create(window->getSize().x, window->getSize().y);
    window->clear();
    window->draw(shape);
    texture.update(*window);
    texture.copyToImage().saveToFile("imagine3.png");
}

int main() {

    svr.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
        });
    svr.listen("127.0.0.1", 8080);
    return 0;
}