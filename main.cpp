#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <queue>
#include <array>
#include <cmath>
#include <sstream>
#include <iterator>
#include <fstream>

struct Cell;

struct Edge {
    Edge(int weight, Cell* from, Cell* to) : weight(weight), from(from), to(to) {}
    Cell* to;
    Cell* from;
    int weight;
};

struct Cell {
    Cell(){}
    Cell(const int& val) : val(val) {}
    int val = -1;
    std::vector<Edge> adj;
};

int getIndexOfSet(std::vector<std::set<int>> v, int val) {
    for (int i = 0 ; i < v.size(); ++ i) {
        if(v.at(i).find(val) != v.at(i).end()) {
            return i;
        }
    }
    return -1;
}


struct Graph {
    Graph(int size) : size(size) {
        cells.resize(size);
    }
    void addCell(int val) {
        auto cell = find_if(cells.begin(), cells.end(), 
                [&](Cell c){return c.val == val;});
        if (cell == cells.end()) {
            cells.push_back(Cell(val));
        }
    }
    void addEdge(int from, int to, int weight) {
        int toPos, fromPos;
        for (int i = 0 ; i < cells.size(); ++i) {
            if (cells[i].val == to) {
                toPos = i;
            } else if (cells[i].val == from) {
                fromPos = i;
            }
        }
        Edge e(weight, &cells[fromPos], &cells[toPos]);
        cells[fromPos].adj.push_back(e);
        cells[toPos].adj.push_back(e);
        edges.push_back(e);
    }
    void removeEdge(int from, int to) {
        int toPos, fromPos;
        for (int i = 0 ; i < cells.size(); ++i) {
            if (cells[i].val == to) {
                toPos = i;
            } else if (cells[i].val == from) {
                fromPos = i;
            }
        }
        cells[fromPos].adj.erase(
            std::find_if(
                cells[fromPos].adj.begin(),
                cells[fromPos].adj.end(), 
                [&](Edge e){ return e.to->val == to;}
            )
        );
        cells[toPos].adj.erase(
            std::find_if(
                cells[toPos].adj.begin(),
                cells[toPos].adj.end(),
                [&](Edge e){ return e.from->val == from;}
            )
        );
    }
    void print() {
        for (auto it : cells) {
            if(it.val == -1) {continue;}
            std::cout<<"cell: " << it.val << "\n";
            for (Edge edge : it.adj) {
                std::cout<< "edge from = " <<edge.from->val<<"\n";
                std::cout<< "edge to = " <<edge.to->val<<"\n";
            }
        }
    }
    Cell getCell(int index) {
        Cell c = *std::find_if(cells.begin(), cells.end(), [&](Cell c){return c.val == index;});
        return c;
    }

    size_t getSumOfWeigths() {
        size_t sum = 0;
        for (Edge e : edges) {
            sum += e.weight;
        }
        return sum;
    }

    size_t getEdgeWithLowestWeight() {
        size_t min = edges.begin()->weight;
        for (Edge e : edges) {
            if (min > e.weight){
                min = e.weight;
            }
        }
        return min;
    }

    int size;
    std::vector<Cell> cells;
    std::vector<Edge> edges;
};

Graph kruskal(size_t numOfCells, std::vector<Edge> input) {
    Graph outputGraph(numOfCells);
    std::vector<std::set<int>> visited;
    for (int i = 0; i < numOfCells; ++i) {
        visited.push_back(std::set<int>({i + 1}));
    }
    std::vector<Edge>::iterator it = input.begin();
    while(visited.size() != 1) {
        size_t indexOfFrom = getIndexOfSet(visited, it->from->val);
        size_t indexOfTo = getIndexOfSet(visited, it->to->val);
        if (indexOfFrom != indexOfTo) {
            visited.at(indexOfFrom).insert(visited.at(indexOfTo).begin(),
                visited.at(indexOfTo).end());
            visited.erase(visited.begin() + indexOfTo);
            outputGraph.addCell(it->from->val);
            outputGraph.addCell(it->to->val);
            outputGraph.addEdge(it->from->val, it->to->val, it->weight);
        }
        ++it;
    }
    return outputGraph;
}

bool isAchivable(int from, int to, Graph g) {
    std::queue<Cell> q;
    std::vector<bool> visited(g.cells.size(), false);
    visited.resize(g.cells.size());
    q.push(g.getCell(from));
    while (!q.empty()) {
        Cell currentCell = q.front();
        visited.at(currentCell.val) = true;
        for (Edge e: currentCell.adj) {
            if (e.from->val == currentCell.val) {
                if (visited.at(e.to->val) == false) {
                    if (e.to->val == to) {return true;}
                    q.push(g.getCell(e.to->val));
                }
            } else {
                if (visited.at(e.from->val) == false) {
                    if (e.from->val == to) {return true;}
                    q.push(g.getCell(e.from->val));
                }
            }
        }
        q.pop();
    }
    return false;
}

std::vector<std::string> parseArgs(std::string line) {
    std::stringstream ss(line);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> args(begin, end);
    return args;
}

Graph reduceGraphToWidestPath(Graph& g, int from, int to) {
    Graph copy = g;
    for (int i = 0 ; i < g.edges.size(); ++i) {
        Graph graph = copy;
        Edge e = g.edges.at(i);
        graph.removeEdge(e.from->val, e.to->val);
        if (isAchivable(from, to, graph)) {
            copy = graph;
        }
    }
    return copy;
}

int main(int argc, char** argv) {
    std::string line;
    std::ifstream ifs(argv[1]);
    std::getline(ifs, line);
    auto firstLineArgs = parseArgs(line);
    int numOfCells = stoi(firstLineArgs.at(0));
    int numOfEdges = stoi(firstLineArgs.at(1));
    Graph graph(numOfCells);
    for (int i = 0 ; i < numOfCells; ++i){
        graph.addCell(i + 1);
    }
    for (int i = 0 ; i < numOfEdges; ++i) {
        std::getline(ifs, line);
        auto edgeErgs = parseArgs(line);
        graph.addEdge(stoi(edgeErgs.at(0)), stoi(edgeErgs.at(1)), 
                stoi(edgeErgs.at(2)));
    }
    std::sort(graph.edges.begin(), graph.edges.end(), 
            [](Edge a, Edge b){return a.weight > b.weight;});
    Graph MST = kruskal(numOfCells, graph.edges);

    std::getline(ifs, line);
    auto targetArgs = parseArgs(line);

    ifs.close();

    Graph widestPathGraph = reduceGraphToWidestPath(MST, 
            stoi(targetArgs.at(0)), stoi(targetArgs.at(1)));
    std::ofstream ofs(argv[2]);
    float numOfToursts = stoi(targetArgs.at(2));
    ofs<<std::ceil(numOfToursts / widestPathGraph.getEdgeWithLowestWeight());
    ofs.close();
    return 0;
}