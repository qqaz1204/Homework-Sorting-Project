#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>

using namespace std;

// --- 1. 隨機排列生成器 (根據 Program 7.20) ---
template <class T>
void Permute(T *a, int n) {
    for (int i = n; i >= 2; i--) {
        int j = rand() % i + 1; // j 為 [1, i] 範圍內的隨機整數
        swap(a[j], a[i]);
    }
}

// --- 2. 排序演算法實作 ---

// (a) 插入排序 (Insertion Sort)
template <class T>
void InsertionSort(T *a, int n) {
    for (int i = 2; i <= n; i++) {
        T temp = a[i];
        int j = i - 1;
        while (j >= 1 && a[j] > temp) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = temp;
    }
}

// (b) 快速排序 (Quick Sort) - 使用三數取中值法
template <class T>
int Partition(T *a, int low, int high) {
    int mid = low + (high - low) / 2;
    // 三數取中值並移動到 low
    if (a[low] > a[mid]) swap(a[low], a[mid]);
    if (a[low] > a[high]) swap(a[low], a[high]);
    if (a[mid] > a[high]) swap(a[mid], a[high]);
    swap(a[low], a[mid]);

    T pivot = a[low];
    int i = low, j = high + 1;
    while (true) {
        while (a[++i] < pivot) if (i == high) break;
        while (a[--j] > pivot);
        if (i >= j) break;
        swap(a[i], a[j]);
    }
    swap(a[low], a[j]);
    return j;
}

template <class T>
void QuickSort(T *a, int low, int high) {
    if (low < high) {
        int j = Partition(a, low, high);
        QuickSort(a, low, j - 1);
        QuickSort(a, j + 1, high);
    }
}

// (c) 迭代版合併排序 (Iterative Merge Sort)
template <class T>
void Merge(T *a, T *b, int l, int m, int r) {
    int i = l, j = m + 1, k = l;
    while (i <= m && j <= r) {
        if (a[i] <= a[j]) b[k++] = a[i++];
        else b[k++] = a[j++];
    }
    while (i <= m) b[k++] = a[i++];
    while (j <= r) b[k++] = a[j++];
}

template <class T>
void IterativeMergeSort(T *a, int n) {
    T *b = new T[n + 1];
    for (int sz = 1; sz < n; sz *= 2) {
        for (int low = 1; low <= n - sz; low += sz * 2) {
            int mid = low + sz - 1;
            int high = min(low + sz * 2 - 1, n);
            Merge(a, b, low, mid, high);
        }
        for (int i = 1; i <= n; i++) a[i] = b[i];
    }
    delete[] b;
}

// (d) 堆積排序 (Heap Sort)
template <class T>
void Adjust(T *a, int i, int n) {
    T j = a[i];
    int itemp = 2 * i;
    while (itemp <= n) {
        if (itemp < n && a[itemp] < a[itemp + 1]) itemp++;
        if (j >= a[itemp]) break;
        a[itemp / 2] = a[itemp];
        itemp *= 2;
    }
    a[itemp / 2] = j;
}

template <class T>
void HeapSort(T *a, int n) {
    for (int i = n / 2; i >= 1; i--) Adjust(a, i, n);
    for (int i = n - 1; i >= 1; i--) {
        swap(a[1], a[i + 1]);
        Adjust(a, 1, i);
    }
}

// --- 3. 複合排序函數 (Composite Function) ---
// 根據實驗結果決定臨界點，此處假設臨界點為 20
template <class T>
void CompositeSort(T *a, int n) {
    if (n <= 20) {
        InsertionSort(a, n);
    } else {
        QuickSort(a, 1, n);
    }
}

// --- 4. 數據生成與計時框架 ---

double GetRuntime(void (*sortFunc)(int*, int), int *data, int n) {
    auto start = chrono::high_resolution_clock::now();
    sortFunc(data, n);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;
    return diff.count();
}

int main() {
    int test_n[] = {500, 1000, 2000, 3000, 4000, 5000};
    
    cout << fixed << setprecision(6);
    cout << "n\tInsertion\tQuick\t\tMerge\t\tHeap" << endl;

    for (int n : test_n) {
        int *data = new int[n + 1];

        // 插入排序最差情況：遞減數列
        for (int i = 1; i <= n; i++) data[i] = n - i + 1;
        double t_ins = GetRuntime(InsertionSort, data, n);

        // 堆積排序最差情況近似：隨機排列多次取最大值
        double t_heap_max = 0;
        for (int k = 0; k < 10; k++) {
            for (int i = 1; i <= n; i++) data[i] = i;
            Permute(data, n);
            double t = GetRuntime(HeapSort, data, n);
            if (t > t_heap_max) t_heap_max = t;
        }

        // 快速與合併排序此處簡化處理，實際作業需按 2e.png 邏輯生成
        // ... (省略部分生成細節)

        cout << n << "\t" << t_ins << "\t" << "..." << "\t\t" << "..." << "\t\t" << t_heap_max << endl;

        delete[] data;
    }

    return 0;
}
