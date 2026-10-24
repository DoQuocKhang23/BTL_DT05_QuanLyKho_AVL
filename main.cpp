#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <limits>

using namespace std;

// Header danh dau phien ban cau truc du lieu nhi phan (chong doc file .dat cu gay loi so rac)
const char FILE_HEADER_MAGIC[] = "KHO_v3";

// 1. CAU TRUC DU LIEU MAT HANG
struct MatHang {
    char maMH[15];          // Ma mat hang (Khoa chinh)
    char tenMH[50];         // Ten mat hang
    int soLuongTon;         // So luong ton kho hien tai
    int tonToiThieu;        // Nguong ton kho toi thieu
    double giaNhap;         // Gia nhap vao
    double giaBan;          // Gia ban ra
    double doanhThu;        // Tong doanh thu tich luy tu ban hang
  	double giaVonHangBan;   // Tong gia von cua cac mat hang DA BAN
    int hanSuDungNgay;      // So ngay con lai cua han su dung

    MatHang() {
        maMH[0] = '\0';
        tenMH[0] = '\0';
        soLuongTon = 0;
        tonToiThieu = 0;
        giaNhap = 0.0;
        giaBan = 0.0;
        doanhThu = 0.0;
       giaVonHangBan = 0.0;
        hanSuDungNgay = 0;
    }
};

struct NodeAVL {
    MatHang data;
    NodeAVL* left;
    NodeAVL* right;
    int height;

    NodeAVL(const MatHang& item) {
        data = item;
        left = nullptr;
        right = nullptr;
        height = 1;
    }
};

int getHeight(NodeAVL* node) {
    return node ? node->height : 0;
}

int getBalanceFactor(NodeAVL* node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

void updateHeight(NodeAVL* node) {
    if (node) {
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    }
}

NodeAVL* rotateRight(NodeAVL* y) {
    NodeAVL* x = y->left;
    NodeAVL* T2 = x->right;
    x->right = y;
    y->left = T2;
    updateHeight(y);
    updateHeight(x);
    return x;
}

NodeAVL* rotateLeft(NodeAVL* x) {
    NodeAVL* y = x->right;
    NodeAVL* T2 = y->left;
    y->left = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

NodeAVL* balanceNode(NodeAVL* node) {
    if (!node) return nullptr;
    
    updateHeight(node);
    int bf = getBalanceFactor(node);

    if (bf > 1 && getBalanceFactor(node->left) >= 0) return rotateRight(node);
    if (bf > 1 && getBalanceFactor(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (bf < -1 && getBalanceFactor(node->right) <= 0) return rotateLeft(node);
    if (bf < -1 && getBalanceFactor(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

NodeAVL* insertNode(NodeAVL* node, const MatHang& item) {
    if (!node) return new NodeAVL(item);

    int cmp = strcmp(item.maMH, node->data.maMH);
    if (cmp < 0) node->left = insertNode(node->left, item);
    else if (cmp > 0) node->right = insertNode(node->right, item);
    else {
        cout << "Loi: Ma mat hang " << item.maMH << " da ton tai!\n";
        return node;
    }
    return balanceNode(node);
}

NodeAVL* findMinNode(NodeAVL* node) {
    NodeAVL* current = node;
    while (current && current->left != nullptr) current = current->left;
    return current;
}

NodeAVL* deleteNode(NodeAVL* node, const char* maMH) {
    if (!node) return nullptr;

    int cmp = strcmp(maMH, node->data.maMH);
    if (cmp < 0) node->left = deleteNode(node->left, maMH);
    else if (cmp > 0) node->right = deleteNode(node->right, maMH);
    else {
        if (!node->left || !node->right) {
            NodeAVL* temp = node->left ? node->left : node->right;
            if (!temp) {
                temp = node;
                node = nullptr;
            } else {
                *node = *temp; 
            }
            delete temp;
        } else {
            NodeAVL* temp = findMinNode(node->right);
            node->data = temp->data;
            node->right = deleteNode(node->right, temp->data.maMH);
        }
    }
    
    if (!node) return nullptr;
    return balanceNode(node);
}

NodeAVL* searchNode(NodeAVL* root, const char* maMH) {
    if (!root || strcmp(root->data.maMH, maMH) == 0) return root;
    if (strcmp(maMH, root->data.maMH) < 0) return searchNode(root->left, maMH);
    return searchNode(root->right, maMH);
}

void deleteTree(NodeAVL* root) {
    if (root) {
        deleteTree(root->left);
        deleteTree(root->right);
        delete root;
    }
}

void writeNodeToFile(NodeAVL* node, ofstream& outFile) {
    if (!node) return;
    outFile.write(node->data.maMH, sizeof(node->data.maMH));
    outFile.write(node->data.tenMH, sizeof(node->data.tenMH));
    outFile.write(reinterpret_cast<char*>(&node->data.soLuongTon), sizeof(int));
    outFile.write(reinterpret_cast<char*>(&node->data.tonToiThieu), sizeof(int));
    outFile.write(reinterpret_cast<char*>(&node->data.giaNhap), sizeof(double));
    outFile.write(reinterpret_cast<char*>(&node->data.giaBan), sizeof(double));
    outFile.write(reinterpret_cast<char*>(&node->data.doanhThu), sizeof(double));
    outFile.write(reinterpret_cast<char*>(&node->data.giaVonHangBan), sizeof(double));
    outFile.write(reinterpret_cast<char*>(&node->data.hanSuDungNgay), sizeof(int));
    
    writeNodeToFile(node->left, outFile);
    writeNodeToFile(node->right, outFile);
}

void saveToFile(NodeAVL* root, const char* filename) {
    ofstream outFile(filename, ios::binary | ios::trunc);
    if (!outFile) {
        cout << "Loi: Khong the mo file de ghi!\n";
        return;
    }
    
    // Ghi Magic Header kiem tra phien ban
    outFile.write(FILE_HEADER_MAGIC, sizeof(FILE_HEADER_MAGIC));
    
    writeNodeToFile(root, outFile);
    
    if (!outFile) {
        cout << "Loi: Co loi xay ra trong qua trinh ghi file du lieu!\n";
    } else {
        cout << "Luu du lieu vao file nhi phan thanh cong!\n";
    }
    outFile.close();
}

NodeAVL* loadFromFile(NodeAVL* root, const char* filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cout << "Khong tim thay file du lieu cu. Tao kho hang moi!\n";
        return root;
    }

    // Kiem tra Magic Header phien ban (Dung strncmp de tranh loi doc tran bo nho)
    char header[sizeof(FILE_HEADER_MAGIC)];
    inFile.read(header, sizeof(header));
    if (inFile.gcount() != sizeof(header) || strncmp(header, FILE_HEADER_MAGIC, sizeof(FILE_HEADER_MAGIC) - 1) != 0) {
        cout << "CANH BAO: File .dat thuoc phien ban cu hoac khong dung dinh dang!\n";
        cout << "Tu dong khoi tao kho hang moi de tranh doc so rac.\n";
        inFile.close();
        return root;
    }

    MatHang item;
    int count = 0;
    while (true) {
        inFile.read(item.maMH, sizeof(item.maMH));
        if (inFile.gcount() != sizeof(item.maMH)) break;
        
        inFile.read(item.tenMH, sizeof(item.tenMH));
        inFile.read(reinterpret_cast<char*>(&item.soLuongTon), sizeof(int));
        inFile.read(reinterpret_cast<char*>(&item.tonToiThieu), sizeof(int));
        inFile.read(reinterpret_cast<char*>(&item.giaNhap), sizeof(double));
        inFile.read(reinterpret_cast<char*>(&item.giaBan), sizeof(double));
        inFile.read(reinterpret_cast<char*>(&item.doanhThu), sizeof(double));
        inFile.read(reinterpret_cast<char*>(&item.giaVonHangBan), sizeof(double));
        inFile.read(reinterpret_cast<char*>(&item.hanSuDungNgay), sizeof(int));
        
        if (!inFile) break;
        root = insertNode(root, item);
        count++;
    }
    inFile.close();

    if (count > 0) {
        cout << "Tai du lieu tu file nhi phan thanh cong (" << count << " mat hang)!\n";
    } else {
        cout << "File du lieu rong. Kho hang hien tai trong!\n";
    }
    return root;
}

void printHeader() {
    cout << left << setw(10) << "Ma MH"
         << setw(25) << "Ten Mat Hang"
         << setw(10) << "Ton Kho"
         << setw(10) << "Ton Min"
         << setw(12) << "Gia Nhap"
         << setw(12) << "Gia Ban"
         << setw(15) << "Doanh Thu"
         << setw(10) << "HSD(ngay)" << "\n";
    cout << string(104, '-') << "\n"; // Da can chinh vua van 104 ky tu
}

void printItem(const MatHang& item) {
    cout << left << setw(10) << item.maMH
         << setw(25) << item.tenMH
         << setw(10) << item.soLuongTon
         << setw(10) << item.tonToiThieu;

    cout << fixed << setprecision(0)
         << setw(12) << item.giaNhap
         << setw(12) << item.giaBan
         << setw(15) << item.doanhThu;

    cout.unsetf(ios::fixed);
    cout << setprecision(6)
         << setw(10) << item.hanSuDungNgay << "\n";
}

void displayInOrder(NodeAVL* root) {
    if (!root) return;
    displayInOrder(root->left);
    printItem(root->data);
    displayInOrder(root->right);
}

void rangeSearchMaMH(NodeAVL* root, const char* minMa, const char* maxMa, bool& found) {
    if (!root) return;

    int cmpMin = strcmp(minMa, root->data.maMH);
    int cmpMax = strcmp(maxMa, root->data.maMH);

    if (cmpMin < 0) {
        rangeSearchMaMH(root->left, minMa, maxMa, found);
    }

    if (cmpMin <= 0 && cmpMax >= 0) {
        printItem(root->data);
        found = true;
    }

    if (cmpMax > 0) {
        rangeSearchMaMH(root->right, minMa, maxMa, found);
    }
}

void rangeSearchGiaBan(NodeAVL* root, double minGia, double maxGia, bool& found) {
    if (!root) return;

    rangeSearchGiaBan(root->left, minGia, maxGia, found);
    
    if (root->data.giaBan >= minGia && root->data.giaBan <= maxGia) {
        printItem(root->data);
        found = true;
    }
    
    rangeSearchGiaBan(root->right, minGia, maxGia, found);
}

void canhBaoTonKho(NodeAVL* root, bool& found) {
    if (!root) return;
    canhBaoTonKho(root->left, found);
    if (root->data.soLuongTon <= root->data.tonToiThieu) {
        printItem(root->data);
        found = true;
    }
    canhBaoTonKho(root->right, found);
}

void canhBaoHanSuDung(NodeAVL* root, int ngatHSD, bool& found) {
    if (!root) return;
    canhBaoHanSuDung(root->left, ngatHSD, found);
    if (root->data.hanSuDungNgay <= ngatHSD) {
        printItem(root->data);
        found = true;
    }
    canhBaoHanSuDung(root->right, ngatHSD, found);
}

void thongKeTaiChinh(NodeAVL* root, double& tongDoanhThu, double& tongGiaVon) {
    if (!root) return;
    thongKeTaiChinh(root->left, tongDoanhThu, tongGiaVon);
    tongDoanhThu += root->data.doanhThu;
    tongGiaVon += root->data.giaVonHangBan;
    thongKeTaiChinh(root->right, tongDoanhThu, tongGiaVon);
}

void collectNodes(NodeAVL* root, vector<MatHang>& list) {
    if (!root) return;
    collectNodes(root->left, list);
    list.push_back(root->data);
    collectNodes(root->right, list);
}

bool soSanhDoanhThuGiamDan(const MatHang& a, const MatHang& b) {
    return a.doanhThu > b.doanhThu;
}

void topDoanhThu(NodeAVL* root) {
    vector<MatHang> list;
    collectNodes(root, list);
    if (list.empty()) {
        cout << "Kho hang trong!\n";
        return;
    }
    
    sort(list.begin(), list.end(), soSanhDoanhThuGiamDan);

    cout << "\n=== TOP MAT HANG CO DOANH THU CAO NHAT ===\n";
    printHeader();
    int count = min((int)list.size(), 3);
    for (int i = 0; i < count; i++) printItem(list[i]);
}

// TIEN ICH DON BO DEM
void clearBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
	 NodeAVL* root = nullptr;
    const char* filename = "danh_muc_kho.dat";

    root = loadFromFile(root, filename);
	
	int choice = -1;
    do {
        cout << "\n=======================================================\n";
        cout << "   HE THONG QUAN LY HANG TON KHO & DOANH THU SIEU THI   \n";
        cout << "=======================================================\n";
        cout << "1. Xem danh sach tat ca mat hang\n";
        cout << "2. Them mat hang moi\n";
        cout << "3. Cap nhat/Sua thong tin mat hang\n";
        cout << "4. Xoa mat hang khoi he thong\n";
        cout << "5. Nhap kho (Tang so luong ton)\n";
        cout << "6. Xuat kho / Ban hang (Cap nhat Doanh thu)\n";
        cout << "7. Canh bao hang ton kho duoi/bang muc toi thieu\n";
        cout << "8. Tra cuu mat hang theo khoang (Range Search)\n";
        cout << "9. Thong ke Doanh thu & Loi nhuan\n";
        cout << "10. Canh bao mat hang sap het HSD\n";
        cout << "11. Xem Top 3 mat hang doanh thu cao nhat\n";
        cout << "0. Luu & Thoat chuong trinh\n";
        cout << "-------------------------------------------------------\n";
        cout << "Nhap lua chon cua ban (0-11): ";
        
        if (!(cin >> choice)) {
            clearBuffer();
            if (cin.eof()) {
                cout << "\nPhat hien ket thuc luong nhap (EOF). Dang thoat chuong trinh...\n";
                saveToFile(root, filename);
                deleteTree(root);
                choice = 0;
                break;
            }
            cout << "Loi: Vui long nhap so nguyen hop le!\n";
			choice = -1;
            continue;
        }

        switch (choice) {
            case 1: {
                if (!root) cout << "Kho hang hien dang trong!\n";
                else {
                    cout << "\n=== DANH SACH MAT HANG TRONG KHO ===\n";
                    printHeader();
                    displayInOrder(root);
                }
                break;
            }
            case 2: {
                MatHang item;
                cout << "Nhap ma MH (toi da 14 ky tu): "; 
                cin.width(15);
                if (!(cin >> item.maMH)) { clearBuffer(); break; }
                clearBuffer();
                
                if (searchNode(root, item.maMH) != nullptr) {
                    cout << "Loi: Ma mat hang " << item.maMH << " da ton tai!\n";
                    break;
                }

                cout << "Nhap ten MH (<50 ki tu): "; 
                cin.getline(item.tenMH, 50);
                if (cin.fail()) clearBuffer(); 

                cout << "Nhap so luong ton ban dau: "; 
                if (!(cin >> item.soLuongTon) || item.soLuongTon < 0) { 
                    cout << "Loi: So luong ton phai la so nguyen >= 0!\n"; 
                    clearBuffer(); break; 
                }

                cout << "Nhap nguong ton toi thieu: "; 
                if (!(cin >> item.tonToiThieu) || item.tonToiThieu < 0) { 
                    cout << "Loi: Ton toi thieu phai >= 0!\n"; 
                    clearBuffer(); break; 
                }

                cout << "Nhap gia nhap (nghin dong): "; 
                if (!(cin >> item.giaNhap) || item.giaNhap <= 0) { 
                    cout << "Loi: Gia nhap phai > 0!\n"; 
                    clearBuffer(); break; 
                }

                cout << "Nhap gia ban (nghin dong): "; 
                if (!(cin >> item.giaBan) || item.giaBan <= 0) { 
                    cout << "Loi: Gia ban phai > 0!\n"; 
                    clearBuffer(); break; 
                }

                cout << "Nhap so ngay con lai cua HSD: "; 
                if (!(cin >> item.hanSuDungNgay) || item.hanSuDungNgay < 0) { 
                    cout << "Loi: So ngay HSD phai >= 0!\n"; 
                    clearBuffer(); break; 
                }

                item.doanhThu = 0.0;
               item.giaVonHangBan = 0.0; // Chua ban gi nen gia von hang ban = 0
                root = insertNode(root, item);
                cout << "Them mat hang thanh cong!\n";
                break;
            }
            case 3: {
                char ma[15];
                cout << "Nhap ma MH can sua: "; cin.width(15); 
                if (!(cin >> ma)) { clearBuffer(); break; }
                clearBuffer();

                NodeAVL* target = searchNode(root, ma);
                if (!target) {
                    cout << "Khong tim thay mat hang co ma: " << ma << "\n";
                } else {
                    string input;

                    // 1. Sua Ten
                    cout << "Nhap ten MH moi (bo trong neu giu nguyen): "; 
                    getline(cin, input);
                    if (!input.empty()) {
                        strncpy(target->data.tenMH, input.c_str(), sizeof(target->data.tenMH) - 1);
                        target->data.tenMH[sizeof(target->data.tenMH) - 1] = '\0';
                    }

                    // 2. Sua Gia Nhap
                    cout << "Nhap gia nhap moi (bo trong neu giu nguyen): ";
                    getline(cin, input);
                    if (!input.empty()) {
                        try { 
                            double val = stod(input);
                            if (val > 0) target->data.giaNhap = val;
                            else cout << "-> Gia nhap phai > 0! Giu nguyen gia tri cu.\n";
                        } catch (...) { cout << "-> Nhap sai kieu so! Giu nguyen gia tri cu.\n"; }
                    }

                    // 3. Sua Gia Ban
                    cout << "Nhap gia ban moi (bo trong neu giu nguyen): ";
                    getline(cin, input);
                    if (!input.empty()) {
                        try { 
                            double val = stod(input);
                            if (val > 0) target->data.giaBan = val;
                            else cout << "-> Gia ban phai > 0! Giu nguyen gia tri cu.\n";
                        } catch (...) { cout << "-> Nhap sai kieu so! Giu nguyen gia tri cu.\n"; }
                    }

                    // 4. Sua Ton Toi Thieu
                    cout << "Nhap nguong ton toi thieu moi (bo trong neu giu nguyen): ";
                    getline(cin, input);
                    if (!input.empty()) {
                        try { 
                            int val = stoi(input);
                            if (val >= 0) target->data.tonToiThieu = val;
                            else cout << "-> Ton toi thieu phai >= 0! Giu nguyen gia tri cu.\n";
                        } catch (...) { cout << "-> Nhap sai kieu so! Giu nguyen gia tri cu.\n"; }
                    }

                    // 5. Sua HSD
                    cout << "Nhap HSD (ngay) moi (bo trong neu giu nguyen): ";
                    getline(cin, input);
                    if (!input.empty()) {
                        try { 
                            int val = stoi(input);
                            if (val >= 0) target->data.hanSuDungNgay = val;
                            else cout << "-> HSD phai >= 0! Giu nguyen gia tri cu.\n";
                        } catch (...) { cout << "-> Nhap sai kieu so! Giu nguyen gia tri cu.\n"; }
                    }
					cout << "Nhap so luong ton moi (bo trong neu giu nguyen): ";
                    getline(cin, input);
                    if (!input.empty()) {
                        try { 
                            int val = stoi(input);
                            if (val >= 0) target->data.soLuongTon = val;
                            else cout << "-> So luong ton phai >= 0! Giu nguyen gia tri cu.\n";
                        } catch (...) { cout << "-> Nhap sai kieu so! Giu nguyen gia tri cu.\n"; }
                    }

                    cout << "Cap nhat thong tin thanh cong!\n";
                }
                break;
            }
            case 4: {
                char ma[15];
                cout << "Nhap ma MH can xoa: "; cin.width(15); 
                if (!(cin >> ma)) { clearBuffer(); break; }
                clearBuffer();

                if (!searchNode(root, ma)) cout << "Khong tim thay ma MH de xoa!\n";
                else {
                    root = deleteNode(root, ma);
                    cout << "Xoa mat hang thanh cong!\n";
                }
                break;
            }
            case 5: {
                char ma[15]; int sl;
                cout << "Nhap ma MH can nhap kho: "; cin.width(15); 
                if (!(cin >> ma)) { clearBuffer(); break; }
                clearBuffer();

                NodeAVL* node = searchNode(root, ma);
                if (!node) {
                    cout << "Khong tim thay mat hang!\n";
                } else {
                    cout << "Nhap so luong nhap them: "; 
                    if (!(cin >> sl)) { cout << "Loi: Nhap sai kieu so!\n"; clearBuffer(); break; }

                    if (sl > 0) {
                        node->data.soLuongTon += sl;
                        cout << "Nhap kho thanh cong. So luong ton moi: " << node->data.soLuongTon << "\n";
                    } else cout << "So luong nhap phai lon hon 0!\n";
                }
                break;
            }
            case 6: {
                char ma[15]; int sl;
                cout << "Nhap ma MH xuat kho (ban hang): "; cin.width(15); 
                if (!(cin >> ma)) { clearBuffer(); break; }
                clearBuffer();

                NodeAVL* node = searchNode(root, ma);
                if (!node) {
                    cout << "Khong tim thay mat hang!\n";
                } else {
                    cout << "Nhap so luong xuat ban: "; 
                    if (!(cin >> sl)) { cout << "Loi: Nhap sai kieu so!\n"; clearBuffer(); break; }

                    if (sl <= 0) cout << "So luong xuat phai lon hon 0!\n";
                    else if (sl > node->data.soLuongTon) 
                        cout << "Loi: So luong ton kho khong du ban! (Hien co: " << node->data.soLuongTon << ")\n";
                    else {
                        node->data.soLuongTon -= sl;
                        double giaTriBan = sl * node->data.giaBan;
                        node->data.doanhThu += giaTriBan;
						node->data.giaVonHangBan += sl * node->data.giaNhap; // Gia von cua so hang vua ban
                        cout << "Xuat kho thanh cong! Doanh thu cong them: " << fixed << setprecision(0) << giaTriBan << " VND\n";
                        cout.unsetf(ios::fixed);
                        cout << setprecision(6);
                    }
                }
                break;
            }
            case 7: {
                cout << "\n=== CANH BAO MAT HANG CAN NHAP THEM ===\n";
                bool found = false; printHeader();
                canhBaoTonKho(root, found);
                if (!found) cout << "Khong co mat hang nao o muc canh bao.\n";
                break;
            }
            case 8: {
                int subChoice;
                cout << "\n--- CHON KIEU TRA CUU PHAM VI (RANGE SEARCH) ---\n";
                cout << "1. Tra cuu theo Khoang MA MAT HANG (Cat nganh AVL chuan)\n";
                cout << "2. Tra cuu theo Khoang GIA BAN\n";
                cout << "Chon (1-2): ";
                
                if (!(cin >> subChoice)) {
                    cout << "Loi: Nhap sai kieu so!\n";
                    clearBuffer();
                    break;
                }

                if (subChoice == 1) {
                    char minMa[15], maxMa[15];
                    cout << "Nhap Ma MH bat dau: "; cin.width(15);
                    if (!(cin >> minMa)) { clearBuffer(); break; }
                    clearBuffer();

                    cout << "Nhap Ma MH ket thuc: "; cin.width(15);
                    if (!(cin >> maxMa)) { clearBuffer(); break; }
                    clearBuffer();

                    if (strcmp(minMa, maxMa) > 0) {
                        cout << "Loi: Ma bat dau phai nho hon hoac bang Ma ket thuc!\n";
                        break;
                    }

                    cout << "\n=== KET QUA TRA CUU PHAM VI MA MH [" << minMa << " - " << maxMa << "] ===\n";
                    bool found = false; printHeader();
                    rangeSearchMaMH(root, minMa, maxMa, found);
                    if (!found) cout << "Khong tim thay mat hang nao trong pham vi ma nay.\n";

                } else if (subChoice == 2) {
                    double minG, maxG;
                    cout << "Nhap gia ban thap nhat: "; 
                    if (!(cin >> minG)) { cout << "Loi: Nhap sai kieu so!\n"; clearBuffer(); break; }

                    cout << "Nhap gia ban cao nhat: "; 
                    if (!(cin >> maxG)) { cout << "Loi: Nhap sai kieu so!\n"; clearBuffer(); break; }

                    if (minG > maxG) {
                        cout << "Loi: Gia thap nhat phai nho hon hoac bang gia cao nhat!\n";
                        break;
                    }

                    cout << "\n=== KET QUA TRA CUU KHOANG GIA [" << minG << " - " << maxG << "] ===\n";
                    bool found = false; printHeader();
                    rangeSearchGiaBan(root, minG, maxG, found);
                    if (!found) cout << "Khong tim thay mat hang nao trong khoang gia nay.\n";
                } else {
                    cout << "Lua chon khong hop le!\n";
                }
                break;
            }
            case 9: {
                double tongDT = 0.0, tongGiaVon = 0.0;
              thongKeTaiChinh(root, tongDT, tongGiaVon);
               double loiNhuan = tongDT - tongGiaVon; // Loi nhuan = Doanh thu - Gia von hang da ban

                 cout << "\n=== THONG KE LOI NHUAN SIEU THI ===\n";
                cout << fixed << setprecision(0);
                cout << "1. Tong Doanh Thu           : " << tongDT << " VND\n";
               cout << "2. Tong Gia Von Hang Da Ban : " << tongGiaVon << " VND\n";
                cout << "---------------------------------------------------\n";
                if (loiNhuan >= 0) {
                     cout << "-> LOI NHUAN: +" << loiNhuan << " VND (Loi)\n";
                } else {
                   cout << "-> LOI NHUAN: " << loiNhuan << " VND (Lo)\n";
                }
                cout.unsetf(ios::fixed);
                cout << setprecision(6);
                break;
            }
            case 10: {
                int ngat;
                cout << "Nhap nguong ngay HSD can canh bao (vd: 30 ngay): "; 
                if (!(cin >> ngat)) { cout << "Loi: Nhap sai kieu so!\n"; clearBuffer(); break; }

                cout << "\n=== CANH BAO MAT HANG CAP BEN HET HAN SU DUNG ===\n";
                bool found = false; printHeader();
                canhBaoHanSuDung(root, ngat, found);
                if (!found) cout << "Khong co mat hang nao sap het han su dung.\n";
                break;
            }
            case 11: topDoanhThu(root); break;
            case 0: {
                saveToFile(root, filename);
                deleteTree(root);
                cout << "Cam on ban da su dung phan mem!\n";
                break;
            }
            default: cout << "Lua chon khong hop le. Vui long chon lai!\n";
        }
    } while (choice != 0);

  return 0;
}

