#include <iostream>     // 輸入輸出
#include <algorithm>    // 提供 swap()、min()、max()
#include <chrono>       // 高精度計時
#include <iomanip>      // 控制輸出格式
#include <cstdlib>      // rand()、srand()
#include <ctime>        // time()

using namespace std;

/*
====================================================
            隨機排列函式 (Permutation)
====================================================

用途：
將原本已排序的陣列隨機打亂。

原理：
使用 Fisher-Yates Shuffle 演算法。

例如：
1 2 3 4 5

可能變成：

3 5 1 2 4

時間複雜度：
O(n)
*/
template <class T>
void Permute(T *a, int n) {

    // 從最後一個元素開始往前交換
    for (int i = n; i >= 2; i--) {

        // 產生 [1, i] 範圍內的隨機位置
        int j = rand() % i + 1;

        // 將目前位置與隨機位置交換
        swap(a[i], a[j]);
    }
}

/*
====================================================
                插入排序 Insertion Sort
====================================================

概念：

每次取出一個元素，
插入到前面已排序區間中的正確位置。

例如：

5 4 3 2

第一次：
4 插到 5 前面

4 5 3 2

第二次：
3 插到 4 前面

3 4 5 2

依此類推

最差時間複雜度：
O(n²)

空間複雜度：
O(1)
*/
template <class T>
void InsertionSort(T *a, int n) {

    // 從第二個元素開始
    for (int i = 2; i <= n; i++) {

        // 暫存目前要插入的元素
        T temp = a[i];

        // 從前一個位置開始往前找
        int j = i - 1;

        // 將比 temp 大的元素往後移
        while (j >= 1 && a[j] > temp) {
            a[j + 1] = a[j];
            j--;
        }

        // 插入到正確位置
        a[j + 1] = temp;
    }
}

/*
====================================================
            快速排序 Partition
====================================================

使用：

Median-of-Three
（三數取中法）

降低快速排序退化成 O(n²) 的機率。

從：

a[low]
a[mid]
a[high]

找出中間大小的元素作為 pivot。
*/
template <class T>
int Partition(T *a, int low, int high) {

    // 中間位置
    int mid = (low + high) / 2;

    /*
        三數取中

        排序後保證：

        a[low] <= a[mid] <= a[high]
    */
    if (a[low] > a[mid])
        swap(a[low], a[mid]);

    if (a[low] > a[high])
        swap(a[low], a[high]);

    if (a[mid] > a[high])
        swap(a[mid], a[high]);

    /*
        將 pivot 放到最前面
    */
    swap(a[low], a[mid]);

    T pivot = a[low];

    /*
        i 從左往右掃描
        j 從右往左掃描
    */
    int i = low;
    int j = high + 1;

    while (true) {

        /*
            找第一個 >= pivot 的元素
        */
        do {
            i++;
        } while (i <= high && a[i] < pivot);

        /*
            找第一個 <= pivot 的元素
        */
        do {
            j--;
        } while (a[j] > pivot);

        /*
            指標交錯
            分割完成
        */
        if (i >= j)
            break;

        /*
            左邊大的和右邊小的交換
        */
        swap(a[i], a[j]);
    }

    /*
        將 pivot 放到正確位置
    */
    swap(a[low], a[j]);

    /*
        回傳 pivot 最終位置
    */
    return j;
}

/*
====================================================
                Quick Sort
====================================================

分治法 (Divide and Conquer)

步驟：

1. 選 pivot
2. 分割
3. 遞迴排序左半
4. 遞迴排序右半

平均：
O(n log n)

最差：
O(n²)
*/
template <class T>
void QuickSort(T *a, int low, int high) {

    if (low < high) {

        int j = Partition(a, low, high);

        // 排序左半部
        QuickSort(a, low, j - 1);

        // 排序右半部
        QuickSort(a, j + 1, high);
    }
}

/*
    包裝函式

    因為 GetRuntime 要求格式：

    void sort(int*, int)
*/
void QuickSortWrapper(int *a, int n) {
    QuickSort(a, 1, n);
}

/*
====================================================
                    Merge
====================================================

將兩個已排序區間合併

例如：

左：
1 4 8

右：
2 3 9

合併後：

1 2 3 4 8 9
*/
template <class T>
void Merge(T *a, T *b, int l, int m, int r) {

    int i = l;       // 左半部起點
    int j = m + 1;   // 右半部起點
    int k = l;       // 暫存陣列位置

    while (i <= m && j <= r) {

        if (a[i] <= a[j])
            b[k++] = a[i++];
        else
            b[k++] = a[j++];
    }

    // 左半部剩餘元素
    while (i <= m)
        b[k++] = a[i++];

    // 右半部剩餘元素
    while (j <= r)
        b[k++] = a[j++];

    // 複製回原陣列
    for (int x = l; x <= r; x++)
        a[x] = b[x];
}

/*
====================================================
            Iterative Merge Sort
====================================================

非遞迴版合併排序

第一次：

每 1 個元素合併

第二次：

每 2 個元素合併

第三次：

每 4 個元素合併

第四次：

每 8 個元素合併

直到覆蓋整個陣列。

時間複雜度：

O(n log n)

空間複雜度：

O(n)
*/
template <class T>
void IterativeMergeSort(T *a, int n) {

    // 暫存陣列
    T *b = new T[n + 1];

    /*
        sz = 子陣列大小

        1
        2
        4
        8
        ...
    */
    for (int sz = 1; sz < n; sz *= 2) {

        for (int low = 1;
             low <= n - sz;
             low += 2 * sz) {

            int mid = low + sz - 1;

            int high =
                min(low + 2 * sz - 1, n);

            Merge(a, b, low, mid, high);
        }
    }

    delete[] b;
}

void MergeSortWrapper(int *a, int n) {
    IterativeMergeSort(a, n);
}

/*
====================================================
                Heap Adjust
====================================================

用途：

維持 Max Heap 性質

父節點 >= 子節點

如果不符合：

則向下調整。
*/
template <class T>
void Adjust(T *a, int i, int n) {

    T temp = a[i];

    int child = 2 * i;

    while (child <= n) {

        /*
            找較大的子節點
        */
        if (child < n &&
            a[child] < a[child + 1])
            child++;

        /*
            已符合 Max Heap
        */
        if (temp >= a[child])
            break;

        /*
            子節點上移
        */
        a[i] = a[child];

        i = child;

        child = 2 * i;
    }

    /*
        放回正確位置
    */
    a[i] = temp;
}

/*
====================================================
                Heap Sort
====================================================

步驟：

1. 建立 Max Heap
2. 最大值放到最後
3. 重新調整 Heap
4. 重複直到完成

時間複雜度：

O(n log n)

空間複雜度：

O(1)
*/
template <class T>
void HeapSort(T *a, int n) {

    /*
        建立最大堆
    */
    for (int i = n / 2; i >= 1; i--)
        Adjust(a, i, n);

    /*
        每次將最大值放到尾端
    */
    for (int i = n - 1; i >= 1; i--) {

        swap(a[1], a[i + 1]);

        Adjust(a, 1, i);
    }
}

/*
====================================================
            計算排序執行時間
====================================================

利用 chrono 計算：

開始時間
↓
執行排序
↓
結束時間

最後回傳秒數。
*/
double GetRuntime(
    void (*sortFunc)(int*, int),
    int *data,
    int n) {

    auto start =
        chrono::high_resolution_clock::now();

    sortFunc(data, n);

    auto end =
        chrono::high_resolution_clock::now();

    chrono::duration<double> diff =
        end - start;

    return diff.count();
}

/*
====================================================
                    主程式
====================================================

測試：

n =
500
1000
2000
3000
4000
5000

輸出：

Insertion Sort
Quick Sort
Merge Sort
Heap Sort

各自執行時間。
*/
int main() {

    // 初始化亂數種子
    srand(time(0));

    int test_n[] =
    {
        500,
        1000,
        2000,
        3000,
        4000,
        5000
    };

    cout << fixed
         << setprecision(6);

    cout
    << "n\tInsertion\tQuick\t\tMerge\t\tHeap\n";

    for (int n : test_n) {

        /*
            建立測試資料
        */
        int *data = new int[n + 1];

        /*
            插入排序：

            使用逆序資料

            最差情況
        */
        for (int i = 1; i <= n; i++)
            data[i] = n - i + 1;

        double t_ins =
            GetRuntime(
                InsertionSort,
                data,
                n
            );

        /*
            Quick / Merge / Heap

            使用隨機資料

            重複 5 次

            取最大耗時
        */
        double t_quick = 0;
        double t_merge = 0;
        double t_heap = 0;

        for (int k = 0; k < 5; k++) {

            for (int i = 1; i <= n; i++)
                data[i] = i;

            Permute(data, n);

            int *copy1 = new int[n + 1];
            int *copy2 = new int[n + 1];
            int *copy3 = new int[n + 1];

            for (int i = 1; i <= n; i++) {

                copy1[i] = data[i];
                copy2[i] = data[i];
                copy3[i] = data[i];
            }

            t_quick =
                max(
                    t_quick,
                    GetRuntime(
                        QuickSortWrapper,
                        copy1,
                        n
                    )
                );

            t_merge =
                max(
                    t_merge,
                    GetRuntime(
                        MergeSortWrapper,
                        copy2,
                        n
                    )
                );

            t_heap =
                max(
                    t_heap,
                    GetRuntime(
                        HeapSort,
                        copy3,
                        n
                    )
                );

            delete[] copy1;
            delete[] copy2;
            delete[] copy3;
        }

        /*
            輸出結果
        */
        cout
        << n << "\t"
        << t_ins << "\t"
        << t_quick << "\t"
        << t_merge << "\t"
        << t_heap << "\n";

        delete[] data;
    }

    return 0;
}
