#include <cmath>
#include <fstream>
#include <string>
#include "score.hpp"


using namespace std;

ifstream input;
ofstream output;
char names[5][20] = {"n","n","n","n","n"};
int scores[5] = {0,0,0,0,0};
int i = 0;
int j = 0;

int compute_score(int previous_score, int lines_cleared) {
  // 1 line 1 point, 2 lines 4 points, 3 lines 9 points, 4 lines 16 points, 
  return previous_score + pow(lines_cleared,2);
}

const char* print_name(int index) {
  load_scores();
  return names[index];
}

int print_score(int index) {
  load_scores();
  return scores[index];
}

void load_scores() {
  i = 0;
  input.open("highscores.txt");
  input.ignore(100,'\n');
  while(i < 5) {
    input.ignore(100,')');
    input.getline(names[i],100, ' ');
    input.ignore(100,':');
    input >> scores[i];
    i++;
  }
  input.close();
}

void save_score(char name[20], int score) {
  load_scores();
  i = 0;
  j = 0;
  while(i < 5) {
    if(score > scores[i]) {
      scores[i] = score;
      while(name[j] != '\0') {
	names[i][j] = name[j];
	j++;
      }
      names[i][j] = '\0';
      i = 50;
    }
    i++;
  }
  j = 1;
  output.open("highscores.txt");
  output << "Highscores:\n";
  while(j <= 5) {
    output << j << ")" << names[j-1] << " - score:" << scores[j-1] << "\n";
    j++;
  }
}
