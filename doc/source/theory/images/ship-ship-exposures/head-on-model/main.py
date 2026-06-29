import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import norm
from matplotlib.patches import Polygon

plt.close('all')

speeds = np.array([7.58, 7.58])
breadths = np.array([33.4, 33.4])
lengths = np.array([214, 214])
mus = np.array([300, 300])
sigmas = np.array([150, 150])
waypoints = np.array([[-600, -100], [600, 100]])
links = np.array([[waypoints[0], waypoints[1]],
                  [waypoints[1], waypoints[0]]])

ship = np.array([[-0.5, -0.3], [-0.4, -0.5], [0.3, -0.5], [0.4, -0.3],
                [0.5, 0.0], [0.4, 0.3], [0.3, 0.5], [-0.4, 0.5], [-0.5, 0.3]])
contour = np.array([[-0.5, -0.5], [0.5, -0.5], [0.5, 0.5], [-0.5, 0.5]])

plot_scale = 30


def create_vector(link):
    vector = link[1]-link[0]
    vector_len = np.sqrt(vector[0]**2 + vector[1]**2)
    vector_norm = vector / vector_len
    return vector_norm


def rotation_matrix_2D(psi, x0):
    R = np.array([[np.cos(psi), -np.sin(psi)],
                  [np.sin(psi), np.cos(psi)]])
    x1 = R.dot(x0)
    return x1


mu12 = mus[0] + mus[1]
sigma12 = np.sqrt(sigmas[0]**2 + sigmas[1]**2)
breadth12 = np.sum(breadths) / 2.0
term1 = norm.cdf(+breadth12, loc=mu12, scale=sigma12)
term2 = norm.cdf(-breadth12, loc=mu12, scale=sigma12)
P = term1 - term2

origin = []
dx = []
dy = []
angle = []
vector = []
u = []
for i in range(2):
    origin.append(links[i][0])
    dx.append(links[i, 1, 0] - links[i, 0, 0])
    dy.append(links[i, 1, 1] - links[i, 0, 1])
    angle.append(np.arctan2(dy[-1], dx[-1]))
    vector.append(create_vector(links[i, :, :]))
    u.append(vector[-1] * speeds[i])

fig, ax = plt.subplots()

for i in range(2):
    lateral = 6.0*np.arange(-sigmas[i], sigmas[i], 0.01)
    pdf = norm.pdf(lateral, loc=mus[i], scale=sigmas[i])
    if i == 0:
        auto_scale = 2.0*sigmas[i] / max(pdf)
        label_link = 'Link'
        label_vel = 'Velocity'
    else:
        label_link = ''
        label_vel = ''
    dist_poly = np.array([auto_scale * pdf, lateral])
    dist_poly = rotation_matrix_2D(angle[i], dist_poly)
    dist_poly[0, :] += origin[i][0]
    dist_poly[1, :] += origin[i][1]
    plot_dis = Polygon(np.transpose(dist_poly), facecolor='b', alpha=0.2)
    ship_poly = np.array([ship[:, 0] * lengths[i], ship[:, 1] * breadths[i]])
    ship_poly[0, :] += 2.0 * lengths[i]
    ship_poly[1, :] += mus[i]
    ship_poly = rotation_matrix_2D(angle[i], ship_poly)
    ship_poly[0, :] += origin[i][0]
    ship_poly[1, :] += origin[i][1]
    plot_ship = Polygon(np.transpose(ship_poly), facecolor='g')
    ax.arrow(origin[i][0], origin[i][1], dx[i], dy[i],
             head_width=30, head_length=50, length_includes_head=True,
             fc='k', ec='k', label=label_link)
    ax.add_patch(plot_dis)
    ax.add_patch(plot_ship)
    arrow = np.array([2.0 * lengths[i], mus[i]])
    arrow = rotation_matrix_2D(angle[i], arrow)
    arrow[0] += origin[i][0]
    arrow[1] += origin[i][1]
    ax.arrow(arrow[0], arrow[1], plot_scale*u[i][0],
             plot_scale*u[i][1], head_width=30, head_length=50,
             linewidth=2, fc='blue', ec='blue', label=label_vel)

ax.axes.get_xaxis().set_visible(False)
ax.axes.get_yaxis().set_visible(False)
plt.title('Head-on Exposure Model')
plt.show()
plt.legend()
plt.axis('equal')

print(P)

# https://en.wikipedia.org/wiki/Sum_of_normally_distributed_random_variables#:~:text=This%20means%20that%20the%20sum,squares%20of%20the%20standard%20deviations).
