# Quản lý Hàng tồn kho và Doanh thu Siêu thị (Dự án DT05)

Hệ thống quản lý kho hàng và doanh thu siêu thị được viết bằng **C++**, sử dụng **cây cân bằng AVL** và cơ chế lưu trữ dữ liệu bền vững bằng **file nhị phân**.

---

## 1. Mô tả đề tài

- **Tên đề tài:** Quản lý Hàng tồn kho và Doanh thu Siêu thị (Dự án DT05).
- **Cấu trúc dữ liệu cốt lõi:** Sử dụng **cây cân bằng AVL (Adelson-Velsky and Landis Tree)** để tổ chức dữ liệu trong bộ nhớ RAM. Các thao tác tìm kiếm, thêm và xóa có độ phức tạp thời gian trung bình và trong trường hợp cây được cân bằng là \(O(\log N)\).
- **Nghiệp vụ quản lý và cảnh báo:**
  - Quản lý danh mục mặt hàng với các thao tác thêm, xem, sửa và xóa (CRUD).
  - Xử lý nghiệp vụ nhập kho và xuất kho.
  - Tự động cảnh báo khi tồn kho thấp hơn ngưỡng an toàn hoặc mặt hàng sắp hết hạn sử dụng.
  - Tra cứu linh hoạt theo mã mặt hàng hoặc khoảng giá bán.
- **Thống kê tài chính:** Tổng hợp vốn, doanh thu, lợi nhuận và trích xuất **Top 3 mặt hàng có doanh thu cao nhất**.
- **Lưu trữ dữ liệu:** Tuần tự hóa cây dữ liệu xuống file nhị phân `danh_muc_kho.dat`, sử dụng cơ chế kiểm tra định dạng phiên bản `FILE_HEADER_MAGIC` nhằm hỗ trợ kiểm tra tính hợp lệ của dữ liệu khi khởi động lại chương trình.

---

## 2. Công nghệ sử dụng

| Thành phần | Công nghệ / Công cụ | Mục đích |
| :--- | :--- | :--- |
| **Ngôn ngữ lập trình** | C++ | Xây dựng chương trình quản lý |
| **Cấu trúc dữ liệu** | Cây AVL | Tổ chức, tìm kiếm và quản lý dữ liệu mặt hàng |
| **Lưu trữ dữ liệu** | File nhị phân | Lưu và khôi phục dữ liệu |
| **Quản lý mã nguồn** | Git, GitHub | Quản lý phiên bản và phối hợp nhóm |
| **Theo dõi tiến độ** | Trello | Phân công và giám sát công việc |
| **Soạn thảo báo cáo** | Microsoft Word, LaTeX, PDF | Trình bày nội dung học thuật |
| **Biên dịch và trình diễn** | Dev-C++, GCC, GDB Online | Biên dịch, kiểm thử và trình diễn chương trình |

---

## 3. Cấu trúc thư mục dự án

```text
.
├── BTL.cpp                 # Mã nguồn C++ của chương trình
├── BaoCao_BTL_DT05.docx    # Báo cáo bài tập lớn
├── danh_muc_kho.dat        # File dữ liệu nhị phân, được tạo khi chạy
└── README.md               # Tài liệu giới thiệu và hướng dẫn dự án
```

---

## 4. Phân công nhiệm vụ và nhánh Git

| Nhánh Git (Branch) | Thành viên phụ trách | Nhiệm vụ và nội dung công việc |
| :--- | :--- | :--- |
| `TV1-khai-bao-du-lieu-mat-hang-chuong-trinh-chinh` | **Đỗ Quốc Khang**<br>MSSV: 66151060023 | Khai báo `struct MatHang`, `struct NodeAVL`, xây dựng giao diện menu chính `main()`, xử lý bộ đệm và lỗi nhập liệu bằng `clearBuffer`. |
| `TV2-cau-truc-nut-cay-avl-4-phep-quay-cay` | **Phan Ngọc Quý**<br>MSSV: 66151060050 | Xây dựng các hàm hỗ trợ chiều cao và hệ số cân bằng (`getHeight`, `getBalanceFactor`), các phép quay đơn và kép (`rotateRight`, `rotateLeft`) cùng hàm `balanceNode`. |
| `TV3-cac-ham-bo-tro-cac-thao-tac-tren-cay` | **Huỳnh Trần Minh Thiện**<br>MSSV: 66151060056 | Xây dựng các thao tác trên cây AVL: chèn nút (`insertNode`), tìm nút nhỏ nhất (`findMinNode`), xóa nút (`deleteNode`), tìm kiếm (`searchNode`) và duyệt cây In-Order. |
| `TV4-thao-tac-doc-ghi-file-nhi-phan` | **Huỳnh Phú Quốc**<br>MSSV: 66151060072 | Phát triển phân hệ đọc và ghi file nhị phân (`saveToFile`, `writeNodeToFile`, `loadFromFile`) và kiểm tra `FILE_HEADER_MAGIC`. |
| `TV5-hien-thi-du-lieu-va-tinh-nang-nghiep-vu` | **Ưng Gia Hào**<br>MSSV: 66151060015 | Xây dựng các nghiệp vụ nhập và xuất kho, tra cứu theo khoảng mã hoặc giá, cảnh báo tồn kho và hạn sử dụng, thống kê tài chính và trích xuất Top 3 doanh thu. |

---

## 5. Tiến độ và sản phẩm bàn giao

Dự án được tổ chức theo các nhóm công việc sau:

| Mã công việc | Hạng mục | Sản phẩm đầu ra |
| :--- | :--- | :--- |
| **YC01** | Quản lý mã nguồn và kiểm soát phiên bản | Repository GitHub, các nhánh làm việc, lịch sử commit và tài liệu README |
| **YC02** | Quản lý và giám sát tiến độ | Bảng quản lý công việc trên Trello, gồm phân công nhiệm vụ, thời hạn và trạng thái hoàn thành |
| **YC03** | Báo cáo học thuật | Báo cáo bài tập lớn theo yêu cầu môn học, kèm kết quả thực hiện và tài liệu tham khảo |
| **YC04** | Chương trình C++ và trình diễn | Mã nguồn hoàn chỉnh, kết quả kiểm thử, slide trình bày và kịch bản demo |

---

## 6. Hướng dẫn thiết lập, biên dịch và chạy chương trình

### Cách 1: Chạy trực tuyến trên GDB Online

Phù hợp khi muốn chạy thử chương trình mà không cần cài đặt trình biên dịch.

1. Truy cập [GDB Online C++ Compiler](https://www.onlinegdb.com/online_c++_compiler).
2. Chọn ngôn ngữ C++ và chuẩn C++11 hoặc phiên bản tương thích.
3. Mở file `BTL.cpp`, sao chép toàn bộ mã nguồn và dán vào trình soạn thảo.
4. Nhấn **Run** để biên dịch và chạy chương trình.

### Cách 2: Chạy bằng Dev-C++

Nếu phiên bản Dev-C++ đang sử dụng chưa bật chuẩn C++11, hãy thực hiện các bước sau:

1. Mở Dev-C++ và mở file `BTL.cpp`.
2. Chọn **Tools → Compiler Options**.
3. Tìm tùy chọn thêm lệnh cho trình biên dịch.
4. Nhập tham số sau:

   ```text
   -std=c++11
   ```

5. Nhấn **OK**, sau đó chọn **Compile & Run** hoặc nhấn `F11`.

### Cách 3: Biên dịch bằng Terminal hoặc Command Prompt

**Bước 1: Mở Terminal** tại thư mục chứa file `BTL.cpp`.

**Bước 2: Biên dịch chương trình** bằng GCC với chuẩn C++11:

```bash
g++ -std=c++11 BTL.cpp -o quan_ly_kho
```

**Bước 3: Chạy chương trình.**

- **Windows:**

  ```bash
  quan_ly_kho.exe
  ```

- **Linux hoặc macOS:**

  ```bash
  ./quan_ly_kho
  ```

**Lưu ý:** Nếu chương trình cần đọc hoặc ghi file `danh_muc_kho.dat`, hãy chạy chương trình trong thư mục phù hợp để dữ liệu được tạo và truy cập đúng vị trí.

---

## 7. Tác giả và thông tin dự án

- **Tên dự án:** Quản lý Hàng tồn kho và Doanh thu Siêu thị
- **Mã dự án:** DT05
- **Ngôn ngữ:** C++
- **Cấu trúc dữ liệu chính:** Cây cân bằng AVL
- **Nền tảng quản lý mã nguồn:** GitHub

Dự án được thực hiện theo hình thức làm việc nhóm, kết hợp kiến thức về cấu trúc dữ liệu, thuật toán cây AVL, quản lý dữ liệu bằng file nhị phân và quy trình quản lý mã nguồn bằng Git.
