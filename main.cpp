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
int main() {
  return 0;
}

