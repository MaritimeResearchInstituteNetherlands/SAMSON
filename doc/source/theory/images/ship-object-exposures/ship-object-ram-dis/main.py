import numpy as np
from scipy.stats import norm
import matplotlib.pyplot as plt

plt.close('all')

samples = 1000

angle = np.array([-30, -20, -10, 0, 10, 20, 30])
angle_plot = np.linspace(3*angle[0], 3*angle[-1], samples)

prob_bin = angle[1] - angle[0]
prob_tab = np.array([0.05, 0.10, 0.20, 0.30, 0.20, 0.10, 0.05])
prob_area = np.sum(prob_bin * prob_tab)

prob = prob_tab * 20
prob_int = prob.astype(int)
data = [] 
for i, iprob_int in enumerate(prob_int):
    for j in range(iprob_int):
        data.append(angle[i])
 
mu, sigma = norm.fit(data)        

n = 3
ylim = 0.04

fig, (ax1, ax2) = plt.subplots(1,2)

ax1.plot(angle, prob_tab/prob_area, 'ob')
ax1.bar(angle, prob_tab/prob_area, width=prob_bin, color='b',
        alpha=0.5, edgecolor='b', linewidth=2)
ax1.plot([-n*sigma, -n* sigma], [0,10], ':k')
ax1.plot([n*sigma, n* sigma], [0,10], ':k')

ax1.plot(angle_plot, norm.pdf(angle_plot, mu, sigma), 'k')
ax1.set_xlim([angle_plot[0], angle_plot[-1]])
ax1.set_ylim([0, ylim])
ax1.set_xlabel('Heading Deviation [deg]')
ax1.set_ylabel('Probability [-]')
ax1.text(-4.0*sigma, 0.25*ylim,r'$-3\sigma$', ha='center')
ax1.text( 4.0*sigma, 0.25*ylim,r'$+3\sigma$', ha='center')
ax1.set_title('pdf' +
              r'$,\mu=$' + '{:.2f}'.format(mu) +
              r'$ ,\sigma=$' + '{:.2f}'.format(sigma))

ax2.plot(np.deg2rad([-n*sigma, -n* sigma]), [0,10], ':k')
ax2.plot(np.deg2rad([n*sigma, n* sigma]), [0,10], ':k')
ax2.plot(np.deg2rad(angle_plot), norm.pdf(np.deg2rad(angle_plot), np.deg2rad(mu), np.deg2rad(sigma)), 'k')
ax2.set_xlim(np.deg2rad([angle_plot[0], angle_plot[-1]]))
ax2.set_ylim([0, np.rad2deg(ylim)])
ax2.set_xlabel('Heading Deviation [rad]')
ax2.text(-4.0*np.deg2rad(sigma), 0.25*np.rad2deg(ylim), r'$-3\sigma$', ha='center')
ax2.text( 4.0*np.deg2rad(sigma), 0.25*np.rad2deg(ylim), r'$+3\sigma$', ha='center')
ax2.set_title('pdf' +
              r'$,\mu=$' + '{:.2f}'.format(np.deg2rad(mu)) +
              r'$ ,\sigma=$' + '{:.4f}'.format(np.deg2rad(sigma)))


