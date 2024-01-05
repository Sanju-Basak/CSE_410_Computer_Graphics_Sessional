#include <iostream>
#include <stack>

// Define a 4x4 matrix type using a 2D array
typedef int Matrix[4][4];

int main() {
    std::stack<Matrix> matrixStack;

    // Example 4x4 matrix to be pushed onto the stack
    Matrix mat1 = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };

    // Push the matrix onto the stack
    matrixStack.push(mat1);

    // To pop the matrix and assign it to a new variable:
    Matrix poppedMatrix[4][4]= {0};

    if (!matrixStack.empty()) {
        poppedMatrix = matrixStack.top();
        matrixStack.pop();

        // Now 'poppedMatrix' contains the matrix popped from the stack
        // You can access its elements like poppedMatrix[row][col]
        // For example:
        std::cout << "Popped Matrix:" << std::endl;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                std::cout << poppedMatrix[i][j] << " ";
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << "Stack is empty. Cannot pop a matrix." << std::endl;
    }

    return 0;
}
