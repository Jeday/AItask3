#include <iostream>
#include <vector>
#include <cstring>
#include <queue>
#include <set>
#include <algorithm>
#include <ctime>
#include <string>
#include <set>
#include <deque>

#define N 4
#define EMPTY 0

#define NOT_FOUND -1
#define FOUND -2

int MAX_LVL = -1;

class game {
public:
	int  field[N*N];
	int empty_pos;
	int lvl;
	game* father;
	int heuristic;

	game() {
		lvl = -1;
		father = nullptr;
		heuristic = -1;
		empty_pos = -1;
	}

	game(game& g) {
		lvl = g.lvl;
		father = g.father;
		heuristic = g.heuristic;
		empty_pos = g.empty_pos;
		memcpy(field, g.field, sizeof(int)*N*N);
	}

	virtual ~game() {}


	int linear_conflict() {
		int conf = 0;
		for (int i = 0; i < N*N; ++i) {
			if (field[i] != 0) {
				for (int j = i + 1; j % N > i % N; ++j)
					if (field[j] != 0 && field[i] > field[j] && (field[i] - 1) / N == i / N
						&& (field[i] - 1) / N == (field[j] - 1) / N)
						conf += 2;

				for (int j = i + N; j < N*N; j += N)
					if (field[j] != 0 && field[i] > field[j] && (field[i] - 1) % N == i % N
						&& (field[i] - 1) % N == (field[j] - 1) % N)
						conf += 2;
			}
		}
		return conf;
	}

	virtual void calc_heur() {
		int h = 0;
		int pos;
		int val;
		for (int i = 0; i<N*N; ++i) {
			pos = i;
			val = field[i] - 1;
			if (val != -1)
				h += std::abs(val / N - pos / N) + std::abs(val%N - pos%N);
		}
		heuristic = h+linear_conflict();
	}
	

	void make_move(int move, std::vector<game*>& res) {
		game * g = new game(*this);
		std::swap(g->field[move], g->field[g->empty_pos]); // change board
		g->empty_pos = move; // update empty pos
		g->lvl++; // update lvl
		g->father = this;
		g->calc_heur();
		res.push_back(g);
	}

	std::vector<game*> gen_moves() {
		std::vector<game*> res;
		res.reserve(3);
		int father_pos = father == nullptr ? -1 : father->empty_pos;
		int new_empty = empty_pos + 1;
		if (new_empty % N != 0 && new_empty != father_pos) { // we can move right, there is no father or new move is not old one

		}
		new_empty = empty_pos - 1;
		if (empty_pos % N != 0 && new_empty != father_pos) {
			make_move(new_empty, res);
		}

		new_empty = empty_pos + 1;;
		if (new_empty % N != 0 && new_empty != father_pos) {
			make_move(new_empty, res);
		}

		new_empty = empty_pos + N;;
		if (new_empty <N*N  && new_empty != father_pos) {
			make_move(new_empty, res);
		}

		new_empty = empty_pos - N;;
		if (new_empty >= 0 && new_empty != father_pos) {
			make_move(new_empty, res);
		}

		return res;

	}

	bool is_solvable() {
		int inv = 0;
		for (int i = 0; i < N*N; ++i)
			if (field[i])
				for (int j = i + 1; j < N*N; ++j)
					if (field[j] < field[i] && field[j])
						++inv;
		return ((empty_pos / N + inv + 1) % 2 == 0);
	}

	class compare {
	public:
		bool operator()(game* g1, game* g2) {
			return g1->heuristic + g1->lvl> g2->heuristic + g2->lvl;

		}
	};


};


game * input_game(std::istream& cin) {
	std::string s;
	std::getline(cin, s);
	if (s.length() < N*N) {
		std::cout << "invalid length" << std::endl;
		return nullptr;
	}
	game* res = new game();
	for (int i = 0; i<N*N; i++) {
		if (s[i] >= '0' && s[i] <= '9') {
			res->field[i] = (int)(s[i] - '0');
		}
		else if (s[i] >= 'A' && s[i] <= 'F') {
			res->field[i] = (int)(s[i] - 'A') + 10;
		}
		else {
			delete res;
			std::cout << "invalid input" << std::endl;
			return nullptr;
		}
		if (res->field[i] == 0)
			res->empty_pos = i;
	}
	res->lvl = 0;
	res->calc_heur();
	return res;
}

void print_moves(game * g, double time) {
	std::vector<int> r;
	std::cout << "finished at: " << time << std::endl;
	while (g) {
		r.push_back(g->empty_pos);
		g = g->father;
	}
	for (auto i = r.rbegin(); i != r.rend(); ++i)
		std::cout << *i << "->";
}

std::priority_queue<game*, std::deque<game*>, game::compare> q;
std::set<game*> visited;
std::vector<game*> _visited;
void astar() {
	q=std::priority_queue<game*, std::deque<game*>, game::compare>(game::compare());
	visited.clear();
	game * beg = input_game(std::cin);
	if (beg) {
		if (beg->is_solvable()) {
			clock_t start = std::clock();
			beg->calc_heur();
			q.push(beg);
			while (!q.empty()) {
				beg = q.top();
				q.pop();
				if (beg->heuristic == 0)
					break;
				std::vector<game*> v = beg->gen_moves();
				for (game * g : v) {
					if (std::find_if(visited.begin(), visited.end(), [g](const game* s) { return memcmp(s->field, g->field, sizeof(int)*N*N) == 0; }) == visited.end())
						q.push(g);
				}
				visited.insert(beg);
			}
			clock_t end = std::clock();
			print_moves(beg, (double)(end - start) / CLOCKS_PER_SEC);
		}
		else
			std::cout << "no solution";

	}

}

game * finish = nullptr;

int search(int bound) {
	game* g = _visited.back();
	if (!g) return NOT_FOUND;
	if (g->lvl + g->heuristic>bound) return NOT_FOUND;
	if (g->heuristic == 0) {
		finish = g;
		return FOUND;
	}
	std::vector<game*> moves = g->gen_moves();
	int min = NOT_FOUND;
	for (game* succ : moves)
		if (std::find_if(_visited.begin(), _visited.end(), [succ](const game* s) { return memcmp(&(s->field), &(succ->field), sizeof(int)*N*N) == 0; }) == _visited.end()) {
			_visited.push_back(succ);
			int t = search(bound);
			if (t == FOUND) return t;
			min = t != NOT_FOUND && min != NOT_FOUND && t < min ? t : min;
			delete succ;
			_visited.pop_back();
		}
	return min;
}


void idastar() {
	_visited.clear();
	game * beg = input_game(std::cin);
	if (beg && beg->is_solvable()) {
		int bound = beg->heuristic;
		_visited.push_back(beg);
		clock_t start = clock();
		while (true) {
			int res = search(bound);
			if (res == FOUND) break;
			bound = res;
		}
		clock_t end = clock();
		double time = (double)(end - start) / CLOCKS_PER_SEC;
		std::cout << "finished at: " << time << std::endl;
		for (size_t i = 0; i<_visited.size(); i++)
			std::cout << _visited[i]->empty_pos << "->";
		std::cout << std::endl;
	}
	else std::cout << "no solution";


}

int main()
{
	astar();
	//idastar();

	bool d;
	std::cin >> d;
	return 0;
}
