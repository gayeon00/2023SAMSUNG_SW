// 01.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <queue>
#include <stack>

#define MAX_N 20
#define MAX_M 20

using namespace std;

int N, M, T;
int board[MAX_N][MAX_M];
int dist[MAX_N][MAX_M];
//바로 이전 위치 저장
pair<int,int> mprev[MAX_N][MAX_M];
//보충 대상을 판별하기 위한 배열
bool visited[MAX_N][MAX_M] = { false };
int dx[] = { 0,1,0,-1 };
int dy[] = { 1,0,-1,0 };
int minR, minC = 0;
int maxR, maxC = 0;
//i,j에 있는 신이 가장 최근에 각성한 시기 저장
int recentWake[20][20] = { 0 };


struct Node {
    //cost: x,y까지 이동한 거리
    int x, y, cost;

    //priority_queue를 위한 operator <
    //cost가 작을 수록 우선순위가 높도록
    bool operator < (const Node& n) const {
        return cost > n.cost;
    }
};

int dijkstra(int n, int m, int startX, int startY, int endX, int endY) {
    bool flag = true;
    memset(dist, -1, sizeof(dist));
    priority_queue<Node> q;
    dist[startX][startY] = 0;
    q.push({ startX,startY,0 });

    while (!q.empty()) {
        Node curr = q.top();
        q.pop();

        if (curr.x == endX && curr.y == endY) break;

        //curr이 최단 거리가 아니니까 패스해도 됨
        if (dist[curr.x][curr.y] < curr.cost) continue;

        for (int i = 0; i < 4; i++) {
            int nx = (curr.x + dx[i] + n) % n;
            int ny = (curr.y + dy[i] + n) % n;

            if (board[nx][ny] < 1) continue;

            int cost = curr.cost + 1;
            if (dist[nx][ny]==-1||cost < dist[nx][ny]) {
                mprev[nx][ny] = { curr.x,curr.y };
                dist[nx][ny] = cost;
                q.push({ nx,ny,cost });
            }
        }

        if (q.empty()) {
            flag = false;
        }
    }

    return flag;
}

void read_data() {
    cin >> N >> M >> T;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            cin >> board[i][j];
        }
    }

}

void wake() {
    /* 힘이 가장 약한 신 구하기*/

    int min = 1000;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (board[i][j] < min) {
                min = board[i][j];
                minR = i; minC = j;
            } else if (board[i][j] == min) {
                if (recentWake[i][j] > recentWake[minR][minC]) {
                    min = board[i][j];
                    minR = i; minC = j;
                } else if (recentWake[i][j] == recentWake[minR][minC]) {
                    if (i + j < minR + minC) {
                        min = board[i][j];
                        minR = i; minC = j;
                    } else if (i + j == minR + minC && j < minC) {
                        min = board[i][j];
                        minR = i; minC = j;
                    }
                }
            }
        }
    }

    /* 힘이 가장 약한 신 각성 */
    board[minR][minC] += N + M;
}

void bomb() {
    board[maxR][maxC] -= board[minR][minC];
    for (int i = 0; i < 4; i++) {
        board[maxR+dx[i]][maxC+dy[i]] -= board[minR][minC]/2;
    }
    board[maxR+1][maxC+1] -= board[minR][minC]/2;
    board[maxR+1][maxC-1] -= board[minR][minC]/2;
    board[maxR-1][maxC+1] -= board[minR][minC]/2;
    board[maxR-1][maxC-1] -= board[minR][minC]/2;
}

void spear() {
    //경로 출력위한 stack
    stack<pair<int, int>> path;
    int cx = maxR, cy = maxC;
    while (cx != minR || cy != minC) {
        path.push({ cx,cy });
        int nx = mprev[cx][cy].first;
        int ny = mprev[cx][cy].second;
        cx = nx;
        cy = ny;
    }
    path.push({ minR, minC });
    while (!path.empty()) {
        pair<int,int> top = path.top();
        path.pop();
        //시작점 제외
        if (top.first == minR && top.second == minC) continue;
        //도착점은 -P
        if (top.first == maxR && top.second == maxC) board[top.first][top.second] -= board[minC][minR];
        //경로는 -P/2
        board[top.first][top.second] -= board[minR][minC] / 2;
        //경로 표시
        visited[top.first][top.second] = true;
    }
}

void attack() {
    /* 힘이 가장 센 신 구하기 */
    int max = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            //방금 각성한 최소였던 신은 제외해야함
            if (i==minR && j==minC) continue;

            if (board[i][j]>max) {
                max = board[i][j];
                maxR = i; maxC = j;
            } else if (board[i][j]== max) {
                if (recentWake[i][j]<recentWake[maxR][maxC]) {
                    max = board[i][j];
                    maxR = i; maxC = j;
                } else if (recentWake[i][j]== recentWake[maxR][maxC]) {
                    if (i + j> maxR + maxC) {
                        max = board[i][j];
                        maxR = i; maxC = j;
                    } else if (i + j==maxR + maxC && j>maxC) {
                        max = board[i][j];
                        maxR = i; maxC = j;
                    }
                }
            }
        }
    }

    if (dijkstra(N, M, minR, minC, maxR, maxC) == false) {
        //어둠의 폭탄
        bomb();
    } else {
        //빛의 창
        spear();
    }
}

void gain() {


    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (visited[i][j]) continue;
            board[i][j]++;
        }
    }
}

void solve() {
    while (T--) {
        //각성
        wake();
        //공격
        attack();
        //보충
        gain();
    }

}

int main() {

    read_data();

    solve();
}
//4 4
//3 2 0 0
//9 0 0 3
//2 3 0 5
//4 2 0 7
//8 8 10 6
