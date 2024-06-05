# PathWyse Community

PathWyse is a dual licensed C++ library designed to provide state-of-the-art algorithms and data structures for modeling and solving a variety of standard resource constrained shortest path problems (RCSPPs). The library can be compiled and used as a standalone solver or can be imported in custom projects through C++ or Python interfaces. Furthermore, algorithms and problem properties can be extended and customize to provide ad-hoc solutions.

The library is introduced in this paper: https://doi.org/10.1080/10556788.2023.2296978

Otherwise, an earlier version is described in this technical report: https://doi.org/10.48550/arXiv.2306.08622

We propose two versions of the library, with a dual-license scheme. See the license section for more details.

PathWyse Community, this repository, represents the core of the library and adheres to an open source license. 

PathWyse Enterprise, a version with additional and experimental features, along with more extensive support, can be provided upon request, through a commercial license for non-open source projects.
Additionally, PathWyse Enterprise binaries can be provided free-to-use for academic purposes. 

## Supported Problems
PathWyse supports resource constrained problems with single or multiple monotone resources. It provides specialized algorithms and techniques for both cyclic and acyclic networks.

Version 0.1 implements the following resources: capacity, time, node limit and time windows.

Additionally, custom resources can be specified by extending the Resource Class.

## Main Techniques
PathWyse includes the following techniques and features:

- **Bi-directional search:** Dynamic programming labeling algorithms are implemented in both mono-directional and bi-directional [Righini and Salani, 2006] fashion; 
- **Relaxation schemes:** Decremental State Space Relaxation (DSSR) [Righini and Salani, 2008], NG-path relaxations (NG) [Baldacci et al., 2011] and hybridizations [Martinelli et al., 2014] are included to tackle the Elementary RCSPP;
- **Dynamic half-way point:** The budget of the critical resource is automatically adjusted in bidirectional algorithms, similarly to [Sadykov et al., 2020]
- **Multiple candidate and join procedure selection:** Multiple methods to select a candidate to extend in labeling algorithms. Different join procedures are available for bidirectional search;
- **A\*-based algorithm:** a custom implementation of the A* algorithm proposed by [Ahmadi et al. 2021] is provided for single resource acyclic problems.

PathWyse also supports simple heuristics, and features data collection tools.

## Getting started
To start using PathWyse as a solver, please compile the project, by following the instructions provided [here](documentation/tutorials/INSTALL.md).

Then, you can run the standalone version in a terminal by providing an instance file:

```
./bin/pathwyse path-to-instance
```

For example:

```
./bin/pathwyse instances/toy.txt
```

The instance file format is specified [here](documentation/tutorials/INSTANCE.md).

A default setting file "pathwyse.set" is provided to change the library configuration. Full details on all the available settings can be found [here](documentation/tutorials/PARAMETERS.md).

Please refer to [this](documentation/tutorials/PYTHON.md) guide to import PathWyse library in your Python project instead.

Additional information about launch parameters and example instances can be found in [extras](documentation/tutorials/EXTRAS.md).

## Documentation
Additional documentation for the library is provided in the documentation folder. See documentation.html. 

Please, refer to the Solver class for details of the main interfaces of the library.

## License
We provide Pathwyse under a dual licensing scheme. 

PathWyse Community is open source, and adheres to the GNU General Public License v3.0. Please, refer to the LICENSE file for full details.

PathWyse Enterprise is instead provided upon request with a commercial license for non-open source projects. 


Please contact pathwyse@supsi.ch for further inquiries.

## Version History


##### PathWyse Community (Open Source)
Version 0.1 - June 2024 - GitHub Repository

##### PathWyse Enterprise (binaries free-to-use for academic projects)
Version 1.0 - June 2024 - binaries, upon request (pathwyse@supsi.ch)

##### PathWyse Enterprise (closed source, for non-open source projects)
Version 1.0 - June 2024 - upon request (pathwyse@supsi.ch)


## How to cite
Salani, M., Basso, S., & Giuffrida, V. (2024). PathWyse: a flexible, open-source library for the resource constrained shortest path problem. Optimization Methods and Software, 1–23. https://doi.org/10.1080/10556788.2023.2296978
