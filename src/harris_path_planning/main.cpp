#include "implementation.cpp"


//template<typename Location, typename Graph>
//std::map<Location, Location>
//breadth_first_search(Graph graph, Location start, Location goal) {
//    std::queue<Location> frontier;
//    frontier.push(start);
//
//    std::map<Location, Location> came_from;
//    came_from[start] = start;
//
//    while (!frontier.empty()) {
//        Location current = frontier.front();
//        frontier.pop();
//
//        if (current == goal) {
//            break;
//        }
//
//        for (Location next : graph.neighbors(current)) {
//            if (came_from.find(next) == came_from.end()) {
//                frontier.push(next);
//                came_from[next] = current;
//            }
//        }
//    }
//    return came_from;
//}
//
//

int main() {
    std::cout << "Starting A* Algorithm!" << std::endl;
    GridWithWeights grid = make_diagram4();
    GridLocation start{1, 4};
    GridLocation goal{8, 5};
    std::map<GridLocation, GridLocation> came_from;
    std::map<GridLocation, double> cost_so_far;
    a_star_search(grid, start, goal, came_from, cost_so_far);
    draw_grid(grid, 2, nullptr, &came_from);
    std::cout << '\n';
    draw_grid(grid, 3, &cost_so_far, nullptr);
    std::cout << '\n';
    std::vector<GridLocation> path = reconstruct_path(start, goal, came_from);
    draw_grid(grid, 3, nullptr, nullptr, &path);
    return 0;
}