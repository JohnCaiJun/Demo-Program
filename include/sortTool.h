//
// Created by 蔡俊 on 2019-03-19.
//

#ifndef CJLIB_SORTTOOL_H
#define CJLIB_SORTTOOL_H



template <class T>
int getArrayLen(T& arr){
    return (sizeof(arr) / sizeof(arr[0]));
}

void swap(int arr[],int i,int j);
void insertionSort(int arr[], int len);
void selectionSort(int arr[], int len);
void bubbleSort(int arr[], int len);
void printArray(int arr[], int len);

void testBubbleSort(int arr[], int len);
void testSelectSort(int arr[], int len);
void testInsertionSort(int arr[], int len);

void testBubbleSort(int arr[], int len){
    std::cout << "before bubble sort:" << std::endl;
    printArray(arr,len);
    bubbleSort(arr,len);
    std::cout << "after bubble sort:" << std::endl;
    printArray(arr,len);

}
void testSelectSort(int arr[], int len){
    std::cout << "before select sort:" << std::endl;
    printArray(arr,len);
    selectionSort(arr,len);
    std::cout << "after select sort:" << std::endl;
    printArray(arr,len);

}

void testInsertionSort(int arr[], int len){
    std::cout << "before Insertion sort:" << std::endl;
    printArray(arr,len);
    insertionSort(arr,len);
    std::cout << "after Insertion sort:" << std::endl;
    printArray(arr,len);

}

void printArray(int arr[], int len){
    for(int i = 0; i < len; i++){
        std::cout << arr[i];
    }
    std::cout<<std::endl;

}

void swap(int arr[], int i, int j){
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}
void insertionSort(int arr[], int len){
   for(int i = 1; i < len; i++){
       for(int j = i - 1; j >= 0 && arr[j] > arr[j+1]; j--){
          swap(arr, j, j+1);
       }
   }
}

void bubbleSort(int arr[], int len){
    for(int i = len; i > 0; i-- ){
        for(int j = 0; j < i - 1; j++){
           if(arr[j] > arr[j+1])
               swap(arr, j, j+1);
        }
    }

}

void selectionSort(int arr[], int len){
    int min_index;
    for(int i = 0; i < len - 1; i++){
        min_index = i;
        for(int j = i + 1; j < len; j++){
            min_index = arr[j] < arr[min_index] ? j : min_index;
        }
        swap(arr, i, min_index);
    }


}
#endif //CJLIB_SORTTOOL_H
