# Social Network Analyzer

A C++ application that allows you to analyze and manage a social network. The project implements various graph algorithms and data structures to provide insights about user connections and community structures.

## Features

- User Management

  - Add new users with profile information (name, age, location, interests)
  - View all users in the network
  - Search users by name, location, or interests using advanced string matching algorithms (KMP and Rabin-Karp)

- Connection Management

  - Add connections between users
  - View user relationships
  - Get friend recommendations based on network topology

- Network Analysis

  - Community detection using Kruskal's algorithm
  - Find shortest paths between users (Floyd-Warshall algorithm)
  - Network traversal using BFS and DFS
  - Visualization of network metrics

- Data Persistence
  - Load/Save network data in JSON format
  - Export data to CSV format
  - Automatic data persistence between sessions

## Prerequisites

- C++17 compiler (g++ recommended)
- Make build system
- Unix-like environment (Linux/MacOS) or Windows with appropriate build tools

## Building the Project

1. Build the project using make:

```bash
make
```

This will:

- Create necessary directories (bin/ and obj/)
- Compile all source files
- Generate the executable in bin/

## Running the Application

After building, you can run the application using:

```bash
./bin/social_network_analyzer
```

The application will automatically load existing network data from `data/Network.json` if it exists.

## Project Structure

```
.
├── Makefile              # Build configuration
├── bin/                  # Compiled binary
├── data/                 # Data files (Network.json)
├── include/             # Header files
│   ├── Graph.hpp        # Graph data structure
│   ├── NetworkParser.hpp # Data parsing utilities
│   ├── StringSearch.hpp # String matching algorithms
│   └── User.hpp         # User class definition
├── src/                 # Source files
│   ├── Graph.cpp        # Graph implementation
│   ├── main.cpp         # Main program
│   ├── NetworkParser.cpp # Parser implementation
│   ├── StringSearch.cpp # Search algorithms
│   └── User.cpp         # User class implementation
└── obj/                 # Object files
```

## Algorithm Implementation Details

1. **String Matching**

   - Knuth-Morris-Pratt (KMP) algorithm for efficient string searching
   - Rabin-Karp algorithm for pattern matching

2. **Graph Algorithms**
   - Breadth-First Search (BFS) for friend recommendations
   - Depth-First Search (DFS) for network traversal
   - Floyd-Warshall algorithm for all-pairs shortest paths
   - Kruskal's algorithm for community detection

## Data Format

The application uses JSON format for data storage. Example structure:

```json
{
  "users": [
    {
      "id": "user1",
      "name": "John Smith",
      "age": 25,
      "location": "New York",
      "interests": ["programming", "music"]
    }
  ],
  "connections": [
    {
      "user1": "user1",
      "user2": "user2"
    }
  ]
}
```

## Clean Build

To clean the build files and start fresh:

```bash
make clean
```

## Contributing

Feel free to submit issues and enhancement requests!
