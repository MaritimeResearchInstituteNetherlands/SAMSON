import numpy as np
import matplotlib.pyplot as plt
import random
from matplotlib.patches import Polygon
from scipy import linalg
from scipy.stats import norm

plt.close('all')

link_num = 2

waypoints = np.array([[600.0, 600.0], [-300.0, 20.00],
                     [200.0, -700.0], [-1400.0, -150.0],
                     [1500.0, 200.0], [2000.0, 600.0],
                     [-500, 1000]])
radii = np.array([200, 320, 150, 200, 150, 175, 240])
links = np.array([[0, 2],
                  [1, 2],
                  [0, 1],
                  [3, 1],
                  [0, 4],
                  [5, 4],
                  [6, 1]])
links_radii = np.zeros((7, 2))


def create_polygon(waypoints, radii):
    # https://en.wikipedia.org/wiki/Tangent_lines_to_circles
    x1 = waypoints[0][0]
    y1 = waypoints[0][1]
    x2 = waypoints[1][0]
    y2 = waypoints[1][1]
    r1 = radii[0]
    r2 = radii[1]

    vec_len = np.sqrt((x2-x1)**2 + (y2-y1)**2)
    gamma = np.arctan2(y2 - y1, x2 - x1)
    beta = np.arcsin((r2 - r1) / vec_len)
    x4 = -r1 * np.sin(beta)
    y4 = -r1 * np.cos(beta)
    x5 = -r2 * np.sin(beta) + vec_len
    y5 = -r2 * np.cos(beta)

    gamma13 = np.pi
    gamma14 = np.arctan2(y4, x4) % (2 * np.pi)
    gamma25 = np.arctan2(y5, x5 - vec_len) % (2 * np.pi)
    gamma26 = 2 * np.pi

    n_segments = 32
    J = np.linspace(gamma13, gamma14, num=n_segments)
    arc_start = np.zeros((n_segments, 2))
    for j, jgamma in enumerate(J):
        arc_start[j, 0] = r1 * np.cos(jgamma)
        arc_start[j, 1] = r1 * np.sin(jgamma)

    K = np.linspace(gamma25, gamma26, num=n_segments)
    arc_end = np.zeros((n_segments, 2))
    for k, kgamma in enumerate(K):
        arc_end[k, 0] = vec_len + r2 * np.cos(kgamma)
        arc_end[k, 1] = r2 * np.sin(kgamma)

    bottom = np.concatenate((arc_start, arc_end), axis=0)
    top = np.copy(np.flip(bottom[1:-1], axis=0))
    top[:, 1] = -top[:, 1]
    closed = np.concatenate((bottom, top), axis=0)

    polygon = rotation_matrix_2D(gamma, closed.T)
    polygon[0, :] = polygon[0, :] + x1
    polygon[1, :] = polygon[1, :] + y1

    return polygon.T


def line_line_intersection(line0, line1):
    # https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
    x1 = line0[0][0]
    x2 = line0[1][0]
    x3 = line1[0][0]
    x4 = line1[1][0]
    y1 = line0[0][1]
    y2 = line0[1][1]
    y3 = line1[0][1]
    y4 = line1[1][1]
    px_nom = (x1*y2 - y1*x2) * (x3 - x4) - (x1 - x2) * (x3*y4 - y3*x4)
    py_nom = (x1*y2 - y1*x2) * (y3 - y4) - (y1 - y2) * (x3*y4 - y3*x4)
    denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)
    if denom != 0:
        x = px_nom / denom
        y = py_nom / denom
        p = np.array([x, y])
    else:
        p = np.array([])
    return p


def is_inside(edges, xp, yp):
    # https://gist.github.com/inside-code-yt/7064d1d1553a2ee117e60217cfd1d099
    # Ray casting algorithm
    cnt = 0
    for edge in edges:
        (x1, y1), (x2, y2) = edge
        if (yp < y1) != (yp < y2) and xp < x1 + ((yp-y1)/(y2-y1))*(x2-x1):
            cnt += 1
    return cnt % 2 == 1


def rotation_matrix_2D(psi, x0):
    R = np.array([[np.cos(psi), -np.sin(psi)],
                  [np.sin(psi), np.cos(psi)]])
    x1 = R.dot(x0)
    return x1


link_poly_all = []
for i, link in enumerate(links):
    link_coor = np.array([waypoints[link[0]], waypoints[link[1]]])
    radii_value = radii[[link[0], link[1]]]

    if links_radii[i, 0] != 0:
        radii_value[0] = links_radii[i, 0]
    if links_radii[i, 1] != 0:
        radii_value[1] = links_radii[i, 1]

    link_poly = create_polygon(link_coor, radii_value)
    link_poly_all.append(link_poly)
link_poly_all = np.array(link_poly_all)

fig, ax = plt.subplots()

for plot_link in link_poly_all:
    plot_poly = Polygon(plot_link, facecolor='b',
                        edgecolor='k', linewidth=2, alpha=0.2)
    ax.add_patch(plot_poly)

for i, waypoint in enumerate(waypoints):
    plot_circ = plt.Circle(
        (waypoint[0], waypoint[1]), radii[i], color='g', alpha=0.2)
    ax.add_patch(plot_circ)

for link in links:
    x0 = waypoints[link[0]][0]
    y0 = waypoints[link[0]][1]
    x1 = waypoints[link[1]][0]
    y1 = waypoints[link[1]][1]
    ax.arrow(x0, y0, x1-x0, y1-y0, head_width=30, head_length=60,
             length_includes_head=True, fc='k', ec='k')

ax.axis('equal')

fig, ax1 = plt.subplots()
fig, ax2 = plt.subplots()
fig, ax3 = plt.subplots()
fig, ax4 = plt.subplots()

link = links[link_num]
poly = link_poly_all[link_num]

x0 = waypoints[link[0]][0]
y0 = waypoints[link[0]][1]
x1 = waypoints[link[1]][0]
y1 = waypoints[link[1]][1]
x01 = (x0 + x1) / 2.0
y01 = (y0 + y1) / 2.0

link_dx = x1 - x0
link_dy = y1 - y0
link_heading = np.arctan2(link_dy, link_dx)
link_length = np.sqrt(link_dx**2 + link_dy**2)
link_xrange = max(poly[:, 0]) - min(poly[:, 0])
link_yrange = max(poly[:, 1]) - min(poly[:, 1])
link_radius = max(link_xrange, link_yrange)

poly_loc = np.copy(poly)
poly_loc[:, 0] = poly_loc[:, 0] - x0
poly_loc[:, 1] = poly_loc[:, 1] - y0
poly_loc = rotation_matrix_2D(-link_heading, poly_loc.T)
poly_loc = poly_loc.T

for i in range(poly.shape[0]):
    shift = np.roll(poly, -i, axis=0)
    shift_loc = np.roll(poly_loc, -i, axis=0)
    edge = [shift[0], shift[1]]
    edge_loc = [shift_loc[0], shift_loc[1]]
    if i == 0:
        edges = np.array([edge])
        edges_loc = np.array([edge_loc])
    else:
        edges = np.vstack((edges, [edge]))
        edges_loc = np.vstack((edges_loc, [edge_loc]))

aabb = np.array([[min(poly[:, 0]), min(poly[:, 1])],
                 [max(poly[:, 0]), min(poly[:, 1])],
                 [max(poly[:, 0]), max(poly[:, 1])],
                 [min(poly[:, 0]), max(poly[:, 1])]])

traj_n = 10
traj_track_ext = 400
traj_samples = int(1 + (link_length + 2.0 * traj_track_ext) // 150)
lateral = np.array([])

for i in range(traj_n):

    Y = []
    for j in range(3):
        seed_number = 34560+j+10*i
        random.seed(seed_number)
        Y.append(400 * (-0.30 + 0.70 * (random.random() - 0.5)))

    X = np.array([-traj_track_ext, link_length /
                 2.0, link_length+traj_track_ext])
    xy = rotation_matrix_2D(link_heading, np.array([X, Y]))
    x = x0 + xy[0, :]
    y = y0 + xy[1, :]
    traj_x = np.linspace(x[0], x[-1], traj_samples, endpoint=True)

    A = np.array(
        [[x[0]**2, x[0], 1], [x[1]**2, x[1], 1], [x[2]**2, x[2], 1]])
    B = np.array([y[0], y[1], y[2]])
    coefs = linalg.solve(A, B)
    traj_y = coefs[0] * traj_x**2 + coefs[1] * traj_x + coefs[2]

    mask_x_aabb = (traj_x <= max(aabb[:, 0])) * (traj_x >= min(aabb[:, 0]))
    mask_y_aabb = (traj_y <= max(aabb[:, 1])) * (traj_y >= min(aabb[:, 1]))
    mask_aabb = mask_x_aabb * mask_y_aabb

    poly_inside = []
    enable_in = True
    enable_out = True
    for x_sample, y_sample, mask_sample in zip(traj_x, traj_y, mask_aabb):
        if mask_sample is False:
            inside = False
        else:
            if enable_out is False and enable_out is False:
                inside = False
            else:
                inside = is_inside(edges, x_sample, y_sample)
            if len(poly_inside) != 0:
                if enable_in is True:
                    if (poly_inside[-1] is False and inside is True):
                        poly_inside[-1] = True
                        enable_in = False
                if enable_out is True:
                    if (poly_inside[-1] is True and inside is False):
                        inside = True
                        enable_out = False
        poly_inside.append(inside)
    poly_inside = np.array(poly_inside)

    traj_xy_loc = np.array(
        [traj_x[poly_inside] - x0, traj_y[poly_inside] - y0])
    traj_xy_loc = rotation_matrix_2D(-link_heading, traj_xy_loc)

    lateral = np.append(lateral, traj_xy_loc[1])

    ax1.plot(traj_x, traj_y, ':xk',
             label="Trajectory" if i == 0 else None)
    ax2.plot(traj_xy_loc[0][1:-1], traj_xy_loc[1][1:-1], 'xk')

link_subs_n = 12
link_subs = np.linspace(-300, 0, num=link_subs_n + 1)
hist = np.histogram(lateral, bins=link_subs)
plot_poly = Polygon(poly, facecolor='b',
                    edgecolor='k', linewidth=2, alpha=0.2)
ax1.add_patch(plot_poly)
plot_aabb = Polygon(aabb, facecolor='none',
                    edgecolor='r', linewidth=2, alpha=1.0, label='AABB')
ax1.add_patch(plot_aabb)
ax1.arrow(x0, y0, link_dx, link_dy, linewidth=5, head_width=30, head_length=60,
          length_includes_head=True, fc='k', ec='k')
ax1.set_title('10 Random trajectories')
ax1.axis('equal')
ax1.legend()
ax1.grid()


for edge_loc in edges_loc:
    ax2.plot(edge_loc[:, 0], edge_loc[:, 1], 'k',
             linestyle='-', linewidth=2.5)
ax2.arrow(0, 0, link_length, 0, linewidth=5, head_width=30, head_length=60,
          length_includes_head=True, fc='k', ec='k')
ax2.arrow(0, 0, 200, 0, linewidth=5, head_width=30, head_length=60,
          length_includes_head=True, fc='r', ec='r')
ax2.text(150, 30, 'x-axis', fontsize=10)
ax2.arrow(0, 0, 0, 200, linewidth=5, head_width=30, head_length=60,
          length_includes_head=True, fc='g', ec='g')
ax2.text(30, 150, 'y-axis', fontsize=10)
ax2.grid()
y_limit = ax2.get_ylim()
y_limit_min = np.floor(y_limit[0]/100) * 100 - 100
y_limit_max = np.ceil(y_limit[1]/100) * 100 + 100
ax2.set_ylim([y_limit_min, y_limit_max])
ax2.set_aspect('equal', adjustable='box')

ax3.set_title('Histogram and PDF - Lateral distribution')
label = 'Lateral distance, ' + '%.0f' % len(lateral) + ' samples'
ax3.hist(lateral, link_subs, orientation='horizontal',
         density=True, rwidth=0.7, label=label)
if len(lateral) > 0:
    mu, sigma = norm.fit(lateral)
    pdf_y = np.linspace(y_limit_min, y_limit_max, 200)
    p = norm.pdf(pdf_y, mu, sigma)
    pdf_label = 'PDF'
    ax3.plot(p, pdf_y, 'k', linewidth=2, label=r'$\mu$=' + '%.2f' %
             mu + ', ' + r'$\sigma$=' + '%.2f' % sigma)
ax3.set_xlim([0, 0.01])
ax3.set_ylim([y_limit_min, y_limit_max])
ax3.legend()
ax3.grid()
