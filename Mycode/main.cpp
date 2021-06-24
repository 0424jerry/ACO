#include <iostream>
#include<cmath>
#include<ctime>

using namespace std;
#define N 20        // city
#define M 30        //ants

double d[N][N];
double c[N][2];


double alpha = 2;
double beta = 3;
double rou = 0.1;
double alpha1 = 0.1;
double qzero = 0.01;
double Lnn;


class antcolonysystem{
private:
    double info[N][N];
    double visible[N][N];
public:
    double transition(int x,int y);
    void updatelocalpath(int x,int y);
    void initparameter(double v);
    void updateglobalpath(int* bestT, int globalbestL);
};

class Ant{
private:
    antcolonysystem* antcolony;
protected:
    int startC, currentC;
    int cityList[N];
    int tour[N][2];
    int currentI;
public:
    Ant(antcolonysystem* acs,int start){
        antcolony = acs;
        startC = start;
    }

    int* search();
    int choose();
    void MoveToNext(int next);
};



int ChooseNextNode(int currentNode , int visitedNode[]){
    int nextN = -1;
    double sd = 0.0;
    for(int i = 0; i < N; i++){
        if(visitedNode[i] == 1){
            if(sd == 0.0){
                sd = d[currentNode][i];
            }
            if(sd < d[currentNode][i]){
                nextN = i;
            }
        }
    }

    return nextN;
}


double CalAjacentD(int node){
    double sum = 0.0;
    int visitedNode[N];
    for(int i = 0; i < N; i++) visitedNode[i] = 1;
    visitedNode[node] = 0;
    int cnode = node;
    int nnode;

    do{
        nnode = ChooseNextNode(cnode,visitedNode);
        if(nnode >= 0){
            sum += d[cnode][nnode];
            cnode = nnode;
            visitedNode[cnode] = 0;
        }
    }while(nnode >= 0);
    sum += d[cnode][node];

    return sum;

}



double calculateSumOfDistance(int* tour)
{
    double sum = 0;
    for (int i = 0; i< N; i++)
    {
        int row = *(tour + 2 * i);
        int col = *(tour + 2 * i + 1);
        sum += d[row][col];
    }
    return sum;
}










int main() {
     int n;
     int input = 0;
     while(cin >> n && n !=0){

             cin >> c[input][0] >> c[input][1];
            input++;
     }



    srand(time(0));
    time_t timer = clock();

    for(int i = 0; i < N;i++){
        for(int j =0; j < N;j++){
            if(i != j){
                d[i][j] = sqrt(pow((c[i][0] - c[j][0]), 2) + pow((c[i][1] - c[j][1]), 2));
                d[j][i] =  d[i][j];
            }
        }
    }






    antcolonysystem* acs = new antcolonysystem;
    Ant* ants[M];


    for (int k = 0; k < M; k++){
        ants[k] = new Ant(acs, (int)(k%N));
    }

    int node = rand() % N;
    Lnn = CalAjacentD(node);

    double initInfo = 1 / (N*Lnn);
    acs ->initparameter(initInfo);

    int globalTour[N][2];
    double globalBestL = 0.0;


    for (int i = 0; i < 500; i++){


        int localtour[N][2];
        double localBestL = 0.0;
        double tourL;
        for (int j = 0; j < M; j++){
            int* tourpath = ants[j] -> search();
            tourL = calculateSumOfDistance(tourpath);

            if(tourL < localBestL || abs(localBestL - 0.0) < 0.000001){
                for(int a = 0; a < N;a++){
                    int row = *(tourpath + 2 * a);
                    int col = *(tourpath + 2 * a + 1);
                    localtour[a][0] = row;
                    localtour[a][1] = col;
                }
                localBestL = tourL;
            }
        }

        if(localBestL < globalBestL || abs(globalBestL - 0) < 0.000001){
            for (int m = 0; m< N; m++)
            {
                globalTour[m][0] = localtour[m][0];
                globalTour[m][1] = localtour[m][1];
            }
            globalBestL = localBestL;
        }
        acs ->updateglobalpath(*globalTour, globalBestL);
    }

    cout << globalBestL << endl;
    return 0;
}


double antcolonysystem::transition(int x, int y) {
    if(x != y){
        return (pow(info[x][y],alpha) * pow(visible[x][y],3));  // 3 represent beta
    }
    else {
        return 0.0;
    }
}


void antcolonysystem::updatelocalpath(int x, int y) {
    info[x][y] = (1.0 - alpha1) * info[x][y] + alpha1 * (1.0 / (N * Lnn));
    info[y][x] = info[x][y];
}


void antcolonysystem::initparameter(double v) {
    for(int i = 0; i < N;i++){
        for(int j =0; j < N;j++){
            info[i][j] = v;
            if(i != j){
                visible[i][j] = 1.0 / d[i][j];
            }
        }
    }
}


void antcolonysystem::updateglobalpath(int* bestT, int globalbestL) {
    for(int i = 0; i < N;i++){
        int r = *(bestT + 2 * i);
        int c = *(bestT + 2 * i + 1);
        info[r][c] = (1 - rou) * info[r][c] + rou * (1.0/globalbestL);
        info[c][r] = info[r][c];
    }
}


int* Ant::search() {
    currentC = startC;
    int nextC;
    currentI = 0;
    for(int i = 0; i < N;i++) cityList[i] = 1;
    cityList[currentC] = 0;
    int endcity;

    int cnt = 0;
    do{
        cnt++;
        endcity = currentC;
        nextC = choose();
        if(nextC >= 0){
            MoveToNext(nextC);
            antcolony ->updatelocalpath(endcity,nextC);
            currentC = nextC;
        }
    }while(nextC >= 0);

    MoveToNext(startC);
    antcolony ->updatelocalpath(endcity, startC);

    return *tour;
}


int Ant::choose() {
    int nextC = -1;

    double q = rand() / (double) RAND_MAX;

    if(q <= qzero){
        double p = -1.0;
        for(int i = 0; i< N;i++){
            if(cityList[i] == 1){
                double p1 = antcolony->transition(currentC , i);
                if(p1 > p){
                    nextC = i;
                    p = p1;
                }
            }
        }
    }

    else{
        double r = rand() / (double) RAND_MAX;
        double sum = 0.0;
        double p = 0.0;

        for(int i = 0; i< N;i++){
            if(cityList[i] == 1){
                sum += antcolony ->transition(currentC , i);
            }
        }

        for(int j = 0; j< N;j++){
            if(cityList[j] == 1 && sum > 0){
                p += antcolony->transition(currentC,j) / sum;
                if(p >= r || (p > 0.9999 && r > 0.9999)){
                    nextC = j;
                    break;
                }
            }
        }
    }

    return  nextC;
}


void Ant::MoveToNext(int next) {
    cityList[next] = 0;
    tour[currentI][0] = currentC;
    tour[currentI][1] = next;
    currentI++;
    currentC = next;
}


