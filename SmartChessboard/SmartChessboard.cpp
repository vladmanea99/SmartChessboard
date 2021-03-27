#include <iostream>
#include <SFML/Graphics.hpp>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <vector>
#include <map>
#include <string>

#include <stdlib.h>
#include <base64.h>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace sf;
using namespace httplib;
using namespace cv;

httplib::Server svr;

map<int, string> piecesMap;
map<char, int> piecesNamesMap;

int board[8][8] = { 0 };

struct Info {
    char turn;
    string castling;
    string enPassant;
    int halfmove;
    int fullmove;
};

class Piece {
private:
    int x;
    int y;
    int code;

public:
    Piece(int x, int y, int code) {
        this->x = x; this->y = y; this->code = code;
    }

    int getCode() {
        return this->code;
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
    string name = piecesMap[piece.getCode()];

    pieceTexture.loadFromFile("pieces/" + name + ".png");

    sf::Sprite pieceSprite;
    pieceSprite.setTexture(pieceTexture);
    pieceSprite.setPosition(piece.getX() * 100, piece.getY() * 100);

    window -> draw(pieceSprite);
}

void initializePieces(vector<Piece> &pieces) {
    pieces.push_back(Piece(0, 0, 5));
    pieces.push_back(Piece(1, 0, 2));
    pieces.push_back(Piece(2, 0, 0));
    pieces.push_back(Piece(3, 0, 4));
    pieces.push_back(Piece(4, 0, 1));
    pieces.push_back(Piece(5, 0, 0));
    pieces.push_back(Piece(6, 0, 2));
    pieces.push_back(Piece(7, 0, 5));
    pieces.push_back(Piece(0, 1, 3));
    pieces.push_back(Piece(1, 1, 3));
    pieces.push_back(Piece(2, 1, 3));
    pieces.push_back(Piece(3, 1, 3));
    pieces.push_back(Piece(4, 1, 3));
    pieces.push_back(Piece(5, 1, 3));
    pieces.push_back(Piece(6, 1, 3));
    pieces.push_back(Piece(7, 1, 3));

    pieces.push_back(Piece(0, 6, 9));
    pieces.push_back(Piece(1, 6, 9));
    pieces.push_back(Piece(2, 6, 9));
    pieces.push_back(Piece(3, 6, 9));
    pieces.push_back(Piece(4, 6, 9));
    pieces.push_back(Piece(5, 6, 9));
    pieces.push_back(Piece(6, 6, 9));
    pieces.push_back(Piece(7, 6, 9));
    pieces.push_back(Piece(0, 7, 11));
    pieces.push_back(Piece(1, 7, 8));
    pieces.push_back(Piece(2, 7, 6));
    pieces.push_back(Piece(3, 7, 10));
    pieces.push_back(Piece(4, 7, 7));
    pieces.push_back(Piece(5, 7, 6));
    pieces.push_back(Piece(6, 7, 8));
    pieces.push_back(Piece(7, 7, 11));

    board[0][0] = 5;
    board[0][1] = 2;
    board[0][2] = 0;
    board[0][3] = 4;
    board[0][4] = 1;
    board[0][5] = 0;
    board[0][6] = 2;
    board[0][7] = 5;
    board[1][0] = 3;
    board[1][1] = 3;
    board[1][2] = 3;
    board[1][3] = 3;
    board[1][4] = 3;
    board[1][5] = 3;
    board[1][6] = 3;
    board[1][7] = 3;

    board[6][0] = 9;
    board[6][1] = 9;
    board[6][2] = 9;
    board[6][3] = 9;
    board[6][4] = 9;
    board[6][5] = 9;
    board[6][6] = 9;
    board[6][7] = 9;
    board[7][0] = 11;
    board[7][1] = 8;
    board[7][2] = 6;
    board[7][3] = 10;
    board[7][4] = 7;
    board[7][5] = 6;
    board[7][6] = 8;
    board[7][7] = 11;
}

void drawPieces(RenderWindow* window, vector<Piece> &pieces) {
    for (auto & it : pieces) {
        drawPiece(window, it);
    }
}

void initializeMap(map<int, string> &map) {
    map.insert({ 0, "black_bishop" });
    map.insert({ 1, "black_king" });
    map.insert({ 2, "black_knight" });
    map.insert({ 3, "black_pawn" });
    map.insert({ 4, "black_queen" });
    map.insert({ 5, "black_rook" });
    map.insert({ 6, "white_bishop" });
    map.insert({ 7, "white_king" });
    map.insert({ 8, "white_knight" });
    map.insert({ 9, "white_pawn" });
    map.insert({ 10, "white_queen" });
    map.insert({ 11, "white_rook" });
}

void initializePiecesNamesMap(map<char, int>& map) {
    map.insert({ 'b', 0 });
    map.insert({ 'k', 1 });
    map.insert({ 'n', 2 });
    map.insert({ 'p', 3 });
    map.insert({ 'q', 4 });
    map.insert({ 'r', 5 });
    map.insert({ 'B', 6 });
    map.insert({ 'K', 7 });
    map.insert({ 'N', 8 });
    map.insert({ 'P', 9 });
    map.insert({ 'Q', 10 });
    map.insert({ 'R', 11 });
}

void getFENstring(string &str) {
    cout << "Enter FEN string:" << endl;
    getline(cin, str);
}

void processFENstring(string str, int mat[][8], vector<Piece> &pieces, Info &info) {
    int i = 0;
    int j = 0;
    int idx = 0;
    for (idx = 0; idx < str.length() && str[idx] != ' '; idx++) {
        if (isalpha(str[idx])) {
            const int code = piecesNamesMap[str[idx]];
            mat[i][j] = code;
            Piece piece = Piece(j, i, code);
            pieces.push_back(piece);
            j++;
        }
        else if (isdigit(str[idx])) {
            j = j + (str[idx] - '0');
        }
        else if (str[idx] == '/') {
            i++;
            j = 0;
        }
    }

    idx++;
    string substr = str.substr(idx, str.length() - idx);
    istringstream ss(substr);
    string word;
    vector<string> infoArr;

    while (ss >> word) infoArr.push_back(word);

    info.turn = infoArr[0][0];
    info.castling = infoArr[1];
    info.enPassant = infoArr[2];
    info.halfmove = stoi(infoArr[3]);
    info.fullmove = stoi(infoArr[4]);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "My window");
    vector<Piece> pieces;
    initializeMap(piecesMap);
    initializePiecesNamesMap(piecesNamesMap);
    string FEN;
    int piecesMat[8][8] = { 0 };
    vector<Piece> FENPieces;
    Info FENInfo;
    getFENstring(FEN);
    processFENstring(FEN, piecesMat, FENPieces, FENInfo);
    
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
        // initializePieces(pieces);
        drawPieces(&window, FENPieces);

        // end the current frame
        window.display();
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout << piecesMat[i][j] << ' ';
        }
        cout << endl;
    }
    
    /*svr.Get("/hi", [&](const Request& req, Response& res) {
        string image_path = "chessboard.png";
        Mat img = cv::imread(image_path, IMREAD_COLOR);
        string encoded_png;

        vector<uchar> buf;
        cv::imencode(".png", img, buf);
        auto base64_png = reinterpret_cast<const unsigned char*>(buf.data());
        encoded_png = "data:image/jpeg;base64," + base64_encode(base64_png, buf.size());

        string ceva = "<img src= " + encoded_png + ">";
        res.set_content(ceva, "text/html");
        });

    svr.listen("127.0.0.1", 8080);*/

    return 0;
}