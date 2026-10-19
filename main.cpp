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

