// ================================================================
//  HE THONG QUAN LY GAO TON KHO & DOANH THU SIEU THI
//  - C++98/Dev-C++ console, cay AVL theo ma mat hang
//  - Bo cuc file gom 9 phan danh so (xem cac tieu de "// N. ..."):
//      1. Cau truc du lieu mat hang      6. Doc / ghi file nhi phan
//      2. Cau truc nut cay AVL           7. Hien thi du lieu & tinh nang nghiep vu
//      3. Cac ham bo tro cay AVL         8. Tien ich don bo dem
//      4. Bon phep quay can bang AVL     9. Chuong trinh chinh
//      5. Cac thao tac tren cay AVL
//  - Ban hang: ban theo kg hoac theo bao (nhieu dong trong 1 don).
//  - Tra cuu doanh thu & Top doanh thu: hom nay / ngay tuy chon / theo thang / theo nam.
//  - Tu dong luu file sau moi thay doi (kem file .bak). Nhap hang tinh gia nhap binh quan.
//  - Ngung ban: an mat hang nhung van giu doanh thu trong bao cao.
//  - Quy uoc nhap lieu: Enter = di tiep; '<' = quay lai; man hinh tong hop: Enter = luu, 1-9 = sua, 10 = huy.
// ================================================================
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <limits>
#include <sstream>
#include <ctime>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <cstdio>
#ifdef _WIN32
#include <conio.h>
#endif

using namespace std;

const char FILE_HEADER_MAGIC[] = "GAO_v6";
const char DATA_FILE[] = "danh_muc_gao.dat";
const int MAX_HISTORY = 4000;   // so ngay toi da luu lich su cho moi mat hang (de xem duoc theo thang / nam)

// ---------- (Ngoai cac phan) KHAI BAO TRUOC cac ham tien ich o phan 8 ----------
string trim(const string& s);
string toLowerVN(const string& s);
bool parseIntStrict(const string& s, int& value);
bool parseDoubleStrict(const string& s, double& value);
void gLine(string& s);
string readLinePrompt(const string& prompt);
template<size_t N> bool setStr(char (&dst)[N], const string& v);

// ================================================================
// 1. CAU TRUC DU LIEU MAT HANG
// ================================================================
struct NgayThang {
    int ngay, thang, nam;
    NgayThang(int d=1, int m=1, int y=2000) : ngay(d), thang(m), nam(y) {}
};

bool operator==(const NgayThang& a, const NgayThang& b) {
    return a.ngay == b.ngay && a.thang == b.thang && a.nam == b.nam;
}

bool laNamNhuan(int y) { return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0); }

int soNgayTrongThang(int m, int y) {
    static int d[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (m == 2) return d[m] + (laNamNhuan(y) ? 1 : 0);
    if (m >= 1 && m <= 12) return d[m];
    return 0;
}

bool ngayHopLe(const NgayThang& x) {
    return x.nam >= 2000 && x.nam <= 2500 && x.thang >= 1 && x.thang <= 12
        && x.ngay >= 1 && x.ngay <= soNgayTrongThang(x.thang, x.nam);
}

NgayThang homNay() {
    time_t now = time(NULL);
    tm* lt = localtime(&now);
    return NgayThang(lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900);
}

long long soThuTuNgay(const NgayThang& x) {
    long long y = x.nam, m = x.thang, d = x.ngay;
    y -= (m <= 2);
    long long era = (y >= 0 ? y : y - 399) / 400;
    unsigned yoe = (unsigned)(y - era * 400);
    unsigned doy = (unsigned)((153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1);
    unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + (long long)doe;
}

int soNgayGiua(const NgayThang& a, const NgayThang& b) { return (int)(soThuTuNgay(b) - soThuTuNgay(a)); }  // b - a

string ngayThanhChuoi(const NgayThang& x) {
    ostringstream oss;
    oss << setfill('0') << setw(2) << x.ngay << "/" << setw(2) << x.thang << "/" << setw(4) << x.nam;
    return oss.str();
}

struct ThongTinGao {
    char loaiGao[50];
    char nhaSanXuat[50];
    char xuatXu[60];
    char dangHat[80];
    char doDeo[50];
    char thanhPhan[100];
    double nangLuongKcal100g;
    char muiHuong[80];
    ThongTinGao() { memset(this, 0, sizeof(*this)); }
};

struct SoBaoBan {
    int kgMotBao; int soBao;
    SoBaoBan(int k = 0, int n = 0) : kgMotBao(k), soBao(n) {}
};

struct NhatKyNgay {
    NgayThang ngayGhi;
    double doanhThu;
    double giaVon;
    double soKg;                // tong kg ban trong ngay (-1 = khong ro, du lieu tu ban cu)
    vector<SoBaoBan> bao;       // so bao ban ra theo tung loai
    NhatKyNgay() : doanhThu(0), giaVon(0), soKg(0) {}
};

struct NhatKyNhap {
    NgayThang ngayNhap;
    double soKg;
    double giaNhapKg;           // gia nhap cua lo do
    NhatKyNhap() : soKg(0), giaNhapKg(0) {}
};

void quyCachMacDinh(vector<int>& v) { v.clear(); v.push_back(50); v.push_back(25); v.push_back(10); v.push_back(5); }

struct MatHang {
    char maMH[15];
    char tenMH[50];
    double soKgTon;
    int tonToiThieuKg;
    double giaNhapKg;
    double giaBanKg;
    double doanhThu;
    double giaVonHangBan;
    NgayThang hanSuDung;
    vector<int> quyCach;          // cac loai bao co the dong (kg)
    ThongTinGao gao;
    vector<NhatKyNgay> lichSuNgay;
    bool ngungBan;                // true = ngung ban (an khoi danh sach, van giu doanh thu)
    vector<NhatKyNhap> lichSuNhap;

    MatHang() : soKgTon(0), tonToiThieuKg(0), giaNhapKg(0), giaBanKg(0), doanhThu(0), giaVonHangBan(0), hanSuDung(), ngungBan(false) {
        memset(maMH,0,sizeof(maMH));
        memset(tenMH,0,sizeof(tenMH));
        quyCachMacDinh(quyCach);
    }
};

// ================================================================
// 2. CAU TRUC NUT CAY AVL
// ================================================================
struct NodeAVL {
    MatHang data;
    NodeAVL* left;
    NodeAVL* right;
    int height;
    NodeAVL(const MatHang& item) : data(item), left(NULL), right(NULL), height(1) {}
};

void traCuuKhoangGia(NodeAVL* root);

// ================================================================
// 3. CAC HAM BO TRO CAY AVL
// ================================================================
int getHeight(NodeAVL* n) { return n ? n->height : 0; }
int getBalanceFactor(NodeAVL* n) { return n ? getHeight(n->left) - getHeight(n->right) : 0; }
void updateHeight(NodeAVL* n) { if (n) n->height = 1 + max(getHeight(n->left), getHeight(n->right)); }
NodeAVL* findMinNode(NodeAVL* n) { while (n && n->left) n = n->left; return n; }

// ================================================================
// 4. BON PHEP QUAY CAN BANG CAY AVL (LL, RR, LR, RL)
// ================================================================
NodeAVL* rotateLL(NodeAVL* y) {            // lech trai-trai: quay phai
    NodeAVL* x = y->left; NodeAVL* t = x->right;
    x->right = y; y->left = t;
    updateHeight(y); updateHeight(x);
    return x;
}
NodeAVL* rotateRR(NodeAVL* x) {            // lech phai-phai: quay trai
    NodeAVL* y = x->right; NodeAVL* t = y->left;
    y->left = x; x->right = t;
    updateHeight(x); updateHeight(y);
    return y;
}
NodeAVL* rotateLR(NodeAVL* n) {            // lech trai-phai: quay trai con trai, roi quay phai
    n->left = rotateRR(n->left);
    return rotateLL(n);
}
NodeAVL* rotateRL(NodeAVL* n) {            // lech phai-trai: quay phai con phai, roi quay trai
    n->right = rotateLL(n->right);
    return rotateRR(n);
}
NodeAVL* balanceNode(NodeAVL* n) {
    if (!n) return NULL;
    updateHeight(n);
    int bf = getBalanceFactor(n);
    if (bf > 1 && getBalanceFactor(n->left) >= 0) return rotateLL(n);
    if (bf > 1) return rotateLR(n);
    if (bf < -1 && getBalanceFactor(n->right) <= 0) return rotateRR(n);
    if (bf < -1) return rotateRL(n);
    return n;
}

// ================================================================
// 5. CAC THAO TAC TREN CAY AVL
// ================================================================
NodeAVL* insertNode(NodeAVL* node, const MatHang& item) {
    if (!node) return new NodeAVL(item);
    int cmp = strcmp(item.maMH, node->data.maMH);
    if (cmp < 0) node->left = insertNode(node->left, item);
    else if (cmp > 0) node->right = insertNode(node->right, item);
    else return node;
    return balanceNode(node);
}

NodeAVL* searchNode(NodeAVL* root, const char* ma) {
    if (!root || strcmp(root->data.maMH, ma) == 0) return root;
    if (strcmp(ma, root->data.maMH) < 0) return searchNode(root->left, ma);
    return searchNode(root->right, ma);
}

NodeAVL* deleteNode(NodeAVL* node, const char* ma) {
    if (!node) return NULL;
    int cmp = strcmp(ma, node->data.maMH);
    if (cmp < 0) node->left = deleteNode(node->left, ma);
    else if (cmp > 0) node->right = deleteNode(node->right, ma);
    else {
        if (!node->left || !node->right) {
            NodeAVL* temp = node->left ? node->left : node->right;
            if (!temp) { temp = node; node = NULL; }
            else { *node = *temp; }
            delete temp;
        } else {
            NodeAVL* temp = findMinNode(node->right);
            node->data = temp->data;
            node->right = deleteNode(node->right, temp->data.maMH);
        }
    }
    if (!node) return NULL;
    return balanceNode(node);
}

void deleteTree(NodeAVL* root) { if (root) { deleteTree(root->left); deleteTree(root->right); delete root; } }

void collectNodes(NodeAVL* root, vector<MatHang>& v) {   // duyet LNR: tang dan theo ma
    if (!root) return;
    collectNodes(root->left, v); v.push_back(root->data); collectNodes(root->right, v);
}

// ================================================================
// 6. THAO TAC DOC / GHI FILE NHI PHAN
// ================================================================
void writeDateF(ofstream& out, const NgayThang& d) { out.write((char*)&d, sizeof(d)); }
void readDateF(ifstream& in, NgayThang& d) { in.read((char*)&d, sizeof(d)); }
void writeGao(ofstream& out, const ThongTinGao& g) { out.write((char*)&g, sizeof(g)); }
void readGao(ifstream& in, ThongTinGao& g) { in.read((char*)&g, sizeof(g)); }

void writeQuyCach(ofstream& out, const vector<int>& q) {
    int n = (int)q.size(); out.write((char*)&n, sizeof(n));
    for (int i = 0; i < n; ++i) { int v = q[i]; out.write((char*)&v, sizeof(v)); }
}
bool readQuyCach(ifstream& in, vector<int>& q) {
    q.clear(); int n = 0; in.read((char*)&n, sizeof(n));
    if (!in || n < 0 || n > 20) return false;
    for (int i = 0; i < n; ++i) { int v = 0; in.read((char*)&v, sizeof(v)); if (!in) return false; q.push_back(v); }
    return true;
}

// Nhat ky ban hang theo ngay: ngay, doanh thu, gia von, so kg, so bao tung loai
void writeHistory(ofstream& out, const vector<NhatKyNgay>& v) {
    int n = (int)v.size(); out.write((char*)&n, sizeof(n));
    for (int i = 0; i < n; ++i) {
        writeDateF(out, v[i].ngayGhi);
        out.write((char*)&v[i].doanhThu, sizeof(double)); out.write((char*)&v[i].giaVon, sizeof(double));
        out.write((char*)&v[i].soKg, sizeof(double));
        int nb = (int)v[i].bao.size(); out.write((char*)&nb, sizeof(nb));
        for (int j = 0; j < nb; ++j) {
            out.write((char*)&v[i].bao[j].kgMotBao, sizeof(int)); out.write((char*)&v[i].bao[j].soBao, sizeof(int));
        }
    }
}
// coChiTiet = false: file cu (v4, v5) chi co doanh thu + gia von -> so kg dat -1 (khong ro)
bool readHistory(ifstream& in, vector<NhatKyNgay>& v, bool coChiTiet) {
    int n = 0; in.read((char*)&n, sizeof(n));
    if (!in || n < 0 || n > MAX_HISTORY) return false;
    for (int i = 0; i < n; ++i) {
        NhatKyNgay x; readDateF(in, x.ngayGhi);
        in.read((char*)&x.doanhThu, sizeof(double)); in.read((char*)&x.giaVon, sizeof(double));
        if (coChiTiet) {
            in.read((char*)&x.soKg, sizeof(double));
            int nb = 0; in.read((char*)&nb, sizeof(nb));
            if (!in || nb < 0 || nb > 50) return false;
            for (int j = 0; j < nb; ++j) {
                SoBaoBan b; in.read((char*)&b.kgMotBao, sizeof(int)); in.read((char*)&b.soBao, sizeof(int));
                if (!in) return false;
                x.bao.push_back(b);
            }
        } else x.soKg = -1;
        if (!in) return false;
        v.push_back(x);
    }
    return true;
}

// Lich su nhap hang: ngay, so kg, gia nhap cua lo do
void writeImportHistory(ofstream& out, const vector<NhatKyNhap>& v) {
    int n = (int)v.size(); out.write((char*)&n, sizeof(n));
    for (int i = 0; i < n; ++i) {
        writeDateF(out, v[i].ngayNhap);
        out.write((char*)&v[i].soKg, sizeof(double)); out.write((char*)&v[i].giaNhapKg, sizeof(double));
    }
}
bool readImportHistory(ifstream& in, vector<NhatKyNhap>& v) {
    int n = 0; in.read((char*)&n, sizeof(n));
    if (!in || n < 0 || n > MAX_HISTORY) return false;
    for (int i = 0; i < n; ++i) {
        NhatKyNhap x; readDateF(in, x.ngayNhap);
        in.read((char*)&x.soKg, sizeof(double)); in.read((char*)&x.giaNhapKg, sizeof(double));
        if (!in) return false;
        v.push_back(x);
    }
    return true;
}

void writeNodeToFile(NodeAVL* n, ofstream& out) {
    if (!n) return;
    out.write(n->data.maMH, sizeof(n->data.maMH)); out.write(n->data.tenMH, sizeof(n->data.tenMH));
    out.write((char*)&n->data.soKgTon, sizeof(double)); out.write((char*)&n->data.tonToiThieuKg, sizeof(int));
    out.write((char*)&n->data.giaNhapKg, sizeof(double)); out.write((char*)&n->data.giaBanKg, sizeof(double));
    out.write((char*)&n->data.doanhThu, sizeof(double)); out.write((char*)&n->data.giaVonHangBan, sizeof(double));
    writeDateF(out, n->data.hanSuDung); writeQuyCach(out, n->data.quyCach);
    writeGao(out, n->data.gao); writeHistory(out, n->data.lichSuNgay);
    int ng = n->data.ngungBan ? 1 : 0; out.write((char*)&ng, sizeof(ng));
    writeImportHistory(out, n->data.lichSuNhap);
    writeNodeToFile(n->left, out); writeNodeToFile(n->right, out);
}

// Luu an toan: ghi ra file tam, ghi that bai thi khong dung den file cu.
// File cu duoc giu lai thanh ".bak" (ban luu truoc do).
bool saveToFile(NodeAVL* root, const char* filename, bool showMsg = true) {
    string tmp = string(filename) + ".tmp", bak = string(filename) + ".bak";
    {
        ofstream out(tmp.c_str(), ios::binary | ios::trunc);
        if (!out) { cout << "Loi: Khong the mo file de ghi!\n"; return false; }
        out.write(FILE_HEADER_MAGIC, sizeof(FILE_HEADER_MAGIC));
        writeNodeToFile(root, out); out.flush();
        if (!out) { cout << "Loi: Ghi file that bai (day o dia?).\n"; out.close(); remove(tmp.c_str()); return false; }
    }
    remove(bak.c_str());
    rename(filename, bak.c_str());              // chua co file cu thi bo qua
    if (rename(tmp.c_str(), filename) != 0) {
        cout << "Loi: Khong the thay file du lieu. Du lieu moi dang nam trong " << tmp << "\n";
        return false;
    }
    if (showMsg) cout << "Da luu du lieu vao file thanh cong.\n";
    return true;
}

// Tu dong luu sau moi thay doi (im lang neu thanh cong)
bool luuTuDong(NodeAVL* root) { return saveToFile(root, DATA_FILE, false); }

NodeAVL* loadFromFile(NodeAVL* root, const char* filename) {
    ifstream in(filename, ios::binary);
    if (!in) {      // khong co file chinh -> thu file backup
        in.clear();
        string bak = string(filename) + ".bak";
        in.open(bak.c_str(), ios::binary);
        if (!in) return root;
        cout << "Khong doc duoc file du lieu chinh, dang dung ban backup.\n";
    }
    char header[sizeof(FILE_HEADER_MAGIC)]; in.read(header, sizeof(header));
    int ver = 0;   // v6: them so kg ban, so bao, ngung ban, lich su nhap; v5: nhieu quy cach bao; v4: 1 don vi bao
    if (in.gcount() == (streamsize)sizeof(header)) {
        if (strncmp(header, "GAO_v6", 6) == 0) ver = 6;
        else if (strncmp(header, "GAO_v5", 6) == 0) ver = 5;
        else if (strncmp(header, "GAO_v4", 6) == 0) ver = 4;
    }
    if (ver == 0) { cout << "CANH BAO: File cu khac dinh dang. Khoi tao kho gao moi.\n"; in.close(); return root; }
    while (true) {
        MatHang x; in.read(x.maMH, sizeof(x.maMH)); if (in.gcount() != (streamsize)sizeof(x.maMH)) break;
        in.read(x.tenMH, sizeof(x.tenMH)); in.read((char*)&x.soKgTon, sizeof(double)); in.read((char*)&x.tonToiThieuKg, sizeof(int));
        in.read((char*)&x.giaNhapKg, sizeof(double)); in.read((char*)&x.giaBanKg, sizeof(double));
        in.read((char*)&x.doanhThu, sizeof(double)); in.read((char*)&x.giaVonHangBan, sizeof(double));
        readDateF(in, x.hanSuDung);
        if (ver >= 5) { if (!readQuyCach(in, x.quyCach)) break; }
        else {
            int old = 50; in.read((char*)&old, sizeof(int)); quyCachMacDinh(x.quyCach);
            if (old > 0 && find(x.quyCach.begin(), x.quyCach.end(), old) == x.quyCach.end()) x.quyCach.push_back(old);
        }
        readGao(in, x.gao);
        if (!readHistory(in, x.lichSuNgay, ver >= 6)) break;
        if (ver >= 6) {
            int ng = 0; in.read((char*)&ng, sizeof(ng)); x.ngungBan = (ng != 0);
            if (!readImportHistory(in, x.lichSuNhap)) break;
        }
        if (!in) break;
        root = insertNode(root, x);
    }
    in.close(); return root;
}

// ---------- (Ngoai cac phan) GOI Y & CHON NHANH BANG PHIM ----------
static const char* GOI_Y_LOAI_GAO_ARR[] = { "Gao ST25", "Gao Jasmine", "Gao Japonica", "Gao Bac Thom", "Gao Tai Nguyen", "Gao Lut", "Gao Nat" };
vector<string> GOI_Y_LOAI_GAO(GOI_Y_LOAI_GAO_ARR, GOI_Y_LOAI_GAO_ARR + sizeof(GOI_Y_LOAI_GAO_ARR)/sizeof(GOI_Y_LOAI_GAO_ARR[0]));
static const char* GOI_Y_XUAT_XU_ARR[] = { "Viet Nam - Soc Trang", "Viet Nam - An Giang", "Viet Nam - Dong Thap", "Viet Nam - Kien Giang", "Viet Nam - Long An" };
vector<string> GOI_Y_XUAT_XU(GOI_Y_XUAT_XU_ARR, GOI_Y_XUAT_XU_ARR + sizeof(GOI_Y_XUAT_XU_ARR)/sizeof(GOI_Y_XUAT_XU_ARR[0]));
static const char* GOI_Y_NHA_SAN_XUAT_ARR[] = { "Loc Troi", "Vinh Hien", "Co May", "Tan Long", "Angimex" };
vector<string> GOI_Y_NHA_SAN_XUAT(GOI_Y_NHA_SAN_XUAT_ARR, GOI_Y_NHA_SAN_XUAT_ARR + sizeof(GOI_Y_NHA_SAN_XUAT_ARR)/sizeof(GOI_Y_NHA_SAN_XUAT_ARR[0]));
static const char* GOI_Y_DANG_HAT_ARR[] = { "Hat dai, thon", "Hat dai, trong", "Hat tron, min", "Hat trung binh, trang" };
vector<string> GOI_Y_DANG_HAT(GOI_Y_DANG_HAT_ARR, GOI_Y_DANG_HAT_ARR + sizeof(GOI_Y_DANG_HAT_ARR)/sizeof(GOI_Y_DANG_HAT_ARR[0]));
static const char* GOI_Y_DO_DEO_ARR[] = { "Deo vua", "Deo cao", "It deo", "Kho, toi" };
vector<string> GOI_Y_DO_DEO(GOI_Y_DO_DEO_ARR, GOI_Y_DO_DEO_ARR + sizeof(GOI_Y_DO_DEO_ARR)/sizeof(GOI_Y_DO_DEO_ARR[0]));
static const char* GOI_Y_MUI_HUONG_ARR[] = { "Thom nhe", "Thom tu nhien", "Thom dam", "Khong mui dac trung" };
vector<string> GOI_Y_MUI_HUONG(GOI_Y_MUI_HUONG_ARR, GOI_Y_MUI_HUONG_ARR + sizeof(GOI_Y_MUI_HUONG_ARR)/sizeof(GOI_Y_MUI_HUONG_ARR[0]));

// Menu chon tu danh sach goi y co san (hoac them moi). Tra ve "" neu quay lai.
string chooseFromSuggestions(const string& title, const vector<string>& options) {
    while (true) {
        cout << "\n" << title << "\n";
        for (size_t i = 0; i < options.size(); ++i) cout << "  " << i+1 << ". " << options[i] << "\n";
        int addNo = (int)options.size() + 1;
        cout << "  " << addNo << ". Them moi\n  0. Quay lai\n";
        string s = readLinePrompt("Chon: "); int k;
        if (!parseIntStrict(s, k)) { cout << "Loi: Vui long nhap so.\n"; continue; }
        if (k == 0) return "";
        if (k >= 1 && k <= (int)options.size()) return options[k-1];
        if (k == addNo) {
            string x = trim(readLinePrompt("Nhap gia tri moi (Enter=quay lai): "));
            if (!x.empty()) return x;
        } else cout << "Loi: Lua chon khong hop le.\n";
    }
}

// Tim cac muc co "khoa" chua chuoi da go (khong phan biet hoa thuong).
void timKhop(const vector<string>& khoa, const string& typed, vector<int>& out) {
    out.clear();
    string k = toLowerVN(trim(typed));
    if (k.empty()) return;
    for (size_t i = 0; i < khoa.size(); ++i)
        if (toLowerVN(khoa[i]).find(k) != string::npos) out.push_back((int)i);
}

void hienGoiY(const vector<string>& hienThi, const vector<int>& m) {
    if (m.empty()) { cout << "  (khong co goi y)\n"; return; }
    size_t n = m.size() < 9 ? m.size() : 9;
    for (size_t i = 0; i < n; ++i) cout << "  [" << i+1 << "] " << hienThi[m[i]] << "\n";
    if (m.size() > n) cout << "  ... con " << m.size()-n << " muc nua, go them de thu hep\n";
}

// Chon 1 muc bang cach go (goi y hien ngay) hoac go so 1-9. Tra ve chi so muc, -1 neu huy.
int chonGoiY(const string& title, const vector<string>& hienThi, const vector<string>& khoa) {
    cout << "\n" << title << "\n";
    vector<int> tatCa; for (size_t i = 0; i < hienThi.size(); ++i) tatCa.push_back((int)i);
    vector<int> m = tatCa;
#ifdef _WIN32
    cout << "(Esc = huy)\n";
    hienGoiY(hienThi, m);
    cout << "> ";
    string typed;
    while (true) {
        int ch = _getch();
        if (ch == 0 || ch == 224) { _getch(); continue; }
        if (ch == 3 || ch == 27) { cout << "\n"; return -1; }
        if (ch == 13) {
            cout << "\n";
            string t = trim(typed);
            if (t.empty()) return -1;
            string tl = toLowerVN(t);
            for (size_t i = 0; i < m.size(); ++i) if (toLowerVN(khoa[m[i]]) == tl) return m[i];
            if (m.size() == 1) return m[0];
            if (m.empty()) cout << "Khong co ket qua. Dung Backspace de go lai.\n";
            else { cout << "Con " << m.size() << " ket qua. Go so de chon hoac go them de thu hep.\n"; hienGoiY(hienThi, m); }
            cout << "> " << typed;
            continue;
        }
        if (ch == 8) {
            if (!typed.empty()) typed.erase(typed.size()-1);
            if (trim(typed).empty()) m = tatCa; else timKhop(khoa, typed, m);
            cout << "\n"; hienGoiY(hienThi, m); cout << "> " << typed;
            continue;
        }
        if (ch >= '1' && ch <= '9' && !typed.empty() && !m.empty()) {
            size_t n = (size_t)(ch - '0');
            vector<int> thu; timKhop(khoa, typed + (char)ch, thu);
            // Chi la "chon so" khi chu so do khong phai phan tiep theo cua mot ma/ten that.
            if (n <= m.size() && thu.empty()) { cout << (char)ch << "\n"; return m[n-1]; }
        }
        if (ch >= 32 && ch < 127) {
            typed += (char)ch; cout << (char)ch;
            timKhop(khoa, typed, m);
            cout << "\n"; hienGoiY(hienThi, m); cout << "> " << typed;
        }
    }
#else
    cout << "(Enter trong = huy)\n";
    hienGoiY(hienThi, m);
    while (true) {
        string line = trim(readLinePrompt("> "));
        if (line.empty()) return -1;
        vector<int> thu; timKhop(khoa, line, thu);
        int k; size_t lim = m.size() < 9 ? m.size() : 9;
        if (thu.empty() && parseIntStrict(line, k) && k >= 1 && k <= (int)lim) return m[k-1];
        string tl = toLowerVN(line);
        for (size_t i = 0; i < thu.size(); ++i) if (toLowerVN(khoa[thu[i]]) == tl) return thu[i];
        if (thu.empty()) { cout << "Khong co goi y. Cac muc dang co:\n"; m = tatCa; hienGoiY(hienThi, m); continue; }
        if (thu.size() == 1) return thu[0];
        m = thu; hienGoiY(hienThi, m);
    }
#endif
}

// ================================================================
// 7. HIEN THI DU LIEU & TINH NANG NGHIEP VU
// ================================================================

// ---------- 7.1 Dinh dang so / tien ----------
// In so gon: 300 -> "300", 12.5 -> "12.5" (chi hien phan le khi co).
string fmtNum(double v, int maxDec=2) {
    ostringstream o; o << fixed << setprecision(maxDec) << v;
    string r = o.str();
    if (r.find('.') != string::npos) {
        while (!r.empty() && r[r.size()-1] == '0') r.erase(r.size()-1);
        if (!r.empty() && r[r.size()-1] == '.') r.erase(r.size()-1);
    }
    if (r.empty() || r == "-0") r = "0";
    return r;
}
string numStr(double v) { return fmtNum(v); }
string intStr(long long v) { ostringstream o; o << v; return o.str(); }
string money(double v) { return fmtNum(v) + " VND"; }
string cutStr(const string& s, size_t n) { return s.size() <= n ? s : s.substr(0, n-2) + ".."; }

// ---------- 7.2 Don vi nhap gia (gia luon luu theo VND) ----------
string gTenDV = "VND";
double gHeSoDV = 1;

void chonDonViGia() {
    while (true) {
        cout << "\n=== DON VI NHAP GIA (hien tai: " << gTenDV << ") ===\n";
        cout << "1. VND\n2. Nghin VND (1.000 dong)\n3. Trieu VND (1.000.000 dong)\n4. USD (nhap ty gia)\n";
        string s = trim(readLinePrompt("Chon (Enter=giu nguyen): "));
        if (s.empty()) return;
        int k;
        if (!parseIntStrict(s, k) || k < 1 || k > 4) { cout << "Loi: Chon tu 1 den 4.\n"; continue; }
        if (k == 1) { gTenDV = "VND"; gHeSoDV = 1; return; }
        if (k == 2) { gTenDV = "nghin VND"; gHeSoDV = 1000; return; }
        if (k == 3) { gTenDV = "trieu VND"; gHeSoDV = 1000000; return; }
        while (true) {
            string a = trim(readLinePrompt("1 USD = bao nhieu VND? (Enter=quay lai): "));
            if (a.empty()) break;
            double d;
            if (parseDoubleStrict(a, d) && d > 0) { gTenDV = "USD"; gHeSoDV = d; return; }
            cout << "Loi: Ty gia phai la so > 0.\n";
        }
    }
}

// ---------- 7.3 Quy cach dong goi (nhieu loai bao) ----------
string quyCachStr(const vector<int>& q) {
    if (q.empty()) return "(khong co - tinh het theo kg le)";
    vector<int> t = q; sort(t.begin(), t.end()); reverse(t.begin(), t.end());
    ostringstream o;
    for (size_t i = 0; i < t.size(); ++i) { if (i) o << ", "; o << t[i] << "kg"; }
    return o.str();
}

// Quy doi ton kho sang bao: dung bao lon truoc de it bao nhat, phan con lai la kg le.
string quyDoiBao(double kg, const vector<int>& quyCach) {
    vector<int> q = quyCach; sort(q.begin(), q.end()); reverse(q.begin(), q.end());
    double con = kg; bool co = false; ostringstream oss;
    for (size_t i = 0; i < q.size(); ++i) {
        if (q[i] <= 0) continue;
        int n = (int)(con / q[i] + 1e-9);
        if (n > 0) { if (co) oss << " + "; oss << n << " bao " << q[i] << "kg"; con -= (double)n * q[i]; co = true; }
    }
    if (con < 1e-9) con = 0;
    if (con > 0) { if (co) oss << " + "; oss << fmtNum(con) << " kg le"; co = true; }
    if (!co) oss << "0 kg";
    return oss.str();
}

// Menu chinh sua danh sach loai bao. Tra ve true neu co luu thay doi.
bool chonQuyCach(vector<int>& q) {
    vector<int> t = q;
    while (true) {
        sort(t.begin(), t.end()); reverse(t.begin(), t.end());
        cout << "\n=== QUY CACH DONG GOI ===\nHien co: " << quyCachStr(t) << "\n";
        cout << "1. Them loai bao\n2. Xoa loai bao\n3. Dat lai mac dinh (50, 25, 10, 5 kg)\n4. Hoan tat\n0. Huy (giu nguyen)\n";
        string s = readLinePrompt("Chon: "); int k;
        if (!parseIntStrict(s, k)) { cout << "Loi: Nhap so.\n"; continue; }
        if (k == 0) return false;
        if (k == 4) { q = t; return true; }
        if (k == 3) { quyCachMacDinh(t); continue; }
        if (k == 1) {
            if (t.size() >= 8) { cout << "Toi da 8 loai bao.\n"; continue; }
            while (true) {
                string a = trim(readLinePrompt("Khoi luong 1 bao (kg, so nguyen 1-1000, Enter=huy): "));
                if (a.empty()) break;
                int v;
                if (!parseIntStrict(a, v) || v < 1 || v > 1000) { cout << "Loi: Phai la so nguyen tu 1 den 1000.\n"; continue; }
                if (find(t.begin(), t.end(), v) != t.end()) { cout << "Loai bao " << v << "kg da co.\n"; continue; }
                t.push_back(v); break;
            }
        } else if (k == 2) {
            if (t.empty()) { cout << "Chua co loai bao nao de xoa.\n"; continue; }
            while (true) {
                string a = trim(readLinePrompt("Xoa loai bao nao? Nhap khoi luong (vi du 25, Enter=huy): "));
                if (a.empty()) break;
                int v; vector<int>::iterator it;
                if (parseIntStrict(a, v) && (it = find(t.begin(), t.end(), v)) != t.end()) { t.erase(it); break; }
                cout << "Loi: Khong co loai bao nay. Hien co: " << quyCachStr(t) << ".\n";
            }
        } else cout << "Lua chon khong hop le.\n";
    }
}

// ---------- 7.4 Nhat ky doanh thu theo ngay ----------
// Cong don so bao cung loai
void congBao(vector<SoBaoBan>& dich, const SoBaoBan& b) {
    for (size_t i = 0; i < dich.size(); ++i)
        if (dich[i].kgMotBao == b.kgMotBao) { dich[i].soBao += b.soBao; return; }
    dich.push_back(b);
}

// Cong don vao nhat ky cua ngay d (doanh thu, gia von, so kg, so bao tung loai)
void ghiNhatKy(MatHang& item, const NgayThang& d, double dt, double gv, double kg, const vector<SoBaoBan>& bao) {
    int idx = -1;
    for (size_t i = 0; i < item.lichSuNgay.size(); ++i) if (item.lichSuNgay[i].ngayGhi == d) { idx = (int)i; break; }
    if (idx < 0) {
        if ((int)item.lichSuNgay.size() >= MAX_HISTORY) item.lichSuNgay.erase(item.lichSuNgay.begin());
        NhatKyNgay x; x.ngayGhi = d; item.lichSuNgay.push_back(x); idx = (int)item.lichSuNgay.size() - 1;
    }
    NhatKyNgay& p = item.lichSuNgay[idx];
    p.doanhThu += dt; p.giaVon += gv;
    if (p.soKg >= 0) p.soKg += kg;        // ngay co du lieu cu khong ro kg thi giu nguyen "khong ro"
    for (size_t i = 0; i < bao.size(); ++i) congBao(p.bao, bao[i]);
}

double doanhThuNgay(const MatHang& x, const NgayThang& d) {
    for (size_t i = 0; i < x.lichSuNgay.size(); ++i) if (x.lichSuNgay[i].ngayGhi == d) return x.lichSuNgay[i].doanhThu;
    return 0;
}

// ---------- 7.5 In bang / chi tiet ----------
string fitCell(const string& s, int width) {
    if ((int)s.size() <= width - 1) return s;
    if (width <= 3) return s.substr(0, width);
    return s.substr(0, width - 3) + "...";
}

void printHeader() {
    cout << "\n";
    cout << left
         << setw(13) << "Ma mat hang"
         << setw(24) << "Ten gao"
         << setw(13) << "Ton (kg)"
         << setw(16) << "Ton toi thieu"
         << setw(16) << "Gia nhap/kg"
         << setw(16) << "Gia ban/kg"
         << setw(18) << "Doanh thu"
         << setw(20) << "Doanh thu hom nay"
         << setw(13) << "Han su dung" << "\n";
    cout << string(149, '-') << "\n";
}

void printItem(const MatHang& x) {
    cout << left
         << setw(13) << fitCell(x.maMH, 12)
         << setw(24) << fitCell(x.tenMH, 23)
         << setw(13) << fmtNum(x.soKgTon)
         << setw(16) << intStr(x.tonToiThieuKg)
         << setw(16) << fmtNum(x.giaNhapKg)
         << setw(16) << fmtNum(x.giaBanKg)
         << setw(18) << fmtNum(x.doanhThu)
         << setw(20) << fmtNum(doanhThuNgay(x, homNay()))
         << setw(13) << ngayThanhChuoi(x.hanSuDung) << "\n";
}

// ngung = false: in cac mat hang dang ban; true: in cac mat hang da ngung ban
void displayInOrder(NodeAVL* root, bool ngung = false) {
    if (!root) return;
    displayInOrder(root->left, ngung);
    if (root->data.ngungBan == ngung) printItem(root->data);
    displayInOrder(root->right, ngung);
}

int demMatHang(NodeAVL* root, bool ngung) {
    if (!root) return 0;
    return demMatHang(root->left, ngung) + demMatHang(root->right, ngung) + (root->data.ngungBan == ngung ? 1 : 0);
}

string hienGT(const char* t) { return t[0] ? string(t) : string("[chua nhap]"); }

void printGaoDetail(const MatHang& x) {
    cout << "\n========== THONG TIN GAO CHI TIET ==========\n";
    cout << "Ma mat hang       : " << x.maMH << "\nTen gao           : " << x.tenMH << "\n";
    cout << "Loai gao          : " << hienGT(x.gao.loaiGao) << "\nNha san xuat      : " << hienGT(x.gao.nhaSanXuat) << "\nXuat xu           : " << hienGT(x.gao.xuatXu) << "\n";
    cout << "Dang hat          : " << hienGT(x.gao.dangHat) << "\nDo deo            : " << hienGT(x.gao.doDeo) << "\nThanh phan        : " << hienGT(x.gao.thanhPhan) << "\n";
    cout << "Nang luong        : " << (x.gao.nangLuongKcal100g > 0 ? fmtNum(x.gao.nangLuongKcal100g) + " kcal/100g" : string("[chua nhap]")) << "\n";
    cout << "Mui huong         : " << hienGT(x.gao.muiHuong) << "\n";
    cout << "Khoi luong ton    : " << fmtNum(x.soKgTon) << " kg\n";
    cout << "Quy cach bao      : " << quyCachStr(x.quyCach) << "\n";
    cout << "Quy doi dong goi  : " << quyDoiBao(x.soKgTon, x.quyCach) << "\n";
    cout << "Gia nhap          : " << money(x.giaNhapKg) << "/kg\nGia ban           : " << money(x.giaBanKg) << "/kg\n";
    cout << "HSD               : " << ngayThanhChuoi(x.hanSuDung) << "\n";
    int con = soNgayGiua(homNay(), x.hanSuDung);
    cout << "Trang thai HSD    : ";
    if (con < 0) cout << "DA HET HAN (" << -con << " ngay)\n"; else cout << "Con " << con << " ngay\n";
    cout << "Trang thai        : " << (x.ngungBan ? "NGUNG BAN" : "Dang ban") << "\n";
    if (!x.lichSuNhap.empty()) {
        cout << "Nhap hang gan day :\n";
        size_t dau = x.lichSuNhap.size() > 5 ? x.lichSuNhap.size() - 5 : 0;
        for (size_t i = x.lichSuNhap.size(); i > dau; --i) {
            const NhatKyNhap& h = x.lichSuNhap[i - 1];
            cout << "  " << ngayThanhChuoi(h.ngayNhap) << " : " << fmtNum(h.soKg) << " kg x " << money(h.giaNhapKg) << "/kg\n";
        }
    }
    cout << "============================================\n";
}

// ---------- 7.6 Cac ham doc 1 truong du lieu ----------
// Quy uoc: "<" = quay lai muc truoc, "huy" = huy, Enter trong = giu gia tri hien tai (neu co).
enum KQ { KQ_OK = 0, KQ_LUI = 1, KQ_HUY = 2, KQ_GIU = 3 };

bool lenhDacBiet(const string& s, KQ& kq) {
    if (s == "<") { kq = KQ_LUI; return true; }
    if (toLowerVN(s) == "huy") { kq = KQ_HUY; return true; }
    return false;
}

string nhanHienTai(bool giu, const string& v) { return giu ? " [hien tai: " + v + "]" : string(""); }

KQ docMaMoi(NodeAVL* root, string& out, bool giu, const string& boQua) {
    while (true) {
        string s = trim(readLinePrompt("Ma mat hang" + nhanHienTai(giu, out) + ": "));
        KQ kq; if (lenhDacBiet(s, kq)) return kq;
        if (s.empty()) { if (giu) return KQ_GIU; cout << "Loi: Khong duoc de trong.\n"; continue; }
        string t;   // tu dong quy ve chu thuong va bo moi khoang trang
        for (size_t i = 0; i < s.size(); ++i) if (!isspace((unsigned char)s[i])) t += (char)tolower((unsigned char)s[i]);
        if (t.size() > 14) { cout << "Loi: Ma toi da 14 ky tu.\n"; continue; }
        NodeAVL* n = (t != boQua) ? searchNode(root, t.c_str()) : NULL;
        if (n) { cout << "Loi: Ma '" << t << "' da co (" << n->data.tenMH << (n->data.ngungBan ? ", dang ngung ban" : "") << "). Nhap ma khac.\n"; continue; }
        if (t != s) cout << "(Ma duoc chuan hoa: " << t << ")\n";
        out = t; return KQ_OK;
    }
}

KQ docTen(string& out, bool giu) {
    while (true) {
        string s = trim(readLinePrompt("Ten gao" + nhanHienTai(giu, out) + ": "));
        KQ kq; if (lenhDacBiet(s, kq)) return kq;
        if (s.empty()) { if (giu) return KQ_GIU; cout << "Loi: Khong duoc de trong.\n"; continue; }
        if (s.size() >= sizeof(((MatHang*)0)->tenMH)) { cout << "Loi: Ten toi da 49 ky tu.\n"; continue; }
        out = s; return KQ_OK;
    }
}

KQ docSoKg(const string& nhan, double& out, bool giu) {
    while (true) {
        string s = trim(readLinePrompt(nhan + nhanHienTai(giu, fmtNum(out)) + ": "));
        KQ kq; if (lenhDacBiet(s, kq)) return kq;
        if (s.empty()) { if (giu) return KQ_GIU; cout << "Loi: Khong duoc de trong.\n"; continue; }
        double d;
        if (parseDoubleStrict(s, d) && d >= 0 && d <= 1e9) { out = d; return KQ_OK; }
        cout << "Loi: Phai la so >= 0.\n";
    }
}

KQ docTonMin(int& out, bool giu) {
    while (true) {
        string s = trim(readLinePrompt("Ton toi thieu (kg)" + nhanHienTai(giu, intStr(out)) + ": "));
        KQ kq; if (lenhDacBiet(s, kq)) return kq;
        if (s.empty()) { if (giu) return KQ_GIU; cout << "Loi: Khong duoc de trong.\n"; continue; }
        int d;
        if (parseIntStrict(s, d) && d >= 0 && d <= 1000000000) { out = d; return KQ_OK; }
        cout << "Loi: Phai la so nguyen >= 0.\n";
    }
}

// Doc gia theo don vi dang chon; ket qua doi ra VND. Go "dv" de doi don vi.
// Gia phai > lonHon va (neu nhoHon > 0) phai < nhoHon (cac gioi han tinh bang VND).
KQ docTien(const string& nhan, double& out, bool giu, double lonHon, double nhoHon) {
    while (true) {
        string s = trim(readLinePrompt(nhan + " (" + gTenDV + ")" + nhanHienTai(giu, fmtNum(out / gHeSoDV, 4)) + ": "));
        KQ kq; if (lenhDacBiet(s, kq)) return kq;
        if (s.empty()) { if (giu) return KQ_GIU; cout << "Loi: Khong duoc de trong.\n"; continue; }
        if (toLowerVN(s) == "dv") { chonDonViGia(); continue; }
        double d;
        if (!parseDoubleStrict(s, d)) { cout << "Loi: Vui long nhap so.\n"; continue; }
        double v = d * gHeSoDV;
        if (v <= lonHon) {
            if (lonHon >= 0) cout << "Loi: Gia phai lon hon " << money(lonHon) << ".\n"; else cout << "Loi: Gia khong duoc am.\n";
            continue;
        }
        if (nhoHon > 0 && v >= nhoHon) { cout << "Loi: Gia phai nho hon " << money(nhoHon) << ".\n"; continue; }
        out = v; return KQ_OK;
    }
}

// Ngay: chap nhan moi dau ngan cach / - . , : hoac khoang trang. Nam 2 so -> 20xx.
bool chuyenNgayLinhHoat(const string& s, NgayThang& out) {
    string t;
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = (unsigned char)s[i];
        if (isdigit(c)) t += (char)c;
        else if (c == '/' || c == '-' || c == '.' || c == ',' || c == ':' || c == ' ' || c == '\t') t += ' ';
        else return false;
    }
    stringstream ss(t);
    int d, m, y; string rest;
    if (!(ss >> d >> m >> y)) return false;
    if (ss >> rest) return false;
    if (y >= 0 && y < 100) y += 2000;
    NgayThang x(d, m, y);
    if (!ngayHopLe(x)) return false;
    out = x; return true;
}

KQ docNgay(const string& nhan, NgayThang& out, bool giu) {
    while (true) {
        string s = trim(readLinePrompt(nhan + " (ngay.thang.nam)" + nhanHienTai(giu, ngayThanhChuoi(out)) + ": "));
        KQ kq; if (lenhDacBiet(s, kq)) return kq;
        if (s.empty()) { if (giu) return KQ_GIU; cout << "Loi: Khong duoc de trong.\n"; continue; }
        if (chuyenNgayLinhHoat(s, out)) return KQ_OK;
        cout << "Loi: Ngay khong hop le.\n";
    }
}

// ---------- 7.7 Chon mat hang co goi y ----------
// Ma mat hang: goi y theo phan dau.
// Ten gao: goi y theo moi vi tri trong ten.
// Cung mot o nhap co the nhap ma HOAC ten.
void timKhopMatHang(const vector<MatHang>& v, const string& typed, vector<int>& out) {
    out.clear();
    string k = toLowerVN(trim(typed));
    if (k.empty()) return;

    for (size_t i = 0; i < v.size(); ++i) {
        string ma = toLowerVN(trim(v[i].maMH));
        string ten = toLowerVN(trim(v[i].tenMH));

        bool khopMa = ma.find(k) == 0;
        bool khopTen = ten.find(k) != string::npos;

        if (khopMa || khopTen) out.push_back((int)i);
    }
}

void hienGoiYMatHang(const vector<MatHang>& v, const vector<int>& m) {
    if (m.empty()) {
        cout << "  (khong co goi y)\n";
        return;
    }

    size_t n = m.size() < 9 ? m.size() : 9;
    for (size_t i = 0; i < n; ++i) {
        cout << "  [" << i + 1 << "] "
             << v[m[i]].maMH << " : " << v[m[i]].tenMH << "\n";
    }

    if (m.size() > n)
        cout << "  ... con " << m.size() - n << " ket qua, go them de thu hep\n";
}

// Chon mat hang bang ma hoac ten. Khong in danh sach khi chua go.
// loc: 0 = tat ca, 1 = chi mat hang dang ban, 2 = chi mat hang da ngung ban
void layDanhSachLoc(NodeAVL* root, int loc, vector<MatHang>& v) {
    vector<MatHang> all; collectNodes(root, all); v.clear();
    for (size_t i = 0; i < all.size(); ++i)
        if (loc == 0 || (loc == 1 && !all[i].ngungBan) || (loc == 2 && all[i].ngungBan)) v.push_back(all[i]);
}

int chonMatHangCoGoiY(const vector<MatHang>& v, const string& title) {
    if (v.empty()) {
        cout << "Khong co mat hang nao phu hop.\n";
        return -1;
    }

#ifdef _WIN32
    cout << "\n" << title << "\n";
    cout << "(Go ma hoac ten gao - go den dau, goi y loc den do)\n";
    cout << "Mui ten len/xuong = chon | Enter = chon | Esc = huy\n";
    cout << "> ";

    string typed;
    vector<int> m;
    int dangChon = 0;

    // Cap nhat danh sach goi y theo tu khoa dang go.
    // Neu con ket qua, mac dinh chon ket qua dau tien.
    auto capNhatGoiY = [&]() {
        m.clear();
        if (!trim(typed).empty())
            timKhopMatHang(v, typed, m);

        if (m.empty()) dangChon = -1;
        else {
            if (dangChon < 0 || dangChon >= (int)m.size())
                dangChon = 0;
        }

        cout << "\n";
        if (!trim(typed).empty())
            hienGoiYMatHang(v, m);
        else
            cout << "  (Bat dau go ma hoac ten gao de hien goi y)\n";

        if (!m.empty())
            cout << "  -> Dang chon: [" << dangChon + 1 << "] "
                 << v[m[dangChon]].maMH << " : " << v[m[dangChon]].tenMH << "\n";

        cout << "> " << typed;
    };

    while (true) {
        int ch = _getch();

        // Mui ten len/xuong
        if (ch == 0 || ch == 224) {
            int ch2 = _getch();

            if (ch2 == 72) { // UP
                if (!m.empty()) {
                    if (dangChon <= 0) dangChon = (int)m.size() - 1;
                    else --dangChon;
                    cout << "\n";
                    hienGoiYMatHang(v, m);
                    cout << "  -> Dang chon: [" << dangChon + 1 << "] "
                         << v[m[dangChon]].maMH << " : " << v[m[dangChon]].tenMH << "\n";
                    cout << "> " << typed;
                }
            }
            else if (ch2 == 80) { // DOWN
                if (!m.empty()) {
                    dangChon = (dangChon + 1) % (int)m.size();
                    cout << "\n";
                    hienGoiYMatHang(v, m);
                    cout << "  -> Dang chon: [" << dangChon + 1 << "] "
                         << v[m[dangChon]].maMH << " : " << v[m[dangChon]].tenMH << "\n";
                    cout << "> " << typed;
                }
            }
            continue;
        }

        // Esc / Ctrl+C = huy
        if (ch == 27 || ch == 3) {
            cout << "\n";
            return -1;
        }

        // Enter: khong con can go lai.
        // Neu co goi y -> chon ngay goi y dang duoc danh dau.
        // Neu chi co 1 ket qua -> chon ket qua do.
        // Neu go dung ma -> chon dung ma.
        if (ch == 13) {
            cout << "\n";

            if (trim(typed).empty())
                return -1;

            timKhopMatHang(v, typed, m);

            string tl = toLowerVN(trim(typed));

            for (size_t i = 0; i < v.size(); ++i) {
                if (toLowerVN(trim(v[i].maMH)) == tl)
                    return (int)i;
            }

            if (m.size() == 1)
                return m[0];

            if (!m.empty()) {
                if (dangChon < 0 || dangChon >= (int)m.size())
                    dangChon = 0;
                return m[dangChon];
            }

            cout << "Khong co ket qua. Go lai hoac dung Backspace.\n";
            cout << "> " << typed;
            continue;
        }

        // Backspace
        if (ch == 8) {
            if (!typed.empty())
                typed.erase(typed.size() - 1);

            capNhatGoiY();
            continue;
        }

        // Phim so 1-9: neu dang co goi y thi chon truc tiep goi y tuong ung.
        // Vi du go "m" -> [1] Gao..., [2] ... -> bam 1 la chon luon.
        if (ch >= '1' && ch <= '9' && !typed.empty() && !m.empty()) {
            int so = ch - '0';

            if (so >= 1 && so <= (int)m.size()) {
                // Neu so nay khong phai la ky tu tiep theo cua ma/ten nao,
                // xem no la lenh chon nhanh.
                vector<int> thu;
                timKhopMatHang(v, typed + (char)ch, thu);

                if (thu.empty()) {
                    cout << "\nDa chon: [" << so << "] "
                         << v[m[so - 1]].maMH << " : " << v[m[so - 1]].tenMH << "\n";
                    return m[so - 1];
                }
            }
        }

        // Ky tu go vao: loc ngay lap tuc.
        if (ch >= 32 && ch < 127) {
            typed += (char)tolower((unsigned char)ch);
            cout << (char)tolower((unsigned char)ch);

            capNhatGoiY();
        }
    }
#else
    // Ban khong phai Windows: van cho phep loc theo tu khoa va chon so.
    // Sau khi go tu khoa, neu chi con 1 ket qua thi tu dong chon.
    cout << "\n" << title << "\n";
    cout << "(Go ma/ten -> goi y tu dong; Enter = chon; Enter trong = huy)\n";

    string typed;
    vector<int> m;

    while (true) {
        typed = trim(readLinePrompt("> "));
        if (typed.empty()) return -1;

        timKhopMatHang(v, typed, m);

        string tl = toLowerVN(typed);
        for (size_t i = 0; i < v.size(); ++i)
            if (toLowerVN(trim(v[i].maMH)) == tl) return (int)i;

        if (m.size() == 1) return m[0];

        if (m.empty()) {
            cout << "Khong co ket qua. Go lai.\n";
            continue;
        }

        hienGoiYMatHang(v, m);

        string ch = trim(readLinePrompt("Chon so (hoac go tiep tu khoa): "));

        int k;
        if (parseIntStrict(ch, k) && k >= 1 && k <= (int)m.size())
            return m[k - 1];

        if (!ch.empty()) {
            string tiep = typed + ch;
            timKhopMatHang(v, tiep, m);
            if (m.size() == 1) return m[0];
        }
    }
#endif
}

// Dung cho ban hang, sua, xoa, nhap hang... (loc: xem layDanhSachLoc)
string chonMaCoGoiY(NodeAVL* root, const string& title, int loc = 0) {
    vector<MatHang> v; layDanhSachLoc(root, loc, v);
    int k = chonMatHangCoGoiY(v, title);
    if (k < 0) return "";
    return string(v[k].maMH);
}

// Tra cuu gom ma + ten trong cung mot chuc nang (ca mat hang ngung ban).
NodeAVL* traCuuTheoGoiY(NodeAVL* root) {
    string ma = chonMaCoGoiY(root, "TRA CUU GAO", 0);
    if (ma.empty()) return NULL;
    return searchNode(root, ma.c_str());
}

// ---------- 7.8 Thong tin gao chi tiet (khong bat buoc) ----------
// Hien thi ngay cac thong tin chi tiet da nhap.
void printThongTinGaoChiTiet(const ThongTinGao& g) {
    cout << "    Nha san xuat : " << hienGT(g.nhaSanXuat) << "\n";
    cout << "    Xuat xu      : " << hienGT(g.xuatXu) << "\n";
    cout << "    Dang hat     : " << hienGT(g.dangHat) << "\n";
    cout << "    Do deo       : " << hienGT(g.doDeo) << "\n";
    cout << "    Thanh phan   : " << hienGT(g.thanhPhan) << "\n";
    cout << "    Nang luong   : "
         << (g.nangLuongKcal100g > 0 ? fmtNum(g.nangLuongKcal100g) + " kcal/100g" : string("[chua nhap]")) << "\n";
    cout << "    Mui huong    : " << hienGT(g.muiHuong) << "\n";
}

bool nhapThongTinGao(ThongTinGao& g) {
    while (true) {
        cout << "\n=== THONG TIN GAO CHI TIET ===\n";
        cout << "1. Nha san xuat  : " << hienGT(g.nhaSanXuat) << "\n";
        cout << "2. Xuat xu       : " << hienGT(g.xuatXu) << "\n";
        cout << "3. Dang hat      : " << hienGT(g.dangHat) << "\n";
        cout << "4. Do deo        : " << hienGT(g.doDeo) << "\n";
        cout << "5. Thanh phan    : " << hienGT(g.thanhPhan) << "\n";
        cout << "6. Nang luong    : " << (g.nangLuongKcal100g > 0 ? fmtNum(g.nangLuongKcal100g) + " kcal/100g" : string("[chua nhap]")) << "\n";
        cout << "7. Mui huong     : " << hienGT(g.muiHuong) << "\n";
        cout << "8. Hoan tat\n";
        cout << "0. Huy thay doi\n";

        string s = trim(readLinePrompt("Chon: "));
        int k;

        if (!parseIntStrict(s, k)) {
            cout << "Loi: Vui long nhap so.\n";
            continue;
        }

        if (k == 0) return false;
        if (k == 8) return true;

        string x;
        double d;

        switch (k) {
            case 1:
                x = chooseFromSuggestions("Chon nha san xuat", GOI_Y_NHA_SAN_XUAT);
                if (!x.empty() && !setStr(g.nhaSanXuat, x))
                    cout << "Loi: Gia tri qua dai.\n";
                break;

            case 2:
                x = chooseFromSuggestions("Chon xuat xu", GOI_Y_XUAT_XU);
                if (!x.empty() && !setStr(g.xuatXu, x))
                    cout << "Loi: Gia tri qua dai.\n";
                break;

            case 3:
                x = chooseFromSuggestions("Dang hat", GOI_Y_DANG_HAT);
                if (!x.empty() && !setStr(g.dangHat, x))
                    cout << "Loi: Gia tri qua dai.\n";
                break;

            case 4:
                x = chooseFromSuggestions("Do deo", GOI_Y_DO_DEO);
                if (!x.empty() && !setStr(g.doDeo, x))
                    cout << "Loi: Gia tri qua dai.\n";
                break;

            case 5:
                x = trim(readLinePrompt("Thanh phan (Enter=giu nguyen): "));
                if (!x.empty() && !setStr(g.thanhPhan, x))
                    cout << "Loi: Toi da 99 ky tu.\n";
                break;

            case 6:
                while (true) {
                    x = trim(readLinePrompt("Nang luong kcal/100g (Enter=giu nguyen): "));
                    if (x.empty()) break;

                    if (parseDoubleStrict(x, d) && d > 0 && d < 1000) {
                        g.nangLuongKcal100g = d;
                        break;
                    }

                    cout << "Loi: Phai > 0 va < 1000.\n";
                }
                break;

            case 7:
                x = chooseFromSuggestions("Mui huong", GOI_Y_MUI_HUONG);
                if (!x.empty() && !setStr(g.muiHuong, x))
                    cout << "Loi: Gia tri qua dai.\n";
                break;

            default:
                cout << "Lua chon khong hop le.\n";
        }
    }
}

// ---------- 7.9 Tra cuu gao: ma/ten + khoang gia ban ----------
void menuTraCuuGao(NodeAVL* root) {
    while (true) {
        cout << "\n=== TRA CUU GAO ===\n";
        cout << "1. Tra cuu theo ma hoac ten\n";
        cout << "2. Tra cuu theo khoang gia ban\n";
        cout << "0. Quay lai\n";

        string s = trim(readLinePrompt("Chon: "));
        int k;
        if (!parseIntStrict(s, k)) {
            cout << "Loi: Vui long nhap so nguyen.\n";
            continue;
        }

        if (k == 0) return;

        switch (k) {
            case 1: {
                NodeAVL* n = traCuuTheoGoiY(root);
                if (n) {
                    printHeader();
                    printItem(n->data);
                }
                break;
            }
            case 2:
                traCuuKhoangGia(root);
                break;
            default:
                cout << "Lua chon khong hop le. Vui long chon lai.\n";
        }
    }
}

// ---------- 7.9 Them mat hang: Enter = di tiep; so = sua; 10 = huy; Enter o xac nhan = luu ----------
bool nhapMatHangMoi(NodeAVL*& root) {
    string ma, ten;
    double kg = 0, gn = 0, gb = 0;
    int tmin = 0;
    NgayThang hsd = homNay();

    vector<int> qc;
    quyCachMacDinh(qc);

    ThongTinGao gao;
    bool daGao = false;
    bool da[7] = {false, false, false, false, false, false, false};

    cout << "\n=== THEM MAT HANG GAO MOI ===\n";
    cout << "(Enter = tiep tuc | < = quay lai | huy = huy)\n";

    int i = 0;

    while (i < 7) {
        KQ kq = KQ_OK;

        switch (i) {
            case 0: kq = docMaMoi(root, ma, da[0], ""); break;
            case 1: kq = docTen(ten, da[1]); break;
            case 2: kq = docSoKg("So kg ton ban dau", kg, da[2]); break;
            case 3: kq = docTonMin(tmin, da[3]); break;
            case 4: kq = docTien("Gia nhap /kg", gn, da[4], 0, 0); break;
            case 5: kq = docTien("Gia ban /kg", gb, da[5], gn, 0); break;
            case 6: kq = docNgay("Han su dung", hsd, da[6]); break;
        }

        if (kq == KQ_LUI) {
            if (i > 0) --i;
            else cout << "Dang o muc dau tien.\n";
            continue;
        }

        if (kq == KQ_HUY) {
            cout << "Da huy them mat hang.\n";
            return false;
        }

        da[i] = true;

        if (i == 4 && da[5] && gb <= gn) {
            da[5] = false;
            cout << "(Gia ban cu khong con hop le, hay nhap lai.)\n";
        }

        ++i;
    }

    // Enter la lenh LUU duy nhat tai man hinh tong hop.
    // 1-9 = sua muc; 10 = huy. Khong con so 11.
    while (true) {
        cout << "\n=== XAC NHAN MAT HANG ===\n";
        cout << "1. Ma mat hang     : " << ma << "\n";
        cout << "2. Ten gao         : " << ten << "\n";
        cout << "3. So kg ton       : " << fmtNum(kg) << " kg\n";
        cout << "4. Ton toi thieu   : " << tmin << " kg\n";
        cout << "5. Gia nhap        : " << money(gn) << "/kg\n";
        cout << "6. Gia ban         : " << money(gb) << "/kg\n";
        cout << "7. Han su dung     : " << ngayThanhChuoi(hsd) << "\n";
        cout << "8. Quy cach bao    : " << quyCachStr(qc) << "\n";
        cout << "9. Thong tin gao   :\n";
        if (daGao) printThongTinGaoChiTiet(gao);
        else cout << "    [chua nhap]\n";
        cout << "10. Huy them mat hang\n";
        cout << "Enter = LUU va tao mat hang\n";

        string s = trim(readLinePrompt("Chon: "));

        if (s.empty()) {
            MatHang x;

            setStr(x.maMH, ma);
            setStr(x.tenMH, ten);

            x.soKgTon = kg;
            x.tonToiThieuKg = tmin;
            x.giaNhapKg = gn;
            x.giaBanKg = gb;
            x.hanSuDung = hsd;
            x.quyCach = qc;
            x.gao = gao;

            x.doanhThu = 0;
            x.giaVonHangBan = 0;

            root = insertNode(root, x);
            luuTuDong(root);

            cout << "Them mat hang gao thanh cong!\n";
            return true;
        }

        int k;

        if (!parseIntStrict(s, k) || k < 1 || k > 10) {
            cout << "Loi: Enter de luu, hoac chon so 1-10.\n";
            continue;
        }

        if (k == 10) {
            cout << "Da huy them mat hang.\n";
            return false;
        }

        switch (k) {
            case 1: docMaMoi(root, ma, true, ""); break;
            case 2: docTen(ten, true); break;
            case 3: docSoKg("So kg ton ban dau", kg, true); break;
            case 4: docTonMin(tmin, true); break;
            case 5: docTien("Gia nhap /kg", gn, true, 0, 0); break;
            case 6: docTien("Gia ban /kg", gb, true, gn, 0); break;
            case 7: docNgay("Han su dung", hsd, true); break;
            case 8: chonQuyCach(qc); break;

            case 9: {
                ThongTinGao tmp = gao;

                if (nhapThongTinGao(tmp)) {
                    gao = tmp;
                    daGao = true;
                }

                break;
            }
        }

        while (gb <= gn) {
            cout << "Gia ban phai lon hon gia nhap "
                 << money(gn) << ". Nhap lai gia ban.\n";

            KQ kq2 = docTien("Gia ban /kg", gb, false, gn, 0);

            if (kq2 == KQ_HUY) {
                cout << "Da huy them mat hang.\n";
                return false;
            }
        }
    }
}

// ---------- 7.10 Sua thong tin: menu lien tuc ----------
bool suaMatHang(NodeAVL*& root, const string& maBanDau) {
    string maHienTai = maBanDau;
    NodeAVL* target = searchNode(root, maHienTai.c_str()); if (!target) return false;
    MatHang backup = target->data;
    while (true) {
        MatHang& d = target->data;
        cout << "\n=== SUA THONG TIN: " << d.maMH << " - " << d.tenMH << " ===\n";
        cout << "1. Sua ten gao\n2. Sua gia nhap/kg\n3. Sua gia ban/kg\n4. Sua ton toi thieu\n5. Sua so kg ton\n6. Sua han su dung\n7. Sua quy cach dong goi\n8. Sua thong tin gao chi tiet\n9. Sua ma mat hang\n10. Hoan tat va luu\n0. Huy toan bo thay doi\n";
        string s = readLinePrompt("Chon: "); int k;
        if (!parseIntStrict(s, k)) { cout << "Loi: Nhap so.\n"; continue; }
        if (k < 0 || k > 10) { cout << "Lua chon khong hop le.\n"; continue; }
        if (k == 0) {
            root = deleteNode(root, maHienTai.c_str()); root = insertNode(root, backup);
            cout << "Da huy thao tac sua, khoi phuc du lieu ban dau.\n"; return false;
        }
        if (k == 10) { luuTuDong(root); cout << "Da cap nhat va luu thong tin mat hang.\n"; return true; }
        switch (k) {
            case 1: { string t = d.tenMH; if (docTen(t, true) == KQ_OK) setStr(d.tenMH, t); } break;
            case 2: { double v = d.giaNhapKg; if (docTien("Gia nhap moi /kg", v, true, 0, d.giaBanKg) == KQ_OK) d.giaNhapKg = v; } break;
            case 3: { double v = d.giaBanKg; if (docTien("Gia ban moi /kg", v, true, d.giaNhapKg, 0) == KQ_OK) d.giaBanKg = v; } break;
            case 4: { int v = d.tonToiThieuKg; if (docTonMin(v, true) == KQ_OK) d.tonToiThieuKg = v; } break;
            case 5: { double v = d.soKgTon; if (docSoKg("So kg ton moi", v, true) == KQ_OK) d.soKgTon = v; } break;
            case 6: { NgayThang v = d.hanSuDung; if (docNgay("Han su dung moi", v, true) == KQ_OK) d.hanSuDung = v; } break;
            case 7: chonQuyCach(d.quyCach); break;
            case 8: { ThongTinGao tmp = d.gao; if (nhapThongTinGao(tmp)) d.gao = tmp; else cout << "Da huy, thong tin gao giu nguyen.\n"; } break;
            case 9: {
                string ma = maHienTai;
                if (docMaMoi(root, ma, true, maHienTai) == KQ_OK && ma != maHienTai) {
                    MatHang tmp = d; setStr(tmp.maMH, ma);
                    root = deleteNode(root, maHienTai.c_str()); root = insertNode(root, tmp);
                    maHienTai = ma; target = searchNode(root, maHienTai.c_str());
                }
            } break;
            default: cout << "Lua chon khong hop le.\n";
        }
    }
}

// ---------- 7.11 Nhap them gao ----------
// Moi lan nhap hoi gia nhap cua lo do, gia nhap cua mat hang duoc tinh lai theo binh quan gia quyen.
bool nhapThemGao(NodeAVL* root) {
    string ma = chonMaCoGoiY(root, "CHON MAT HANG CAN NHAP THEM GAO", 1);
    if (ma.empty()) return false;

    NodeAVL* n = searchNode(root, ma.c_str());
    if (!n) return false;
    MatHang& item = n->data;
    cout << "\nMat hang: " << item.tenMH << " (" << item.maMH << ") | Ton: " << fmtNum(item.soKgTon)
         << " kg | Gia nhap hien tai: " << money(item.giaNhapKg) << "/kg\n";

    double kg = 0;
    while (true) {
        string s = trim(readLinePrompt("So kg gao nhap them (Enter=huy): "));
        if (s.empty()) { cout << "Da huy nhap them gao.\n"; return false; }
        if (parseDoubleStrict(s, kg) && kg > 0) break;
        cout << "Loi: So kg phai la so > 0.\n";
    }

    double giaLo = item.giaNhapKg;      // Enter = giu gia hien tai
    KQ kq = docTien("Gia nhap lo nay /kg", giaLo, true, 0, item.giaBanKg);
    if (kq == KQ_HUY || kq == KQ_LUI) { cout << "Da huy nhap them gao.\n"; return false; }

    double giaCu = item.giaNhapKg;
    double giaMoi = (item.soKgTon * giaCu + kg * giaLo) / (item.soKgTon + kg);
    item.soKgTon += kg;
    item.giaNhapKg = giaMoi;

    if ((int)item.lichSuNhap.size() >= MAX_HISTORY) item.lichSuNhap.erase(item.lichSuNhap.begin());
    NhatKyNhap h; h.ngayNhap = homNay(); h.soKg = kg; h.giaNhapKg = giaLo;
    item.lichSuNhap.push_back(h);
    luuTuDong(root);

    cout << "\nNhap gao thanh cong.\n";
    cout << "Ma mat hang  : " << item.maMH << "\n";
    cout << "Ten gao      : " << item.tenMH << "\n";
    cout << "Nhap them    : " << fmtNum(kg) << " kg x " << money(giaLo) << "/kg\n";
    if (giaMoi - giaCu > 1e-6 || giaCu - giaMoi > 1e-6)
        cout << "Gia nhap binh quan: " << money(giaCu) << " -> " << money(giaMoi) << "/kg\n";
    cout << "Ton hien tai : " << fmtNum(item.soKgTon) << " kg\n";
    cout << "Quy doi bao  : " << quyDoiBao(item.soKgTon, item.quyCach) << "\n";
    if (item.soKgTon <= item.tonToiThieuKg)
        cout << "Luu y: Ton kho van o muc toi thieu hoac thap hon.\n";
    return true;
}

// ---------- 7.12 Ban hang ----------
// Mot don hang co the gom nhieu dong: moi dong ban theo kg hoac theo bao (chon loai bao).

// Nhap so kg khach mua. Tra ve false neu quay lai.
bool nhapBanTheoKg(double conLai, double& kg) {
    while (true) {
        string s = trim(readLinePrompt("So kg (Enter = quay lai): "));
        if (s.empty()) return false;
        double v;
        if (!parseDoubleStrict(s, v) || v <= 0) { cout << "Loi: So kg phai la so > 0.\n"; continue; }
        if (v > conLai + 1e-9) { cout << "Loi: Chi con " << fmtNum(conLai) << " kg co the ban.\n"; continue; }
        if (v > conLai) v = conLai;
        kg = v; return true;
    }
}

// Chon loai bao roi nhap so bao. Tra ve false neu quay lai.
bool nhapBanTheoBao(double conLai, const vector<int>& bao, double& kg, int& loaiBao, int& soBao) {
    if (bao.empty()) { cout << "Mat hang nay chua co quy cach bao, hay ban theo kg.\n"; return false; }
    int viTri = 0;
    while (true) {
        cout << "\nChon loai bao:\n";
        for (size_t i = 0; i < bao.size(); ++i) cout << "  " << i + 1 << ". Bao " << bao[i] << " kg\n";
        string s = trim(readLinePrompt("Loai bao (Enter = quay lai): "));
        if (s.empty()) return false;
        int k;
        if (parseIntStrict(s, k) && k >= 1 && k <= (int)bao.size()) { viTri = k - 1; break; }
        cout << "Loi: Chon tu 1 den " << bao.size() << ".\n";
    }
    int kgMotBao = bao[viTri];
    int toiDa = (int)(conLai / kgMotBao + 1e-9);
    if (toiDa < 1) { cout << "Loi: Khong du ton de ban 1 bao " << kgMotBao << " kg (con " << fmtNum(conLai) << " kg).\n"; return false; }
    while (true) {
        string s = trim(readLinePrompt("So bao " + intStr(kgMotBao) + " kg (Enter = quay lai): "));
        if (s.empty()) return false;
        int n;
        if (!parseIntStrict(s, n) || n <= 0) { cout << "Loi: So bao phai la so nguyen > 0.\n"; continue; }
        if (n > toiDa) { cout << "Loi: Chi ban duoc toi da " << toiDa << " bao " << kgMotBao << " kg.\n"; continue; }
        loaiBao = kgMotBao; soBao = n; kg = (double)n * kgMotBao;
        return true;
    }
}

bool banHang(NodeAVL* root) {
    string ma = chonMaCoGoiY(root, "CHON MAT HANG CAN BAN", 1); if (ma.empty()) return false;
    NodeAVL* n = searchNode(root, ma.c_str()); if (!n) return false;
    MatHang& item = n->data;
    cout << "\nMat hang: " << item.tenMH << " (" << item.maMH << ") | Ton: " << fmtNum(item.soKgTon) << " kg | Gia ban: " << money(item.giaBanKg) << "/kg\n";
    if (item.soKgTon <= 0) { cout << "Mat hang nay da het hang, khong ban duoc.\n"; return false; }
    int con = soNgayGiua(homNay(), item.hanSuDung);
    if (con < 0) cout << "CANH BAO: Mat hang nay DA HET HAN " << -con << " ngay!\n";

    vector<int> bao = item.quyCach; sort(bao.begin(), bao.end()); reverse(bao.begin(), bao.end());
    double tongKg = 0;
    vector<string> dong;   // cac dong cua don hang, de hien thi
    vector<SoBaoBan> baoBan;   // so bao da chon theo tung loai (de ghi nhat ky)

    while (true) {
        double conLai = item.soKgTon - tongKg;
        if (conLai < 1e-9) { cout << "\nDa chon het so gao trong kho.\n"; break; }

        cout << "\n--- DON HANG ---\n";
        if (dong.empty()) cout << "(chua co)\n";
        else {
            for (size_t i = 0; i < dong.size(); ++i) cout << "  " << i + 1 << ". " << dong[i] << "\n";
            cout << "Tong: " << fmtNum(tongKg) << " kg = " << money(tongKg * item.giaBanKg) << "\n";
        }
        cout << "1. Ban theo kg\n2. Ban theo bao\n";
        string s = trim(readLinePrompt("Chon (Enter = ket thuc don, 0 = huy don): "));
        if (s.empty()) break;
        int k;
        if (!parseIntStrict(s, k) || k < 0 || k > 2) { cout << "Loi: Chon 1, 2 hoac Enter/0.\n"; continue; }
        if (k == 0) { cout << "Da huy don hang.\n"; return false; }

        double kg = 0; string mota;
        if (k == 1) {
            if (!nhapBanTheoKg(conLai, kg)) continue;
            mota = fmtNum(kg) + " kg";
        } else {
            int loaiBao = 0, soBao = 0;
            if (!nhapBanTheoBao(conLai, bao, kg, loaiBao, soBao)) continue;
            mota = intStr(soBao) + " bao " + intStr(loaiBao) + " kg (" + fmtNum(kg) + " kg)";
            congBao(baoBan, SoBaoBan(loaiBao, soBao));
        }
        tongKg += kg;
        dong.push_back(mota + " = " + money(kg * item.giaBanKg));
    }

    if (tongKg <= 0) { cout << "Don hang trong, khong ban.\n"; return false; }

    cout << "\n=== XAC NHAN DON HANG ===\n" << item.tenMH << " (" << item.maMH << ")\n";
    for (size_t i = 0; i < dong.size(); ++i) cout << "  " << i + 1 << ". " << dong[i] << "\n";
    cout << "Tong: " << fmtNum(tongKg) << " kg = " << money(tongKg * item.giaBanKg) << "\n";
    string yn = trim(readLinePrompt("Enter = xac nhan ban, n = huy: "));
    if (toLowerVN(yn) == "n") { cout << "Da huy don hang.\n"; return false; }

    if (tongKg > item.soKgTon) tongKg = item.soKgTon;
    double dt = tongKg * item.giaBanKg, gv = tongKg * item.giaNhapKg;
    item.soKgTon -= tongKg; if (item.soKgTon < 1e-9) item.soKgTon = 0;
    item.doanhThu += dt; item.giaVonHangBan += gv; ghiNhatKy(item, homNay(), dt, gv, tongKg, baoBan);
    luuTuDong(root);
    cout << "Ban hang thanh cong.\nSo kg ban: " << fmtNum(tongKg) << " kg\nDoanh thu: " << money(dt)
         << "\nGia von: " << money(gv) << "\nLoi nhuan: " << money(dt - gv) << "\n";
    cout << "Ton con lai: " << fmtNum(item.soKgTon) << " kg (" << quyDoiBao(item.soKgTon, item.quyCach) << ")\n";
    if (item.soKgTon <= item.tonToiThieuKg) cout << "Luu y: Ton kho da cham/duoi muc toi thieu (" << item.tonToiThieuKg << " kg), can nhap them gao.\n";
    return true;
}

// ---------- 7.13 Tra cuu theo khoang gia ban ----------
void traCuuKhoangGia(NodeAVL* root) {
    double minG = 0, maxG = 0;
    KQ k = docTien("Gia ban thap nhat", minG, false, -1, 0); if (k != KQ_OK) return;
    k = docTien("Gia ban cao nhat", maxG, false, minG - 1e-9, 0); if (k != KQ_OK) return;
    vector<MatHang> v; collectNodes(root, v); bool f = false;
    printHeader();
    for (size_t i = 0; i < v.size(); ++i) if (!v[i].ngungBan && v[i].giaBanKg >= minG && v[i].giaBanKg <= maxG) { printItem(v[i]); f = true; }
    if (!f) cout << "Khong co mat hang trong khoang gia.\n";
}

// ---------- 7.14 Canh bao ----------
void canhBaoTonKho(NodeAVL* r, bool& found) {
    if (!r) return;
    canhBaoTonKho(r->left, found);
    if (!r->data.ngungBan && r->data.soKgTon <= r->data.tonToiThieuKg) { printItem(r->data); found = true; }
    canhBaoTonKho(r->right, found);
}

void canhBaoHSD(NodeAVL* r, int nguong, bool& found) {
    if (!r) return;
    canhBaoHSD(r->left, nguong, found);
    int con = soNgayGiua(homNay(), r->data.hanSuDung);
    if (!r->data.ngungBan && con <= nguong) { printItem(r->data); found = true; }
    canhBaoHSD(r->right, nguong, found);
}

void chucNangCanhBaoHSD(NodeAVL* root) {
    while (true) {
        string a = trim(readLinePrompt("Canh bao gao het han trong bao nhieu ngay toi? (Enter=huy): "));
        if (a.empty()) return;
        int k;
        if (parseIntStrict(a, k) && k >= 0) {
            bool f = false; printHeader(); canhBaoHSD(root, k, f);
            if (!f) cout << "Khong co gao sap/da het han trong nguong.\n";
            return;
        }
        cout << "Loi: Phai la so nguyen >= 0.\n";
    }
}

// ---------- 7.15 Tra cuu doanh thu ----------
// kieu: 1 = theo ngay, 2 = theo thang, 3 = theo nam. "moc" la ngay / thang / nam can xem.
bool trongKhoang(const NgayThang& d, int kieu, const NgayThang& moc) {
    if (kieu == 1) return d == moc;
    if (kieu == 2) return d.thang == moc.thang && d.nam == moc.nam;
    if (kieu == 3) return d.nam == moc.nam;
    return true;   // kieu 4: toan bo thoi gian
}

struct DongDoanhThu {
    string ma, ten;
    double doanhThu, giaVon, soKg;
    bool thieuKg;                // co don ban cu khong ghi so kg
    vector<SoBaoBan> bao;
    DongDoanhThu() : doanhThu(0), giaVon(0), soKg(0), thieuKg(false) {}
};

// Duyet cay, gom doanh thu / so kg / so bao cua tung mat hang trong khoang can xem.
void gomDoanhThu(NodeAVL* root, int kieu, const NgayThang& moc, vector<DongDoanhThu>& kq) {
    if (!root) return;
    gomDoanhThu(root->left, kieu, moc, kq);
    const MatHang& mh = root->data;
    DongDoanhThu x; x.ma = mh.maMH; x.ten = mh.tenMH;
    bool co = false;
    if (kieu == 4) {         // toan bo: dung tong tich luy cua mat hang
        x.doanhThu = mh.doanhThu; x.giaVon = mh.giaVonHangBan; co = mh.doanhThu > 0;
    } else {
        for (size_t i = 0; i < mh.lichSuNgay.size(); ++i) {
            const NhatKyNgay& l = mh.lichSuNgay[i];
            if (!trongKhoang(l.ngayGhi, kieu, moc)) continue;
            co = true; x.doanhThu += l.doanhThu; x.giaVon += l.giaVon;
            if (l.soKg < 0) x.thieuKg = true; else x.soKg += l.soKg;
            for (size_t j = 0; j < l.bao.size(); ++j) congBao(x.bao, l.bao[j]);
        }
    }
    if (co) kq.push_back(x);
    gomDoanhThu(root->right, kieu, moc, kq);
}

// "4 bao 50kg + 2 bao 25kg + 3.5 kg le": quy doi so kg ban ra thanh bao + kg le
string moTaBanRa(const vector<SoBaoBan>& bao, double soKg) {
    vector<SoBaoBan> t = bao;
    for (size_t i = 0; i < t.size(); ++i)       // lon -> nho
        for (size_t j = i + 1; j < t.size(); ++j) if (t[j].kgMotBao > t[i].kgMotBao) swap(t[i], t[j]);
    ostringstream o; bool co = false; double kgBao = 0;
    for (size_t i = 0; i < t.size(); ++i) {
        if (t[i].soBao <= 0) continue;
        if (co) o << " + ";
        o << t[i].soBao << " bao " << t[i].kgMotBao << "kg"; co = true;
        kgBao += (double)t[i].soBao * t[i].kgMotBao;
    }
    double le = soKg - kgBao;
    if (le > 1e-9) { if (co) o << " + "; o << fmtNum(le) << " kg le"; co = true; }
    return o.str();
}

// So kg da ban; "?" neu hoan toan khong co du lieu kg, them "*" neu chi co mot phan
string chuoiKg(double kg, bool thieu) {
    if (thieu && kg <= 0) return "?";
    return fmtNum(kg) + (thieu ? "*" : "");
}

void hienDoanhThu(NodeAVL* root, int kieu, const NgayThang& moc, const string& tieuDe) {
    vector<DongDoanhThu> kq; gomDoanhThu(root, kieu, moc, kq);
    cout << "\n=== " << tieuDe << " ===\n";
    if (kq.empty()) { cout << "Khong co giao dich nao.\n"; return; }
    cout << left << setw(13) << "Ma mat hang" << setw(24) << "Ten gao" << setw(14) << "Da ban (kg)"
         << setw(18) << "Doanh thu" << setw(18) << "Gia von" << setw(18) << "Loi nhuan" << "\n";
    cout << string(105, '-') << "\n";
    double tongDT = 0, tongGV = 0, tongKg = 0; bool thieu = false;
    vector<SoBaoBan> tongBao;
    for (size_t i = 0; i < kq.size(); ++i) {
        const DongDoanhThu& d = kq[i];
        cout << left << setw(13) << fitCell(d.ma, 12) << setw(24) << fitCell(d.ten, 23)
             << setw(14) << chuoiKg(d.soKg, d.thieuKg)
             << setw(18) << fmtNum(d.doanhThu) << setw(18) << fmtNum(d.giaVon)
             << setw(18) << fmtNum(d.doanhThu - d.giaVon) << "\n";
        string mt = moTaBanRa(d.bao, d.soKg);
        if (!mt.empty()) cout << "    -> " << mt << "\n";
        tongDT += d.doanhThu; tongGV += d.giaVon; tongKg += d.soKg;
        if (d.thieuKg) thieu = true;
        for (size_t j = 0; j < d.bao.size(); ++j) congBao(tongBao, d.bao[j]);
    }
    cout << string(105, '-') << "\n";
    cout << left << setw(37) << "TONG" << setw(14) << chuoiKg(tongKg, thieu)
         << setw(18) << fmtNum(tongDT) << setw(18) << fmtNum(tongGV) << setw(18) << fmtNum(tongDT - tongGV) << "\n";
    string mtTong = moTaBanRa(tongBao, tongKg);
    if (!mtTong.empty()) cout << "Quy doi tong: " << mtTong << "\n";
    cout << "(Don vi tien: VND)\n";
    if (thieu) cout << "* Chua tinh so kg cua cac don ban truoc khi nang cap file du lieu (khong co so kg).\n";
}

// Chuyen "thang nam" (vi du 10.2026, 10/26) hoac chi "thang" (lay nam hien tai). Dau ngan cach nhu ngay.
bool chuyenThangNam(const string& s, int& thang, int& nam) {
    string t;
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = (unsigned char)s[i];
        if (isdigit(c)) t += (char)c;
        else if (c == '/' || c == '-' || c == '.' || c == ',' || c == ':' || c == ' ' || c == '\t') t += ' ';
        else return false;
    }
    stringstream ss(t); vector<int> so; string tk;
    while (ss >> tk) {
        if (tk.size() > 4) return false;
        so.push_back(atoi(tk.c_str()));
    }
    if (so.size() == 1) { thang = so[0]; nam = homNay().nam; }
    else if (so.size() == 2) { thang = so[0]; nam = so[1]; }
    else return false;
    if (nam >= 0 && nam < 100) nam += 2000;
    return thang >= 1 && thang <= 12 && nam >= 2000 && nam <= 2500;
}

// Enter = thang nay. Tra ve false neu quay lai / huy.
bool docThang(NgayThang& out) {
    while (true) {
        string s = trim(readLinePrompt("Thang can xem (thang.nam, Enter = thang nay): "));
        KQ kq; if (lenhDacBiet(s, kq)) return false;
        if (s.empty()) { out = homNay(); out.ngay = 1; return true; }
        int t, n;
        if (chuyenThangNam(s, t, n)) { out = NgayThang(1, t, n); return true; }
        cout << "Loi: Thang khong hop le (thang 1-12, nam 2000-2500).\n";
    }
}

// Enter = nam nay. Tra ve false neu quay lai / huy.
bool docNam(int& out) {
    while (true) {
        string s = trim(readLinePrompt("Nam can xem (Enter = nam nay): "));
        KQ kq; if (lenhDacBiet(s, kq)) return false;
        if (s.empty()) { out = homNay().nam; return true; }
        int n;
        if (parseIntStrict(s, n)) {
            if (n >= 0 && n < 100) n += 2000;
            if (n >= 2000 && n <= 2500) { out = n; return true; }
        }
        cout << "Loi: Nam khong hop le (2000-2500).\n";
    }
}

string chuoiThang(const NgayThang& d) {
    ostringstream o; o << setfill('0') << setw(2) << d.thang << "/" << setw(4) << d.nam; return o.str();
}

// Chon khoang thoi gian de xem. kieu: 1 = ngay, 2 = thang, 3 = nam, 4 = toan bo.
// Tra ve false neu chon 0 (quay lai).
bool chonKhoangThoiGian(const string& tenMenu, bool coToanBo, int& kieu, NgayThang& moc, string& nhan) {
    while (true) {
        cout << "\n=== " << tenMenu << " ===\n";
        cout << "1. Hom nay\n2. Ngay tuy chon\n3. Theo thang\n4. Theo nam\n";
        if (coToanBo) cout << "5. Toan bo thoi gian\n";
        cout << "0. Quay lai\n";
        string s = readLinePrompt("Chon: "); int k; int toiDa = coToanBo ? 5 : 4;
        if (!parseIntStrict(s, k) || k < 0 || k > toiDa) { cout << "Loi: Chon tu 0 den " << toiDa << ".\n"; continue; }
        if (k == 0) return false;
        NgayThang hn = homNay();
        if (k == 1) { kieu = 1; moc = hn; nhan = "HOM NAY (" + ngayThanhChuoi(hn) + ")"; return true; }
        if (k == 2) {
            NgayThang d = hn;
            if (docNgay("Ngay can xem", d, false) == KQ_OK) { kieu = 1; moc = d; nhan = "NGAY " + ngayThanhChuoi(d); return true; }
            continue;
        }
        if (k == 3) {
            NgayThang d;
            if (docThang(d)) { kieu = 2; moc = d; nhan = "THANG " + chuoiThang(d); return true; }
            continue;
        }
        if (k == 4) {
            int nam;
            if (docNam(nam)) { kieu = 3; moc = NgayThang(1, 1, nam); nhan = "NAM " + intStr(nam); return true; }
            continue;
        }
        kieu = 4; moc = hn; nhan = "TOAN BO THOI GIAN"; return true;
    }
}

void traCuuDoanhThu(NodeAVL* root) {
    int kieu = 1; NgayThang moc; string nhan;
    while (chonKhoangThoiGian("TRA CUU DOANH THU", false, kieu, moc, nhan))
        hienDoanhThu(root, kieu, moc, "DOANH THU " + nhan);
}

struct SoSanhDoanhThu { bool operator()(const DongDoanhThu& a, const DongDoanhThu& b) const { return a.doanhThu > b.doanhThu; } };

void topDoanhThu(NodeAVL* root) {
    int kieu = 1; NgayThang moc; string nhan;
    while (chonKhoangThoiGian("TOP DOANH THU", true, kieu, moc, nhan)) {
        vector<DongDoanhThu> all, v; gomDoanhThu(root, kieu, moc, all);
        for (size_t i = 0; i < all.size(); ++i) if (all[i].doanhThu > 0) v.push_back(all[i]);
        if (v.empty()) { cout << "\n=== " << nhan << " ===\nChua co mat hang nao co doanh thu, chua the xep hang.\n"; continue; }
        stable_sort(v.begin(), v.end(), SoSanhDoanhThu());
        int n = (int)v.size() < 3 ? (int)v.size() : 3;
        cout << "\n=== TOP " << n << " GAO CO DOANH THU CAO NHAT - " << nhan << " ===\n";
        cout << left << setw(6) << "Hang" << setw(13) << "Ma mat hang" << setw(24) << "Ten gao"
             << setw(18) << "Doanh thu" << setw(18) << "Gia von" << setw(18) << "Loi nhuan" << "\n";
        cout << string(97, '-') << "\n";
        for (int i = 0; i < n; ++i)
            cout << left << setw(6) << (i + 1) << setw(13) << fitCell(v[i].ma, 12) << setw(24) << fitCell(v[i].ten, 23)
                 << setw(18) << fmtNum(v[i].doanhThu) << setw(18) << fmtNum(v[i].giaVon)
                 << setw(18) << fmtNum(v[i].doanhThu - v[i].giaVon) << "\n";
        cout << "(Don vi tien: VND)\n";
        if (n < 3) cout << "Luu y: Chi co " << n << " mat hang co doanh thu nen chi xep hang " << n << " mat hang.\n";
    }
}

// ---------- 7.16 Ngung ban / xoa mat hang ----------
// Ngung ban: an mat hang khoi danh sach & khi ban/nhap, nhung van giu lich su doanh thu trong bao cao.
void quanLyNgungBan(NodeAVL*& root) {
    while (true) {
        cout << "\n=== NGUNG BAN / XOA MAT HANG ===\n";
        cout << "1. Ngung ban (an khoi danh sach, van giu doanh thu)\n2. Ban lai mat hang da ngung\n";
        cout << "3. Xem mat hang dang ngung ban\n4. Xoa han (mat luon lich su doanh thu)\n0. Quay lai\n";
        string s = readLinePrompt("Chon: "); int k;
        if (!parseIntStrict(s, k) || k < 0 || k > 4) { cout << "Loi: Chon tu 0 den 4.\n"; continue; }
        if (k == 0) return;
        if (k == 3) {
            if (demMatHang(root, true) == 0) cout << "Khong co mat hang nao dang ngung ban.\n";
            else { printHeader(); displayInOrder(root, true); }
            continue;
        }
        string ma;
        if (k == 1) ma = chonMaCoGoiY(root, "CHON MAT HANG CAN NGUNG BAN", 1);
        else if (k == 2) ma = chonMaCoGoiY(root, "CHON MAT HANG CAN BAN LAI", 2);
        else ma = chonMaCoGoiY(root, "CHON MAT HANG CAN XOA HAN", 0);
        if (ma.empty()) continue;
        NodeAVL* n = searchNode(root, ma.c_str()); if (!n) continue;

        if (k == 1) {
            n->data.ngungBan = true; luuTuDong(root);
            cout << "Da ngung ban mat hang " << ma << ".\n";
        } else if (k == 2) {
            n->data.ngungBan = false; luuTuDong(root);
            cout << "Da cho ban lai mat hang " << ma << ".\n";
        } else {
            string canh = n->data.doanhThu > 0 ? " (se mat lich su doanh thu " + money(n->data.doanhThu) + " khoi bao cao)" : string("");
            string yn = trim(readLinePrompt("Xac nhan xoa han mat hang " + ma + canh + "? (y = xoa, Enter = huy): "));
            if (toLowerVN(yn) == "y") {
                root = deleteNode(root, ma.c_str()); luuTuDong(root);
                cout << "Xoa mat hang thanh cong.\n";
            } else cout << "Da huy xoa.\n";
        }
    }
}

// ================================================================
// 8. TIEN ICH DON BO DEM
// ================================================================
string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string toLowerVN(const string& s) {   // ASCII A-Z -> a-z
    string r = s;
    for (size_t i = 0; i < r.size(); ++i) { unsigned char c = (unsigned char)r[i]; if (c >= 'A' && c <= 'Z') r[i] = (char)(c - 'A' + 'a'); }
    return r;
}

void clearBuffer() { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); }

// Doc 1 dong; het du lieu (EOF) thi thoat chuong trinh an toan thay vi lap vo han.
void gLine(string& s) {
    if (!std::getline(std::cin, s)) { cout << "\nDa dong dau vao. Thoat chuong trinh.\n"; exit(0); }
}

string readLinePrompt(const string& prompt) { cout << prompt; string s; gLine(s); return s; }

bool parseDoubleStrict(const string& s, double& value) {
    string t = trim(s);
    if (t.empty()) return false;
    int dots = 0, digits = 0;   // chi nhan chu so, dau +/-, dau cham (chan nan, inf, 0x.., 1e5)
    for (size_t i = 0; i < t.size(); ++i) {
        char c = t[i];
        if (c >= '0' && c <= '9') ++digits;
        else if (c == '.') ++dots;
        else if ((c == '-' || c == '+') && i == 0) {}
        else return false;
    }
    if (digits == 0 || dots > 1) return false;
    char* endPtr = NULL;
    double v = strtod(t.c_str(), &endPtr);
    if (*endPtr != '\0') return false;
    if (v != v || v > 1e12 || v < -1e12) return false;
    value = v; return true;
}

bool parseIntStrict(const string& s, int& value) {
    string t = trim(s);
    if (t.empty()) return false;
    char* endPtr = NULL; errno = 0;
    long v = strtol(t.c_str(), &endPtr, 10);
    if (errno == ERANGE || endPtr == t.c_str()) return false;
    while (*endPtr) { if (!isspace((unsigned char)*endPtr)) return false; ++endPtr; }
    if (v < numeric_limits<int>::min() || v > numeric_limits<int>::max()) return false;
    value = (int)v; return true;
}

// Gan chuoi vao mang char: xoa sach truoc, tu choi neu qua dai.
template<size_t N>
bool setStr(char (&dst)[N], const string& v) {
    if (v.size() >= N) return false;
    memset(dst, 0, N);
    memcpy(dst, v.c_str(), v.size());
    return true;
}

// ================================================================
// 9. CHUONG TRINH CHINH
// ================================================================
int main() {
    NodeAVL* root = NULL;
    const char* filename = DATA_FILE;
    root = loadFromFile(root, filename);

    int choice = -1;

    do {
        cout << "\n===============================================================\n";
        cout << "        HE THONG QUAN LY GAO TON KHO & DOANH THU SIEU THI\n";
        cout << "===============================================================\n";

        // Thu tu quan ly theo yeu cau:
        // 1. Xem danh sach
        // 2. Them
        // 3. Sua
        // 4. Xoa
        // 5. Ban
        // 6. Nhap hang
        cout << "1. Xem danh sach mat hang\n";
        cout << "2. Them mat hang\n";
        cout << "3. Sua thong tin mat hang\n";
        cout << "4. Ngung ban / xoa mat hang\n";
        cout << "5. Ban hang\n";
        cout << "6. Nhap hang\n";
        cout << "7. Tra cuu gao\n";
        cout << "8. Thong tin gao chi tiet\n";
        cout << "9. Canh bao ton kho\n";
        cout << "10. Canh bao gao sap/da het han\n";
        cout << "11. Tra cuu doanh thu & loi nhuan\n";
        cout << "12. Top 3 doanh thu\n";
        cout << "13. Doi don vi nhap gia (hien tai: " << gTenDV << ")\n";
        cout << "0. Luu & Thoat\n";
        cout << "---------------------------------------------------------------\n";

        string s = readLinePrompt("Chon chuc nang: ");

        if (!parseIntStrict(s, choice)) {
            cout << "Loi: Vui long nhap so nguyen.\n";
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1:
                if (demMatHang(root, false) == 0) {
                    cout << "Kho gao dang trong.\n";
                } else {
                    printHeader();
                    displayInOrder(root);
                }
                break;

            case 2:
                nhapMatHangMoi(root);
                break;

            case 3: {
                string ma = chonMaCoGoiY(root, "CHON MAT HANG CAN SUA");
                if (!ma.empty())
                    suaMatHang(root, ma);
                break;
            }

            case 4:
                quanLyNgungBan(root);
                break;

            case 5:
                banHang(root);
                break;

            case 6:
                nhapThemGao(root);
                break;

            case 7:
                menuTraCuuGao(root);
                break;

            case 8: {
                string ma = chonMaCoGoiY(root, "CHON GAO XEM THONG TIN CHI TIET", 0);
                if (!ma.empty()) {
                    NodeAVL* n = searchNode(root, ma.c_str());
                    if (n) printGaoDetail(n->data);
                }
                break;
            }

            case 9: {
                bool f = false;

                cout << "\n=== CANH BAO TON KHO DUOI/BANG MUC TOI THIEU ===\n";
                printHeader();
                canhBaoTonKho(root, f);

                if (!f)
                    cout << "Khong co mat hang nao can bo sung.\n";

                break;
            }

            case 10:
                chucNangCanhBaoHSD(root);
                break;

            case 11:
                traCuuDoanhThu(root);
                break;

            case 12:
                topDoanhThu(root);
                break;

            case 13:
                chonDonViGia();
                break;

            case 0:
                saveToFile(root, filename);
                deleteTree(root);
                cout << "Cam on ban da su dung phan mem!\n";
                break;

            default:
                cout << "Lua chon khong hop le. Vui long chon lai.\n";
        }

    } while (choice != 0);

    return 0;
}
