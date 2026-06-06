```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>

using namespace std;

// =========================================================================
// 0. 工具函式：隨機排列生成器 (Program 7.20)
// =========================================================================
template <class T>
void Permute(T *a, int n) {
    // 從陣列尾端 (n) 開始向前迭代到 2
    for (int i = n; i >= 2; i--) {
        // rand() % i 產生 [0, i-1] 的餘數，加 1 後對應到 [1, i] 範圍內的隨機整數索引
        int j = rand() % i + 1; 
        // 將當前的最後一個元素 a[i] 與隨機挑選出的 a[j] 交換，達成洗牌 (Shuffle) 效果
        swap(a[j], a[i]);
    }
}

// (a) 插入排序 (Insertion Sort) - 時間複雜度 Worst-case: O(n^2)

template <class T>
void InsertionSort(T *a, int n) {
    // 從第 2 個元素開始，逐一將後方元素插入到前方已排序的數組中
    for (int i = 2; i <= n; i++) {
        T temp = a[i];  // 備份目前準備進行插入的元素目標值
        int j = i - 1;  // 指針 j 初始化為已排序部分的最後一個位置 (即當前位置的前一格)
        
        // 當指標尚未超出邊界(>=1) 且 前方的元素大於當前目標值時，進行向後平移
        while (j >= 1 && a[j] > temp) {
            a[j + 1] = a[j]; // 將較大的元素向後挪動一格，騰出插入空間
            j--;             // 指標繼續向前推進
        }
        a[j + 1] = temp; // 找到正確的插入空位，將暫存的目標值填入
    }
}

// (b) 快速排序 (Quick Sort) - 使用三數取中值法 (Median-of-Three)

template <class T>
int Partition(T *a, int low, int high) {
    int mid = low + (high - low) / 2; // 計算數組的中心點索引，防止數值溢位
    
    // 三數取中值邏輯：比較 low, mid, high 三個位置的元素大小，並重新進行有序排序
    if (a[low] > a[mid]) swap(a[low], a[mid]);   // 確保 low 不大於 mid
    if (a[low] > a[high]) swap(a[low], a[high]); // 確保 low 不大於 high
    if (a[mid] > a[high]) swap(a[mid], a[high]); // 確保 mid 不大於 high
    
    // 此時 mid 位置的值必為三數中的「中位數」，將其與 a[low] 交換，以便作為後續的 Pivot
    swap(a[low], a[mid]);

    T pivot = a[low];  // 將剛才選出的中位數定為基準點 Pivot 值
    int i = low;       // 初始化左指針 i
    int j = high + 1;  // 初始化右指針 j (因為後面會先做 --j，故設為 high + 1)
    
    while (true) {
        // 左指針 i 先右移並跳過小於 pivot 的元素；若撞到 high 邊界則強制停止
        while (a[++i] < pivot) if (i == high) break;
        // 右指針 j 先左移並跳過大於 pivot 的元素
        while (a[--j] > pivot);
        
        // 若左右指針相遇或發生交叉，代表本輪 Partition 掃描完成，跳出無窮迴圈
        if (i >= j) break;
        
        // 否則，將左邊偏大與右邊偏小的異常元素進行對調
        swap(a[i], a[j]);
    }
    swap(a[low], a[j]); // 將位於 low 的基準點 Pivot 交換到交叉點 j 的位置，使 Pivot 正確定位
    return j;           // 回傳 Pivot 的最終邊界索引位置，做為下次切分依據
}

template <class T>
void QuickSort(T *a, int low, int high) {
    // 遞迴終止條件：當區間長度大於 1 時才繼續進行切分排序
    if (low < high) {
        int j = Partition(a, low, high); // 進行劃分，並取得基準點定位後的索引
        QuickSort(a, low, j - 1);        // 對基準點左側的子數組進行遞迴排序
        QuickSort(a, j + 1, high);       // 對基準點右側的子數組進行遞迴排序
    }
}


// (c) 迭代版合併排序 (Iterative Merge Sort) - 消除遞迴懲罰

template <class T>
void Merge(T *a, T *b, int l, int m, int r) {
    int i = l;      // 左子陣列起始索引 (l 到 m)
    int j = m + 1;  // 右子陣列起始索引 (m+1 到 r)
    int k = l;      // 輔助陣列 b 的寫入指標
    
    // 當左右子陣列皆尚未讀取完畢時，進行雙向排序比較
    while (i <= m && j <= r) {
        if (a[i] <= a[j]) b[k++] = a[i++]; // 左邊小，將左邊元素拉入輔助陣列
        else b[k++] = a[j++];              // 右邊小，將右邊元素拉入輔助陣列
    }
    
    // 若左側陣列尚有剩餘元素，將其依序搬移至輔助陣列尾端
    while (i <= m) b[k++] = a[i++];
    // 若右側陣列尚有剩餘元素，將其依序搬移至輔助陣列尾端
    while (j <= r) b[k++] = a[j++];
}

template <class T>
void IterativeMergeSort(T *a, int n) {
    T *b = new T[n + 1]; // 動態配置一個大小相同的輔助陣列 b (1-based，長度為 n+1)
    
    // sz 代表當前合併子區間的長度，由 1 開始，每次乘以 2 進行 Bottom-Up 迭代
    for (int sz = 1; sz < n; sz *= 2) {
        // 以 2*sz 為步長跨步，依序對相鄰的子數組進行兩兩合併
        for (int low = 1; low <= n - sz; low += sz * 2) {
            int mid = low + sz - 1;                // 計算左子數組的末端索引
            int high = min(low + sz * 2 - 1, n);   // 計算右子數組的右邊界，若超過 n 則取 n 進行截斷
            Merge(a, b, low, mid, high);          // 呼叫 Merge 將範圍內的子陣列進行排序合併至 b
        }
        // 在當前階段 (當前 sz 步長下) 所有區間合併結束後，將輔助陣列 b 的排序結果同步回原陣列 a
        for (int i = 1; i <= n; i++) a[i] = b[i];
    }
    delete[] b; // 釋放動態記憶體，防止記憶體洩漏 (Memory Leak)
}


// (d) 堆積排序 (Heap Sort) - 理論穩定最壞情況上界 O(n log n)

template <class T>
void Adjust(T *a, int i, int n) {
    T j = a[i];       // 暫存當前欲向下調整之節點的值
    int itemp = 2 * i; // itemp 指向當前節點 i 的左子節點 (1-based: 2*i)
    
    // 當左子節點索引仍在有效陣列範圍 n之內時，持續向下滲透調整
    while (itemp <= n) {
        // 若右子節點存在 (itemp < n) 且 右子節點的值大於左子節點
        if (itemp < n && a[itemp] < a[itemp + 1]) itemp++; // 將指標移至較大的右子節點上
        
        // 如果原本暫存的根節點值 j 已經大於或等於當前最大子節點的值，代表結構已符合最大堆積，停止調整
        if (j >= a[itemp]) break;
        
        // 否則，將較大的子節點移上來填補父親位置
        a[itemp / 2] = a[itemp];
        itemp *= 2; // 指標向下推移，準備去檢查該子節點的下一層子樹
    }
    a[itemp / 2] = j; // 將一開始備份的目標值 j 填入最終落腳的平衡空位
}

template <class T>
void HeapSort(T *a, int n) {
    // 階段一：建構最大堆積 (Build Heap)
    // 從最後一個擁有子節點的非葉子節點 (n/2) 開始，由下至上逐一呼叫 Adjust
    for (int i = n / 2; i >= 1; i--) Adjust(a, i, n);
    
    // 階段二：排序輸出 (Extract Max & Sort)
    // 逐一將位於堆積頂端的最大值 (a[1]) 與當前未排序部分的最後一個元素交換，再重新平衡剩餘堆積
    for (int i = n - 1; i >= 1; i--) {
        swap(a[1], a[i + 1]); // 將堆積頂端最大值與未排序部分的尾端元素進行對調
        Adjust(a, 1, i);      // 剩餘堆積規模縮小為 i，並對根節點進行向下調整維持最大堆積特性
    }
}


// (e) 複合排序函式 (Composite Sorting Function)

template <class T>
void CompositeSort(T *a, int n) {
    // 當資料規模極小 (n <= 20) 時，使用常數項極小、且就地工作的 Insertion Sort 效果最好
    if (n <= 20) {
        InsertionSort(a, n);
    } else {
        // 當數據量較大時，切換為高效率的快速排序
        QuickSort(a, 1, n);
    }
}

// 測試與時間量測函數

double GetRuntime(void (*sortFunc)(int*, int), int *data, int n) {
    auto start = chrono::high_resolution_clock::now(); // 擷取高解析度系統起始時間點
    sortFunc(data, n);                                  // 執行指定的排序演算法
    auto end = chrono::high_resolution_clock::now();   // 擷取演算法執行完畢後的時間點
    chrono::duration<double> diff = end - start;       // 計算時間差 (單位為秒)
    return diff.count();                               // 回傳秒數浮點數結果
}

int main() {
    int test_n[] = {500, 1000, 2000, 3000, 4000, 5000}; // 定義實驗要求量測的六大資料規模大小
    
    cout << fixed << setprecision(6); // 設定標準輸出流格式固定顯示至小數點後 6 位
    cout << "n\tInsertion\tQuick\t\tMerge\t\tHeap" << endl;

    for (int n : test_n) {
        int *data = new int[n + 1]; // 依據 1-based index 規範配置長度為 n + 1 的動態數組

        // 1. Insertion Sort 最壞情況資料生成與量測 (生成完全逆序序列: n 到 1)
        for (int i = 1; i <= n; i++) data[i] = n - i + 1;
        double t_ins = GetRuntime(InsertionSort, data, n); // 量測插入排序耗時

        // 2. Heap Sort 最壞情況探討 (透過重複 10 次隨機洗牌並取其中的最大執行時間值)
        double t_heap_max = 0;
        for (int k = 0; k < 10; k++) {
            for (int i = 1; i <= n; i++) data[i] = i; // 初始化順序陣列
            Permute(data, n);                         // 調用 Permute 洗牌函式打亂排列
            double t = GetRuntime(HeapSort, data, n); // 量測當前隨機分佈下的 Heap Sort 執行時間
            if (t > t_heap_max) t_heap_max = t;       // 記錄 10 次嘗試中的最大時間值，作為最壞情況近似
        }
        
        // 格式化輸出各演算法在資料規模 n底下的最壞情況執行效能數據
        cout << n << "\t" << t_ins << "\t" << "..." << "\t\t" << "..." << "\t\t" << t_heap_max << endl;

        delete[] data; // 釋放動態記憶體，重置下一輪規模的空間配置
    }

    return 0;
}
