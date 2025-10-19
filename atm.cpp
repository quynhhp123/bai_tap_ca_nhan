#include<bits/stdc++.h>
#include <SFML/Graphics.hpp>
#if defined(_WIN32)
#include <windows.h>
#include <shellapi.h>
#endif
#include <locale>
#include <codecvt>
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
bool luu_ds_tk(const string &duong_dan, const unordered_map<string, tai_khoan> &tk) {
    ofstream f(duong_dan, ios::trunc);
    if (!f.is_open()) return false;
    f << "stk,mk,ten,so_du\n";
    for (const auto &p : tk) {
        const tai_khoan &a = p.second;
        string ten = a.ten;
        for (auto &c : ten) if (c == ',') c = ' ';
        f << a.stk << "," << a.mk << "," << ten << "," << fixed << setprecision(2) << a.so_du << "\n";
    }
    f.close();
    return true;
}
// forward declaration for receipt generation (defined after main)
void createReceiptRTF(const string &path, const string &type, const string &fromAccount, const string &toAccount, float amount, float balance);
int main(){

auto ds = ds_tk("accounts.txt");
    string so_tk_input, mat_khau_input;
    wstring message;
    bool dang_go_so_tk = true, dang_go_mk = false;
Texture bgTexture, menuTexture;
if (!bgTexture.loadFromFile("hinhnen.jpg")) {
    cout << "Khong load duoc hinh nen!\n";
}
if (!menuTexture.loadFromFile("trungtam.jpg")) {
    cout << "Khong load duoc trungtam.jpg!\n";
}
RenderWindow window(VideoMode(bgTexture.getSize().x, bgTexture.getSize().y), "ATM Login");
Sprite bgSprite(bgTexture);
Sprite menuSprite(menuTexture);
menuTexture.setSmooth(true);

Font font;
font.loadFromFile("CascadiaCode.ttf");

Text title(L"VitekBank", font, 60);
title.setFillColor(Color::White);
title.setOutlineColor(Color::Black);
title.setOutlineThickness(2.f);
{
    FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width/2.f, tb.top + tb.height/2.f);
    title.setPosition((float)window.getSize().x/2.f, 120.f);
}

    Text labelStk(L"Số tài khoản:", font, 24); labelStk.setPosition(280, 300); labelStk.setFillColor(Color::White);
    Text labelMk(L"Mật khẩu:", font, 24); labelMk.setPosition(280, 400); labelMk.setFillColor(Color::White);

    RectangleShape hop_so_tk(Vector2f(300, 40)); hop_so_tk.setPosition(500, 300); hop_so_tk.setFillColor(Color::White);
    RectangleShape hop_mat_khau(Vector2f(300, 40)); hop_mat_khau.setPosition(500, 400); hop_mat_khau.setFillColor(Color::White);

    Text txt_so_tk("", font, 22); txt_so_tk.setPosition(500, 300); txt_so_tk.setFillColor(Color::Black);
    Text txt_mat_khau("", font, 22); txt_mat_khau.setPosition(500, 400); txt_mat_khau.setFillColor(Color::Black);

    RectangleShape nut_dang_nhap(Vector2f(180, 50)); nut_dang_nhap.setPosition(500, 500); nut_dang_nhap.setFillColor(Color(100,200,100));
    Text txt_dang_nhap(L"Đăng nhập", font, 26); txt_dang_nhap.setPosition(520, 510);

    Text thong_bao("", font, 22); thong_bao.setPosition(500, 550); thong_bao.setFillColor(Color::Black);
enum State { LOGIN, MENU };
State state = LOGIN;
tai_khoan* currentUser = nullptr;
// Xem số dư - #2E5CFF (46,92,255)
RectangleShape nut_xem_so_du(Vector2f(180, 50)); nut_xem_so_du.setPosition(80, 150); nut_xem_so_du.setFillColor(Color(46,92,255));
Text txt_xem_so_du(L"Xem số dư", font, 24); txt_xem_so_du.setPosition(90, 160); txt_xem_so_du.setFillColor(Color::White);

// Nạp tiền - #28A745 (40,167,69)
RectangleShape nut_nap_tien(Vector2f(180, 50)); nut_nap_tien.setPosition(80, 350); nut_nap_tien.setFillColor(Color(40,167,69));
Text txt_nap_tien(L"Nạp tiền", font, 24); txt_nap_tien.setPosition(90, 360); txt_nap_tien.setFillColor(Color::White);

// Rút tiền - #D9534F (217,83,79)
RectangleShape nut_rut_tien(Vector2f(180, 50)); nut_rut_tien.setPosition(80, 550); nut_rut_tien.setFillColor(Color(217,83,79));
Text txt_rut_tien(L"Rút tiền", font, 24); txt_rut_tien.setPosition(90, 560); txt_rut_tien.setFillColor(Color::White);

// Chuyển khoản - #1A73E8 (26,115,232)
RectangleShape nut_chuyen_khoan(Vector2f(180, 50)); nut_chuyen_khoan.setPosition(1100, 150); nut_chuyen_khoan.setFillColor(Color(26,115,232));
Text txt_chuyen_khoan(L"Chuyển khoản", font, 24); txt_chuyen_khoan.setPosition(1110, 160); txt_chuyen_khoan.setFillColor(Color::White);

// Đăng xuất - #6C757D (108,117,125)
RectangleShape nut_dang_xuat(Vector2f(180, 50)); nut_dang_xuat.setPosition(600, 650); nut_dang_xuat.setFillColor(Color(108,117,125));
Text txt_dang_xuat(L"Đăng xuất", font, 24); txt_dang_xuat.setPosition(625,660); txt_dang_xuat.setFillColor(Color::White);

    float so_tien = 0;
    wstring nhap_tien_msg;
    bool hien_nhap_tien = false;
    bool dang_nap_tien = false, dang_rut_tien = false;
    string so_tien_input;

    bool hien_chuyen_khoan = false;
    int buoc_chuyen = 0; 
    string tai_khoan_nhan;
    string so_tien_chuyen_input;
    bool hien_so_du = false;

struct giao_dich_cuoi { 
    string loai; 
    string tu_tk; 
    string den_tk; 
    float so_tien_gd; 
    float so_du_sau; 
    bool hop_le; } gd_cuoi{"", "", "", 0.0f, 0.0f, false};

    RectangleShape nut_in(Vector2f(160, 40)); nut_in.setFillColor(Color(70,130,180));
    Text txt_in(L"In biên lai", font, 20); txt_in.setFillColor(Color::White);

while (window.isOpen()) {
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) window.close();
        if (state == LOGIN) {
            if (event.type == Event::MouseButtonPressed) {
                auto x = event.mouseButton.x, y = event.mouseButton.y;  
                dang_go_so_tk = hop_so_tk.getGlobalBounds().contains(x, y);
                dang_go_mk = hop_mat_khau.getGlobalBounds().contains(x, y);
                if (nut_dang_nhap.getGlobalBounds().contains(x, y)) {
                    if (ds.find(so_tk_input) != ds.end() && ds[so_tk_input].mk == mat_khau_input) {
                        state = MENU;
                        currentUser = &ds[so_tk_input];
                        message = L"";
                    } else {
                        message = L"Đăng nhập thất bại!";
                    }
                }
            }
            if (event.type == Event::TextEntered) {
                if (dang_go_so_tk && event.text.unicode < 128 && event.text.unicode != 13) {
                    if (event.text.unicode == 8 && !so_tk_input.empty()) so_tk_input.pop_back();
                    else if (event.text.unicode != 8) so_tk_input += static_cast<char>(event.text.unicode);
                }
                if (dang_go_mk && event.text.unicode < 128 && event.text.unicode != 13) {
                    if (event.text.unicode == 8 && !mat_khau_input.empty()) mat_khau_input.pop_back();
                    else if (event.text.unicode != 8) mat_khau_input += static_cast<char>(event.text.unicode);
                }
            }
        } else if (state == MENU) {
            if (event.type == Event::MouseButtonPressed) {
                auto x = event.mouseButton.x, y = event.mouseButton.y;
                if (nut_xem_so_du.getGlobalBounds().contains(x, y)) {
                    hien_so_du = true;
                    hien_nhap_tien = false;
                    hien_chuyen_khoan = false;
                    message = L"";
                }
                if (nut_nap_tien.getGlobalBounds().contains(x, y)) {
                    hien_nhap_tien = true; dang_nap_tien = true; dang_rut_tien = false; so_tien_input = ""; nhap_tien_msg = L"Nhập số tiền nạp:";
                    hien_chuyen_khoan = false; hien_so_du = false;
                }
                if (nut_rut_tien.getGlobalBounds().contains(x, y)) {
                    hien_nhap_tien = true; dang_rut_tien = true; dang_nap_tien = false; so_tien_input = ""; nhap_tien_msg = L"Nhập số tiền rút:";
                    hien_chuyen_khoan = false; hien_so_du = false;
                }
                if (nut_chuyen_khoan.getGlobalBounds().contains(x, y)) {
                    hien_chuyen_khoan = true; buoc_chuyen = 0; tai_khoan_nhan = ""; so_tien_chuyen_input = ""; nhap_tien_msg = L"Nhập số tài khoản nhận:";
                    hien_nhap_tien = false; hien_so_du = false;
                }
                if (nut_dang_xuat.getGlobalBounds().contains(x, y)) {
                    state = LOGIN; currentUser = nullptr; message = L""; so_tk_input = ""; mat_khau_input = "";
                }

                if (gd_cuoi.hop_le && nut_in.getGlobalBounds().contains(x, y)) {
                    string path = "receipt.rtf";
                    createReceiptRTF(path, gd_cuoi.loai, gd_cuoi.tu_tk, gd_cuoi.den_tk, gd_cuoi.so_tien_gd, gd_cuoi.so_du_sau);
                    #if defined(_WIN32)
                    ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    #endif
                }
            }
            if (event.type == Event::TextEntered) {
                
            if (hien_nhap_tien && !hien_so_du) {
                    if (event.text.unicode >= '0' && event.text.unicode <= '9') {
                        so_tien_input += static_cast<char>(event.text.unicode);
                    }
                    if (event.text.unicode == 8 && !so_tien_input.empty()) so_tien_input.pop_back();
                    if (event.text.unicode == 13 && !so_tien_input.empty()) { 
                        so_tien = stof(so_tien_input);
                        if (dang_nap_tien) {
                            currentUser->so_du += so_tien;
                            if (!luu_ds_tk("accounts.txt", ds)) {
                                message = L"Lỗi lưu dữ liệu!";
                            } else {
                                message = L"Nạp thành công! Số dư: " + to_wstring(currentUser->so_du);
                                // record last transaction
                                gd_cuoi = {"Nạp tiền", currentUser->stk, "", so_tien, currentUser->so_du, true};
                            }
                        } else if (dang_rut_tien) {
                            if (currentUser->so_du >= so_tien) {
                                currentUser->so_du -= so_tien;
                                if (!luu_ds_tk("accounts.txt", ds)) {
                                    message = L"Lỗi lưu dữ liệu!";
                                } else {
                                    message = L"Rút thành công! Số dư: " + to_wstring(currentUser->so_du);
                                    gd_cuoi = {"Rút tiền", currentUser->stk, "", so_tien, currentUser->so_du, true};
                                }
                            } else {
                                message = L"Không đủ số dư!";
                            }
                        }
                        hien_nhap_tien = false; dang_nap_tien = false; dang_rut_tien = false; so_tien_input = "";
                    }
                }
                
            if (hien_chuyen_khoan && !hien_so_du) {
                    if (buoc_chuyen == 0) {
                        if (event.text.unicode >= 32 && event.text.unicode < 128) tai_khoan_nhan += static_cast<char>(event.text.unicode);
                        if (event.text.unicode == 8 && !tai_khoan_nhan.empty()) tai_khoan_nhan.pop_back();
                        if (event.text.unicode == 13 && !tai_khoan_nhan.empty()) {
                            buoc_chuyen = 1; so_tien_chuyen_input = ""; nhap_tien_msg = L"Nhập số tiền chuyển:";
                        }
                    } else if (buoc_chuyen == 1) {
                        if (event.text.unicode >= '0' && event.text.unicode <= '9') so_tien_chuyen_input += static_cast<char>(event.text.unicode);
                        if (event.text.unicode == 8 && !so_tien_chuyen_input.empty()) so_tien_chuyen_input.pop_back();
                        if (event.text.unicode == 13 && !so_tien_chuyen_input.empty()) {
                            float amt = stof(so_tien_chuyen_input);
                            auto it = ds.find(tai_khoan_nhan);
                            if (it == ds.end()) {
                                message = L"Tài khoản nhận không tồn tại!";
                            } else if (currentUser->so_du < amt) {
                                message = L"Không đủ số dư!";
                            } else {
                                currentUser->so_du -= amt;
                                it->second.so_du += amt;
                                if (!luu_ds_tk("accounts.txt", ds)) {
                                    message = L"Lỗi lưu dữ liệu!";
                                } else {
                                    message = L"Chuyển khoản thành công! Số dư: " + to_wstring(currentUser->so_du);
                                    gd_cuoi = {"Chuyển khoản", currentUser->stk, it->second.stk, amt, currentUser->so_du, true};
                                }
                            }
                            hien_chuyen_khoan = false; buoc_chuyen = 0; tai_khoan_nhan = ""; so_tien_chuyen_input = "";
                        }
                    }
                }
            }
        }

        
    }
    txt_so_tk.setString(so_tk_input);
    txt_mat_khau.setString(string(mat_khau_input.size(), '*'));
    thong_bao.setString(message);

    window.clear(Color::White);
    if (state == LOGIN) {
        window.draw(bgSprite);
        window.draw(title);
        window.draw(labelStk); window.draw(labelMk);
        window.draw(hop_so_tk); window.draw(hop_mat_khau);
        window.draw(txt_so_tk); window.draw(txt_mat_khau);
        window.draw(nut_dang_nhap); window.draw(txt_dang_nhap);
        window.draw(thong_bao);
    } else if (state == MENU) {

        float winW = (float)window.getSize().x;
        float winH = (float)window.getSize().y;
        float texW = (float)menuTexture.getSize().x;
        float texH = (float)menuTexture.getSize().y;
        if (texW > 0 && texH > 0) {
            float scaleX = winW / texW;
            float scaleY = winH / texH;
    

            float scale = max(scaleX, scaleY);
            menuSprite.setScale(scale, scale);
           
            float dispW = texW * scale;
            float dispH = texH * scale;
            menuSprite.setPosition((winW - dispW) / 2.f, (winH - dispH) / 2.f);
        }
        window.draw(menuSprite);
            float panelW = winW * 0.55f;
            float panelH = winH * 0.6f; 
            float panelX = (winW - panelW) / 2.f;
            float panelY = (winH - panelH) / 2.f;
            float cornerRadius = 14.f;
            float padding = 20.f;

            RectangleShape panelShadow(Vector2f(panelW + 8.f, panelH + 8.f));
            panelShadow.setPosition(panelX + 6.f, panelY + 6.f);
            panelShadow.setFillColor(Color(0, 0, 0, 80));
            panelShadow.setOutlineThickness(0);
            window.draw(panelShadow);

            RectangleShape panel(Vector2f(panelW, panelH));
            panel.setPosition(panelX, panelY);
            panel.setFillColor(Color(0xF2, 0xF4, 0xFA));
            panel.setOutlineColor(Color(200,200,200));
            panel.setOutlineThickness(1.f);
            window.draw(panel);
    

        
        window.draw(nut_xem_so_du); window.draw(txt_xem_so_du);
        window.draw(nut_nap_tien); window.draw(txt_nap_tien);
        window.draw(nut_rut_tien); window.draw(txt_rut_tien);
        window.draw(nut_chuyen_khoan); window.draw(txt_chuyen_khoan);
        window.draw(nut_dang_xuat); window.draw(txt_dang_xuat);
        window.draw(thong_bao);

            
            float contentX = panelX + padding;
            float contentW = panelW - padding*2.f;
            float currentY = panelY + padding + 10.f;

            if (hien_nhap_tien) {
                
                RectangleShape boxNhap(Vector2f(contentW, 120.f));
                boxNhap.setPosition(contentX, currentY);
                boxNhap.setFillColor(Color(0xEE,0xEF,0xF5));
                boxNhap.setOutlineColor(Color(190,190,190));
                boxNhap.setOutlineThickness(1.f);
                window.draw(boxNhap);

                Text txtNhap(nhap_tien_msg, font, 22); txtNhap.setPosition(contentX + 12.f, currentY + 12.f); txtNhap.setFillColor(Color::Black);
                Text txtSoTien(so_tien_input, font, 22); txtSoTien.setPosition(contentX + 12.f, currentY + 56.f); txtSoTien.setFillColor(Color::Black);
                window.draw(txtNhap); window.draw(txtSoTien);
            }
                if (hien_chuyen_khoan) {
                RectangleShape boxTransfer(Vector2f(contentW, 140.f));
                boxTransfer.setPosition(contentX, currentY);
                boxTransfer.setFillColor(Color(0xEE,0xEF,0xF5));
                boxTransfer.setOutlineColor(Color(190,190,190));
                boxTransfer.setOutlineThickness(1.f);
                window.draw(boxTransfer);
                if (buoc_chuyen == 0) {
                    Text txtTarget(nhap_tien_msg, font, 22); txtTarget.setPosition(contentX + 12.f, currentY + 12.f); txtTarget.setFillColor(Color::Black);
                    Text txtTargetVal(tai_khoan_nhan, font, 22); txtTargetVal.setPosition(contentX + 12.f, currentY + 56.f); txtTargetVal.setFillColor(Color::Black);
                    window.draw(txtTarget); window.draw(txtTargetVal);
                } else if (buoc_chuyen == 1) {
                    Text txtAmt(nhap_tien_msg, font, 22); txtAmt.setPosition(contentX + 12.f, currentY + 12.f); txtAmt.setFillColor(Color::Black);
                    Text txtAmtVal(so_tien_chuyen_input, font, 22); txtAmtVal.setPosition(contentX + 12.f, currentY + 56.f); txtAmtVal.setFillColor(Color::Black);
                    window.draw(txtAmt); window.draw(txtAmtVal);
                }
            }

                if (hien_so_du) {
                RectangleShape balBox(Vector2f(contentW, 100.f));
                balBox.setPosition(contentX, currentY);
                balBox.setFillColor(Color(0xEE,0xEF,0xF5));
                balBox.setOutlineColor(Color(190,190,190));
                balBox.setOutlineThickness(1.f);
                window.draw(balBox);
                wstringstream wss;
                wss << fixed << setprecision(2) << currentUser->so_du;
                wstring balText = L"Số dư hiện tại: " + wss.str();
                Text txtBal(sf::String(balText), font, 24);
                txtBal.setPosition(contentX + 12.f, currentY + 28.f);
                txtBal.setFillColor(Color::Black);
                window.draw(txtBal);
            }

            if (gd_cuoi.hop_le) {
                float printX = panelX + panelW - padding - nut_in.getSize().x;
                float printY = panelY + panelH - padding - nut_in.getSize().y;
                nut_in.setPosition(printX, printY-100);
                txt_in.setPosition(printX + 12.f, printY-100 + 8.f);
                window.draw(nut_in); window.draw(txt_in);
            }
    }
    window.display();
  }
}

void createReceiptRTF(const string &path, const string &type, const string &fromAccount, const string &toAccount, float amount, float balance) {
    ofstream f(path, ios::trunc);
    if (!f.is_open()) return;

    auto utf8_to_wstring = [](const string &s)->wstring{
#if defined(_WIN32)
        if (s.empty()) return L"";
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0);
        if (size_needed <= 0) return wstring(s.begin(), s.end());
        wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &wstrTo[0], size_needed);
        return wstrTo;
#else
        return wstring(s.begin(), s.end());
#endif
    };

    auto rtfEscape = [](const wstring &w)->string{
        string out;
        for (wchar_t wc : w) {
            if (wc == L'\\' || wc == L'{' || wc == L'}') {
                out.push_back('\\');
                out.push_back((char)wc);
                continue;
            }
            if (wc >= 0x20 && wc < 0x7f) {
                out.push_back((char)wc);
            } else {
                int n = (int)wc;
                if (n > 32767) n -= 65536;
                out += "\\u" + to_string(n) + "?";
            }
        }
        return out;
    };

    f << "{\\rtf1\\ansi\\deff0{";
    f << "\\fonttbl{\\f0 Arial;}}\\uc1\n";
    f << "\\f0\\fs24\n";

    wstring title = L"BIÊN LAI GIAO DỊCH";
    wstring wtype = L"Loại: ";
    wstring wfrom = L"Tài khoản: ";
    wstring wto = L"Tài khoản nhận: ";
    wstring wamount = L"Số tiền: ";
    wstring wbal = L"Số dư: ";
    wstring wtime = L"Thời gian: ";

    f << rtfEscape(title) << "\\line\\line\n";

    wstring wtypeFull = wtype + utf8_to_wstring(type);
    f << rtfEscape(wtypeFull) << "\\line\n";

    wstring wfromFull = wfrom + utf8_to_wstring(fromAccount);
    f << rtfEscape(wfromFull) << "\\line\n";
    if (!toAccount.empty()) {
        wstring wtoFull = wto + utf8_to_wstring(toAccount);
        f << rtfEscape(wtoFull) << "\\line\n";
    }

    {
        wostringstream wss;
        wss << fixed << setprecision(2) << amount;
        wstring wam = wamount + wss.str();
        f << rtfEscape(wam) << "\\line\n";
    }
    {
        wostringstream wss;
        wss << fixed << setprecision(2) << balance;
        wstring wbo = wbal + wss.str();
        f << rtfEscape(wbo) << "\\line\n";
    }

    // timestamp
    time_t now = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    wstring wbuf = wtime + wstring(buf, buf + strlen(buf));
    f << rtfEscape(wbuf) << "\\line\n";

    f << "}\n";
    f.close();
}