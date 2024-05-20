#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <vector>
#include <set>
#include <chrono>

using namespace std;

struct preference{
    int n;
    set<int> freew; //skup slobodnih žena
    vector<vector<int>> women; //vektor vektora preferencije žena
    vector<vector<int>> men; //vektor vektora preferencije muškaraca
    vector<int> wnext; //vektor u kojem spremamo kojeg muškarca koja žena treba idućeg zaprositi (odn. na kojoj poziciji u matrici je stala)  
    vector<int> engaged_women; // parovi (žena, muškarac) koji su trenutno zaručeni
    vector<int> engaged_men; // parovi (muškarac, žena) koji su trenutno zaručeni

    preference(int _n, const vector<vector<int>>& _women, const vector<vector<int>>& _men)
        : n(_n), freew(),  women(_women), men(_men), wnext(_n, 0), engaged_women(_n, -1), engaged_men(_n, -1) {
        for (int i = 0; i < n; ++i) { //assign each woman and man as free
            freew.insert(i);
        }
    }
};

void GaleShapley(preference &P){
    while(!P.freew.empty()){ //while some woman w is free
        int w = *P.freew.begin();
        int m = P.women[w][P.wnext[w]++]; // let m be the first man on w’s ranked list to whom she has not proposed

        if(P.engaged_men[m] == -1){ //m is free
            P.engaged_women[w] =  m; //w and m become engaged to each other (and not free)
            P.engaged_men[m] =  w;
            P.freew.erase(w);
        } else {
            int w0 = P.engaged_men[m];
            if(P.men[m][w] < P.men[m][w0]) { // elseif m ranks w higher than the woman w0 he is currently engaged to
                P.freew.insert(w0); //m breaks the engagement to w 0 , who becomes free
                P.freew.erase(w);
                P.engaged_women[w] =  m; //w and m become engaged to each other (and not free)
                P.engaged_men[m] =  w;
            }
            // else m rejects w, with w remaining free
        }
    }
}

vector<vector<int>> Permutation (int n){
    vector<vector<int>> all_permutations;

    vector<int> elements(n);
    for (int i = 0; i < n; ++i) {
        elements[i] = i;
    }

    for (int i = 0; i < n; ++i) {

        unsigned seed = chrono::system_clock::now().time_since_epoch().count();
        shuffle(elements.begin(), elements.end(), default_random_engine(seed));

        all_permutations.push_back(elements);
    }

    return all_permutations;
}

int main() {

    ofstream file("results_average.csv");

    if (!file.is_open()) {
        cerr << "Nemoguće otvoriti datoteku za pisanje." << endl;
        return 1;
    }
    
    for(int n = 1; n <= 2000; n++){
        cout << "n = " << n << endl;
        double total_time = 0;

        for(int i = 1; i <= 10; i++){
            vector<vector<int>> women = Permutation(n);
            vector<vector<int>> men =  Permutation(n);

            preference Pr(n, women, men);

            auto start_time = chrono::high_resolution_clock::now();

            GaleShapley(Pr);

            auto end_time = chrono::high_resolution_clock::now();

            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();

            total_time += duration;
        }
        auto average_time = total_time / 10;
        
        file << n << "," << average_time << endl;
    }
    file.close();
    return 0;
}