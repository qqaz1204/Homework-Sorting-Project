#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <ctime>

using namespace std;

// ============================
// 隨機排列 (打亂陣列)
// ============================
template <class T>
void Permute(T *a, int n) {
    for (int i = n; i >= 2; i--) {
        int j = rand() % i + 1;   // 產生 1 ~ i 的隨機索引
        swap(a[i], a[j]);         // 與第 i 個元素交換
    }
}

// ============================
// 插入排序
// ============================
template <class T>
void InsertionSort(T *a, int n) {
    for (int i = 2; i <= n; i++) {
        T temp = a[i];            // 暫存當前元素
        int j = i - 1;
        while (j >= 1 && a[j] > temp) { // 往前比較並移動
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = temp;          // 插入正確位置
    }
}

// ============================
// 快速排序 (三數取中法)
// ============================
template <class T>
int Partition(T *a, int low, int high) {
    int mid = (low + high) / 2;

    // 三數取中：確保 pivot 是中間大小的值
    if (a[low] > a[mid]) swap(a[low], a[mid]);
    if (a[low] > a[high]) swap(a[low], a[high]);
    if (a[mid] > a[high]) swap(a[mid], a[high]);

    swap(a[low], a[mid]);         // 將 pivot 放到開頭
    T pivot = a[low];

    int i = low, j = high + 1;

    while (true) {
        do { i++; } while (i <= high && a[i] < pivot); // 往右找比 pivot 大的
        do { j--; } while (a[j] > pivot);              // 往左找比 pivot 小的

        if (i >= j) break;        // 指標交錯時停止
        swap(a[i], a[j]);         // 交換
    }

    swap(a[low], a[j]);           // pivot 放到正確位置
    return j;                     // 回傳分割點
}

template <class T>
void QuickSort(T *a, int low, int high) {
    if (low < high) {
        int j = Partition(a, low, high);
        QuickSort(a, low, j - 1); // 左半部遞迴
        QuickSort(a, j + 1, high);// 右半部遞迴
    }
}

void QuickSortWrapper(int *a, int n) {
    QuickSort(a, 1, n);
}

// ============================
// 迭代版合併排序
// ============================
template <class T>
void Merge(T *a, T *b, int l, int m, int r) {
    int i = l, j = m + 1, k = l;

    // 合併兩個已排序子陣列
    while (i <= m && j <= r) {
        if (a[i] <= a[j]) b[k++] = a[i++];
        else b[k++] = a[j++];
    }

    while (i <= m) b[k++] = a[i++]; // 左邊剩餘
    while (j <= r) b[k++] = a[j++]; // 右邊剩餘

    // 複製回原陣列
    for (int x = l; x <= r; x++)
        a[x] = b[x];
}

template <class T>
void IterativeMergeSort(T *a, int n) {
    T *b = new T[n + 1];          // 暫存陣列

    // sz 為子陣列大小，每次倍增
    for (int sz = 1; sz < n; sz *= 2) {
        for (int low = 1; low <= n - sz; low += 2 * sz) {
            int mid = low + sz - 1;
            int high = min(low + 2 * sz - 1, n);
            Merge(a, b, low, mid, high);
        }
    }

    delete[] b;
}

void MergeSortWrapper(int *a, int n) {
    IterativeMergeSort(a, n);
}

// ============================
// 堆積排序
// ============================
template <class T>
void Adjust(T *a, int i, int n) {
    T temp = a[i];
    int child = 2 * i;

    while (child <= n) {
        if (child < n && a[child] < a[child + 1]) // 找較大的子節點
            child++;

        if (temp >= a[child]) // 若父節點已大於子節點，停止
            break;

        a[i] = a[child];      // 父節點下移
        i = child;
        child = 2 * i;
    }

    a[i] = temp;              // 放回正確位置
}

template <class T>
void HeapSort(T *a, int n) {
    // 建立最大堆
    for (int i = n / 2; i >= 1; i--)
        Adjust(a, i, n);

    // 逐一取出最大值
    for (int i = n - 1; i >= 1; i--) {
        swap(a[1], a[i + 1]); // 將堆頂放到尾端
        Adjust(a, 1, i);      // 調整剩餘堆
    }
}

// ============================
// 計算排序執行時間
// ============================
double GetRuntime(void (*sortFunc)(int*, int), int *data, int n) {
    auto start = chrono::high_resolution_clock::now();
    sortFunc(data, n); // 執行排序
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> diff = end - start;
    return diff.count(); // 回傳秒數
}

// ============================
// 主程式
// ============================
int main() {
    srand(time(0)); // 初始化隨機種子

    int test_n[] = {500, 1000, 2000, 3000, 4000, 5000};

    cout << fixed << setprecision(6);
    cout << "n\tInsertion\tQuick\t\tMerge\t\tHeap\n";

    for (int n : test_n) {
        int *data = new int[n + 1];

        // 插入排序測試 (最壞情況：逆序)
        for (int i = 1; i <= n; i++)
            data[i] = n - i + 1;

        double t_ins = GetRuntime(InsertionSort, data, n);

        // 快速 / 合併 / 堆積排序 (隨機排列取最大耗時)
        double t_quick = 0, t_merge = 0, t_heap = 0;

        for (int k = 0; k < 5; k++) {
            for (int i = 1; i <= n; i++)
                data[i] = i;

            Permute(data, n); // 打亂陣列

            int *copy1 = new int[n + 1];
            int *copy2 = new int[n + 1];
            int *copy3 = new int[n + 1];

            for (int i = 1; i <= n; i++) {
                copy1[i] = copy2[i] = copy3[i] = data[i];
            }

            t_quick = max(t_quick, GetRuntime(QuickSortWrapper, copy1, n));
            t_merge = max(t_merge, GetRuntime(MergeSortWrapper, copy2, n));
            t_heap  = max(t_heap,  GetRuntime(HeapSort, copy3, n));

            delete[] copy1;
            delete[] copy2;
            delete[] copy3;
        }

        cout << n << "\t"
             << t_ins << "\t"
             << t_quick << "\t"
             << t_merge << "\t"
             << t_heap << "\n";

        delete[] data;
    }

    return 0;
}
