#include <iostream>
#include <fstream>
#include <omp.h>
#include <random>

#define _CRT_SECURE_NO_WARNINGS
#define MATRIX_SIZE 1000
#define STREAMS_COUNT 16

using namespace std;

void randomiseMatrix(float** matrix) { //Генерация случайных чисел типа float с 0 до 1
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> dis(0.0f, 1.0f);

	for (int i = 0; i < MATRIX_SIZE; i++) {
		for (int j = 0; j < MATRIX_SIZE; j++) {
			matrix[i][j] = dis(gen);
		}
	}
	return;
}

void readMatrixFromFile(const char* filename, float** matrix) { //Чтение матриц 1000 на 1000 с файла (в файле числа типа float с 0 до 1)
    ifstream file;
    file.open(filename);

    if (file) {
        for (int i = 0; i < MATRIX_SIZE; i++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                file >> matrix[i][j];
            }
        }
        file.close();
        printf("Матрица с файла %s считалась успешно\n", filename);
    }
    else {
        printf("Файл не открыт %s", filename);
    }
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "Russian");
    
    //Инициализация матриц
    float** matrix1 = (float**)malloc(sizeof(float*) * MATRIX_SIZE);
    if (matrix1) {
        for (int i = 0; i < MATRIX_SIZE; i++) {
            matrix1[i] = (float*)malloc(sizeof(float) * MATRIX_SIZE);
        }
    }

    float** matrix2 = (float**)malloc(sizeof(float*) * MATRIX_SIZE);
    if (matrix2) {
        for (int i = 0; i < MATRIX_SIZE; i++) {
            matrix2[i] = (float*)malloc(sizeof(float) * MATRIX_SIZE);
        }
    }

    float** result = (float**)malloc(sizeof(float*) * MATRIX_SIZE);
    if (result) {
        for (int i = 0; i < MATRIX_SIZE; i++) {
            result[i] = (float*)malloc(sizeof(float) * MATRIX_SIZE);
        }
    }

    //Генерирация случайных чисел матриц для умножения
    /*printf("Генерация матриц %d на %d\n", MATRIX_SIZE, MATRIX_SIZE);
    randomiseMatrix(matrix1);
    randomiseMatrix(matrix2);*/

    //Считывание с файла матриц для умножения
    printf("Считывание с файла матриц %d на %d\n", MATRIX_SIZE, MATRIX_SIZE);
    readMatrixFromFile("matrix1.txt", matrix1);
    readMatrixFromFile("matrix2.txt", matrix2);

    printf("Перемножение матриц 5 раз\n");
    //Установка количества потоков
    omp_set_num_threads(STREAMS_COUNT);

    //Замер начала перемножения матриц
    double start = omp_get_wtime();

    //Распределение цикла на потоки с помощью parallel for
    //Для всех потоков матрицы matrix1, matrix2 и result будут общими 
    //Для каждого потока будут свои счётчики i, j, k (можно сделать через private)

#pragma omp parallel for shared(matrix1, matrix2, result)
    for (int g = 0; g < 5; g++) { //Матрицы будут перемножаться 5 раз
        if (matrix1 && matrix2 && result) { //Чтобы не было ошибки C6011 (Разыменование пустого указателя)
            for (int i = 0; i < MATRIX_SIZE; i++) {
                if (matrix1[i] && matrix2[i] && result[i]) { //Чтобы не было ошибки C6011 (Разыменование пустого указателя)
                    for (int j = 0; j < MATRIX_SIZE; j++) {
                        result[i][j] = 0;
                        for (int k = 0; k < MATRIX_SIZE; k++) {
                            result[i][j] += (matrix1[i][k] * matrix2[k][j]);
                        }
                    }
                }
            }
        }
    }

    //Замер конца перемножения матриц
    double end = omp_get_wtime();

    printf("Время перемножения матриц в %d потоках: %.16g секунд\n", STREAMS_COUNT, end - start);
    /*
    if (result) { //Чтобы не было ошибки C6011 (Разыменование пустого указателя)
        for (int i = 0; i < MATRIX_SIZE; i++) {
            if (result[i]) { //Чтобы не было ошибки C6011 (Разыменование пустого указателя)
                for (int j = 0; j < MATRIX_SIZE; j++) {
                    printf(" %f", result[i][j]);
                }
                printf("\n");
            }
        }
    }*/
    return 0;
}
