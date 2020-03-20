
//排序和查找常见算法
#include "CommonSort"


/**************************************常见的排序算法**************************************/
//1、快速排序  时间复杂度：O(n^2)，最优时间复杂度：O(nlogn),平均时间复杂度：O(nlogn)
static void Partition(char *pdata, uint8_t left, uint8_t right)
{
	uint8_t i, j;
	char temp, t;
	if (left > right)
	{
		return;
	}
	temp = pdata[left];
	i = left;
	j = right;
	while (i < j)
	{
		while (pdata[j] >= temp && i<j) j--;
		while (pdata[i] <= temp && i<j) i++;

		if (i < j)
		{
			t = pdata[i];
			pdata[i] = pdata[j];
			pdata[j] = t;
		}
	}
	if (left == i)
	{
		left = i + 1;
		Partition(pdata, left+1, right);
		return;
	}
	else
	{
		pdata[left] = pdata[i];
		pdata[i] = temp;
		Partition(pdata, left, i - 1);
	}
	//左边全部排好了
	Partition(pdata, i + 1, right);
	return;
}

void QuickSort(char *pdata, uint8_t len)
{
	Partition(pdata, 0, len -1);
}
//2、归并排序 时间复杂度：O(nlogn)，最优时间复杂度：O(n),平均时间复杂度：O(nlogn),空间复杂度O(n)
void MergeSort(char *k, uint8_t n)
{
	int16_t i, next, left_min, left_max, right_min, right_max;
	char *temp = (char *)malloc(n * sizeof(char));
	for (i = 1; i<n; i *= 2)
	{
		for (left_min = 0; left_min<n - i; left_min = right_max)
		{
			right_min = left_max = left_min + i;
			right_max = left_max + i;
			if (right_max>n)
			{
				right_max = n;
			}
			next = 0;
			while (left_min<left_max&&right_min<right_max)
			{
				if (k[left_min] < k[right_min])
				{
					temp[next++] = k[left_min++];

				}
				else
				{
					temp[next++] = k[right_min++];
				}
			}
			while (left_min < left_max)
			{
				k[--right_min] = k[--left_max];
			}
			while (next>0)
			{
				k[--right_min] = temp[--next];
			}
		}
	}
}
//3、堆排序 时间复杂度：O(nlogn)，最优时间复杂度：O(nlogn),平均时间复杂度：O(nlogn)
static void sink(char *pdata, uint8_t k, uint8_t n)
{
	char temp;
	uint16_t j;
	while (2 * k <= n)
	{
		j = 2 * k;
		if (j < n && pdata[j - 1] < pdata[j])
		{
			j++;
		}
		if (pdata[k - 1] >= pdata[j - 1])
		{
			break;
		}
		temp = pdata[k - 1];
		pdata[k - 1] = pdata[j - 1];
		pdata[j - 1] = temp;
		k = j;
	}
}

void heapSort(char *pdata, uint8_t len)
{
	char temp;
	uint8_t k;
	for (k = len / 2; k >= 1; k--) {
		sink(pdata, k, len);
	}
	while (len > 0) {
		temp = pdata[0];
		pdata[0] = pdata[len - 1];
		pdata[len - 1] = temp;
		len--;
		sink(pdata, 1, len);
	}
}
//4、选择排序 时间复杂度：O(n^2)，最优时间复杂度：O(n^2),平均时间复杂度：O(n^2)
void selectionSort(char *pdata, uint8_t len)
{
	uint8_t i,j,min;
    char temp;
    for (i = 0; i < len; i++) {
        min = i;
        for (j = i + 1; j < len; j++) {
            if (pdata[j] < pdata[min]) {
                min = j;
            }
        }
        temp = pdata[min];
        pdata[min] = pdata[i];
        pdata[i] = temp;
    }
}
//5、冒泡排序(按小到大排序) (时间复杂度O(n2) 最优O(n) 空间复杂度O(n))
void bubble_sort(char *pdata, uint8_t len)
{
	char i,j,index;
	uint8_t cache;
	for (i=0; i < len-1; i++)
	{
		index = i;
		for (j=i+1; j < len; j++)
		{
			if (pdata[index] > pdata[j])
			{
				index = j; 
			}
		}
		if (index == i) 
		{
			continue;
		}
		cache = pdata[i];
		pdata[i] = pdata[index];
		pdata[index] = cache;
	}	
}
//插入排序 时间复杂度：O(n^2)，最优时间复杂度：O(n),平均时间复杂度：O(n^2)
void insert_sort(char *pdata, uint8_t len) //原始排序
{
	char cache;
	uint8_t i, j, index;
	for (int i = 1; i < len; i++) {
		for (j = i; j > 0 && pdata[j] < pdata[j - 1]; j--) {
			cache = pdata[j];
			pdata[j] = pdata[j - 1];
			pdata[j - 1] = cache;
		}
	}
}

void insert_sort1(char *pdata, uint8_t len) //优化算法效率
{
	char cache;
	uint8_t i, j;
	for (i = 1; i < len; i++) {
		cache = pdata[i];
		for (j = i; j > 0 && cache < pdata[j - 1]; j--) {
			pdata[j] = pdata[j - 1];
		}
		pdata[j] = cache;
	}
}
//7、希尔排序 时间复杂度：根据步长而不同，最优时间复杂度：O(n),平均时间复杂度：根据步长而不同
void shellSort(char *pdata, uint8_t len)
{
	uint8_t h = 1, i, j;
	char cache;
	while (h < len / 3) {
		h = 3 * h + 1;
	}
	while (h >= 1) {
		for (i = h; i < len; i++) {
			for (j = i; j >= h && pdata[j] < pdata[j - h]; j -= h) {
				cache = pdata[j];
				pdata[j] = pdata[j - h];
				pdata[j - h] = cache;
			}
		}
		h /= 3;
	}
}



/**************************************常见的查找算法**************************************/
//1、顺序查找
int8_t SequenceSearch(char a[], char value, uint8_t max_len)
{
    int8_t i;
    for(i=0; i < max_len; i++)
    {
		if(a[i] == value)
        {
			return i;
		}
	}
    return -1;
}
//2、二分查找
int8_t BinarySearch1(char a[], char value, uint8_t max_len)
{
    uint8_t low, high, mid;
    low = 0;
    high = max_len-1;
    while(low<=high)
    {
        mid = (low+high)/2;
        if(a[mid] == value)
            return mid;
        if(a[mid] > value)
            high = mid-1;
        if(a[mid] < value)
            low = mid+1;
    }
    return -1;
}
//3、插值查找
int InsertionSearch(int a[], int value, int low, int high)
{
    int mid = low+(value-a[low])/(a[high]-a[low])*(high-low);
    if(a[mid]==value)
        return mid;
    if(a[mid]>value)
        return InsertionSearch(a, value, low, mid-1);
    if(a[mid]<value)
        return InsertionSearch(a, value, mid+1, high);
}
//4、斐波那契查找
//5、树表查找
//6、分块查找
//7、哈希查找
