#include <iostream>
#include <queue>
#include <set>
#include <map>
#include <list>

template <class V, class C> class mygraph {
    public:
    std::set<V> vertices;
    std::set<std::pair<V, V> > edges;
    std::map<V, std::map<V, C> > adjacencies;

    void add_vertex (V v)
    {
        this->vertices.insert(v);
    }
    void add_edge (V u, V v, C c)
    {
        this->edges.insert(std::pair<V, V>(u, v));
        this->adjacencies[u].insert(std::pair<V,C>(v, c));
        this->adjacencies[v].insert(std::pair<V,C>(u, c));
    }

    bool find_path_cost (V u, V v) {
      std::queue<V> visitq;
      std::set<V>   visited;

      visitq.push(u);
      while (!visitq.empty()) {
          V node = visitq.front();
          visitq.pop();

          if (visited.find(node) == visited.end()) {
              typename std::map<V,C>::iterator adj;
              for (adj = this->adjacencies[node].begin(); adj != this->adjacencies[node].end(); adj++) {
                   if (adj->first == v) return true;
                   visitq.push(adj->first);
              }
              visited.insert(node);
          }
      }

      return false;
    }
    mygraph *build_spanning_tree (void)
    {
        mygraph *spt = new mygraph<V, C>();
        for (typename std::set<V>::iterator it = this->vertices.begin();
             it != this->vertices.end(); it++) {
            if (spt->vertices.find(*it) == spt->vertices.end()) {
                for (typename std::map<V, C>::iterator adj = this->adjacencies[*it].begin();
                     adj != this->adjacencies[*it].end(); adj++) {
                     if (spt->vertices.find(adj->first) == spt->vertices.end()
                         || spt->vertices.find(*it) == spt->vertices.end()) {
                         spt->add_vertex(adj->first);
                         spt->add_edge(*it, adj->first, adj->second);
                     }
                }
                spt->add_vertex(*it);
            }
        }

        return spt;
    }

    bool is_connected_graph ()
    {
        bool ans = false;
        mygraph *spt = this->build_spanning_tree();
        if (spt->vertices.size() - 1 == spt->edges.size()) {
            ans = true;
        }

        delete spt;
        return ans;
    }

    void display (void)
    {
        std::cout << "Vertices are - { ";
        for (typename std::set<V>::iterator it = this->vertices.begin();
             it != this->vertices.end(); it++) {
            std::cout << *it << ", ";
        }
        std::cout << " } " << std::endl;

        std::cout << "Edges are - { ";
        for (typename std::set<std::pair<V, V> >::iterator it = this->edges.begin();
             it != this->edges.end(); it++) {
            std::cout << "(" << it->first << "," << it->second << "), ";
        }
        std::cout << " } " << std::endl;
    }
};


int main ()
{
    mygraph<char, int> mg1;

    mg1.add_vertex('A');
    mg1.add_vertex('B');
    mg1.add_vertex('C');
    mg1.add_vertex('D');
    mg1.add_vertex('E');

    mg1.add_edge('B', 'A', 5);
    mg1.add_edge('C', 'A', 6);
    mg1.add_edge('D', 'A', 3);
    mg1.add_edge('C', 'D', 4);
    mg1.add_edge('E', 'D', 7);

    mg1.display();
    mg1.build_spanning_tree()->display();

    std::cout << "Mygraph is" << (mg1.is_connected_graph() ? " " : " NOT ") << "connected" << std::endl;

    char start, end;
    std::cin >> start;
    std::cin >> end;

    std::cout << "Path between " << start << " and " << end << " is " << (mg1.find_path_cost(start, end) ?
                                                                          "" : "NOT") <<" there "  << std::endl;
    std::cout << "Path between " << start << "and " << end << (mg1.adjacencies[start].find(end))->second;

    return 0;
}

