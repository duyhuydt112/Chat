#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    // Xóa phần tử tại chỉ số 2 (phần tử có giá trị 3)
    vec.erase(vec.begin() + 2);

    // In ra kết quả
    for (int num : vec) {
        std::cout << num << " ";
    }

    return 0;
}