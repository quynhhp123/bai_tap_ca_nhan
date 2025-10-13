#include<bits/stdc++.h>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;
struct tai_khoan{
    string stk;
    string mk;
    string ten;
    float so_du;
};
unordered_map<string, tai_khoan> ds_tk(const string& duong_dan){
    unordered_map<string, tai_khoan> tk;
    ifstream f(duong_dan);
    string s;
    getline(f,s);
    while(getline(f,s)){
        stringstream ss(s);
        string stk, mk, ten, so_du;
        getline(ss,stk,',');
        getline(ss,mk,',');
        getline(ss,ten,',');
        getline(ss,so_du,',');
        tk[stk] = {stk,mk,ten,stof(so_du)};
    }

    return tk;
}
int main(){

auto ds = ds_tk("accounts.txt");
string stkInput, mkInput;
wstring message;
bool typingStk = true, typingMk = false;
Texture bgTexture, menuTexture;
if (!bgTexture.loadFromFile("hinhnen.jpg")) {
    std::cout << "Khong load duoc hinh nen!\n";
}
if (!menuTexture.loadFromFile("trungtam.jpg")) {
    std::cout << "Khong load duoc trungtam.jpg!\n";
}
RenderWindow window(sf::VideoMode(bgTexture.getSize().x, bgTexture.getSize().y), "ATM Login");
Sprite bgSprite(bgTexture);
Sprite menuSprite(menuTexture);

Font font;
font.loadFromFile("C:/Windows/Fonts/Arial.ttf");

Text labelStk(L"Số tài khoản:", font, 24); labelStk.setPosition(80, 120); labelStk.setFillColor(Color::White);
Text labelMk(L"Mật khẩu:", font, 24); labelMk.setPosition(80, 200); labelMk.setFillColor(Color::White);

RectangleShape boxStk(Vector2f(300, 40)); boxStk.setPosition(250, 120); boxStk.setFillColor(Color::White);
RectangleShape boxMk(Vector2f(300, 40)); boxMk.setPosition(250, 200); boxMk.setFillColor(Color::White);

Text textStk("", font, 22); textStk.setPosition(260, 125); textStk.setFillColor(Color::Black);
Text textMk("", font, 22); textMk.setPosition(260, 205); textMk.setFillColor(Color::Black);

RectangleShape btnLogin(Vector2f(180, 50)); btnLogin.setPosition(260, 300); btnLogin.setFillColor(Color(100,200,100));
Text txtLogin(L"Đăng nhập", font, 26); txtLogin.setPosition(280, 310);

Text txtMsg("", font, 22); txtMsg.setPosition(200, 350); txtMsg.setFillColor(Color::Red);
enum State { LOGIN, MENU };
State state = LOGIN;
tai_khoan* currentUser = nullptr;
RectangleShape btnBalance(Vector2f(180, 50)); btnBalance.setPosition(40, 70); btnBalance.setFillColor(Color(200,200,255));
Text txtBalance(L"Xem số dư", font, 24); txtBalance.setPosition(50, 80);

RectangleShape btnDeposit(Vector2f(180, 50)); btnDeposit.setPosition(40, 150); btnDeposit.setFillColor(Color(200,255,200));
Text txtDeposit(L"Nạp tiền", font, 24); txtDeposit.setPosition(50, 160);

RectangleShape btnWithdraw(Vector2f(180, 50)); btnWithdraw.setPosition(40, 230); btnWithdraw.setFillColor(Color(255,200,200));
Text txtWithdraw(L"Rút tiền", font, 24); txtWithdraw.setPosition(50, 230);

RectangleShape btnLogout(Vector2f(180, 50)); btnLogout.setPosition(40, 310); btnLogout.setFillColor(Color(220,220,220));
Text txtLogout(L"Đăng xuất", font, 24); txtLogout.setPosition(50, 320);

float soTien = 0;
wstring nhapTienMsg;
bool showNhapTien = false;
bool isDeposit = false, isWithdraw = false;
string soTienInput;

while (window.isOpen()) {
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) window.close();
        if (state == LOGIN) {
            if (event.type == Event::MouseButtonPressed) {
                auto x = event.mouseButton.x, y = event.mouseButton.y;  
                typingStk = boxStk.getGlobalBounds().contains(x, y);
                typingMk = boxMk.getGlobalBounds().contains(x, y);
                if (btnLogin.getGlobalBounds().contains(x, y)) {
                    if (ds.find(stkInput) != ds.end() && ds[stkInput].mk == mkInput) {
                        state = MENU;
                        currentUser = &ds[stkInput];
                        message = L"Đăng nhập thành công!";
                    } else {
                        message = L"Đăng nhập thất bại!";
                    }
                }
            }
            if (event.type == Event::TextEntered) {
                if (typingStk && event.text.unicode < 128 && event.text.unicode != 13) {
                    if (event.text.unicode == 8 && !stkInput.empty()) stkInput.pop_back();
                    else if (event.text.unicode != 8) stkInput += static_cast<char>(event.text.unicode);
                }
                if (typingMk && event.text.unicode < 128 && event.text.unicode != 13) {
                    if (event.text.unicode == 8 && !mkInput.empty()) mkInput.pop_back();
                    else if (event.text.unicode != 8) mkInput += static_cast<char>(event.text.unicode);
                }
            }
        } else if (state == MENU) {
            if (event.type == Event::MouseButtonPressed) {
                auto x = event.mouseButton.x, y = event.mouseButton.y;
                if (btnBalance.getGlobalBounds().contains(x, y)) {
                    message = L"Số dư: " + to_wstring(currentUser->so_du);
                }
                if (btnDeposit.getGlobalBounds().contains(x, y)) {
                    showNhapTien = true; isDeposit = true; isWithdraw = false; soTienInput = ""; nhapTienMsg = L"Nhập số tiền nạp:";
                }
                if (btnWithdraw.getGlobalBounds().contains(x, y)) {
                    showNhapTien = true; isWithdraw = true; isDeposit = false; soTienInput = ""; nhapTienMsg = L"Nhập số tiền rút:";
                }
                if (btnLogout.getGlobalBounds().contains(x, y)) {
                    state = LOGIN; currentUser = nullptr; message = L""; stkInput = ""; mkInput = "";
                }
            }
            if (event.type == Event::TextEntered && showNhapTien) {
                if (event.text.unicode >= '0' && event.text.unicode <= '9') {
                    soTienInput += static_cast<char>(event.text.unicode);
                }
                if (event.text.unicode == 8 && !soTienInput.empty()) soTienInput.pop_back();
                if (event.text.unicode == 13 && !soTienInput.empty()) { // Enter
                    soTien = stof(soTienInput);
                    if (isDeposit) {
                        currentUser->so_du += soTien;
                        message = L"Nạp thành công! Số dư: " + to_wstring(currentUser->so_du);
                    } else if (isWithdraw) {
                        if (currentUser->so_du >= soTien) {
                            currentUser->so_du -= soTien;
                            message = L"Rút thành công! Số dư: " + to_wstring(currentUser->so_du);
                        } else {
                            message = L"Không đủ số dư!";
                        }
                    }
                    showNhapTien = false; isDeposit = false; isWithdraw = false; soTienInput = "";
                }
            }
        }
    }
    textStk.setString(stkInput);
    textMk.setString(string(mkInput.size(), '*'));
    txtMsg.setString(message);

    window.clear(Color::White);
    if (state == LOGIN) {
        window.draw(bgSprite);
        window.draw(labelStk); window.draw(labelMk);
        window.draw(boxStk); window.draw(boxMk);
        window.draw(textStk); window.draw(textMk);
        window.draw(btnLogin); window.draw(txtLogin);
        window.draw(txtMsg);
    } else if (state == MENU) {
        menuSprite.setScale(
        float(window.getSize().x) / menuTexture.getSize().x,
        float(window.getSize().y) / menuTexture.getSize().y
        );
        window.draw(menuSprite);
        window.draw(btnBalance); window.draw(txtBalance);
        window.draw(btnDeposit); window.draw(txtDeposit);
        window.draw(btnWithdraw); window.draw(txtWithdraw);
        window.draw(btnLogout); window.draw(txtLogout);
        window.draw(txtMsg);
        if (showNhapTien) {
            RectangleShape boxNhap(Vector2f(200, 40)); boxNhap.setPosition(400, 200); boxNhap.setFillColor(Color::Black);
            Text txtNhap(nhapTienMsg, font, 22); txtNhap.setPosition(400, 150); txtNhap.setFillColor(Color::White);
            Text txtSoTien(soTienInput, font, 22); txtSoTien.setPosition(400, 200); txtSoTien.setFillColor(Color::White);
            window.draw(boxNhap); window.draw(txtNhap); window.draw(txtSoTien);
        }
    }
    window.display();
  }
}