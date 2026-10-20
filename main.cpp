#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <limits>
using namespace std;

// 1. CAU TRUC DU LIEU MAT HANG
struct MatHang {
    char maMH[15];          // Ma mat hang (Khoa chinh)
    char tenMH[50];         // Ten mat hang
    int soLuongTon;         // So luong ton kho hien tai
    int tonToiThieu;        // Nguong ton kho toi thieu
    double giaNhap;         // Gia nhap vao
    double giaBan;          // Gia ban ra
    double doanhThu;        // Tong doanh thu tich luy tu ban hang
    double tongChiPhiNhap;  // Tong chi phi von da chi ra de nhap hang
    int hanSuDungNgay;      // So ngay con lai cua han su dung

    MatHang() {
        maMH[0] = '\0';
        tenMH[0] = '\0';
        soLuongTon = 0;
        tonToiThieu = 0;
        giaNhap = 0.0;
        giaBan = 0.0;
        doanhThu = 0.0;
        tongChiPhiNhap = 0.0;
        hanSuDungNgay = 0;
    }
};





// Phan Ngoc Quy up code CAU TRUC NUT CAY AVL
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















// Minh Thien gui bai Cac ham bo tro AVL
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

// Ngoc Quy up BON PHEP QUAY CAN BANG CAY AVL (LL, RR, LR, RL)
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








































//Minh Thien gui phan cac thao tac tren cay AVL
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



// Phu Quoc nop THAO TAC DOC / GHI FILE NHI PHAN
void writeNodeToFile(NodeAVL* node, ofstream& outFile) {
    if (!node) return;
    outFile.write(node->data.maMH, sizeof(node->data.maMH));
    outFile.write(node->data.tenMH, sizeof(node->data.tenMH));
    outFile.write(reinterpret_cast<char*>(&node->data.soLuongTon), sizeof(int));
    outFile.write(reinterpret_cast<char*>(&node->data.tonToiThieu), sizeof(int));
    outFile.write(reinterpret_cast<char*>(&node->data.giaNhap), sizeof(double));
    outFile.write(reinterpret_cast<char*>(&node->data.giaBan), sizeof(double));
    outFile.write(reinterpret_cast<char*>(&node->data.doanhThu), sizeof(double));
    outFile.write(reinterpret_cast<char*>(&node->data.tongChiPhiNhap), sizeof(double));
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
        inFile.read(reinterpret_cast<char*>(&item.tongChiPhiNhap), sizeof(double));
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






//Ung Gia Hao nop hien thi du lieu và tinh nang nghiep vu
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

void thongKeTaiChinh(NodeAVL* root, double& tongDoanhThu, double& tongVon) {
    if (!root) return;
    thongKeTaiChinh(root->left, tongDoanhThu, tongVon);
    tongDoanhThu += root->data.doanhThu;
    tongVon += root->data.tongChiPhiNhap;
    thongKeTaiChinh(root->right, tongDoanhThu, tongVon);
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






int main() {
	
	
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
        
  return 0;
}

