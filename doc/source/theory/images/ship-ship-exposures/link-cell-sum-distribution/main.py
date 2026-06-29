import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import scipy.stats as stats

# Close all existing plots
plt.close('all')
# Create a figure with two subplots side by side
fig, (ax1, ax2) = plt.subplots(1, 2)

# Width of ship 0
b0 = 33.4
# Width of ship 1
b1 = 20.0
# Mean value of the normal distribution
mu = 50
# Standard deviation of the normal distribution
sigma = 150
# Left boundary of the uniform distribution
x0 = -150
# Right boundary of the uniform distribution
x1 = 500
# Number of segments for the uniform distribution
N = 5
# Lower limit for the grid
value_min = -600
# Upper limit for the grid
value_max = 600

# Calculate the distance between the centrelines of the two ships
b01 = (b0 + b1) / 2

# Generate a grid of 1000 points between value_min and value_max
grid = np.linspace(value_min, value_max, 1000)

# Define a uniform distribution between x0 and x1
uni = stats.uniform(loc=x0, scale=(x1 - x0))
# Calculate the CDF of the uniform distribution shifted by b01
uni_cdf_pos = uni.cdf(grid + b01)
uni_cdf_neg = uni.cdf(grid - b01)
# Calculate the difference in CDFs
uni_cdf_delta = uni_cdf_pos - uni_cdf_neg

# Define a normal distribution with mean mu and standard deviation sigma
norm = stats.norm(loc=mu, scale=sigma)
# Calculate the PDF of the normal distribution
norm_pdf = norm.pdf(grid)
# Calculate the product of the normal PDF and the CDF delta
pdf = norm_pdf * uni_cdf_delta
# Integrate the PDF to get the probability P
P = np.trapz(pdf, grid)

# Plot the uniform PDF, normal PDF, and the combined PDF
ax1.plot(grid, uni.pdf(grid), '-', color='blue', label='Uniform')
ax1.plot(grid, norm.pdf(grid), '-', color='green', label='Normal')
ax1.plot(grid, pdf, '-', color='orange', label='Exposure')
ax1.legend(loc='best')
ax1.set_ylabel('Probability [-]')
ax1.set_title('N=1, ' + r'$P_{ij}=$' + '{:.6f}'.format(P), fontsize=10)

# Initialize an empty list to store probabilities for sub-intervals
P_subs = []
# Calculate the length of each sub-interval
grid_sub_length = (x1 - x0) / N
for i in range(N):
    # Define the start of the current sub-interval
    x_start = x0 + grid_sub_length * i

    # Define a uniform distribution for the current sub-interval
    uni_sub = stats.uniform(loc=x_start, scale=grid_sub_length)
    # Calculate the PDF and CDF delta for the sub-interval
    uni_sub_pdf = uni_sub.pdf(grid) / N
    uni_sub_cdf_pos = uni_sub.cdf(grid + b01) / N
    uni_sub_cdf_neg = uni_sub.cdf(grid - b01) / N
    uni_sub_cdf_delta = uni_sub_cdf_pos - uni_sub_cdf_neg

    # Calculate the PDF for the sub-interval
    norm_sub_pdf = norm.pdf(grid)
    pdf_sub = norm_sub_pdf * uni_sub_cdf_delta
    # Integrate the PDF to get the probability for the sub-interval
    P_sub = np.trapz(pdf_sub, grid)
    P_subs.append(P_sub)

    # Plot the uniform PDF and combined PDF for the sub-interval
    ax2.plot(grid, uni_sub_pdf, '-', color='blue')
    ax2.plot(grid, pdf_sub, '-', color='orange')

# Plot the normal PDF and add legend and title
ax2.set_yticks([])
ax2.plot(grid, norm.pdf(grid), '-', color='green')
ax2.set_title('N=' + str(N) + ', ' +
              r'$P_{ij}=$' + '{:.6f}'.format(np.sum(P_subs)), fontsize=10)

# Add a title to the figure
plt.suptitle('PDF' +
             r'$,\mu=$' + '{:.0f}'.format(mu) +
             r'$ ,\sigma=$' + '{:.0f}'.format(sigma) +
             r'$ ,\overline{b}_{ij}=$' + '{:.0f}'.format(b01) +
             r'$, y_0=$' + '{:.0f}'.format(x0) +
             r'$, \Delta{y}=$' + '{:.0f}'.format(x1 - x0),
             fontsize=12)
