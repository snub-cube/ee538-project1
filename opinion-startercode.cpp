#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
using namespace std;

/********************DO NOT EDIT**********************/
// Function prototype. Defined later.
void read_opinions(string filename); // reads file into opinions vector and updates total_nodes as needed
void read_edges(string filename); // reads file into edge_list, defined later
void build_adj_matrix(); // convert edge_list to adjacency matrix

int total_nodes = 0; // We keep track of the total number of nodes based on largest node id.


/****************************************************************/

/******** Create adjacency matrix and vector of opinions */
// simple vector to hold each node's opinion (0 or 1)
std::vector<int> opinions;

// global adjacency matrix initialized later
std::vector<std::vector<int>> adj_list;

// edge list: each row contains {source, target}
std::vector<std::vector<int>> edge_list;

void build_adj_matrix()
{

    // (1) allocate matrix adj of appropriate size


    // Find the maximum index to determine matrix dimensions
    int maxIdx = 0;
    for (const auto& p : edge_list) {
        maxIdx = std::max({maxIdx, p[0], p[1]});
    }

    int size = maxIdx + 1;

    // Resize adjacency list
    adj_list.resize(size);
    
    // Build adjacency list from directed edges
    // Key change: Only store outgoing edges (source -> target)
    for (const auto& p : edge_list) {
        int source = p[0];  // Influencer
        int target = p[1];  // Influenced by source
        
        // Target's opinion is influenced by source
        adj_list[target].push_back(source);
    }
    
}

double calculate_fraction_of_ones()
{

   // (3) Calculate the fraction of nodes with opinion 1 and return it.

   // edge case: empty vector
    if (opinions.empty()) {
        return 0.0;
    }

    int count_ones = 0;

    // Count the occurrences of opinion 1
    for (int op : opinions) {
        if (op == 1) {
            count_ones++;
        }
    }

    // Return fraction
    return static_cast<double>(count_ones) / opinions.size();

}

// For a given node, count majority opinion among its neighbours. Tie -> 0.
int get_majority_friend_opinions(int node)
{
    // Key change: Using adjacency list instead of matrix
    if (node >= adj_list.size() || adj_list[node].empty()) {
        return opinions[node]; // No neighbors, keep own opinion
    }
    
    const auto& neighbors = adj_list[node];
    int count_ones = 0;
    
    // Only iterate over actual neighbors, not all nodes
    for (int neighbor : neighbors) {
        if (neighbor < opinions.size() && opinions[neighbor] == 1) {
            count_ones++;
        }
    }
    
    int total_neighbors = neighbors.size();
    
    // Tie goes to 0
    return (count_ones > total_neighbors / 2.0) ? 1 : 0;
}

// Calculate new opinions for all voters and return if anyone's opinion changed
bool update_opinions()
{

    // (5) For each node, calculate the majority opinion among its neighbours and update the node's opinion.
    // Return true if any node's opinion changed, false otherwise.

    int n = opinions.size();
    std::vector<int> new_opinions = opinions;
    bool changed = false;

    for (int i = 0; i < n; ++i) {
        int majority = get_majority_friend_opinions(i);

        if (new_opinions[i] != majority) {
            new_opinions[i] = majority;
            changed = true;
        }
    }
        
    // Apply all changes at once
    if (changed) {
        opinions = std::move(new_opinions);
    }
    
    return changed;

}

int main() {
    // no preallocation; vectors grow on demand

    // Read input files
    read_opinions("opinions.txt"); 
    read_edges("edge_list.txt");

    // convert edge list into adjacency matrix once we know total_nodes
    build_adj_matrix();
    
    cout << "Total nodes: " << total_nodes << endl;
    
    // Run simulation
    int max_iterations = 30;
    int iteration = 0;
    bool opinions_changed = true;
    
    // Print initial state
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << calculate_fraction_of_ones() << endl;
    

    // (6) Run until consensus or max iterations
    while (opinions_changed && iteration < max_iterations)
    {
        // Update opinions based on neighbor majority
        opinions_changed = update_opinions();

        if (opinions_changed) {
            iteration++;
            cout << "Iteration " << iteration << ": fraction of 1's = " 
                 << calculate_fraction_of_ones() << endl;
        }
    }


    ////////////////////////////////////////////////////////
    // Print final result
    double final_fraction = calculate_fraction_of_ones();
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << final_fraction << endl;
    
    if(final_fraction == 1.0)
        cout << "Consensus reached: all 1's" << endl;
    else if(final_fraction == 0.0)
        cout << "Consensus reached: all 0's" << endl;
    else
        cout << "No consensus reached after " << iteration << " iterations" << endl;
    
    return 0;
}


/*********** Functions to read files **************************/ 

// Read opinion vector from file.
void read_opinions(string filename)
{
    ifstream file(filename);
    int id, opinion;
    while(file >> id >> opinion)
    {
        opinions.push_back(opinion);
        if(id >= total_nodes) total_nodes = id+1;
    }
    file.close();
}

// Read edge list from file and update total nodes as needed.
void read_edges(string filename)
{
    ifstream file(filename);
    int source, target;
    
    while(file >> source >> target)
    {
        edge_list.push_back({source, target});
        if(source >= total_nodes) total_nodes = source+1;
        if(target >= total_nodes) total_nodes = target+1;
    }
    file.close();
}

/********************************************************************** */
