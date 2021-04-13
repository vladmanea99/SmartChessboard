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

void searchPieceInVector(int endingLine, int endingColumn, int startingLine, int startingColumn, int code, vector<vector<int>>& b, struct Info& i) {
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
            i.fullmove++;
            return;
        }
    }
}

boolean searchLineOrColumn(int endingLine, int endingColumn, int startingLine, int startingColumn, int code, vector<vector<int>>& b, struct Info& i) {
    if (startingColumn > -1) {
        for (int j = 0; j < 8; j++) {
            if (8 - endingLine + j < 8 && b[8 - endingLine + j][startingColumn] == code) {
                searchPieceInVector(endingLine, endingColumn, 8 - endingLine + j, startingColumn, code, b, i);
                return true;
            }
            if (8 - endingLine - j > -1 && b[8 - endingLine - j][startingColumn] == code) {
                searchPieceInVector(endingLine, endingColumn, 8 - endingLine - j, startingColumn, code, b, i);
                return true;
            }
        }
    }
    else if (startingLine > -1) {
        for (int j = 0; j < 8; j++) {
            if (endingColumn + j < 8 && b[8 - startingLine][endingColumn + j] == code) {
                searchPieceInVector(endingLine, endingColumn, 8 - startingLine, endingColumn + j, code, b, i);
                return true;
            }
            if (endingColumn - j > -1 && b[8 - startingLine][endingColumn - j] == code) {
                searchPieceInVector(endingLine, endingColumn, 8 - startingLine, endingColumn - j, code, b, i);
                return true;
            }
        }
    }
    else return false;
}

boolean searchLine(int endingLine, int endingColumn, int startingLine, int code, vector<vector<int>>& b, struct Info& i) {
    for (int j = 0; j < 8; j++) {
        if (endingColumn + j < 8 && b[8 - startingLine][endingColumn + j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - startingLine, endingColumn + j, code, b, i);
            return true;
        }
        if (endingColumn - j > -1 && b[8 - startingLine][endingColumn - j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - startingLine, endingColumn - j, code, b, i);
            return true;
        }
    }
    return false;
}

boolean searchColumn(int endingLine, int endingColumn, int startingColumn, int code, vector<vector<int>>& b, struct Info& i) {
    for (int j = 0; j < 8; j++) {
        if (8 - endingLine + j < 8 && b[8 - endingLine + j][startingColumn] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine + j, startingColumn, code, b, i);
            return true;
        }
        if (8 - endingLine - j > -1 && b[8 - endingLine - j][startingColumn] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine - j, startingColumn, code, b, i);
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
        for (int j = 8 - endingLine + 1; j < 8; j++) {
            if (b[j][endingColumn] == 10) {
                for (auto& p : pieces) {
                    if ((p.getY() == j) && (p.getX() == endingColumn)) {
                        p.setY(8 - endingLine);
                        p.setX(endingColumn);
                        b[8 - endingLine][endingColumn] = (pieceName != 'a') ? piecesNamesMap[pieceName] : 10;
                        b[j][endingColumn] = 0;
                        i.enPassant = '-';
                        break;
                    }
                }
            }
            if (j - (8 - endingLine + 1) == 2) {
                i.enPassant = it;
            }
        }
        i.turn = 'b';
        i.halfmove++;
        i.fullmove++;
        return;
    }

    if (i.turn == 'b') {
        for (int j = 8 - endingLine - 1; j > 0; j--) {
            if (b[j][endingColumn] == 4) {
                for (auto& p : pieces) {
                    if ((p.getY() == j) && (p.getX() == endingColumn)) {
                        p.setY(8 - endingLine);
                        p.setX(endingColumn);
                        b[8 - endingLine][endingColumn] = (pieceName != 'a') ? piecesNamesMap[tolower(pieceName)] : 4;
                        b[j][endingColumn] = 0;
                        i.enPassant = '-';
                        break;
                    }
                }
            }
            if (j - (8 - endingLine - 1) == -2) {
                i.enPassant = it;
            }
        }
        i.turn = 'w';
        i.halfmove++;
        i.fullmove++;
        return;
    }
}

void rookMove(vector<vector<int>>& b, struct Info& i, string it, int startingLine = -1, int startingColumn = -1) {
    int endingColumn = int(it[1]) - 97;
    int endingLine = it[2] - '0';
    int code = (i.turn == 'w') ? 12 : 6;

    if (startingLine != -1 && searchLine(endingLine, endingColumn, startingLine, code, b, i)) {
        return;
    } 
    else if (startingColumn != -1 && searchColumn(endingLine, endingColumn, startingColumn, code, b, i)) {
        return;
    }

    for (int j = 0; j < 8; j++) {
        if (8 - endingLine + j < 8 && b[8 - endingLine + j][endingColumn] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn, code, b, i);
            return;
        }
        if (8 - endingLine - j > -1 && b[8 - endingLine - j][endingColumn] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn, code, b, i);
            return;
        }
        if (endingColumn + j < 8 && b[8 - endingLine][endingColumn + j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine, endingColumn + j, code, b, i);
            return;
        }
        if (endingColumn - j > -1 && b[8 - endingLine][endingColumn - j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine, endingColumn - j, code, b, i);
            return;
        }
    }
}

void bishopMove(vector<vector<int>>& b, struct Info& i, string it, int startingLine = -1, int startingColumn = -1) {
    int endingColumn = int(it[1]) - 97;
    int endingLine = it[2] - '0';
    int code = (i.turn == 'w') ? 7 : 1;

    if (startingLine != -1 && searchLine(endingLine, endingColumn, startingLine, code, b, i)) {
        return;
    }
    else if (startingColumn != -1 && searchColumn(endingLine, endingColumn, startingColumn, code, b, i)) {
        return;
    }

    for (int j = 0; j < 8; j++) {
        if (8 - endingLine + j < 8 && endingColumn + j < 8 && b[8 - endingLine + j][endingColumn + j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn + j, code, b, i);
            return;
        }
        if (8 - endingLine - j > -1 && endingColumn + j < 8 && b[8 - endingLine - j][endingColumn + j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn + j, code, b, i);
            return;
        }
        if (8 - endingLine + j < 8 && endingColumn - j > -1 && b[8 - endingLine + j][endingColumn - j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn - j, code, b, i);
            return;
        }
        if (8 - endingLine - j > -1 && endingColumn - j > -1 && b[8 - endingLine - j][endingColumn - j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn - j, code, b, i);
            return;
        }
    }
}

void queenMove(vector<vector<int>>& b, struct Info& i, string it, int startingLine = -1, int startingColumn = -1) {
    int endingColumn = int(it[1]) - 97;
    int endingLine = it[2] - '0';
    int code = (i.turn == 'w') ? 11 : 5;

    if (startingLine != -1 && searchLine(endingLine, endingColumn, startingLine, code, b, i)) {
        return;
    }
    else if (startingColumn != -1 && searchColumn(endingLine, endingColumn, startingColumn, code, b, i)) {
        return;
    }

    for (int j = 0; j < 8; j++) {
        if (8 - endingLine + j < 8 && b[8 - endingLine + j][endingColumn] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn, code, b, i);
            return;
        }
        if (8 - endingLine - j > -1 && b[8 - endingLine - j][endingColumn] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn, code, b, i);
            return;
        }
        if (endingColumn + j < 8 && b[8 - endingLine][endingColumn + j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine, endingColumn + j, code, b, i);
            return;
        }
        if (endingColumn - j > -1 && b[8 - endingLine][endingColumn - j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine, endingColumn - j, code, b, i);
            return;
        }
        if (8 - endingLine + j < 8 && endingColumn + j < 8 && b[8 - endingLine + j][endingColumn + j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn + j, code, b, i);
            return;
        }
        if (8 - endingLine - j > -1 && endingColumn + j < 8 && b[8 - endingLine - j][endingColumn + j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn + j, code, b, i);
            return;
        }
        if (8 - endingLine + j < 8 && endingColumn - j > -1 && b[8 - endingLine + j][endingColumn - j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn - j, code, b, i);
            return;
        }
        if (8 - endingLine - j > -1 && endingColumn - j > -1 && b[8 - endingLine - j][endingColumn - j] == code) {
            searchPieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn - j, code, b, i);
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
        searchPieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn, code, b, i);
        return;
    }
    if (8 - endingLine - j > -1 && b[8 - endingLine - j][endingColumn] == code) {
        searchPieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn, code, b, i);
        return;
    }
    if (endingColumn + j < 8 && b[8 - endingLine][endingColumn + j] == code) {
        searchPieceInVector(endingLine, endingColumn, 8 - endingLine, endingColumn + j, code, b, i);
        return;
    }
    if (endingColumn - j > -1 && b[8 - endingLine][endingColumn - j] == code) {
        searchPieceInVector(endingLine, endingColumn, 8 - endingLine, endingColumn - j, code, b, i);
        return;
    }
    if (8 - endingLine + j < 8 && endingColumn + j < 8 && b[8 - endingLine + j][endingColumn + j] == code) {
        searchPieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn + j, code, b, i);
        return;
    }
    if (8 - endingLine - j > -1 && endingColumn + j < 8 && b[8 - endingLine - j][endingColumn + j] == code) {
        searchPieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn + j, code, b, i);
        return;
    }
    if (8 - endingLine + j < 8 && endingColumn - j > -1 && b[8 - endingLine + j][endingColumn - j] == code) {
        searchPieceInVector(endingLine, endingColumn, 8 - endingLine + j, endingColumn - j, code, b, i);
        return;
    }
    if (8 - endingLine - j > -1 && endingColumn - j > -1 && b[8 - endingLine - j][endingColumn - j] == code) {
        searchPieceInVector(endingLine, endingColumn, 8 - endingLine - j, endingColumn - j, code, b, i);
        return;
    }
}

void knightMove(vector<vector<int>>& b, struct Info& i, string it, int startingLine = -1, int startingColumn = -1) {
    int endingColumn = int(it[1]) - 97;
    int endingLine = it[2] - '0';
    int code = (i.turn == 'w') ? 9 : 3;

    if (startingLine != -1 && searchLine(endingLine, endingColumn, startingLine, code, b, i)) {
        return;
    }
    else if (startingColumn != -1 && searchColumn(endingLine, endingColumn, startingColumn, code, b, i)) {
        return;
    }

    for (int j = endingColumn - 2; j <= endingColumn + 2; j++) {
        if (j > -1 && j < 8 && j != endingColumn) {
            if (8 - endingLine - 2 > -1 && 8 - endingLine - 2 < 8 && b[8 - endingLine - 2][j] == code) {
                searchPieceInVector(endingLine, endingColumn, 8 - endingLine - 2, j, code, b, i);
                return;
            }
            if (8 - endingLine + 2 > -1 && 8 - endingLine + 2 < 8 && b[8 - endingLine + 2][j] == code) {
                searchPieceInVector(endingLine, endingColumn, 8 - endingLine + 2, j, code, b, i);
                return;
            }
            if (8 - endingLine - 1 > -1 && 8 - endingLine - 2 < 8 && b[8 - endingLine - 1][j] == code) {
                searchPieceInVector(endingLine, endingColumn, 8 - endingLine - 1, j, code, b, i);
                return;
            }
            if (8 - endingLine + 1 > -1 && 8 - endingLine + 1 < 8 && b[8 - endingLine + 1][j] == code) {
                searchPieceInVector(endingLine, endingColumn, 8 - endingLine + 1, j, code, b, i);
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
                    // Orice alta piesa in afara de pion ia alta piesa
                }
                else {
                    pawnTakes(b, i, it);
                }
            }
            else if (it[2] == '=') {
                // pawnMove(b, i, it, it[3]);
            }
            else if (it[it.size() - 1] == '+') {
                // O piesa a pus regele in sah
            }
            else {
                int endingColumn = -1, endingLine = -1;
                if (isdigit(it[1])) {
                    endingLine = it[1] - '0';
                }
                else if(isalpha(it[1])) {
                    endingColumn = int(it[1]) - 97;
                }

                if (it[0] == 'R') {
                    it = it.substr(1, 3);
                    rookMove(b, i, it, endingLine, endingColumn);
                }
                else if (it[0] == 'B') {
                    it = it.substr(1, 3);
                    bishopMove(b, i, it, endingLine, endingColumn);
                }
                else if (it[0] == 'Q') {
                    it = it.substr(1, 3);
                    queenMove(b, i, it, endingLine, endingColumn);
                }
                else if (it[0] = 'N') {
                    it = it.substr(1, 3);
                    knightMove(b, i, it, endingLine, endingColumn);
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
    moves.push_back("b4");
    moves.push_back("a5");
    moves.push_back("bxa5");
    moves.push_back("Ra7");
    moves.push_back("a3");
    moves.push_back("Ra6");
    moves.push_back("h3");
    moves.push_back("Rh6");

    listProcessing(moves);
    Position lastPos = positions.at(positions.size() - 1);
    vector<vector<int>> b = positions.at(positions.size() - 1).getBoard();
    
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
        drawPieces(&window, pieces);

        // end the current frame
        window.display();
    }
    
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout << b[i][j] << ' ';
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