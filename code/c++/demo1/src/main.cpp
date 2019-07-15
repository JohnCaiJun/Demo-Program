#include <iostream>
#include <string>
//#include "functions.h"
#include "sortTool.h"

using namespace std;



/*
 * 简单主函数：
 * 测试functions.h函数的功能
 */
int main(){

    int arr[] = {7,4,1,3,2,9,4,3};
    int len = getArrayLen(arr);

    //testBubbleSort(arr,len);
    //testInsertionSort(arr,len);
    testSelectSort(arr,len);

	return 0;
}
