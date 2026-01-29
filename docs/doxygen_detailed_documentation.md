# Details {#details-page}

ALDCCsim is a C++ command-line tool designed to solve linear electrical circuits using Modified Nodal Analysis (MNA). The solver supports resistors, independent voltage sources, and independent current sources. It also features a bisection-based optimization engine to find specific component values required to achieve a desired circuit output (voltage, current, or power).

### Below is a full description of the program focusing on Analysis, Internal Specification and Testing. For External Specification read the [main](@ref main-page) page

---

## Problem and analysis
![](../images/aldcc-task-2.jpg)

Based on the provided code and the task description image, the core problem is the development of a software simulation tool capable of analyzing linear direct current (DC) circuits.
The fundamental objective is to parse a textual representation of a circuit, commonly referred to as a netlist, and calculate the steady-state operating point for every component.
This involves determining the voltage across, current through, and power dissipated by each resistor, voltage source, and current source in the network.

The mathematical foundation selected to solve this problem is Modified Nodal Analysis (MNA).
This approach is necessary because the circuit elements include independent voltage sources, which standard nodal analysis cannot easily handle without supernodes.
MNA addresses this by expanding the system of linear equations to solve simultaneously for node potentials and the currents flowing through voltage sources.
The program can construct an augmented matrix where rows correspond to Kirchhoff's Current Law (KCL) for each node and the constitutive equations for voltage sources,
solving the system Ax=b using Gaussian elimination with partial pivoting to ensure numerical stability.

Several improvements can be made on top of what the task specifies.
I decided to add a simple parallel voltage source check and a check for nodes with only current sources connected since these configurations result in the circuit being indeterminate.
Obviously in some cases we need the output to be more precise than others and too high precision at all times results in the output being less readable.
This is why I also added a third command line switch allowing the user to specify precision of output parameters.

The task mentions only solving a system with known element data and outputting currents, voltages and power balance, but in real use cases I found that I have to perform the opposite almost just as often.
This is why I additionally implemented [search mode](#bisection-optimization) which uses bisection to find some element parameter knowing one of the currents, voltages or power balances.
The program still isn't suitable for solving more elaborate problems, but this add-on makes its applicability significantly higher.

I also decided to replace the `I` current source symbol from the task decription with `J` as it is more commonly used and can't be misinterpretted as current.

## Internal specification
### Global Context
The simulation state is aggregated into a central data structure, [Context](@ref Context), which serves as the primary data transfer object between modules.

- Element Storage: All circuit components are stored in a std::vector<[Element](@ref Element)>, where each Element contains its type, connection nodes, value, and computed results (voltage, current, power).
- Equation System: The MNA system is represented by an augmented matrix (std::vector<std::vector<double>> equations), where the number of unknowns corresponds to the maximum node index plus the number of independent voltage sources.
- Bisection State: To support the optimization mode, the context holds a pointer to a [variable_elem](@ref variable_elem) (the component being tuned) and a [GivenValue](@ref GivenValue) structure defining the target metric (desired_value) and type (v_type).

### Mathematical Core
#### Modified Nodal Analysis (MNA)

The solver constructs a linear system `Ax=b` to determine the circuit state.

Equation Formulation:
- KCL Equations: Generated for every non-ground node, summing currents leaving the node.
- Constitutive Equations: Generated for voltage sources, introducing the source current as an additional unknown.
- Ground Reference: The potential of Node `1` is explicitly constrained to zero.

Matrix Solution:
- The system is solved using Gaussian elimination with partial pivoting to maximize numerical stability before decoding the results into node potentials and source currents.

#### Bisection Optimization

When a variable element is detected, the bisect module wraps the MNA solver in an iterative loop.
- Monotonicity Check: The variable element is perturbed by [EPSILON](@ref EPSILON) (2E-10) to determine the sign of the derivative of the output with respect to the input.
- Bound Search: The search interval is iteratively doubled until the target value is bracketed by the output range.
- Convergence: The algorithm performs a fixed number of iterations ([BISECTION_PREC](@ref BISECTION_PREC) = 50) to narrow the interval, updating the [output_value](@ref output_value) in the [Context](@ref Context) after every step.

### Validation and Error Handling

The system includes pre-solve validation to reject topologically invalid circuits:
- Indeterminate Nodes: Detects nodes connected exclusively to current sources, which result in undefined node potentials.
- Parallel Voltage Sources: Identifies voltage sources sharing the same node pair, which creates a singular matrix due to indeterminate loop currents.
- Singular Matrices: The Gaussian elimination process throws a runtime error if inconsistent or dependent equations (e.g., 0 = non-zero) are encountered.

### Output Generation

The simulator writes a comprehensive report to the specified output file.
- Element Table: Lists every element's type, nodes, and calculated Voltage (U), Current (I), and Power (P).
- Potentials: Lists the calculated electric potential (V) for every node in the circuit.
- Formatting: All floating-point values are rounded to the user-specified precision (defaulting to 4 decimal places) using a [scaling factor](@ref prec_factor), with specific handling to prevent negative zero outputs.

---

## Testing - Basic Mode
### Example 1

![](../images/example1.jpg)

Command:
`ALDCCsim.exe -i data.txt -o solution.txt -p 5`

data.txt:
```
E 1 2 4
R 2 3 16
R 1 3 16
R 3 4 8
R 1 4 8
J 1 4 6
```

> Note: Because we marked the ground node on the left as node `1`, we can do the same on the right - they are effectively the same node.

solution.txt:
```
 Element |      U[V]     |      I[A]     |      P[W]     
---------+---------------+---------------+---------------
   E12   |             4 |      -0.83333 |       -3.3333 
   R23   |        13.333 |       0.83333 |        11.111 
   R13   |        17.333 |        1.0833 |        18.778 
   R34   |        15.333 |        1.9167 |        29.389 
   R14   |        32.667 |        4.0833 |        133.39 
   J14   |        32.667 |             6 |           196 

 Node |      V[V]     
------+---------------
  1   |             0 
  2   |             4 
  3   |        17.333 
  4   |        32.667 
```
---
### Example 2

![](../images/example2.jpg)

Command:
`ALDCCsim.exe -i data.txt -o solution.txt`

data.txt:
```
R 2 1 4
E 3 2 20
R 3 1 12
R 1 4 4
E 1 6 0
R 3 6 2
R 6 4 4
R 5 4 8
J 5 6 3
```

> Note: In this circuit we can model an ideal ammeter using a `0V` voltage source.

solution.txt:
```
 Element |      U[V]     |      I[A]     |      P[W]     
---------+---------------+---------------+---------------
   R21   |           -14 |          -3.5 |            49 
   E32   |            20 |           3.5 |            70 
   R31   |             6 |           0.5 |             3 
   R14   |            -6 |          -1.5 |             9 
   E16   |             0 |           1.5 |             0 
   R36   |             6 |             3 |            18 
   R64   |            -6 |          -1.5 |             9 
   R54   |            24 |             3 |            72 
   J56   |            30 |             3 |            90 

 Node |      V[V]     
------+---------------
  1   |             0 
  2   |            14 
  3   |            -6 
  4   |            -6 
  5   |           -30 
  6   |             0 
```
---
## Testing - Search Mode
### Example 1

![](../images/example3.jpg)

Command:
`ALDCCsim.exe -i data.txt -o solution.txt`

data.txt:
```
E 1 2 10
R 2 3 x
R 3 1 50 U 5
```

solution.txt:
```
Answer: x = 50

 Element |      U[V]     |      I[A]     |      P[W]     
---------+---------------+---------------+---------------
   E12   |            10 |           0.1 |             1 
   R23   |            -5 |          -0.1 |           0.5 
   R31   |            -5 |          -0.1 |           0.5 

 Node |      V[V]     
------+---------------
  1   |             0 
  2   |            10 
  3   |             5 
```
### Example 2

![](../images/example4.jpg)

Command:
`ALDCCsim.exe -i data.txt -o solution.txt -p 7`

data.txt:
```
E 5 1 x
R 1 2 3.8
R 2 3 3.8
J 4 2 1.2
R 5 4 3.8
E 5 3 18.5 P 4.9
```

solution.txt:
```
Answer: x = 12

 Element |      U[V]     |      I[A]     |      P[W]     
---------+---------------+---------------+---------------
   E51   |            12 |          -1.5 |           -17 
   R12   |           5.6 |           1.5 |           8.2 
   R23   |             1 |          0.26 |          0.27 
   J42   |            22 |           1.2 |            26 
   R54   |          -4.6 |          -1.2 |           5.5 
   E53   |            18 |          0.26 |           4.9 

 Node |      V[V]     
------+---------------
  1   |             0 
  2   |           5.6 
  3   |           6.6 
  4   |           -16 
  5   |           -12 
```

The same example with higher precision:

Command:
`ALDCCsim.exe -i data.txt -o solution.txt -p 5`

data.txt:
```
E 5 1 x
R 1 2 3.8
R 2 3 3.8
J 4 2 1.2
R 5 4 3.8
E 5 3 18.5 P 4.9
```

solution.txt:
```
Answer: x = 11.927

 Element |      U[V]     |      I[A]     |      P[W]     
---------+---------------+---------------+---------------
   E51   |        11.927 |       -1.4649 |       -17.471 
   R12   |        5.5665 |        1.4649 |        8.1541 
   R23   |        1.0065 |       0.26486 |       0.26658 
   J42   |        22.054 |           1.2 |        26.464 
   R54   |         -4.56 |          -1.2 |         5.472 
   E53   |          18.5 |       0.26486 |           4.9 

 Node |      V[V]     
------+---------------
  1   |             0 
  2   |        5.5665 
  3   |         6.573 
  4   |       -16.487 
  5   |       -11.927 
```
---
### Example 3

![](../images/example5.jpg)

Command:
`ALDCCsim.exe -i data.txt -o solution.txt -p 7`

data.txt:
```
J 3 1 x P 5.5
R 1 2 1
R 2 3 5
R 2 3 20
```

solution.txt:
```
Answer: x = 1.048809

 Element |      U[V]     |      I[A]     |      P[W]     
---------+---------------+---------------+---------------
   J31   |      5.244044 |      1.048809 |           5.5 
   R12   |     -1.048809 |     -1.048809 |           1.1 
   R23   |     -4.195235 |    -0.8390471 |          3.52 
   R23   |     -4.195235 |    -0.2097618 |          0.88 

 Node |      V[V]     
------+---------------
  1   |             0 
  2   |     -1.048809 
  3   |     -5.244044 
```