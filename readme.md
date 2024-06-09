# Disease Prediction System

## Project Overview

This project is a Disease Prediction System developed for a Data Structures course. The system uses the ID3 algorithm to build decision trees and predict diseases based on user-provided symptoms. The aim of the project is to demonstrate the application of decision trees in solving real-world problems and to deepen the understanding of data structures and algorithms.

## Table of Contents

1. [Introduction](#introduction)
2. [Setup and Requirements](#setup-and-requirements)
3. [Implementation Details](#implementation-details)
4. [Running the Code](#running-the-code)
5. [Example Usage](#example-usage)
6. [Conclusion](#conclusion)

## Introduction

The Disease Prediction System predicts diseases by analyzing symptoms entered by the user. It utilizes the ID3 algorithm, a popular decision tree algorithm, to classify symptoms and predict the most probable disease. This project illustrates the practical use of decision trees in medical diagnostics and showcases the power of data structures in solving complex problems.

## Setup and Requirements

### Prerequisites

- C++ compiler (e.g., GCC)
- Basic knowledge of data structures and algorithms
- Understanding of decision trees and the ID3 algorithm

### Installation

1. Ensure a C++ compiler is installed on your system. You can install GCC on Unix-based systems using:
    ```sh
    sudo apt-get install g++
    ```

2. Clone this repository to your local machine:
    ```sh
    git clone https://github.com/your-repo/disease-prediction-system.git
    cd disease-prediction-system
    ```

## Implementation Details

### ID3 Algorithm

The ID3 (Iterative Dichotomiser 3) algorithm is used to generate a decision tree from a dataset. It selects the attribute that best separates the data based on information gain, recursively partitions the data, and creates nodes in the decision tree.

### Decision Tree Structure

The decision tree is a tree-like structure where each internal node represents a test on an attribute, each branch represents the outcome of the test, and each leaf node represents a class label (in this case, a disease).

## Running the Code

1. Compile the code using g++:
    ```sh
    g++ main.cpp -o disease_prediction_system
    ```

2. Run the executable:
    ```sh
    ./disease_prediction_system
    ```

## Example Usage

1. The program will prompt you to enter symptoms.
2. Based on the symptoms, it will use the decision tree to predict the most likely disease.

### Sample Input

```
Enter symptoms (comma-separated): headache,fever,cough
```

### Sample Output

```
Predicted Disease: Flu
```

## Conclusion

The Disease Prediction System demonstrates the application of the ID3 decision tree algorithm in predicting diseases based on symptoms. This project highlights the importance of data structures and algorithms in solving real-world problems and provides a foundation for further exploration into machine learning and data science.