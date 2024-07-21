import numpy as np
from pymoo.core.problem import ElementwiseProblem
from pymoo.algorithms.soo.nonconvex.ga import GA
from pymoo.optimize import minimize
from pymoo.visualization.scatter import Scatter

from testbench import run_test
import random

# Define the custom objective function
def objective_function(x):
    args = list(map(str, x))
    score = 0
    for T in range(10):
        test_id = random.randint(0, 99)
        score += run_test(test_id, args)
    return -score


# Define the optimization problem
class MyProblem(ElementwiseProblem):

    def __init__(self):
        super().__init__(n_var=12,  # Number of decision variables (inputs)
                         n_obj=1,  # Number of objectives
                         n_constr=0,  # Number of constraints
                         xl=-10,  # Lower bounds of decision variables
                         xu=10)  # Upper bounds of decision variables

    def _evaluate(self, x, out, *args, **kwargs):
        out["F"] = objective_function(x)

# Create the problem instance
problem = MyProblem()

# Define the algorithm to use (Genetic Algorithm in this case)
algorithm = GA(pop_size=100)

# Perform the optimization
res = minimize(problem,
               algorithm,
               ('n_gen', 200),  # Number of generations
               seed=1,
               verbose=True)

# Print the optimization result
print("Best solution found: %s" % res.X)
print("Function value: %s" % res.F)

# Plot the result
# plot = Scatter()
# plot.add(problem.pareto_front(), plot_type="line", color="black", alpha=0.7)
# plot.add(res.F, color="red")
# plot.show()