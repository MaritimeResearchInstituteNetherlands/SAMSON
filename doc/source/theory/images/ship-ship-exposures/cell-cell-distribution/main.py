import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import uniform

plt.close('all')

# Width ship 0 and ship 1
b = 33.4
# Left boundary grid
value_min = -750
# Right boundary grid
value_max = 750

xdelta = [500, 250, 125]
# xdelta > 2*b
length = [750, 1500, 3000]
rho = 3.0

color = ['blue', 'green', 'orange']

for idelta, ilength, icolor in zip(xdelta, length, color):

    area = idelta * ilength
    print('Area:')
    print(area)
    Q = rho * area / ilength
    print('Flow:')
    print(Q)

    # Calculate the probability
    # Only valid for condition b<idelta
    P0 = 1/idelta * (2*b - 2*b**2/(2*idelta))

    # # Print the integrated probability
    print('Probability directly computed with CDF:')
    print(P0)

    P1 = 1/idelta * 2*b

    print('Probability directly computed with CDF (modified):')
    print(P1)

    # Generate a grid of 1000 points between value_min and value_max
    grid = np.linspace(value_min, value_max, 10000)

    # Define two uniform distributions
    uni_check0 = uniform(loc=-idelta/2, scale=idelta)
    uni_check1 = uniform(loc=-idelta/2, scale=idelta)

    # Calculate the PDF of the first normal distribution
    pdf = uni_check0.pdf(grid)
    # Calculate the CDF of the second normal distribution shifted by b
    cdf_pos = uni_check1.cdf(grid + b)
    cdf_neg = uni_check1.cdf(grid - b)
    # Calculate the difference in CDFs
    cdf_delta = cdf_pos - cdf_neg
    # Calculate the product of the PDF and the CDF delta
    p = pdf * cdf_delta
    # Integrate the product to get the probability Pnew
    P2 = np.trapz(p, grid)
    label = r'$y_0=$' + '{:.1f}'.format(-idelta/2) + \
            r'$, \Delta{y}=$' + '{:.0f}'.format(idelta) + \
            r'$, P_{ij}=$' + '{:.4f}'.format(P0)
    plt.plot(grid, p, ':', color=icolor, label=label)
    p[p != 0] = max(p)
    P3 = np.trapz(p, grid)
    label = 'Modified' + \
            r'$, P_{ij}=$' + '{:.4f}'.format(P1)
    plt.plot(grid, p, '-', color=icolor, label=label)
    plt.xlim([-400, 400])
    plt.ylim([0, 0.005])
    plt.ylabel('Probability [-]')
    plt.title('PDF' + r'$ ,\overline{b}_{ij}=$' +
              '{:.1f}'.format(b), fontsize=12)

    # Print the integrated probability
    print('Probability numerically integrated with PDF and CDF:')
    print(P2)
    print('Probability numerically integrated with PDF and CDF (modified):')
    print(P3)

    head_on_model = P1 * ilength * Q**2
    print('Head-on model exposures (modified):')
    print(head_on_model)
    print(' ')

plt.legend()
plt.show()
