#include <bits/stdc++.h>
#include <stack>
#include <vector>
using namespace std;

bool check_valid(int x, int y, int max_x, int max_y)
{
    if (x < 0 || y < 0 || x >= max_x || y >= max_y)
    {
        return false;
    }
    return true;
}

bool exist(vector<vector<char>> &board, string word)
{
    int max_x = board.size();
    int max_y = board[0].size();
    vector<int> dx = {1, 0, -1, 0};
    vector<int> dy = {0, 1, 0, -1};
    vector<vector<int>> visited;
    vector<vector<int>> starting_points;
    stack<vector<int>> mystack;
    
    char starting_character = word[0];
    for (int i = 0; i < max_x; i++)
    {
        for (int j = 0; j < max_y; j++)
        {
            if (board[i][j] == starting_character)
            {
                starting_points.push_back({i, j});
            }
        }
    }

    for (int k = 0; k < starting_points.size(); k++)
    {
        visited = vector<vector<int>>(max_x, vector<int>(max_y, 0));
        while (!mystack.empty())
        {
            mystack.pop();
        }
        mystack.push({starting_points[k][0], starting_points[k][1], 0});
        vector<int> current_point;
        int current_x, current_y, current_char_index, next_x, next_y;
        while (!mystack.empty())
        {
            current_point = mystack.top();
            mystack.pop();
            current_x = current_point[0];
            current_y = current_point[1];
            current_char_index = current_point[2];
            if (current_char_index == word.size() - 1)
            {
                return true;
            }

            for (int dir = 0; dir < 4; dir++)
            {
                next_x = current_x + dx[dir];
                next_y = current_y + dy[dir];
                if (visited[next_x][next_y] == 0 & check_valid(next_x, next_y, max_x, max_y) && board[next_x][next_y] == word[current_char_index + 1])
                {
                    mystack.push({next_x, next_y, current_char_index + 1});
                    visited[next_x][next_y] = 1;
                }
            }
        }
    }
    return false;
}

int main()
{
    vector<vector<char>> visited(5, vector<char>(5, 'a'));
    cout << exist(visited, "abc");
}