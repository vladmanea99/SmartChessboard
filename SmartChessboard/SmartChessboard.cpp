#include <iostream>
#include <SFML/Graphics.hpp>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <vector>
#include <map>
#include <string>
#include <typeinfo>
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

vector<vector<int>> startingBoard;

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

    void setX(int x) {
        this->x = x;
    }

    void setY(int y) {
        this->y = y;
    }
};

class Position {
private:
    vector<vector<int>> board;
    struct Info info;

public:
    Position() {
        this->board.resize(8);
        for (int i = 0; i < 8; i++) {
            this->board[i].resize(8);
        }
    }

    Position(vector<vector<int>> b, struct Info i) {
        this->board.resize(8);
        for (int i = 0; i < 8; i++) {
            this->board[i].resize(8);
        }

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                this->board[i][j] = b[i][j];
            }
        }
        this->info = i;
    }

    vector<vector<int>> getBoard() {
        return this->board;
    }

    struct Info getInfo() {
        return this->info;
    }

    void setBoard(int b[8][8]) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                this->board[i][j] = b[i][j];
            }
        }
    }

    void setInfo(struct Info i) {
        this->info = i;
    }
};

vector<Position> positions;
vector<Piece> pieces;

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
    pieces.push_back(Piece(0, 0, 6));
    pieces.push_back(Piece(1, 0, 3));
    pieces.push_back(Piece(2, 0, 1));
    pieces.push_back(Piece(3, 0, 5));
    pieces.push_back(Piece(4, 0, 2));
    pieces.push_back(Piece(5, 0, 1));
    pieces.push_back(Piece(6, 0, 3));
    pieces.push_back(Piece(7, 0, 6));
    pieces.push_back(Piece(0, 1, 4));
    pieces.push_back(Piece(1, 1, 4));
    pieces.push_back(Piece(2, 1, 4));
    pieces.push_back(Piece(3, 1, 4));
    pieces.push_back(Piece(4, 1, 4));
    pieces.push_back(Piece(5, 1, 4));
    pieces.push_back(Piece(6, 1, 4));
    pieces.push_back(Piece(7, 1, 4));

    pieces.push_back(Piece(0, 6, 10));
    pieces.push_back(Piece(1, 6, 10));
    pieces.push_back(Piece(2, 6, 10));
    pieces.push_back(Piece(3, 6, 10));
    pieces.push_back(Piece(4, 6, 10));
    pieces.push_back(Piece(5, 6, 10));
    pieces.push_back(Piece(6, 6, 10));
    pieces.push_back(Piece(7, 6, 10));
    pieces.push_back(Piece(0, 7, 12));
    pieces.push_back(Piece(1, 7, 9));
    pieces.push_back(Piece(2, 7, 7));
    pieces.push_back(Piece(3, 7, 11));
    pieces.push_back(Piece(4, 7, 8));
    pieces.push_back(Piece(5, 7, 7));
    pieces.push_back(Piece(6, 7, 9));
    pieces.push_back(Piece(7, 7, 12));

    startingBoard[0][0] = 6;
    startingBoard[0][1] = 3;
    startingBoard[0][2] = 1;
    startingBoard[0][3] = 5;
    startingBoard[0][4] = 2;
    startingBoard[0][5] = 1;
    startingBoard[0][6] = 3;
    startingBoard[0][7] = 6;
    startingBoard[1][0] = 4;
    startingBoard[1][1] = 4;
    startingBoard[1][2] = 4;
    startingBoard[1][3] = 4;
    startingBoard[1][4] = 4;
    startingBoard[1][5] = 4;
    startingBoard[1][6] = 4;
    startingBoard[1][7] = 4;

    startingBoard[6][0] = 10;
    startingBoard[6][1] = 10;
    startingBoard[6][2] = 10;
    startingBoard[6][3] = 10;
    startingBoard[6][4] = 10;
    startingBoard[6][5] = 10;
    startingBoard[6][6] = 10;
    startingBoard[6][7] = 10;
    startingBoard[7][0] = 12;
    startingBoard[7][1] = 9;
    startingBoard[7][2] = 7;
    startingBoard[7][3] = 11;
    startingBoard[7][4] = 8;
    startingBoard[7][5] = 7;
    startingBoard[7][6] = 9;
    startingBoard[7][7] = 12;
}

void drawPieces(RenderWindow* window, vector<Piece> &pieces) {
    for (auto & it : pieces) {
        drawPiece(window, it);
    }
}

void initializeMap(map<int, string> &map) {
    map.insert({ 1, "black_bishop" });
    map.insert({ 2, "black_king" });
    map.insert({ 3, "black_knight" });
    map.insert({ 4, "black_pawn" });
    map.insert({ 5, "black_queen" });
    map.insert({ 6, "black_rook" });
    map.insert({ 7, "white_bishop" });
    map.insert({ 8, "white_king" });
    map.insert({ 9, "white_knight" });
    map.insert({ 10, "white_pawn" });
    map.insert({ 11, "white_queen" });
    map.insert({ 12, "white_rook" });
}

void initializePiecesNamesMap(map<char, int>& map) {
    map.insert({ 'b', 1 });
    map.insert({ 'k', 2 });
    map.insert({ 'n', 3 });
    map.insert({ 'p', 4 });
    map.insert({ 'q', 5 });
    map.insert({ 'r', 6 });
    map.insert({ 'B', 7 });
    map.insert({ 'K', 8 });
    map.insert({ 'N', 9 });
    map.insert({ 'P', 10 });
    map.insert({ 'Q', 11 });
    map.insert({ 'R', 12 });
}

void getFENstring(string &str) {
    cout << "Enter FEN string:" << endl;
    getline(cin, str);
}

void processFENstring(string str, vector<vector<int>> mat, vector<Piece> &pieces, Info &info) {
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

void kingSideCastling(vector<vector<int>> b, struct Info& i) {
    if (i.turn == 'b') {
        for (auto& p : pieces) {
            if (p.getCode() == 2) {
                b[0][6] = 2;
                b[p.getY()][p.getX()] = 0;
                p.setY(0);
                p.setX(6);
            }
            if (p.getCode() == 6 && p.getX() == 7) {
                b[0][5] = 6;
                b[p.getY()][p.getX()] = 0;
                p.setY(0);
                p.setX(5);
            }
        }
        size_t p = i.castling.find("kq");
        if (p != string::npos) {
            i.castling.erase(p, 2);
        }
        p = i.castling.find("k");
        if (p != string::npos) {
            i.castling.erase(p, 1);
        }
        i.turn = 'w';
        i.halfmove++;
        i.fullmove++;
    }
    else {
        for (auto& p : pieces) {
            if (p.getCode() == 8) {
                b[7][6] = 8;
                b[p.getY()][p.getX()] = 0;
                p.setY(7);
                p.setX(6);
            }
            if (p.getCode() == 12 && p.getX() == 7) {
                b[7][5] = 12;
                b[p.getY()][p.getX()] = 0;
                p.setY(7);
                p.setX(5);
            }
        }
        size_t p = i.castling.find("KQ");
        if (p != string::npos) {
            i.castling.erase(p, 2);
        }
        p = i.castling.find("K");
        if (p != string::npos) {
            i.castling.erase(p, 1);
        }
        i.turn = 'b';
        i.halfmove++;
    }
}

void queenSideCastling(vector<vector<int>> b, struct Info& i) {
    if (i.turn == 'b') {
        for (auto& p : pieces) {
            if (p.getCode() == 2) {
                b[0][2] = 2;
                b[p.getY()][p.getX()] = 0;
                p.setY(0);
                p.setX(2);
            }
            if (p.getCode() == 6 && p.getX() == 0) {
                b[0][3] = 6;
                b[p.getY()][p.getX()] = 0;
                p.setY(0);
                p.setX(3);
            }
        }
        size_t p = i.castling.find("kq");
        if (p != string::npos) {
            i.castling.erase(p, 2);
        }
        p = i.castling.find("q");
        if (p != string::npos) {
            i.castling.erase(p, 1);
        }
        i.turn = 'w';
        i.halfmove++;
        i.fullmove++;
    }
    else {
        for (auto& p : pieces) {
            if (p.getCode() == 8) {
                b[7][2] = 8;
                b[p.getY()][p.getX()] = 0;
                p.setY(7);
                p.setX(2);
            }
            if (p.getCode() == 12 && p.getX() == 0) {
                b[7][3] = 12;
                b[p.getY()][p.getX()] = 0;
                p.setY(7);
                p.setX(3);
            }
        }
        size_t p = i.castling.find("KQ");
        if (p != string::npos) {
            i.castling.erase(p, 2);
        }
        p = i.castling.find("Q");
        if (p != string::npos) {
            i.castling.erase(p, 1);
        }
        i.turn = 'b';
        i.halfmove++;
    }
}

void updateCastlingRook(int endingLine, int endingColumn, struct Info& i) {
    size_t p = string::npos;
    if (i.turn == 'w') {
        if (endingColumn == 0) {
            p = i.castling.find("Q");
        }
        else if (endingColumn == 7) {
            p = i.castling.find("K");
        }
    }
    else if (i.turn == 'b') {
        if (endingColumn == 0) {
            p = i.castling.find("q");
        }
        else if (endingColumn == 7) {
            p = i.castling.find("k");
        }
    }
    if (p != string::npos) {
        i.castling.erase(p, 1);
    }
    if (i.castling == "") {
        i.castling = "-";
    }
}

void updateCastlingKing(int endingLine, int endingColumn, struct Info& i) {
    size_t p = string::npos;
    if (i.turn == 'w') {
        p = i.castling.find("KQ");
    }
    else if (i.turn == 'b') {
        p = i.castling.find("kq");
    }
    if (p != string::npos) {
        i.castling.erase(p, 2);
    }
    if (i.castling == "") {
        i.castling = "-";
    }
}

void searchAndMovePieceInVector(int endingLine, int endingColumn, int startingLine, int startingColumn, int code, vector<vector<int>>& b, struct Info& i) {
    for (auto& p : pieces) {
        if ((p.getY() == startingLine) && (p.getX() == startingColumn)) {
            p.setY(8 - endingLine);
            p.setX(endingColumn);
            b[8 - endingLine][endingColumn] = code;
            b[startingLine][startingColumn] = 0;
            i.enPassant = '-';
            if (code == 12 || code == 6) {
                updateCastlingRook(startingLine, startingColumn, i);
            }
            if (code == 8 || code == 2) {
                updateCastlingKing(startingLine, startingColumn, i);
            }
            i.turn = (i.turn == 'w') ? 'b' : 'w';
            i.halfmove++;
            if (i.turn == 'w') {
                i.fullmove++;
            }
            return;
        }
    }
}

boolean searchLineOrColumn(int endingLine, int endingColumn, int startingLine, int startingColumn, int code, vector<vector<int>>& b, struct Info& i) {
    if (startingColumn > -1) {
        for (int j = 0; j < 8; j++) {
            if (8 - endingLine + j < 8 && b[8 - endingLine + j][startingColumn] == code) {
                searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + j, startingColumn, code, b, i);
                return true;
            }
            if (8 - endingLine - j > -1 && b[8 - endingLine - j][startingColumn] == code) {
                searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - j, startingColumn, code, b, i);
                return true;
            }
        }
    }
    else if (startingLine > -1) {
        for (int j = 0; j < 8; j++) {
            if (endingColumn + j < 8 && b[8 - startingLine][endingColumn + j] == code) {
                searchAndMovePieceInVector(endingLine, endingColumn, 8 - startingLine, endingColumn + j, code, b, i);
                return true;
            }
            if (endingColumn - j > -1 && b[8 - startingLine][endingColumn - j] == code) {
                searchAndMovePieceInVector(endingLine, endingColumn, 8 - startingLine, endingColumn - j, code, b, i);
                return true;
            }
        }
    }
    else return false;
}

boolean searchLine(int endingLine, int endingColumn, int startingLine, int code, vector<vector<int>>& b, struct Info& i) {
    for (int j = 0; j < 8; j++) {
        if (endingColumn + j < 8 && b[8 - startingLine][endingColumn + j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - startingLine, endingColumn + j, code, b, i);
            return true;
        }
        if (endingColumn - j > -1 && b[8 - startingLine][endingColumn - j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - startingLine, endingColumn - j, code, b, i);
            return true;
        }
    }
    return false;
}

boolean searchColumn(int endingLine, int endingColumn, int startingColumn, int code, vector<vector<int>>& b, struct Info& i) {
    for (int j = 0; j < 8; j++) {
        if (8 - endingLine + j < 8 && b[8 - endingLine + j][startingColumn] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + j, startingColumn, code, b, i);
            return true;
        }
        if (8 - endingLine - j > -1 && b[8 - endingLine - j][startingColumn] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - j, startingColumn, code, b, i);
            return true;
        }
    }
    return false;
}

void pawnMove(vector<vector<int>>& b, struct Info& i, string it, int startingColumn = -1, char pieceName = 'a')
{
    int endingColumn = int(it[0]) - 97;
    int endingLine = it[1] - '0';
    int code = (i.turn == 'w') ? 10 : 4;

    if (startingColumn != -1 && searchColumn(endingLine, endingColumn, startingColumn, code, b, i)) {
        return;
    }

    if (i.turn == 'w') {
        bool breakCheck = false;
        for (int j = 8 - endingLine + 1; j < 8; j++) {        
            if (b[j][endingColumn] == 10) {
                for (auto& p : pieces) {
                    if ((p.getY() == j) && (p.getX() == endingColumn)) {
                        p.setY(8 - endingLine);
                        p.setX(endingColumn);
                        b[8 - endingLine][endingColumn] = (pieceName != 'a') ? piecesNamesMap[pieceName] : 10;
                        b[j][endingColumn] = 0;
                        i.enPassant = '-';
                        breakCheck = true;
                        break;
                    }
                }  
                if (breakCheck) {
                    if (j - (8 - endingLine) == 2) {
                        cout << "aici";
                        i.enPassant = it;
                    }
                    break;
                }
            }
        }
        i.turn = 'b';
        i.halfmove = 0;
        return;
    }

    if (i.turn == 'b') {
        bool breakCheck = false;
        for (int j = 8 - endingLine - 1; j > 0; j--) {
            if (b[j][endingColumn] == 4) {
                for (auto& p : pieces) {
                    if ((p.getY() == j) && (p.getX() == endingColumn)) {
                        p.setY(8 - endingLine);
                        p.setX(endingColumn);
                        b[8 - endingLine][endingColumn] = (pieceName != 'a') ? piecesNamesMap[tolower(pieceName)] : 4;
                        b[j][endingColumn] = 0;
                        i.enPassant = '-';
                        breakCheck = true;
                        break;
                    }
                }    
                if (breakCheck) {
                    if (j - (8 - endingLine) == -2) {
                        i.enPassant = it;
                    }
                    break;
                }
            }       
        }
        i.turn = 'w';
        i.halfmove = 0;
        i.fullmove++;
        return;
    }
}

void rookMove(vector<vector<int>>& b, struct Info& i, string it, int startingLine = -1, int startingColumn = -1) {
    int endingColumn = int(it[1]) - 97;
    int endingLine = it[2] - '0';
    int code = (i.turn == 'w') ? 12 : 6;


    if (startingLine != -1 && startingColumn != -1) {
        searchAndMovePieceInVector(endingLine, endingColumn, startingLine, startingColumn, code, b, i);
        return;
    }
    if (startingLine != -1 && searchLine(endingLine, endingColumn, startingLine, code, b, i)) {
        return;
    } 
    else if (startingColumn != -1 && searchColumn(endingLine, endingColumn, startingColumn, code, b, i)) {
        return;
    }

    bool goodDirections[4] = { true, true, true, true };

    for (int j = 0; j < 8; j++) {

        
        //Check if the direction we are going is good or not by checking if between the ending position and current position there is a piece other than the one we looking for
        if (goodDirections[0] && 8 - endingLine + j < 8 && b[8 - endingLine + j][endingColumn] != 0 && b[8 - endingLine + j][endingColumn] != code) {
            goodDirections[0] = false;
        }
        if (goodDirections[1] && 8 - endingLine - j > -1 && b[8 - endingLine - j][endingColumn] != 0 && b[8 - endingLine - j][endingColumn] != code) {
            goodDirections[1] = false;
        }
        if (goodDirections[2] && endingColumn + j < 8 && b[8 - endingLine][endingColumn + j] != 0 && b[8 - endingLine][endingColumn + j] != code) {
            goodDirections[2] = false;
        }
        if (goodDirections[3] && endingColumn - j > -1 && b[8 - endingLine][endingColumn - j] != 0 && b[8 - endingLine][endingColumn - j] != code) {
            goodDirections[3] = false;
        }

        
        if (goodDirections[0] && 8 - endingLine + j < 8 && b[8 - endingLine + j][endingColumn] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn, code, b, i);
            return;
        }
        if (goodDirections[1] && 8 - endingLine - j > -1 && b[8 - endingLine - j][endingColumn] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn, code, b, i);
            return;
        }
        if (goodDirections[2] &&  endingColumn + j < 8 && b[8 - endingLine][endingColumn + j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine, endingColumn + j, code, b, i);
            return;
        }
        if (goodDirections[3] &&  endingColumn - j > -1 && b[8 - endingLine][endingColumn - j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine, endingColumn - j, code, b, i);
            return;
        }
    }
}

void bishopMove(vector<vector<int>>& b, struct Info& i, string it, int startingLine = -1, int startingColumn = -1) {
    int endingColumn = int(it[1]) - 97;
    int endingLine = it[2] - '0';
    int code = (i.turn == 'w') ? 7 : 1;

    if (startingLine != -1 && startingColumn != -1) {
        searchAndMovePieceInVector(endingLine, endingColumn, startingLine, startingColumn, code, b, i);
        return;
    }
    if (startingLine != -1 && searchLine(endingLine, endingColumn, startingLine, code, b, i)) {
        return;
    }
    else if (startingColumn != -1 && searchColumn(endingLine, endingColumn, startingColumn, code, b, i)) {
        return;
    }

    bool goodDirections[4] = { true, true, true, true };

    for (int j = 0; j < 8; j++) {
        if (goodDirections[0] && 8 - endingLine + j < 8 && endingColumn + j < 8 && b[8 - endingLine + j][endingColumn + j] != 0 && b[8 - endingLine + j][endingColumn + j] != code) {
            goodDirections[0] = false;
        }
        if (goodDirections[1] && 8 - endingLine - j > -1 && endingColumn + j < 8 && b[8 - endingLine - j][endingColumn + j] != 0 && b[8 - endingLine - j][endingColumn + j] != code) {
            goodDirections[1] = false;
        }
        if (goodDirections[2] && 8 - endingLine + j < 8 && endingColumn - j > -1 && b[8 - endingLine + j][endingColumn - j] != 0 && b[8 - endingLine + j][endingColumn - j] != code) {
            goodDirections[2] = false;
        }
        if (goodDirections[3] && 8 - endingLine - j > -1 && endingColumn - j > -1 && b[8 - endingLine - j][endingColumn - j] != 0 && b[8 - endingLine - j][endingColumn - j] != code) {
            goodDirections[3] = false;
        }
        if (goodDirections[0] && 8 - endingLine + j < 8 && endingColumn + j < 8 && b[8 - endingLine + j][endingColumn + j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn + j, code, b, i);
            return;
        }
        if (goodDirections[1] && 8 - endingLine - j > -1 && endingColumn + j < 8 && b[8 - endingLine - j][endingColumn + j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn + j, code, b, i);
            return;
        }
        if (goodDirections[2] &&  8 - endingLine + j < 8 && endingColumn - j > -1 && b[8 - endingLine + j][endingColumn - j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn - j, code, b, i);
            return;
        }
        if (goodDirections[3] &&  8 - endingLine - j > -1 && endingColumn - j > -1 && b[8 - endingLine - j][endingColumn - j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn - j, code, b, i);
            return;
        }
    }
}

void queenMove(vector<vector<int>>& b, struct Info& i, string it, int startingLine = -1, int startingColumn = -1) {
    int endingColumn = int(it[1]) - 97;
    int endingLine = it[2] - '0';
    int code = (i.turn == 'w') ? 11 : 5;

    if (startingLine != -1 && startingColumn != -1) {
        searchAndMovePieceInVector(endingLine, endingColumn, 8 - startingLine, startingColumn, code, b, i);
        return;
    }
    if (startingLine != -1 && searchLine(endingLine, endingColumn, startingLine, code, b, i)) {
        return;
    }
    else if (startingColumn != -1 && searchColumn(endingLine, endingColumn, startingColumn, code, b, i)) {
        return;
    }

    bool goodDirections[8] = { true, true, true, true, true, true, true, true };

    for (int j = 0; j < 8; j++) {
        if (goodDirections[0] && 8 - endingLine + j < 8 && b[8 - endingLine + j][endingColumn] != 0 && b[8 - endingLine + j][endingColumn] != code) {
            goodDirections[0] = false;
        }
        if (goodDirections[1] && 8 - endingLine - j > -1 && b[8 - endingLine - j][endingColumn] != 0 && b[8 - endingLine - j][endingColumn] != code) {
            goodDirections[1] = false;
        }
        if (goodDirections[2] && endingColumn + j < 8 && b[8 - endingLine][endingColumn + j] != 0 && b[8 - endingLine][endingColumn + j] != code) {
            goodDirections[2] = false;
        }
        if (goodDirections[3] && endingColumn - j > -1 && b[8 - endingLine][endingColumn - j] != 0 && b[8 - endingLine][endingColumn - j] != code) {
            goodDirections[3] = false;
        }
        if (goodDirections[4] && 8 - endingLine + j < 8 && endingColumn + j < 8 && b[8 - endingLine + j][endingColumn + j] != 0 && b[8 - endingLine + j][endingColumn + j] != code) {
            goodDirections[4] = false;
        }
        if (goodDirections[5] && 8 - endingLine - j > -1 && endingColumn + j < 8 && b[8 - endingLine - j][endingColumn + j] != 0 && b[8 - endingLine - j][endingColumn + j] != code) {
            goodDirections[5] = false;
        }
        if (goodDirections[6] && 8 - endingLine + j < 8 && endingColumn - j > -1 && b[8 - endingLine + j][endingColumn - j] != 0 && b[8 - endingLine + j][endingColumn - j] != code) {
            goodDirections[6] = false;
        }
        if (goodDirections[7] && 8 - endingLine - j > -1 && endingColumn - j > -1 && b[8 - endingLine - j][endingColumn - j] != 0 && b[8 - endingLine - j][endingColumn - j] != code) {
            goodDirections[7] = false;
        }
        
        if (goodDirections[0] && 8 - endingLine + j < 8 && b[8 - endingLine + j][endingColumn] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn, code, b, i);
            return;
        }
        if (goodDirections[1] && 8 - endingLine - j > -1 && b[8 - endingLine - j][endingColumn] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn, code, b, i);
            return;
        }
        if (goodDirections[2] && endingColumn + j < 8 && b[8 - endingLine][endingColumn + j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine, endingColumn + j, code, b, i);
            return;
        }
        if (goodDirections[3] && endingColumn - j > -1 && b[8 - endingLine][endingColumn - j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine, endingColumn - j, code, b, i);
            return;
        }
        if (goodDirections[4] && 8 - endingLine + j < 8 && endingColumn + j < 8 && b[8 - endingLine + j][endingColumn + j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn + j, code, b, i);
            return;
        }
        if (goodDirections[5] && 8 - endingLine - j > -1 && endingColumn + j < 8 && b[8 - endingLine - j][endingColumn + j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn + j, code, b, i);
            return;
        }
        if (goodDirections[6] && 8 - endingLine + j < 8 && endingColumn - j > -1 && b[8 - endingLine + j][endingColumn - j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn - j, code, b, i);
            return;
        }
        if (goodDirections[7] && 8 - endingLine - j > -1 && endingColumn - j > -1 && b[8 - endingLine - j][endingColumn - j] == code) {
            searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn - j, code, b, i);
            return;
        }
    }
}

void kingMove(vector<vector<int>>& b, struct Info& i, string it) {
    int endingColumn = int(it[1]) - 97;
    int endingLine = it[2] - '0';
    int code = (i.turn == 'w') ? 8 : 2;
    int j = 1;

    if (8 - endingLine + j < 8 && b[8 - endingLine + j][endingColumn] == code) {
        searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn, code, b, i);
        return;
    }
    if (8 - endingLine - j > -1 && b[8 - endingLine - j][endingColumn] == code) {
        searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn, code, b, i);
        return;
    }
    if (endingColumn + j < 8 && b[8 - endingLine][endingColumn + j] == code) {
        searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine, endingColumn + j, code, b, i);
        return;
    }
    if (endingColumn - j > -1 && b[8 - endingLine][endingColumn - j] == code) {
        searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine, endingColumn - j, code, b, i);
        return;
    }
    if (8 - endingLine + j < 8 && endingColumn + j < 8 && b[8 - endingLine + j][endingColumn + j] == code) {
        searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn + j, code, b, i);
        return;
    }
    if (8 - endingLine - j > -1 && endingColumn + j < 8 && b[8 - endingLine - j][endingColumn + j] == code) {
        searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn + j, code, b, i);
        return;
    }
    if (8 - endingLine + j < 8 && endingColumn - j > -1 && b[8 - endingLine + j][endingColumn - j] == code) {
        searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn - j, code, b, i);
        return;
    }
    if (8 - endingLine - j > -1 && endingColumn - j > -1 && b[8 - endingLine - j][endingColumn - j] == code) {
        searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn - j, code, b, i);
        return;
    }
}

void knightMove(vector<vector<int>>& b, struct Info& i, string it, int startingLine = -1, int startingColumn = -1) {
    int endingColumn = int(it[1]) - 97;
    int endingLine = it[2] - '0';
    int code = (i.turn == 'w') ? 9 : 3;

    if (startingLine != -1 && startingColumn != -1) {
        searchAndMovePieceInVector(endingLine, endingColumn, startingLine, startingColumn, code, b, i);
        return;
    }
    if (startingLine != -1 && searchLine(endingLine, endingColumn, startingLine, code, b, i)) {
        return;
    }
    else if (startingColumn != -1 && searchColumn(endingLine, endingColumn, startingColumn, code, b, i)) {
        return;
    }

    for (int j = endingColumn - 2; j <= endingColumn + 2; j++) {
        if (j > -1 && j < 8 && j != endingColumn) {
            if (8 - endingLine - 2 > -1 && 8 - endingLine - 2 < 8 && b[8 - endingLine - 2][j] == code) {
                searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - 2, j, code, b, i);
                return;
            }
            if (8 - endingLine + 2 > -1 && 8 - endingLine + 2 < 8 && b[8 - endingLine + 2][j] == code) {
                searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + 2, j, code, b, i);
                return;
            }
            if (8 - endingLine - 1 > -1 && 8 - endingLine - 2 < 8 && b[8 - endingLine - 1][j] == code) {
                searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine - 1, j, code, b, i);
                return;
            }
            if (8 - endingLine + 1 > -1 && 8 - endingLine + 1 < 8 && b[8 - endingLine + 1][j] == code) {
                searchAndMovePieceInVector(endingLine, endingColumn, 8 - endingLine + 1, j, code, b, i);
                return;
            }
        }
    }
}

void pawnTakes(vector<vector<int>>& b, struct Info& i, string it) {
    int startingColumn = int(it[0]) - 97;
    it = it.substr(2, 2);  // tai din mutare coloana precizata si caracterul 'x'
    // sterg din vectorul de piese piesa care va fi luata
    int index = 0;
    int endingColumn = int(it[0]) - 97;
    int endingLine = it[1] - '0';
    for (auto& p : pieces) {
        if (p.getX() == endingColumn && p.getY() == 8 - endingLine) {
            pieces.erase(pieces.begin() + index);
        }
        index++;
    }
    pawnMove(b, i, it, startingColumn);
}

void removePiece(int endingLine, int endingColumn, vector<vector<int>>& b) {
    int index = 0;
    for (auto& p : pieces) {
        if (p.getX() == endingColumn && p.getY() == 8 - endingLine) {
            pieces.erase(pieces.begin() + index);
            b[8 - endingLine][endingColumn] = 0;
            break;
        }
        index++;
    }
}

void listProcessing(vector<string> list) {
    for (auto& it : list) {
        vector<vector<int>> b;
        struct Info i;
        b.resize(8);
        for (int i = 0; i < 8; i++) {
            b[i].resize(8);
        }

        if (positions.size() == 0) {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    b[i][j] = startingBoard[i][j];
                }
            }
            i.turn = 'w';
            i.castling = "KQkq";
            i.enPassant = "-";
            i.halfmove = 0;
            i.fullmove = 1;
        }
        else {
            vector<vector<int>> lastBoard = positions.at(positions.size() - 1).getBoard();
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    b[i][j] = lastBoard[i][j];
                }
            }
            i = positions.at(positions.size() - 1).getInfo();
        }
        //+ = sah, # = sah mat
        if (it[it.size() - 1] == '+' || it[it.size() - 1] == '#') {
            it = it.erase(it.size() - 1, 1);
        }
        if (it == "O-O") {
            kingSideCastling(b, i);
        }
        else if (it == "O-O-O") {
            queenSideCastling(b, i);
        }
        else if (it.size() == 2) {
            pawnMove(b, i, it);
        }
        else if (it.size() == 3) {
            if (it[it.size() - 1] == '+') {
                pawnMove(b, i, it);
            }
            else if (it == "1-0" || it == "0-1") {
                cout << "Meciul s-a terminat.";
            }
            else {
                if (it[0] == 'R') {
                    rookMove(b, i, it);
                }
                else if (it[0] == 'B') {
                    bishopMove(b, i, it);
                }
                else if (it[0] == 'Q') {
                    queenMove(b, i, it);
                }
                else if (it[0] == 'K') {
                    kingMove(b, i, it);
                }
                else if (it[0] = 'N') {
                    knightMove(b, i, it);
                }
            }
        }
        else if (it.size() == 4) {
            if (it[1] == 'x') {
                if (isalpha(it[0]) && isupper(it[0])) {
                    it = it.erase(1, 1);
                    int endingColumn = int(it[1]) - 97;
                    int endingLine = it[2] - '0';
                    removePiece(endingLine, endingColumn, b);
                    if (it[0] == 'R') {
                        rookMove(b, i, it);
                    }
                    else if (it[0] == 'B') {
                        bishopMove(b, i, it);
                    }
                    else if (it[0] == 'Q') {
                        queenMove(b, i, it);
                    }
                    else if (it[0] == 'K') {
                        kingMove(b, i, it);
                    }
                    else if (it[0] = 'N') {
                        knightMove(b, i, it);
                    }
                }
                else {
                    pawnTakes(b, i, it);
                }
            }
            else if (it[2] == '=') {
                int endingColumn = int(it[0]) - 97;
                int endingLine = it[1] - '0';
                pawnMove(b, i, it);
                removePiece(endingLine, endingColumn, b);
                i.halfmove = 0;
                if (it[3] == 'Q') {
                    int code = -1;
                    if (i.turn != 'w') {
                        code = 11;
                    }
                    else {
                        code = 5;
                    }
                    pieces.push_back(Piece(endingColumn, 8 - endingLine, code));
                    b[8 - endingLine][endingColumn] = code;
                }
                if (it[3] == 'R') {
                    int code = -1;
                    if (i.turn != 'w') {
                        code = 12;
                    }
                    else {
                        code = 6;
                    }
                    pieces.push_back(Piece(endingColumn, 8 - endingLine, code));
                    b[8 - endingLine][endingColumn] = code;
                }
                if (it[3] == 'B') {
                    int code = -1;
                    if (i.turn != 'w') {
                        code = 7;
                    }
                    else {
                        code = 1;
                    }
                    pieces.push_back(Piece(endingColumn, 8 - endingLine, code));
                    b[8 - endingLine][endingColumn] = code;
                }
                if (it[3] == 'N') {
                    int code = -1;
                    if (i.turn != 'w') {
                        code = 9;
                    }
                    else {
                        code = 3;
                    }
                    pieces.push_back(Piece(endingColumn, 8 - endingLine, code));
                    b[8 - endingLine][endingColumn] = code;
                }
                // pawnMove(b, i, it, it[3]);
            }
            else {
                int startingColumn = -1, startingLine = -1;
                if (isdigit(it[1])) {
                    startingLine = it[1] - '0';
                }
                else if(isalpha(it[1])) {
                    startingColumn = int(it[1]) - 97;
                }

                if (it[0] == 'R') {
                    it = it.substr(1, 3);
                    rookMove(b, i, it, startingLine, startingColumn);
                }
                else if (it[0] == 'B') {
                    it = it.substr(1, 3);
                    bishopMove(b, i, it, startingLine, startingColumn);
                }
                else if (it[0] == 'Q') {
                    it = it.substr(1, 3);
                    queenMove(b, i, it, startingLine, startingColumn);
                }
                else if (it[0] = 'N') {
                    it = it.substr(1, 3);
                    knightMove(b, i, it, startingLine, startingColumn);
                }
            }
        }
        else if (it.size() == 5) {
            if (it[2] == 'x') {
                int startingColumn = -1, startingLine = -1;
                if (isdigit(it[1])) {
                    startingLine = it[1] - '0';
                }
                else if (isalpha(it[1])) {
                    startingColumn = int(it[1]) - 97;
                }
                int endingColumn = int(it[3]) - 97;
                int endingLine = it[4] - '0';
                removePiece(endingLine, endingColumn, b);
                char piece = it[0];
                it = it.substr(2, 3);
                if (piece == 'R') {
                    rookMove(b, i, it, startingLine, startingColumn);
                }
                else if (piece == 'B') {
                    bishopMove(b, i, it, startingLine, startingColumn);
                }
                else if (piece == 'Q') {
                    queenMove(b, i, it, startingLine, startingColumn);
                }
                else if (piece == 'N') {
                    knightMove(b, i, it, startingLine, startingColumn);
                }
                i.halfmove = 0;
            }
            else if (isalpha(it[1]) && isdigit(it[2])) {
                int startingColumn = int(it[1]) - 97;
                int startingLine = it[2] - '0';
                char piece = it[0];
                it = it.substr(2, 3);
                if (piece == 'R') {
                    rookMove(b, i, it, startingLine, startingColumn);
                }
                else if (piece == 'B') {
                    bishopMove(b, i, it, startingLine, startingColumn);
                }
                else if (piece == 'Q') {
                    queenMove(b, i, it, startingLine, startingColumn);
                }
                else if (piece == 'N') {
                    knightMove(b, i, it, startingLine, startingColumn);
                }
            }
        }
        else if (it.size() == 6) {
            if (it[1] == 'x' && it[4] == '=') {
                int endingColumn = int(it[2]) - 97;
                int endingLine = it[3] - '0';
                int startingColumn = int(it[0]) - 97;
                int startingLine = 1;
                int index = 0;
                for (auto& piece : pieces) {
                    if (piece.getX() == startingColumn && piece.getY() == startingLine) {
                        pieces.erase(pieces.begin() + index);
                        b[startingLine][startingColumn] = 0;
                        break;
                    }
                    index++;
                }
                removePiece(endingLine, endingColumn, b);

                if (it[5] == 'Q') {
                    int code = -1;
                    if (i.turn == 'w') {
                        code = 11;
                    }
                    else {
                        code = 5;
                    }
                    pieces.push_back(Piece(endingColumn, 8 - endingLine, code));
                    b[8 - endingLine][endingColumn] = code;
                }
                if (it[5] == 'R') {
                    int code = -1;
                    if (i.turn == 'w') {
                        code = 12;
                    }
                    else {
                        code = 6;
                    }
                    pieces.push_back(Piece(endingColumn, 8 - endingLine, code));
                    b[8 - endingLine][endingColumn] = code;
                }
                if (it[5] == 'B') {
                    int code = -1;
                    if (i.turn == 'w') {
                        code = 7;
                    }
                    else {
                        code = 1;
                    }
                    pieces.push_back(Piece(endingColumn, 8 - endingLine, code));
                    b[8 - endingLine][endingColumn] = code;
                }
                if (it[5] == 'N') {
                    int code = -1;
                    if (i.turn == 'w') {
                        code = 9;
                    }
                    else {
                        code = 3;
                    }
                    pieces.push_back(Piece(endingColumn, 8 - endingLine, code));
                    b[8 - endingLine][endingColumn] = code;
                }
                i.turn = (i.turn == 'w') ? 'b' : 'w';
                i.halfmove = 0;
                if (i.turn == 'w') {
                    i.fullmove++;
                }
            }
            else if (it[3] == 'x' && isalpha(it[1]) && isdigit(it[2])) {

                int startingColumn = int(it[1]) - 97;
                int startingLine = it[2] - '0';

                char piece = it[0];

                int endingColumn = int(it[4]) - 97;
                int endingLine = it[5] - '0';

                removePiece(endingLine, endingColumn, b);

                it = it.substr(3, 3);
                if (piece == 'R') {
                    rookMove(b, i, it, startingLine, startingColumn);
                }
                else if (piece == 'B') {
                    bishopMove(b, i, it, startingLine, startingColumn);
                }
                else if (piece == 'Q') {
                    queenMove(b, i, it, startingLine, startingColumn);
                }
                else if (piece == 'N') {
                    knightMove(b, i, it, startingLine, startingColumn);
                }
            }
        }
        if (i.castling == "") {
            i.castling = "-";
        }

        Position pos = Position(b, i);
        positions.push_back(pos);
    }
}

int evaluatePosition(vector<vector<int>>& b) {
    int evaluation = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            switch (b[i][j]) {
            case (1):
                evaluation -= 3;
                break;
            case (3):
                evaluation -= 3;
                break;
            case(4):
                evaluation -= 1;
                break;
            case(5):
                evaluation -= 9;
                break;
            case(6):
                evaluation -= 5;
                break;
            case (7):
                evaluation += 3;
                break;
            case (9):
                evaluation += 3;
                break;
            case(10):
                evaluation += 1;
                break;
            case(11):
                evaluation += 9;
                break;
            case(12):
                evaluation += 5;
                break;
            }
        }
    }

    return evaluation;
}

vector<Piece> generatePiecesVector(vector<vector<int>>& b) {
    vector<Piece> piecesArr;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (b[i][j]) {
                piecesArr.push_back(Piece(j, i, b[i][j]));
            }
        }
    }

    return piecesArr;
}

void createImage(RenderWindow* window, vector<vector<int>>& b) {
    vector<Piece> piecesArr = generatePiecesVector(b);

    Texture texture;
    texture.create(window->getSize().x, window->getSize().y);
    window->clear();
    drawBoard(window);
    drawPieces(window, piecesArr);
    texture.update(*window);
    texture.copyToImage().saveToFile("pozitie.png");
}


int main() {
    startingBoard.resize(8);
    for (int i = 0; i < 8; i++) {
        startingBoard[i].resize(8);
    }

    sf::RenderWindow window(sf::VideoMode(800, 800), "My window");
    initializeMap(piecesMap);
    initializePiecesNamesMap(piecesNamesMap);
    initializePieces(pieces);

    string FEN;
    vector<vector<int>> piecesMat;
    piecesMat.resize(8);
    for (int i = 0; i < 8; i++) {
        piecesMat[i].resize(8);
    }
    vector<Piece> FENPieces;
    Info FENInfo;
    // getFENstring(FEN);
    // processFENstring(FEN, piecesMat, FENPieces, FENInfo);
    
    vector<string> moves;
    /*
    moves.push_back("b4");
    moves.push_back("a5");
    moves.push_back("bxa5");
    moves.push_back("Ra7");
    moves.push_back("a3");
    moves.push_back("Ra6");
    moves.push_back("h3");
    moves.push_back("Rh6");
    */

    /*
    moves.push_back("h4");
    moves.push_back("Na6");
    moves.push_back("h5");
    moves.push_back("Rb8");
    moves.push_back("h6");
    moves.push_back("Ra8");
    moves.push_back("hxg7");
    moves.push_back("h6");
    moves.push_back("g4");
    moves.push_back("Nf6");
    moves.push_back("g8=Q");
    moves.push_back("Rb8");
    moves.push_back("Qh7");
    moves.push_back("Ra8");
    moves.push_back("g5");
    moves.push_back("Rb8");
    moves.push_back("g6");
    moves.push_back("Ra8");
    moves.push_back("g7");
    moves.push_back("Rb8");
    moves.push_back("g8=Q");
    moves.push_back("Ra8");
    moves.push_back("e4");
    moves.push_back("Rb8");
    moves.push_back("Qgg4");
    moves.push_back("Ra8");
    moves.push_back("Qh4");
    moves.push_back("Rb8");
    moves.push_back("Qg6");
    moves.push_back("Ra8");
    moves.push_back("Qgg5");
    moves.push_back("Rb8");
    moves.push_back("Qdh5");
    moves.push_back("Ng4");
    moves.push_back("Qh5xg4");
    */
    moves.push_back("h4");
    moves.push_back("Na6");
    moves.push_back("h5");
    moves.push_back("Rb8");
    moves.push_back("h6");
    moves.push_back("Ra8");
    moves.push_back("hxg7");
    moves.push_back("Rb8");
    moves.push_back("gxh8=Q");
    moves.push_back("Ra8");
    moves.push_back("Qxg8");
    moves.push_back("Rb8");
    moves.push_back("Rxh7");
    moves.push_back("Ra8");
    moves.push_back("Rh8");
    moves.push_back("Rb8");
    moves.push_back("Qxf8#");
    moves.push_back("1-0");


    listProcessing(moves);
    Position lastPos = positions.at(positions.size() - 1);
    vector<vector<int>> b = positions.at(7).getBoard();

    pieces = generatePiecesVector(b);

    createImage(&window, b);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout << b[i][j] << ' ';
        }
        cout << endl;
    }

    for (int i = 0; i < positions.size(); i++) {
        cout << positions.at(i).getInfo().turn;
        cout << "\n";
        cout << positions.at(i).getInfo().castling;
        cout << "\n";
        cout << positions.at(i).getInfo().enPassant;
        cout << "\n";
        cout << positions.at(i).getInfo().halfmove;
        cout << "\n";
        cout << positions.at(i).getInfo().fullmove;
        cout << "\n";
        cout << "***************************";
        cout << "\n";

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