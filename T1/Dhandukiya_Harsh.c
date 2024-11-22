#include <stdio.h>
#include <stdlib.h>

int maximum_integer(int *ar2, int size) {
    int max = *ar2;  // Initializing max with the first element
    for(int i = 1; i < size; i++) {
        if (*(ar2 + i) > max) {
            max = *(ar2 + i);
        }
    }
    return max;
}

int main() {
    int n;
    printf("Enter the size of the array you want: ");
    scanf("%d", &n);

    int *arr = (int *)malloc(n * sizeof(int)); // malloc function 
    if (arr == NULL) {  // error handling
        fprintf(stderr, "Memory allocation failed\n");  
        return 1;
    }

    printf("Enter the elements of the array: ");
    for(int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }
    printf("Elements of the array: "); //printing the elements of the array
    for(int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    int largest_element = maximum_integer(arr, n);
    printf("The largest element in the array is: %d\n", largest_element);

    free(arr);
    return 0;
}

