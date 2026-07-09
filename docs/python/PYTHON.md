# Python

## Getting started in Python

Python interfaces can be used by importing the Cython wrapper. 

Please, place (or copy) the bin folder one level below your python file. 

You can then import the wrapper like this:

```python
from bin.wrapper import PWSolver
```

The interface to the library is provided by the PWSolver class. To build it:

```python
pathwyse = PWSolver()
```

A list of all the methods provided by PWSolver can be found [here](PYTHON_INTERFACES_FULL.md).

---

## Examples

- [Solving from an instance file](PYTHON_EXAMPLE_FILE.md)
- [Creating an instance programmatically](PYTHON_EXAMPLE_CREATE.md)
