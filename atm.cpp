#include<bits/stdc++.h>  // Thư viện chuẩn C++
#include <SFML/Graphics.hpp> // Thư viện SFML cho đồ họa
#if defined(_WIN32) // Kiểm tra nếu hệ điều hành là Windows
#include <windows.h> // Thư viện Windows API
#include <shellapi.h> // Thư viện để thực thi các lệnh hệ thống
#endif // Kết thúc kiểm tra hệ điều hành
#include <locale> // Thư viện hỗ trợ địa phương hóa
#include <codecvt> // Thư viện chuyển đổi mã hóa ký tự
using namespace std; // Sử dụng không gian tên chuẩn
using namespace sf; // Sử dụng không gian tên SFML
struct tai_khoan{ // Cấu trúc tài khoản
    string stk;
    string mk;
    string ten;
    float so_du;
};
unordered_map<string, tai_khoan> ds_tk(const string& duong_dan){ // Hàm đọc danh sách tài khoản từ file
    unordered_map<string, tai_khoan> tk;
    ifstream f(duong_dan); // Mở file để đọc
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
bool luu_ds_tk(const string &duong_dan, const unordered_map<string, tai_khoan> &tk) { // Hàm lưu danh sách tài khoản vào file
    ofstream f(duong_dan, ios::trunc); // Mở file để ghi, xóa nội dung cũ
    if (!f.is_open()) return false;
    f << "stk,mk,ten,so_du\n"; // Ghi tiêu đề cột
    for (const auto &p : tk) {
        const tai_khoan &a = p.second;
        string ten = a.ten;
        for (auto &c : ten) if (c == ',') c = ' '; // Thay dấu phẩy trong tên để tránh lỗi định dạng CSV
        f << a.stk << "," << a.mk << "," << ten << "," << fixed << setprecision(2) << a.so_du << "\n"; // Ghi thông tin tài khoản
    }
    f.close();
    return true;
}

void createReceiptRTF(const string &path, const string &type, 
                      const string &fromAccount, const string &toAccount, 
                      float amount, float balance); // Hàm tạo biên lai RTF

                    
int main(){

auto ds = ds_tk("accounts.txt"); // Đọc danh sách tài khoản từ file
    string so_tk_input, mat_khau_input; // Nhập số tài khoản và mật khẩu
    wstring message; // Thông báo chung
    bool dang_go_so_tk = true, dang_go_mk = false; //   Biến theo dõi trạng thái nhập liệu
Texture bgTexture, menuTexture; // Tải hình nền và hình menu
if (!bgTexture.loadFromFile("hinhnen.jpg")) { // Tải hình nền
    cout << "Khong load duoc hinh nen!\n";
}
if (!menuTexture.loadFromFile("trungtam.jpg")) { // Tải hình menu
    cout << "Khong load duoc trungtam.jpg!\n";
}
RenderWindow window(VideoMode(bgTexture.getSize().x, bgTexture.getSize().y), "ATM Login"); // Tạo cửa sổ ứng dụng
Sprite bgSprite(bgTexture); // Tạo sprite cho hình nền
Sprite menuSprite(menuTexture); // Tạo sprite cho hình menu
menuTexture.setSmooth(true); // Tạo hiệu ứng mượt cho hình menu

Font font; // Tạo font chữ
font.loadFromFile("CascadiaCode.ttf"); // Tải font chữ từ file

Text title(L"VitekBank", font, 60); // Tiêu đề ứng dụng
title.setFillColor(Color::White); // Màu chữ trắng
title.setOutlineColor(Color::Black); // Màu viền chữ đen
title.setOutlineThickness(2.f); // Độ dày viền chữ
{
    FloatRect tb = title.getLocalBounds(); // Lấy kích thước chữ
    title.setOrigin(tb.left + tb.width/2.f, tb.top + tb.height/2.f); // Đặt gốc tọa độ ở giữa chữ
    title.setPosition((float)window.getSize().x/2.f, 120.f); // Đặt vị trí chữ ở giữa cửa sổ
}

    Text labelStk(L"Số tài khoản:", font, 24); labelStk.setPosition(280, 300); labelStk.setFillColor(Color::White); // Nhãn số tài khoản
    Text labelMk(L"Mật khẩu:", font, 24); labelMk.setPosition(280, 400); labelMk.setFillColor(Color::White); // Nhãn mật khẩu

    RectangleShape hop_so_tk(Vector2f(300, 40)); hop_so_tk.setPosition(500, 300); hop_so_tk.setFillColor(Color::White); // Hộp nhập số tài khoản
    RectangleShape hop_mat_khau(Vector2f(300, 40)); hop_mat_khau.setPosition(500, 400); hop_mat_khau.setFillColor(Color::White); // Hộp nhập mật khẩu

    Text txt_so_tk("", font, 22); txt_so_tk.setPosition(500, 300); txt_so_tk.setFillColor(Color::Black); // Hiển thị số tài khoản nhập
    Text txt_mat_khau("", font, 22); txt_mat_khau.setPosition(500, 400); txt_mat_khau.setFillColor(Color::Black); // Hiển thị mật khẩu nhập

    RectangleShape nut_dang_nhap(Vector2f(180, 50)); nut_dang_nhap.setPosition(500, 500); 
    nut_dang_nhap.setFillColor(Color(100,200,100)); // Nút đăng nhập

    Text txt_dang_nhap(L"Đăng nhập", font, 26); txt_dang_nhap.setPosition(520, 510); // Nhãn nút đăng nhập

    Text thong_bao("", font, 22); thong_bao.setPosition(500, 550); thong_bao.setFillColor(Color::Black); // Hiển thị thông báo
enum State { LOGIN, MENU }; // Trạng thái ứng dụng
State state = LOGIN; // Bắt đầu ở trạng thái đăng nhập
tai_khoan* ng_dung_hien_tai = nullptr; // Con trỏ đến tài khoản người dùng hiện tại
// Xem số dư - #2E5CFF (46,92,255)
RectangleShape nut_xem_so_du(Vector2f(180, 50)); nut_xem_so_du.setPosition(80, 150); nut_xem_so_du.setFillColor(Color(46,92,255)); // Nút xem số dư
Text txt_xem_so_du(L"Xem số dư", font, 24); txt_xem_so_du.setPosition(90, 160); txt_xem_so_du.setFillColor(Color::White); // Nhãn nút xem số dư

// Nạp tiền - #28A745 (40,167,69)
RectangleShape nut_nap_tien(Vector2f(180, 50)); nut_nap_tien.setPosition(80, 350); nut_nap_tien.setFillColor(Color(40,167,69)); // Nút nạp tiền
Text txt_nap_tien(L"Nạp tiền", font, 24); txt_nap_tien.setPosition(90, 360); txt_nap_tien.setFillColor(Color::White); // Nhãn nút nạp tiền

// Rút tiền - #D9534F (217,83,79)
RectangleShape nut_rut_tien(Vector2f(180, 50)); nut_rut_tien.setPosition(80, 550); nut_rut_tien.setFillColor(Color(217,83,79)); // Nút rút tiền
Text txt_rut_tien(L"Rút tiền", font, 24); txt_rut_tien.setPosition(90, 560); txt_rut_tien.setFillColor(Color::White); // Nhãn nút rút tiền

// Chuyển khoản - #1A73E8 (26,115,232)
RectangleShape nut_chuyen_khoan(Vector2f(180, 50)); nut_chuyen_khoan.setPosition(1100, 150); nut_chuyen_khoan.setFillColor(Color(26,115,232)); // Nút chuyển khoản
Text txt_chuyen_khoan(L"Chuyển khoản", font, 24); txt_chuyen_khoan.setPosition(1110, 160); txt_chuyen_khoan.setFillColor(Color::White); // Nhãn nút chuyển khoản

// Đăng xuất - #6C757D (108,117,125)
RectangleShape nut_dang_xuat(Vector2f(180, 50)); nut_dang_xuat.setPosition(600, 650); nut_dang_xuat.setFillColor(Color(108,117,125)); // Nút đăng xuất
Text txt_dang_xuat(L"Đăng xuất", font, 24); txt_dang_xuat.setPosition(625,660); txt_dang_xuat.setFillColor(Color::White); // Nhãn nút đăng xuất

    float so_tien = 0; // Biến số tiền cho các giao dịch
    wstring nhap_tien_msg; // Thông báo nhập tiền
    bool hien_nhap_tien = false; // Hiển thị hộp nhập tiền
    bool dang_nap_tien = false, dang_rut_tien = false; //   Trạng thái nạp/rút tiền
    string so_tien_input; // Chuỗi nhập số tiền

    bool hien_chuyen_khoan = false;  // Hiển thị giao diện chuyển khoản
    int buoc_chuyen = 0;  // Bước trong quy trình chuyển khoản
    string tai_khoan_nhan; // Tài khoản nhận trong chuyển khoản
    string so_tien_chuyen_input; // Chuỗi nhập số tiền chuyển
    bool hien_so_du = false; // Hiển thị số dư

struct giao_dich_cuoi {  // Cấu trúc giao dịch cuối cùng
    string loai; 
    string tu_tk; 
    string den_tk; 
    float so_tien_gd; 
    float so_du_sau; 
    bool hop_le; } gd_cuoi{"", "", "", 0.0f, 0.0f, false}; // Khởi tạo giao dịch cuối cùng

    RectangleShape nut_in(Vector2f(160, 40)); nut_in.setFillColor(Color(70,130,180)); // Nút in biên lai
    Text txt_in(L"In biên lai", font, 20); txt_in.setFillColor(Color::White); // Nhãn nút in biên lai

while (window.isOpen()) { // Vòng lặp chính của ứng dụng
    Event event; // Sự kiện SFML 
    while (window.pollEvent(event)) { // Xử lý sự kiện
        if (event.type == Event::Closed) window.close(); // Đóng cửa sổ khi nhận sự kiện đóng
        if (state == LOGIN) { // Trạng thái đăng nhập
            if (event.type == Event::MouseButtonPressed) { // Xử lý nhấn chuột
                auto x = event.mouseButton.x, y = event.mouseButton.y;  // Lấy tọa độ chuột
                dang_go_so_tk = hop_so_tk.getGlobalBounds().contains(x, y); // Kiểm tra nếu nhấn vào hộp số tài khoản  
                dang_go_mk = hop_mat_khau.getGlobalBounds().contains(x, y); // Kiểm tra nếu nhấn vào hộp mật khẩu
                if (nut_dang_nhap.getGlobalBounds().contains(x, y)) { // Kiểm tra nếu nhấn vào nút đăng nhập
                    if (ds.find(so_tk_input) != ds.end() && ds[so_tk_input].mk == mat_khau_input) { // Kiểm tra thông tin đăng nhập
                        state = MENU; // Chuyển sang trạng thái menu
                        ng_dung_hien_tai = &ds[so_tk_input]; // Lưu con trỏ đến tài khoản hiện tại
                        message = L""; // Xóa thông báo
                    } else {
                        message = L"Đăng nhập thất bại!"; // Thông báo đăng nhập thất bại
                    }
                }
            }
            if (event.type == Event::TextEntered) { // Xử lý nhập liệu
                if (dang_go_so_tk && event.text.unicode < 128 && event.text.unicode != 13) { // Nhập số tài khoản
                    if (event.text.unicode == 8 && !so_tk_input.empty()) so_tk_input.pop_back(); // Xóa ký tự cuối nếu nhấn backspace
                    else if (event.text.unicode != 8) so_tk_input += static_cast<char>(event.text.unicode); // Thêm ký tự mới
                }
                if (dang_go_mk && event.text.unicode < 128 && event.text.unicode != 13) { // Nhập mật khẩu
                    if (event.text.unicode == 8 && !mat_khau_input.empty()) mat_khau_input.pop_back(); // Xóa ký tự cuối nếu nhấn backspace
                    else if (event.text.unicode != 8) mat_khau_input += static_cast<char>(event.text.unicode);// Thêm ký tự mới
                }
            }
        } else if (state == MENU) { // Trạng thái menu
            if (event.type == Event::MouseButtonPressed) { // Xử lý nhấn chuột
                auto x = event.mouseButton.x, y = event.mouseButton.y; // Lấy tọa độ chuột
                if (nut_xem_so_du.getGlobalBounds().contains(x, y)) { //  Xem số dư
                    hien_so_du = true; // Hiển thị số dư
                    hien_nhap_tien = false; // Ẩn hộp nhập tiền
                    hien_chuyen_khoan = false; // Ẩn giao diện chuyển khoản
                    
                    message = L""; // Xóa thông báo
                    gd_cuoi.hop_le = false; // Vô hiệu hóa biên lai giao dịch cuối
                }
                if (nut_nap_tien.getGlobalBounds().contains(x, y)) { // Nạp tiền
                    hien_nhap_tien = true; dang_nap_tien = true; dang_rut_tien = false; so_tien_input = ""; nhap_tien_msg = L"Nhập số tiền nạp:"; // Hiển thị hộp nhập tiền và thiết lập trạng thái nạp tiền
                    hien_chuyen_khoan = false; hien_so_du = false; // Ẩn giao diện chuyển khoản và số dư
        
                    message = L""; gd_cuoi.hop_le = false; // Xóa thông báo và vô hiệu hóa biên lai giao dịch cuối
                }
                if (nut_rut_tien.getGlobalBounds().contains(x, y)) { // Rút tiền
                    hien_nhap_tien = true; dang_rut_tien = true; dang_nap_tien = false; so_tien_input = ""; nhap_tien_msg = L"Nhập số tiền rút:"; // Hiển thị hộp nhập tiền và thiết lập trạng thái rút tiền
                    hien_chuyen_khoan = false; hien_so_du = false; // Ẩn giao diện chuyển khoản và số dư
                    message = L""; gd_cuoi.hop_le = false; // Xóa thông báo và vô hiệu hóa biên lai giao dịch cuối
                }
                if (nut_chuyen_khoan.getGlobalBounds().contains(x, y)) { // Chuyển khoản
                    hien_chuyen_khoan = true; buoc_chuyen = 0; tai_khoan_nhan = ""; so_tien_chuyen_input = ""; nhap_tien_msg = L"Nhập số tài khoản nhận:"; // Hiển thị giao diện chuyển khoản và thiết lập bước đầu tiên
                    hien_nhap_tien = false; hien_so_du = false; // Ẩn giao diện nhập tiền và số dư
                    message = L""; gd_cuoi.hop_le = false; // Xóa thông báo và vô hiệu hóa biên lai giao dịch cuối
                }
                if (nut_dang_xuat.getGlobalBounds().contains(x, y)) { // Đăng xuất
                    state = LOGIN; ng_dung_hien_tai = nullptr; message = L""; so_tk_input = ""; mat_khau_input = ""; // Quay lại trạng thái đăng nhập và xóa thông tin
                    gd_cuoi.hop_le = false; // Vô hiệu hóa biên lai giao dịch cuối
                }

                if (gd_cuoi.hop_le && nut_in.getGlobalBounds().contains(x, y)) { // In biên lai nếu có giao dịch hợp lệ
                    string path = "receipt.rtf"; // Đường dẫn file biên lai
                    createReceiptRTF(path, gd_cuoi.loai, gd_cuoi.tu_tk, gd_cuoi.den_tk, gd_cuoi.so_tien_gd, gd_cuoi.so_du_sau); // Tạo biên lai RTF
                    #if defined(_WIN32) // Nếu hệ điều hành là Windows
                    ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL); // Mở file biên lai
                    #endif // Kết thúc kiểm tra hệ điều hành
                }
            }
            if (event.type == Event::TextEntered) { // Xử lý nhập liệu
                
            if (hien_nhap_tien && !hien_so_du) { // Nhập số tiền nạp/rút
                    if (event.text.unicode >= '0' && event.text.unicode <= '9') { // Chỉ cho phép nhập số
                        so_tien_input += static_cast<char>(event.text.unicode); // Thêm ký tự số vào chuỗi nhập
                    }
                    if (event.text.unicode == 8 && !so_tien_input.empty()) so_tien_input.pop_back(); // Xóa ký tự cuối nếu nhấn backspace
                    if (event.text.unicode == 13 && !so_tien_input.empty()) {  //  Xử lý khi nhấn Enter
                        so_tien = stof(so_tien_input); // Chuyển chuỗi nhập sang số thực
                        if (dang_nap_tien) { // Nạp tiền
                            ng_dung_hien_tai->so_du += so_tien; // Cộng số tiền vào số dư
                            if (!luu_ds_tk("accounts.txt", ds)) { // Lưu danh sách tài khoản
                                message = L"Lỗi lưu dữ liệu!"; // Thông báo lỗi lưu dữ liệu
                            } else {
                                message = L"Nạp thành công! Số dư: " + to_wstring(ng_dung_hien_tai->so_du); // Thông báo nạp thành công

                                gd_cuoi = {"Nạp tiền", ng_dung_hien_tai->stk, "", so_tien, ng_dung_hien_tai->so_du, true}; //   Lưu thông tin giao dịch cuối cùng
                            }
                        } else if (dang_rut_tien) { // Rút tiền
                            if (ng_dung_hien_tai->so_du >= so_tien) { // Kiểm tra đủ số dư
                                ng_dung_hien_tai->so_du -= so_tien; // Trừ số tiền từ số dư
                                if (!luu_ds_tk("accounts.txt", ds)) { // Lưu danh sách tài khoản
                                    message = L"Lỗi lưu dữ liệu!"; // Thông báo lỗi lưu dữ liệu
                                } else { 
                                    message = L"Rút thành công! Số dư: " + to_wstring(ng_dung_hien_tai->so_du); // Thông báo rút thành công
                                    gd_cuoi = {"Rút tiền", ng_dung_hien_tai->stk, "", so_tien, ng_dung_hien_tai->so_du, true}; // Lưu thông tin giao dịch cuối cùng
                                }
                            } else {
                                message = L"Không đủ số dư!"; // Thông báo không đủ số dư
                            }
                        }
                        hien_nhap_tien = false; dang_nap_tien = false; dang_rut_tien = false; so_tien_input = ""; // Ẩn hộp nhập tiền và đặt lại trạng thái
                    }
                }
                
            if (hien_chuyen_khoan && !hien_so_du) { // Chuyển khoản
                    if (buoc_chuyen == 0) { // Bước nhập tài khoản nhận
                        if (event.text.unicode >= 32 && event.text.unicode < 128) tai_khoan_nhan += static_cast<char>(event.text.unicode); // Thêm ký tự vào tài khoản nhận
                        if (event.text.unicode == 8 && !tai_khoan_nhan.empty()) tai_khoan_nhan.pop_back(); // Xóa ký tự cuối nếu nhấn backspace
                        if (event.text.unicode == 13 && !tai_khoan_nhan.empty()) { // Xử lý khi nhấn Enter
                            buoc_chuyen = 1; so_tien_chuyen_input = ""; nhap_tien_msg = L"Nhập số tiền chuyển:"; // Chuyển sang bước nhập số tiền chuyển
                        }
                    } else if (buoc_chuyen == 1) { // Bước nhập số tiền chuyển
                        if (event.text.unicode >= '0' && event.text.unicode <= '9') so_tien_chuyen_input += static_cast<char>(event.text.unicode); // Thêm ký tự số vào chuỗi nhập
                        if (event.text.unicode == 8 && !so_tien_chuyen_input.empty()) so_tien_chuyen_input.pop_back(); // Xóa ký tự cuối nếu nhấn backspace
                        if (event.text.unicode == 13 && !so_tien_chuyen_input.empty()) { // Xử lý khi nhấn Enter
                            float amt = stof(so_tien_chuyen_input); // Chuyển chuỗi nhập sang số thực
                            auto it = ds.find(tai_khoan_nhan); // Tìm tài khoản nhận trong danh sách
                            if (it == ds.end()) { // Nếu không tìm thấy tài khoản nhận
                                message = L"Tài khoản nhận không tồn tại!"; //  Thông báo lỗi tài khoản nhận không tồn tại
                            } else if (ng_dung_hien_tai->so_du < amt) { // Kiểm tra đủ số dư
                                message = L"Không đủ số dư!"; // Thông báo không đủ số dư
                            } else { // Thực hiện chuyển khoản
                                ng_dung_hien_tai->so_du -= amt; // Trừ số tiền từ số dư người gửi
                                it->second.so_du += amt; // Cộng số tiền vào số dư người nhận
                                if (!luu_ds_tk("accounts.txt", ds)) { // Lưu danh sách tài khoản
                                    message = L"Lỗi lưu dữ liệu!"; // Thông báo lỗi lưu dữ liệu
                                } else {
                                    message = L"Chuyển khoản thành công! Số dư: " + to_wstring(ng_dung_hien_tai->so_du); // Thông báo chuyển khoản thành công
                                    gd_cuoi = {"Chuyển khoản", ng_dung_hien_tai->stk, it->second.stk, amt, ng_dung_hien_tai->so_du, true}; // Lưu thông tin giao dịch cuối cùng
                                }
                            }
                            hien_chuyen_khoan = false; buoc_chuyen = 0; tai_khoan_nhan = ""; so_tien_chuyen_input = ""; // Ẩn giao diện chuyển khoản và đặt lại trạng thái
                        }
                    }
                }
            }
        }

        
    }
    txt_so_tk.setString(so_tk_input); // Cập nhật hiển thị số tài khoản
    txt_mat_khau.setString(string(mat_khau_input.size(), '*')); // Cập nhật hiển thị mật khẩu dưới dạng dấu '*'
    thong_bao.setString(message); // Cập nhật hiển thị thông báo

    window.clear(Color::White); //  Xóa cửa sổ với màu trắng
    if (state == LOGIN) { //    Trạng thái đăng nhập
        window.draw(bgSprite); // Vẽ hình nền
        window.draw(title); // Vẽ tiêu đề
        window.draw(labelStk); window.draw(labelMk); // Vẽ nhãn số tài khoản và mật khẩu
        window.draw(hop_so_tk); window.draw(hop_mat_khau); // Vẽ hộp nhập số tài khoản và mật khẩu
        window.draw(txt_so_tk); window.draw(txt_mat_khau); // Vẽ hiển thị số tài khoản và mật khẩu
        window.draw(nut_dang_nhap); window.draw(txt_dang_nhap); // Vẽ nút đăng nhập
        window.draw(thong_bao); // Vẽ thông báo
    } else if (state == MENU) { // Trạng thái menu

        float winW = (float)window.getSize().x; //  Lấy kích thước cửa sổ
        float winH = (float)window.getSize().y; //  Lấy kích thước cửa sổ
        float texW = (float)menuTexture.getSize().x; // Lấy kích thước hình menu
        float texH = (float)menuTexture.getSize().y; // Lấy kích thước hình menu
        if (texW > 0 && texH > 0) { //  Kiểm tra kích thước hình menu hợp lệ
            float scaleX = winW / texW; // Tính tỷ lệ co theo chiều rộng
            float scaleY = winH / texH; // Tính tỷ lệ co theo chiều cao


            float scale = max(scaleX, scaleY); // Chọn tỷ lệ co lớn hơn để bao phủ toàn bộ cửa sổ
            menuSprite.setScale(scale, scale); // Áp dụng tỷ lệ co cho sprite hình menu
           
            float dispW = texW * scale; // Tính kích thước hiển thị sau khi co
            float dispH = texH * scale; // Tính kích thước hiển thị sau khi co
            menuSprite.setPosition((winW - dispW) / 2.f, (winH - dispH) / 2.f); // Đặt vị trí để căn giữa hình menu
        }
        window.draw(menuSprite); // Vẽ hình menu
            float panelW = winW * 0.55f; // Kích thước bảng điều khiển
            float panelH = winH * 0.6f;  // Kích thước bảng điều khiển
            float panelX = (winW - panelW) / 2.f; // Vị trí X của bảng điều khiển
            float panelY = (winH - panelH) / 2.f; // Vị trí Y của bảng điều khiển
            float cornerRadius = 14.f; // Bán kính góc bảng điều khiển
            float padding = 20.f; // Khoảng cách lề bảng điều khiển

            RectangleShape panelShadow(Vector2f(panelW + 8.f, panelH + 8.f)); // Bóng đổ bảng điều khiển
            panelShadow.setPosition(panelX + 6.f, panelY + 6.f); // Vị trí bóng đổ
            panelShadow.setFillColor(Color(0, 0, 0, 80)); // Màu bóng đổ
            panelShadow.setOutlineThickness(0); // Độ dày viền bóng đổ
            window.draw(panelShadow); // Vẽ bóng đổ bảng điều khiển

            RectangleShape panel(Vector2f(panelW, panelH)); // Bảng điều khiển chính
            panel.setPosition(panelX, panelY); // Vị trí bảng điều khiển
            panel.setFillColor(Color(0xF2, 0xF4, 0xFA)); // Màu nền bảng điều khiển
            panel.setOutlineColor(Color(200,200,200)); // Màu viền bảng điều khiển
            panel.setOutlineThickness(1.f); // Độ dày viền bảng điều khiển
            window.draw(panel); // Vẽ bảng điều khiển
    

        
        window.draw(nut_xem_so_du); window.draw(txt_xem_so_du); // Vẽ nút xem số dư
        window.draw(nut_nap_tien); window.draw(txt_nap_tien); // Vẽ nút nạp tiền
        window.draw(nut_rut_tien); window.draw(txt_rut_tien); // Vẽ nút rút tiền
        window.draw(nut_chuyen_khoan); window.draw(txt_chuyen_khoan); // Vẽ nút chuyển khoản
        window.draw(nut_dang_xuat); window.draw(txt_dang_xuat); // Vẽ nút đăng xuất
        window.draw(thong_bao); // Vẽ thông báo

            
            float contentX = panelX + padding; // Vị trí X nội dung bên trong bảng điều khiển
            float contentW = panelW - padding*2.f; // Chiều rộng nội dung bên trong bảng điều khiển
            float currentY = panelY + padding + 10.f; // Vị trí Y hiện tại để vẽ nội dung bên trong bảng điều khiển

            if (hien_nhap_tien) { // Hiển thị hộp nhập tiền
                
                RectangleShape boxNhap(Vector2f(contentW, 120.f)); //   Hộp nhập tiền
                boxNhap.setPosition(contentX, currentY); // Vị trí hộp nhập tiền
                boxNhap.setFillColor(Color(0xEE,0xEF,0xF5)); // Màu nền hộp nhập tiền
                boxNhap.setOutlineColor(Color(190,190,190)); // Màu viền hộp nhập tiền
                boxNhap.setOutlineThickness(1.f); // Độ dày viền hộp nhập tiền
                window.draw(boxNhap); // Vẽ hộp nhập tiền

                Text txtNhap(nhap_tien_msg, font, 22); txtNhap.setPosition(contentX + 12.f, currentY + 12.f); txtNhap.setFillColor(Color::Black); // Nhãn nhập tiền
                Text txtSoTien(so_tien_input, font, 22); txtSoTien.setPosition(contentX + 12.f, currentY + 56.f); txtSoTien.setFillColor(Color::Black); // Hiển thị số tiền nhập
                window.draw(txtNhap); window.draw(txtSoTien);  //       Vẽ nhãn và hiển thị số tiền nhập
            }
                if (hien_chuyen_khoan) { // Hiển thị giao diện chuyển khoản
                RectangleShape boxTransfer(Vector2f(contentW, 140.f)); // Hộp chuyển khoản
                boxTransfer.setPosition(contentX, currentY); // Vị trí hộp chuyển khoản
                boxTransfer.setFillColor(Color(0xEE,0xEF,0xF5)); // Màu nền hộp chuyển khoản
                boxTransfer.setOutlineColor(Color(190,190,190)); // Màu viền hộp chuyển khoản
                boxTransfer.setOutlineThickness(1.f); // Độ dày viền hộp chuyển khoản
                window.draw(boxTransfer); // Vẽ hộp chuyển khoản
                if (buoc_chuyen == 0) { // Bước nhập tài khoản nhận
                    Text txtTarget(nhap_tien_msg, font, 22); txtTarget.setPosition(contentX + 12.f, currentY + 12.f); txtTarget.setFillColor(Color::Black); // Nhãn tài khoản nhận
                    Text txtTargetVal(tai_khoan_nhan, font, 22); txtTargetVal.setPosition(contentX + 12.f, currentY + 56.f); txtTargetVal.setFillColor(Color::Black); //    Hiển thị tài khoản nhận
                    window.draw(txtTarget); window.draw(txtTargetVal); // Vẽ nhãn và hiển thị tài khoản nhận    
                } else if (buoc_chuyen == 1) { // Bước nhập số tiền chuyển
                    Text txtAmt(nhap_tien_msg, font, 22); txtAmt.setPosition(contentX + 12.f, currentY + 12.f); txtAmt.setFillColor(Color::Black); // Nhãn số tiền chuyển
                    Text txtAmtVal(so_tien_chuyen_input, font, 22); txtAmtVal.setPosition(contentX + 12.f, currentY + 56.f); txtAmtVal.setFillColor(Color::Black); // Hiển thị số tiền chuyển
                    window.draw(txtAmt); window.draw(txtAmtVal); // Vẽ nhãn và hiển thị số tiền chuyển
                }
            }

                if (hien_so_du) { // Hiển thị số dư hiện tại
                RectangleShape balBox(Vector2f(contentW, 100.f)); // Hộp số dư
                balBox.setPosition(contentX, currentY); // Vị trí hộp số dư
                balBox.setFillColor(Color(0xEE,0xEF,0xF5)); // Màu nền hộp số dư
                balBox.setOutlineColor(Color(190,190,190)); // Màu viền hộp số dư
                balBox.setOutlineThickness(1.f); // Độ dày viền hộp số dư
                window.draw(balBox); // Vẽ hộp số dư
                wstringstream wss; // Chuỗi luồng để định dạng số dư
                wss << fixed << setprecision(2) << ng_dung_hien_tai->so_du; // Định dạng số dư với 2 chữ số thập phân
                wstring balText = L"Số dư hiện tại: " + wss.str(); // Chuỗi hiển thị số dư
                Text txtBal(sf::String(balText), font, 24); // Tạo đối tượng Text để hiển thị số dư
                txtBal.setPosition(contentX + 12.f, currentY + 28.f); // Vị trí hiển thị số dư
                txtBal.setFillColor(Color::Black); // Màu chữ số dư
                window.draw(txtBal); // Vẽ hiển thị số dư
            }

            if (gd_cuoi.hop_le) { // Hiển thị nút in biên lai nếu có giao dịch hợp lệ
                float printX = panelX + panelW - padding - nut_in.getSize().x; // Vị trí X nút in biên lai
                float printY = panelY + panelH - padding - nut_in.getSize().y; // Vị trí Y nút in biên lai
                nut_in.setPosition(printX, printY-100); // Đặt vị trí nút in biên lai
                txt_in.setPosition(printX + 12.f, printY-100 + 8.f); // Đặt vị trí nhãn nút in biên lai
                window.draw(nut_in); window.draw(txt_in); // Vẽ nút in biên lai   
            }
    }
    window.display(); // Hiển thị nội dung đã vẽ lên cửa sổ
  }
}
//============================================================== Phần tạo biên lai RTF(RTF là định dạng văn bản phong phú, Rich Text Format) ==============================================================
void createReceiptRTF(const string &path, const string &type, const string &fromAccount, const string &toAccount, float amount, float balance) { // Tạo biên lai RTF
    ofstream f(path, ios::trunc); // Mở file để ghi
    if (!f.is_open()) return; // Kiểm tra nếu không mở được file thì thoát

    auto utf8_to_wstring = [](const string &s)->wstring{ // Chuyển đổi chuỗi UTF-8 sang wstring
#if defined(_WIN32) // Nếu đang chạy trên Windows
        if (s.empty()) return L""; //   Kiểm tra chuỗi rỗng
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0); // Tính kích thước cần thiết cho wstring
        if (size_needed <= 0) return wstring(s.begin(), s.end()); // Trả về wstring nếu kích thước không hợp lệ
        wstring wstrTo(size_needed, 0); // Tạo wstring với kích thước cần thiết
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &wstrTo[0], size_needed); // Chuyển đổi chuỗi UTF-8 sang wstring
        return wstrTo; //   Trả về wstring đã chuyển đổi
#else // Nếu không phải Windows
        return wstring(s.begin(), s.end()); // Trả về wstring bằng cách sao chép ký tự
#endif // Kết thúc kiểm tra hệ điều hành
    };

    auto rtfEscape = [](const wstring &w)->string{ // Chuyển đổi wstring sang chuỗi RTF với ký tự đặc biệt được thoát
        string out; // Chuỗi đầu ra
        for (wchar_t wc : w) { //   Duyệt qua từng ký tự trong wstring
            if (wc == L'\\' || wc == L'{' || wc == L'}') { // Kiểm tra ký tự đặc biệt trong RTF
                out.push_back('\\'); // Thoát ký tự đặc biệt
                out.push_back((char)wc); // Thêm ký tự đặc biệt vào chuỗi đầu ra
                continue; // Tiếp tục vòng lặp
            }
            if (wc >= 0x20 && wc < 0x7f) { // Kiểm tra ký tự ASCII in được
                out.push_back((char)wc); // Thêm ký tự vào chuỗi đầu ra
            } else { // Ký tự không phải ASCII in được
                int n = (int)wc; // Chuyển wchar_t sang int
                if (n > 32767) n -= 65536; // Điều chỉnh giá trị nếu vượt quá phạm vi
                out += "\\u" + to_string(n) + "?"; // Thêm ký tự Unicode vào chuỗi đầu ra theo định dạng RTF
            }
        }
        return out; // Trả về chuỗi RTF đã chuyển đổi
    };

    f << "{\\rtf1\\ansi\\deff0{"; //    Bắt đầu tài liệu RTF
    f << "\\fonttbl{\\f0 Arial;}}\\uc1\n"; // Định nghĩa bảng font chữ với Arial
    f << "\\f0\\fs24\n"; // Thiết lập font chữ và kích thước font

    wstring title = L"BIÊN LAI GIAO DỊCH"; //       Tiêu đề biên lai
    wstring wtype = L"Loại: "; // Nhãn loại giao dịch
    wstring wfrom = L"Tài khoản: "; // Nhãn tài khoản gửi
    wstring wto = L"Tài khoản nhận: "; // Nhãn tài khoản nhận
    wstring wamount = L"Số tiền: "; // Nhãn số tiền
    wstring wbal = L"Số dư: "; // Nhãn số dư
    wstring wtime = L"Thời gian: "; // Nhãn thời gian

    f << rtfEscape(title) << "\\line\\line\n"; //   Viết tiêu đề biên lai và xuống dòng
 
    wstring wtypeFull = wtype + utf8_to_wstring(type); // Chuỗi loại giao dịch đầy đủ
    f << rtfEscape(wtypeFull) << "\\line\n"; // Viết loại giao dịch và xuống dòng

    wstring wfromFull = wfrom + utf8_to_wstring(fromAccount); // Chuỗi tài khoản gửi đầy đủ
    f << rtfEscape(wfromFull) << "\\line\n"; // Viết tài khoản gửi và xuống dòng
    if (!toAccount.empty()) { // Kiểm tra nếu có tài khoản nhận
        wstring wtoFull = wto + utf8_to_wstring(toAccount); // Chuỗi tài khoản nhận đầy đủ
        f << rtfEscape(wtoFull) << "\\line\n"; // Viết tài khoản nhận và xuống dòng
    }

    {
        wostringstream wss; // Chuỗi luồng để định dạng số tiền
        wss << fixed << setprecision(2) << amount; //   Định dạng số tiền với 2 chữ số thập phân
        wstring wam = wamount + wss.str(); // Chuỗi số tiền đầy đủ
        f << rtfEscape(wam) << "\\line\n"; // Viết số tiền và xuống dòng
    } 

    {
        wostringstream wss; // Chuỗi luồng để định dạng số dư
        wss << fixed << setprecision(2) << balance; //   Định dạng số dư với 2 chữ số thập phân
        wstring wbo = wbal + wss.str(); // Chuỗi số dư đầy đủ
        f << rtfEscape(wbo) << "\\line\n"; // Viết số dư và xuống dòng
    }

    // timestamp
    time_t now = time(nullptr); // Lấy thời gian hiện tại
    char buf[64]; // Bộ đệm để lưu trữ chuỗi thời gian
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now)); // Định dạng thời gian thành chuỗi
    wstring wbuf = wtime + wstring(buf, buf + strlen(buf)); // Chuỗi thời gian đầy đủ
    f << rtfEscape(wbuf) << "\\line\n"; // Viết thời gian và xuống dòng

    f << "}\n"; // Kết thúc tài liệu RTF
    f.close(); // Đóng file
}// Kết thúc hàm tạo biên lai RTF
//============================================================== Kết thúc phần tạo biên lai RTF ==============================================================