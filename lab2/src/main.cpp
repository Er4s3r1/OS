#include <iostream>
#include <fstream>
#include <pthread.h>
#include <time.h>
#include <vector>
#include <cmath>
#include <chrono>
using namespace std;

using matrix = vector<vector<int>>;

int det(const matrix& M1) {
	int i, j, n = M1.size();
	if (n == 1) return M1[0][0];
	matrix M2(n - 1, vector<int>(n - 1));
	vector<int>line(n - 1);
	for (i = 1; i < n; ++i) {
		line[i - 1] = M1[0][i];
		for (j = 1; j < n; ++j)
			M2[i - 1][j - 1] = M1[i][j];
	}
	int ans = 0;
	for (i = 0; i < n; ++i) {
        if (i % 2 == 0)
            ans += M1[i][0] * det(M2);
        else
            ans -= M1[i][0] * det(M2);
		if (i != n - 1)
			swap(M2[i], line);
	}
	return ans;
}

matrix M;
int res = 0;
int step;

struct pthread_args {
    int begin;
    int end;
};

void* Solve(void* arg) {
    struct pthread_args* args = ((struct pthread_args*)arg);
    int begin = args->begin, end = args->end;
    int n = M.size();
    if(n == 1) { 
        res = M[0][0];
        pthread_exit(0);
        return 0;
    }
    for (int i = begin; i < end; ++i) {
        vector<vector<int>>M2(n - 1, vector<int>(n - 1));
        int dj = 0;
        for(int j = 0; j < n - 1; ++j) {
            if(j == i) ++dj;
            for(int k = 0; k < n - 1; ++k) {
                M2[j][k] = M[j + dj][k + 1];
            }
        }
        if (i % 2 == 0)
            res += M[i][0] * det(M2);
        else
            res -= M[i][0] * det(M2);
    }
    pthread_exit(0);
    return 0;
}

int main() {
    int n, flow;
    cin >> flow >> n;
    int start_flow = flow;
    flow = min(flow, n);
    M.resize(n, vector<int>(n));
    for(int i = 0; i < M.size(); i++) 
        for(int j = 0; j < M.size(); j++) 
            cin >> M[i][j];
    auto start = std::chrono::high_resolution_clock::now();           
    step = n / flow;
    pthread_t tid[flow];
    struct pthread_args* args_pth = (struct pthread_args*)malloc(flow * sizeof(struct pthread_args));
    for (int i = 0; i < flow; ++i) {
        args_pth[i].begin = i * step;
        if (i == flow - 1)
            args_pth[i].end = n;
        else 
            args_pth[i].end = (i + 1) * step;
    }
    for (int i = 0; i < flow; ++i)
        pthread_create(&tid[i], NULL, Solve, (void*)&args_pth[i]);
    for (int i = 0; i < flow; ++i)
        pthread_join(tid[i], NULL);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    cout << start_flow << ' ' << duration.count() << endl;
    cout << "res = " << res << endl;

    return 0;
}