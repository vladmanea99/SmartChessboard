#include <iostream>
#include <SFML/Graphics.hpp>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <vector>


using namespace std;
using namespace sf;

httplib::Server svr;

class Piece {
private:
    int x;
    int y;
    string name;

public:
    Piece(int x, int y, string name) {
        this->x = x; this->y = y; this->name = name;
    }

    string getName() {
        return this->name;
    }

    int getX() {
        return this->x;
    }

    int getY() {
        return this->y;
    }
};

void drawBoard(RenderWindow* window) {
    sf::Texture texture;

    if (!texture.loadFromFile("chessboard.png"))
    {
        // error...
    }

    sf::Sprite sprite;
    sprite.setTexture(texture);
    
    window -> draw(sprite);
}

void drawPiece(RenderWindow* window, Piece piece) {
    sf::Texture pieceTexture;
    pieceTexture.loadFromFile("pieces/" + piece.getName() + ".png");

    sf::Sprite pieceSprite;
    pieceSprite.setTexture(pieceTexture);
    pieceSprite.setPosition(piece.getX() * 100, piece.getY() * 100);

    window -> draw(pieceSprite);
}

void initializePieces(vector<Piece> &pieces) {
    pieces.push_back(Piece(0, 0, "black_rook"));
    pieces.push_back(Piece(1, 0, "black_knight"));
    pieces.push_back(Piece(2, 0, "black_bishop"));
    pieces.push_back(Piece(3, 0, "black_queen"));
    pieces.push_back(Piece(4, 0, "black_king"));
    pieces.push_back(Piece(5, 0, "black_bishop"));
    pieces.push_back(Piece(6, 0, "black_knight"));
    pieces.push_back(Piece(7, 0, "black_rook"));
    pieces.push_back(Piece(0, 1, "black_pawn"));
    pieces.push_back(Piece(1, 1, "black_pawn"));
    pieces.push_back(Piece(2, 1, "black_pawn"));
    pieces.push_back(Piece(3, 1, "black_pawn"));
    pieces.push_back(Piece(4, 1, "black_pawn"));
    pieces.push_back(Piece(5, 1, "black_pawn"));
    pieces.push_back(Piece(6, 1, "black_pawn"));
    pieces.push_back(Piece(7, 1, "black_pawn"));

    pieces.push_back(Piece(0, 6, "white_pawn"));
    pieces.push_back(Piece(1, 6, "white_pawn"));
    pieces.push_back(Piece(2, 6, "white_pawn"));
    pieces.push_back(Piece(3, 6, "white_pawn"));
    pieces.push_back(Piece(4, 6, "white_pawn"));
    pieces.push_back(Piece(5, 6, "white_pawn"));
    pieces.push_back(Piece(6, 6, "white_pawn"));
    pieces.push_back(Piece(7, 6, "white_pawn"));

    pieces.push_back(Piece(0, 7, "white_rook"));
    pieces.push_back(Piece(1, 7, "white_knight"));
    pieces.push_back(Piece(2, 7, "white_bishop"));
    pieces.push_back(Piece(3, 7, "white_queen"));
    pieces.push_back(Piece(4, 7, "white_king"));
    pieces.push_back(Piece(5, 7, "white_bishop"));
    pieces.push_back(Piece(6, 7, "white_knight"));
    pieces.push_back(Piece(7, 7, "white_rook"));
}

void initializeBoard(RenderWindow* window, vector<Piece> &pieces) {
    for (auto & it : pieces) {
        drawPiece(window, it);
    }
}

int main() {

    sf::RenderWindow window(sf::VideoMode(800, 800), "My window");
    vector<Piece> pieces;

    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything here...
        drawBoard(&window);
        initializePieces(pieces);
        initializeBoard(&window, pieces);

        // end the current frame
        window.display();
    }

    svr.Get("/hi", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
        });
    svr.listen("127.0.0.1", 8080);
    return 0;
}