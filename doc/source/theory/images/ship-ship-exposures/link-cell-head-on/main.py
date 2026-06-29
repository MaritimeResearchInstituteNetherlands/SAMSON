import matplotlib.pyplot as plt
import numpy as np
import scipy.stats as stats
from scipy.stats import norm

plt.close('all')

# Width ship 0 and ship 1
b = 33.4
# Mu-value for both ship 0 and ship 1
mu = 300
# sigma-value for both ship 0 and ship 1
sigma = 150
# Left boundary grid
value_min = -500
# Right boundary grid
value_max = 500


# IWRAP Test Case
#################

# Mean sailing distance between ship 0 and ship 1
mu12 = 2 * mu
# Standard deviation of the joint distribution
sigma12 = np.sqrt(2.0 * sigma**2)
# calculate the probability
P = norm.cdf(b, loc=mu12, scale=sigma12) - \
    norm.cdf(-b, loc=mu12, scale=sigma12)

# Print the integrated probability
print('Probability directly computed with CDF:')
print(P)


# IWRAP Test Case Numerical approach
####################################

# Generate a grid of 1000 points between value_min and value_max
grid = np.linspace(value_min, value_max, 1000)

# Define two normal distributions with means -mu and mu, and standard deviation sigma
norm_check0 = stats.norm(loc=-mu, scale=sigma)
norm_check1 = stats.norm(loc=mu, scale=sigma)

# Calculate the PDF of the first normal distribution
pdf = norm_check0.pdf(grid)
# Calculate the CDF of the second normal distribution shifted by b
cdf_pos = norm_check1.cdf(grid + b)
cdf_neg = norm_check1.cdf(grid - b)
# Calculate the difference in CDFs
cdf_delta = cdf_pos - cdf_neg
# Calculate the product of the PDF and the CDF delta
p = pdf * cdf_delta
# Integrate the product to get the probability Pnew
Pnew = np.trapz(p, grid)

# Print the integrated probability
print('Probability numerically integrated with PDF and CDF:')
print(Pnew)

# Plot the resulting function
plt.plot(grid, p, label='Gaussian')
plt.legend()
plt.show()
