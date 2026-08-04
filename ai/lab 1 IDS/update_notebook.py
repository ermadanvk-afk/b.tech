import json

file_path = "c:/Users/udayv/Desktop/5th sem/ai/lab 1 IDS/IDS.ipynb"

with open(file_path, "r", encoding="utf-8") as f:
    notebook = json.load(f)

dls_code = """def depth_limited_search(graph, current_node, goal_node, depth_limit, visited, path):
    visited.append(current_node)
    path.append(current_node)
    
    if current_node == goal_node:
        return path
    
    if depth_limit <= 0:
        path.pop()
        return None
    
    if current_node in graph:
        for neighbor in graph[current_node]:
            result = depth_limited_search(graph, neighbor, goal_node, depth_limit - 1, visited, path)
            if result is not None:
                return result
                
    path.pop()
    return None"""

ids_code = """def iterative_deepening_search():
    num_nodes = int(input("Enter number of nodes: "))
    nodes = []
    for i in range(num_nodes):
        x = input(f"Enter node {i+1}: ")
        nodes.append(x)
        
    edge = int(input("Enter number of edges: "))
    graph = {}
    print("Enter edges (format: source destination):")
    for i in range(edge):
        u, v = input().split()
        if u not in graph:
            graph[u] = []
        graph[u].append(v)
        
    start_node = input("Enter start node: ")
    goal_node = input("Enter goal node: ")
    
    depth = 0
    while True:
        visited = []
        path = []
        print(f"\\nDepth Limit: {depth}")
        result = depth_limited_search(graph, start_node, goal_node, depth, visited, path)
        print(f"Nodes visited at this limit: {visited}")
        
        if result is not None:
            print(f"Goal found! Path from start to goal: {' -> '.join(result)}")
            break
        depth += 1

iterative_deepening_search()"""

notebook['cells'][1]['source'] = dls_code.splitlines(keepends=True)
notebook['cells'][2]['source'] = ids_code.splitlines(keepends=True)
notebook['cells'][2]['outputs'] = []
if 'execution_count' in notebook['cells'][2]:
    notebook['cells'][2]['execution_count'] = None

with open(file_path, "w", encoding="utf-8") as f:
    json.dump(notebook, f, indent=1)
