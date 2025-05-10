#pragma once
#include <iostream>

void PrintArray(int arr[], int size)
{
    for(int i = 0; i < size; i++)
    {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

template<typename T>
void swap(T& a, T& b)
{
    T temp = a;
    a = b;
    b = temp;
}

template<typename T>
T three_way_partition(T arr[], T low, T high)
{
    int mid = (low + high) / 2;
    if (arr[low] > arr[mid])
    {
        if(arr[low] < arr[high])
        {
            return arr[low];
        }
        else if(arr[mid] > arr[high])
        {
            return arr[mid];
        }
        else
        {
            return arr[high];
        }
    }
    else if(arr[mid] > arr[low])
    {
        if(arr[mid] < arr[high])
        {
            return arr[mid];
        }
        else if(arr[high] < arr[low])
        {
            return arr[low];
        }
        else
        {
            return arr[high];
        }
    }
    else
    {
        return arr[mid];
    }
}

template<typename T>
void QuickSort(T arr[], T low, T high)
{
    if(high == low)return;
    T pivot = three_way_partition(arr, low, high);
    int i = low , j = high;
    while(i <= j)
    {
        while(arr[i] < pivot)i++;
        while(arr[j] > pivot)j--;
        if(i <= j)
        {
            swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }
    if(low < j)QuickSort(arr, low, j);
    if(high > i)QuickSort(arr, i, high);
}