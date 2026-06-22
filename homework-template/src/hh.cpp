#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <ctime>

using namespace std;

// ============================
// Permutation
// ============================
template <class T>
void Permute(T *a, int n) {
    for (int i = n; i >= 2; i--) {
        int j = rand() % i + 1;
        swap(a[i], a[j]);
    }
}

// ============================
// Insertion Sort
// ============================
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

// ============================
// Quick Sort (Median of Three)
// ============================
template <class T>
int Partition(T *a, int low, int high) {
    int mid = (low + high) / 2;

    if (a[low] > a[mid]) swap(a[low], a[mid]);
    if (a[low] > a[high]) swap(a[low], a[high]);
    if (a[mid] > a[high]) swap(a[mid], a[high]);

    swap(a[low], a[mid]);
    T pivot = a[low];

    int i = low, j = high + 1;

    while (true) {
        do { i++; } while (i <= high && a[i] < pivot);
        do { j--; } while (a[j] > pivot);

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

void QuickSortWrapper(int *a, int n) {
    QuickSort(a, 1, n);
}

// ============================
// Iterative Merge Sort
// ============================
template <class T>
void Merge(T *a, T *b, int l, int m, int r) {
    int i = l, j = m + 1, k = l;

    while (i <= m && j <= r) {
        if (a[i] <= a[j]) b[k++] = a[i++];
        else b[k++] = a[j++];
    }

    while (i <= m) b[k++] = a[i++];
    while (j <= r) b[k++] = a[j++];

    for (int x = l; x <= r; x++)
        a[x] = b[x];
}

template <class T>
void IterativeMergeSort(T *a, int n) {
    T *b = new T[n + 1];

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
// Heap Sort
// ============================
template <class T>
void Adjust(T *a, int i, int n) {
    T temp = a[i];
    int child = 2 * i;

    while (child <= n) {
        if (child < n && a[child] < a[child + 1])
            child++;

        if (temp >= a[child])
            break;

        a[i] = a[child];
        i = child;
        child = 2 * i;
    }

    a[i] = temp;
}

template <class T>
void HeapSort(T *a, int n) {
    for (int i = n / 2; i >= 1; i--)
        Adjust(a, i, n);

    for (int i = n - 1; i >= 1; i--) {
        swap(a[1], a[i + 1]);
        Adjust(a, 1, i);
    }
}

// ============================
// Runtime Measurement
// ============================
double GetRuntime(void (*sortFunc)(int*, int), int *data, int n) {
    auto start = chrono::high_resolution_clock::now();
    sortFunc(data, n);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> diff = end - start;
    return diff.count();
}

// ============================
// Main
// ============================
int main() {
    srand(time(0));

    int test_n[] = {500, 1000, 2000, 3000, 4000, 5000};

    cout << fixed << setprecision(6);
    cout << "n\tInsertion\tQuick\t\tMerge\t\tHeap\n";

    for (int n : test_n) {
        int *data = new int[n + 1];

        // ================= Insertion (worst case)
        for (int i = 1; i <= n; i++)
            data[i] = n - i + 1;

        double t_ins = GetRuntime(InsertionSort, data, n);

        // ================= Quick / Merge / Heap (random worst observed)
        double t_quick = 0, t_merge = 0, t_heap = 0;

        for (int k = 0; k < 5; k++) {
            for (int i = 1; i <= n; i++)
                data[i] = i;

            Permute(data, n);

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
